/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <new>

#include "syskit-pch.h"
#include "syskit/ItemQ.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)

const unsigned int ItemQ::ETERNITY = Semaphore::ETERNITY;
const unsigned int ItemQ::MAX_CAP = Semaphore::MAX_CAP;


//!
//! Construct an empty queue with initial capacity of capacity items.
//! The queue does not grow if growBy is zero, exponentially grows by
//! doubling if growBy is negative, and grows by growBy items otherwise.
//!
ItemQ::ItemQ(unsigned int capacity, int growBy):
Fifo((capacity > MAX_CAP)? MAX_CAP: capacity, growBy),
numFails_(0U /*u32*/),
inUseSlotCount_(0U /*capacity*/),
ss_()
{
    unsigned int qCap = Fifo::capacity();
    emptySlotCount_ = new Semaphore(qCap);
}


ItemQ::~ItemQ()
{
    delete emptySlotCount_;
    for (size_t i = numItems(); i > 0; Item::release(static_cast<const Item*>(peek(--i))));
}


//!
//! Peek at queue by applying callback to all queued items. The callback should
//! return true to continue iterating and should return false to abort iterating.
//! Return false if the callback aborted the iterating. Return true otherwise.
//! The queue is frozen during this expectedly quick operation, and any attempt
//! to modify this queue in the callback will cause deadlocks.
//!
bool ItemQ::apply(cb0_t cb, void* arg) const
{
    bool ok = true;
    SpinSection::Lock lock(ss_);
    for (size_t i = 0, length = numItems(); i < length; ++i)
    {
        Item* item = static_cast<Item*>(peek(i));
        if (!cb(arg, item, i))
        {
            ok = false;
            break;
        }
    }

    return ok;
}


//!
//! Enqueue given item into the FIFO queue at the head. Wait up to
//! timeoutInMsecs msecs if necessary (i.e., if queue is full).
//! Return true if successful. Successful or not, the sender should
//! no longer reference the item since it either will be destroyed
//! by the receiver or will self-destruct.
//!
bool ItemQ::expedite(Item* item, unsigned int timeoutInMsecs)
{

    // Don't wait if queue is growable.
    if (canGrow())
    {
        timeoutInMsecs = 0;
    }

    bool ok;
    for (;;)
    {

        // Wait up to timeoutInMsecs msecs if queue is full.
        if (emptySlotCount_->decrement(timeoutInMsecs))
        {
            {
                SpinSection::Lock lock(ss_);
                addAtHead(item);
            }
            inUseSlotCount_.increment();
            ok = true;
            break;
        }

        // Increase capacity and try again.
    {
        SpinSection::Lock lock(ss_);
        ok = grow();
    }

        if (!ok)
        {
            ++numFails_;
            Item::release(item);
            break;
        }
    }

    return ok;
}


//!
//! Dequeue from the FIFO queue. Wait up to timeoutInMsecs msecs if
//! necessary (i.e., if queue is empty). Return true if successful. Receiver
//! is responsible for freeing the received item using release().
//!
bool ItemQ::get(Item*& item, unsigned int timeoutInMsecs)
{

    // Wait up to timeoutInMsecs msecs if queue is empty.
    bool ok;
    if (inUseSlotCount_.decrement(timeoutInMsecs))
    {
        {
            SpinSection::Lock lock(ss_);
            rm((void*&)(item));
        }
        emptySlotCount_->increment();
        ok = true;
    }

    // Timed out.
    else
    {
        ok = false;
    }

    // Return true if successful.
    return ok;
}


//
// Grow if growable. Return true if grown.
//
bool ItemQ::grow()
{
    bool grown = false;
    if (canGrow())
    {

        // Limit growth due to semaphore's limit.
        unsigned int oldCap = capacity();
        unsigned int newCap = nextCap();
        if (newCap > MAX_CAP)
        {
            newCap = MAX_CAP;
        }

        // Reflect growth in semaphore.
        unsigned int delta = newCap - oldCap;
        if ((delta > 0) && resize(newCap))
        {
            emptySlotCount_->incrementBy(delta);
            grown = true;
        }
    }

    return grown;
}


//!
//! Enqueue given item into the FIFO queue at the tail. Wait up to
//! timeoutInMsecs msecs if necessary (i.e., if queue is full).
//! Return true if successful. Successful or not, the sender should
//! no longer reference the item since it either will be destroyed
//! by the receiver or will self-destruct.
//!
bool ItemQ::put(Item* item, unsigned int timeoutInMsecs)
{

    // Don't wait if queue is growable.
    if (canGrow())
    {
        timeoutInMsecs = 0;
    }

    bool ok;
    for (;;)
    {

        // Wait up to timeoutInMsecs msecs if queue is full.
        if (emptySlotCount_->decrement(timeoutInMsecs))
        {
            {
                SpinSection::Lock lock(ss_);
                add(item);
            }
            inUseSlotCount_.increment();
            ok = true;
            break;
        }

        // Increase capacity and try again.
    {
        SpinSection::Lock lock(ss_);
        ok = grow();
    }

        if (!ok)
        {
            ++numFails_;
            Item::release(item);
            break;
        }
    }

    return ok;
}


ItemQ::Item::Item()
{
}


ItemQ::Item::~Item()
{
}


//!
//! Release instance. The default implementation releases the instance via the
//! delete operator.
//!
void ItemQ::Item::release() const
{
    delete this;
}


//!
//! Reset instance with statistics from given queue.
//!
void ItemQ::Stat::reset(const ItemQ& q)
{

    // These following stats are static, so no locking required.
    growBy_ = q.growthFactor();
    initialCap_ = q.initialCap();

    // Treat dynamic stats as one.
    Fifo::Stat* stat;
    unsigned char buf[sizeof(Fifo::Stat)];
    {
        SpinSection::Lock lock(q.ss_);
        stat = new(buf)Fifo::Stat(q);
        capacity_ = q.capacity();
        length_ = q.numItems();
        numFails_ = q.numFails_;
    }

    numFails_ += stat->numFails();
    numGets_ = stat->numRms();
    numPuts_ = stat->numAdds();
    usagePeak_ = stat->usagePeak();
    stat->Fifo::Stat::~Stat();
}

END_NAMESPACE1
