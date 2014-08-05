/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/BufArena.hpp"
#include "syskit/macros.h"

// Magic mark stored in the second pointer of available buffers to detect
// freeing freed buffers. Also stored in the first pointer of newly allocated
// buffers.
void* const MAGIC_MARK = (sizeof(void*) == 8)? (void*)(0x0badbeef1badbeefLL): (void*)(0x0badbeefL);

BEGIN_NAMESPACE1(syskit)


//!
//! Construct an arena producing and recycling capacity buffers. Each
//! buffer has a fixed size of bufSize bytes. If given buffer size is
//! less than sizeof(void*), then sizeof(void*) will be used. Zero
//! capacity is allowed. An arena does not grow if growBy is zero,
//! exponentially grows by doubling if growBy is negative, and linearly
//! grows by growBy buffers otherwise.
//!
BufArena::BufArena(unsigned int bufSize, unsigned int capacity, int growBy):
Growable(capacity, growBy)
{
    bufSize_ = (bufSize < sizeof(void*))? sizeof(void*): bufSize;
    numInUseBufs_ = 0;
    useMagicMark_ = (bufSize_ >= sizeof(*firstAvail_));
    resetStat();

    // Allocate initial bucket.
    bucket_ = new Bucket(bufSize_, BufArena::capacity());
    firstAvail_ = 0;
    lastAvail_ = 0;
    useBucket(*bucket_);
}


BufArena::~BufArena()
{
    // Minimize damage from destructing an arena with in-use buffers. If there
    // are still in-use buffers at destruction, avoid freeing the buckets and
    // accept some memory leakage instead.
    if (numInUseBufs_ == 0)
    {
        Bucket::deleteAll(bucket_);
    }
}


//!
//! Determine if given address refers to an available buffer. Return
//! true if yes. If the buffer size is small (sizeof(void*)), this
//! method uses linear search. Otherwise, available buffers are marked,
//! so the magic mark is used to determine if a buffer is available.
//!
bool BufArena::bufIsAvail(const BufArena& arena, const void* buf)
{

    // Available buffers are marked.
    const link_t* p = static_cast<const link_t*>(buf);
    if (arena.useMagicMark_)
    {
        bool isAvail = (p->magicMark == MAGIC_MARK);
        return isAvail;
    }

    // Using linear search, determine if given address points to an
    // available buffer.
    bool isAvail = false;
    for (p = arena.firstAvail_; p != 0; p = p->next)
    {
        if (p == buf)
        {
            isAvail = true;
            break;
        }
    }

    // Return true if given address points to an available buffer.
    return isAvail;
}


//!
//! Free a buffer. Result is unpredictable if given buffer address is invalid.
//! Return true if successful.
//!
bool BufArena::freeBuf(const void* buf)
{

    // Sanity check for freeing freed buffer.
    // Also mark freed buffers.
    link_t* p = (link_t*)(buf);
    if (useMagicMark_)
    {
        if ((p->magicMark == MAGIC_MARK) && bufIsAvail(*this, buf))
        {
            const char* const TOMBSTONE = "BufArena::freeBuf: freeing freed buffer!!";
            const char** grave = 0;
            *grave = TOMBSTONE;
        }
        p->magicMark = MAGIC_MARK;
    }

    // Sanity check for too many frees.
    else if (numInUseBufs_ == 0)
    {
        bool ok = false;
        return ok;
    }

    // Put given buffer at the end of the available list so
    // we can allocate least recently used buffers first.
    p->next = 0;
    if (firstAvail_ != 0)
    {
        lastAvail_->next = p;
        lastAvail_ = p;
    }

    // Given buffer is the only available buffer now.
    else
    {
        firstAvail_ = p;
        lastAvail_ = p;
    }

    // Update stats.
    ++numFrees_;
    --numInUseBufs_;
    bool ok = true;
    return ok;
}


//!
//! Return true if given address refers to any part of any buffer
//! in the arena (i.e., if the arena owns the memory address).
//!
bool BufArena::ownsAddr(const void* addr) const
{
    bool answer = false;
    for (const Bucket* bucket = bucket_; bucket != 0; bucket = bucket->next())
    {
        const unsigned char* buf0 = bucket->buf0();
        const unsigned char* bufN = bucket->bufN();
        if ((addr < bufN) && (addr >= buf0))
        {
            answer = true;
            break;
        }
    }

    return answer;
}


//!
//! Return true if given address refers to a valid buffer address
//! (i.e., if the arena owns the buffer address).
//!
bool BufArena::ownsBuf(const void* buf) const
{
    bool answer = false;
    for (const Bucket* bucket = bucket_; bucket != 0; bucket = bucket->next())
    {
        const unsigned char* buf0 = bucket->buf0();
        const unsigned char* bufN = bucket->bufN();
        if ((buf < bufN) && (buf >= buf0))
        {
            size_t delta = static_cast<const unsigned char*>(buf)-buf0;
            if ((delta % bufSize_) == 0)
            {
                answer = true;
            }
            break;
        }
    }

    return answer;
}


//!
//! Resize arena. Only growable arenas can be resized. If given capacity
//! is larger than current capacity, the arena will grow until the given
//! capacity can be accomodated (new capacity will be greater than or equal
//! to given capacity). If given capacity is smaller than current capacity
//! and if doable, the arena will shrink as much as possible (new capacity
//! will be greater than or equal to given capacity but not less than initial
//! capacity). An arena grows by allocating new buckets and shrinks by
//! freeing full buckets. Most recently allocated buckets are freed first.
//! Only growable arenas with buffer size of 8 or larger can shrink. Return
//! true if some resize occurred.
//!
bool BufArena::resize(unsigned int newCap)
{

    // Grow if growable.
    bool resized = false;
    unsigned int curCap = capacity();
    if (curCap < newCap)
    {
        int growBy;
        if (canGrow(growBy))
        {
            do
            {
                unsigned int delta = (growBy < 0)? curCap: growBy;
                Bucket* newBucket = new Bucket(bufSize_, delta);
                useBucket(*newBucket);
                newBucket->setNext(bucket_);
                bucket_ = newBucket;
                increaseCap(delta);
                resized = true;
            } while ((curCap = capacity()) < newCap);
        }
        return resized;
    }

    // Shrink if shrinkable.
    if ((curCap > newCap) &&    //shrinking?
        (bucket_->next() != 0) && //can only shrink if already grown
        useMagicMark_)            //can only shrink if buffer size is 8 or larger
    {
        size_t initialCap = BufArena::initialCap();
        shrink((newCap >= initialCap)? newCap: initialCap);
        resized = (curCap != capacity());
    }

    // Return true if some resize occurred.
    return resized;
}


void BufArena::freeFullBuckets()
{

    // Unlink available buffers which reside in full buckets so these
    // full buckets can be freed. Available buffers have been marked
    // with the owning bucket.
    link_t seedLink = {0};
    link_t* prevLink = &seedLink;
    for (link_t* p = firstAvail_; p != 0; p = p->next)
    {
        const Bucket* bucket = static_cast<const Bucket*>(p->magicMark);
        if (bucket->inUse() != 0)
        {
            prevLink->next = p;
            prevLink->magicMark = MAGIC_MARK;
            prevLink = p;
        }
    }

    // Update head and tail of the available list.
    firstAvail_ = seedLink.next;
    lastAvail_ = prevLink;
    lastAvail_->next = 0;
    lastAvail_->magicMark = MAGIC_MARK;

    // Free full buckets.
    Bucket* prevBucket = 0;
    Bucket* nextBucket = 0;
    for (Bucket* bucket = bucket_; bucket != 0; bucket = nextBucket)
    {
        nextBucket = bucket->next();
        if (bucket->inUse() == 0)
        {
            if (prevBucket != 0)
            {
                prevBucket->setNext(nextBucket);
            }
            else
            {
                bucket_ = nextBucket;
            }
            delete bucket;
        }
        else
        {
            prevBucket = bucket;
        }
    }
}


//!
//! Reinitialize by freeing all buffers. This method reconstructs the available
//! list of buffers from scratch and can be expensive. If the allocated buffers
//! are known, consider freeing the buffers individually instead.
//!
void BufArena::reset()
{
    if (numInUseBufs_ != 0)
    {

        // Update stats.
        numFrees_ += numInUseBufs_;
        numInUseBufs_ = 0;

        // Form links to all buffers in all buckets.
        firstAvail_ = 0;
        lastAvail_ = 0;
        for (const Bucket* bucket = bucket_; bucket != 0; bucket = bucket->next())
        {
            useBucket(*bucket);
        }
    }
}


//
// Shrink as much as possible. Make sure new capacity will be greater than
// or equal to given capacity. Shrink by freeing full buckets. Most recently
// allocated buckets are freed first.
//
void BufArena::shrink(size_t minCap)
{

    // Assume all buckets are empty.
    for (const Bucket* bucket = bucket_; bucket != 0; bucket = bucket->next())
    {
        unsigned int bucketCap = bucket->capacity();
        bucket->setInUse(bucketCap);
    }

    // For each available buffer, locate its bucket and keep track
    // of the in-use buffer count to identify full buckets later.
    for (link_t* p = firstAvail_; p != 0; p = p->next)
    {
        for (const Bucket* bucket = bucket_; bucket != 0; bucket = bucket->next())
        {
            if (bucket->ownsAddr(p))
            {
                bucket->decrementInUse();
                p->magicMark = const_cast<Bucket*>(bucket);
                break;
            }
        }
    }

    // For each full bucket, see if it is too big to be freed. If it
    // is too big, consider it not full. Make sure new capacity will
    // be greater than or equal to given capacity. Also, avoid freeing
    // the initial zero-capacity bucket.
    unsigned int newCap = capacity();
    for (const Bucket* bucket = bucket_; bucket != 0; bucket = bucket->next())
    {
        if (bucket->inUse() == 0)
        {
            unsigned int bucketCap = bucket->capacity();
            newCap -= bucketCap;
            if ((newCap < minCap) || (bucketCap == 0))
            {
                bucket->setInUse(0xffffffffU);
                newCap += bucketCap;
            }
        }
    }

    // Cannot shrink as requested. Give up.
    if (newCap == capacity())
    {
        for (link_t* p = firstAvail_; p != 0; p = p->next)
        {
            p->magicMark = MAGIC_MARK;
        }
    }

    // Free full buckets and update the available list.
    else
    {
        freeFullBuckets();
        setCapacity(newCap);
    }
}


//
// Use given bucket by linking all of its buffers into the available list.
//
void BufArena::useBucket(const Bucket& bucket)
{

    // Watch out for zero capacity.
    link_t* buf0 = reinterpret_cast<link_t*>(bucket.buf0());
    link_t* lastBuf = reinterpret_cast<link_t*>(bucket.bufN() - bufSize_);
    if (buf0 <= lastBuf)
    {

        // Link buffers from bucket so that buffer(i) links with buffer(i+1),
        // and mark each buffer.
        if (useMagicMark_)
        {
            link_t* p;
            for (p = buf0; p < lastBuf; p = p->next)
            {
                p->next = reinterpret_cast<link_t*>(reinterpret_cast<unsigned char*>(p)+bufSize_);
                p->magicMark = MAGIC_MARK;
            }
            p->next = 0;
            p->magicMark = MAGIC_MARK;
        }

        // Link buffers from bucket so that buffer(i) links with buffer(i+1),
        // but do not mark each buffer.
        else
        {
            link_t* p;
            for (p = buf0; p < lastBuf; p = p->next)
            {
                p->next = reinterpret_cast<link_t*>(reinterpret_cast<unsigned char*>(p)+bufSize_);
            }
            p->next = 0;
        }

        // Link the buffers into the available list.
        (firstAvail_ == 0)? (firstAvail_ = buf0): (lastAvail_->next = buf0);
        lastAvail_ = lastBuf;
    }
}


//!
//! Allocate a buffer. Return its address. Return zero if none is available.
//!
void* BufArena::allocateBuf()
{

    // None available.
    void* buf;
    if ((firstAvail_ == 0) && (!grow()))
    {
        ++numFails_;
        buf = 0;
        return buf;
    }

    // Allocate first available buffer.
    buf = firstAvail_;
    firstAvail_ = firstAvail_->next;

    // This is the last available one.
    // No more buffers are available unless the arena can grow.
    if (firstAvail_ == 0)
    {
        lastAvail_ = 0;
    }

    // Mark newly allocated buffer.
    link_t* p = static_cast<link_t*>(buf);
    p->next = static_cast<link_t*>(MAGIC_MARK);
    if (useMagicMark_)
    {
        p->magicMark = 0;
    }

    // Update stats.
    ++numAllocs_;
    if (++numInUseBufs_ > usagePeak_)
    {
        usagePeak_ = numInUseBufs_;
    }

    // Return allocated buffer. Return zero if none available.
    return buf;
}


//
// Construct an unlinked bucket of capacity buffers.
// Each buffer has size bufSize bytes.
//
BufArena::Bucket::Bucket(unsigned int bufSize, unsigned int capacity)
{

    // Allocate buffers owned by this bucket.
    size_t bufVecSize = bufSize * capacity;
    buf0_ = new unsigned char[bufVecSize];
    bufN_ = buf0_ + bufVecSize;

    // Bucket is unlinked.
    capacity_ = capacity;
    inUse_ = 0U;
    next_ = 0;
}


BufArena::Bucket::~Bucket()
{
    delete[] buf0_;
}


//
// Delete given bucket including linked buckets.
//
void BufArena::Bucket::deleteAll(const Bucket* bucket)
{
    while (bucket != 0)
    {
        const Bucket* next = bucket->next_;
        delete bucket;
        bucket = next;
    }
}


//!
//! Get areana stats.
//!
BufArena::Stat::Stat(const BufArena& arena)
{
    reset(arena);
}

END_NAMESPACE1
