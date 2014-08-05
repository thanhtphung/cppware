/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <string.h>

#include "syskit-pch.h"
#include "syskit/HashTable.hpp"
#include "syskit/Prime.hpp"
#include "syskit/macros.h"

inline void incrementWmark(unsigned int& counter, unsigned int& wmark)
{
    if (++counter > wmark)
    {
        wmark = counter;
    }
}

BEGIN_NAMESPACE1(syskit)


//!
//! Construct an empty hash table. The table has at least capacity buckets
//! to start with. The table exponentially grows by doubling when growth
//! occurs. Growth occurs when the average itemsPerBucket ratio exceeds
//! bucketCap. When items are compared, the given comparison function will be
//! used. The given hash function dictates which bucket an item must reside
//! in. It's assumed a simple modular hash function is used, and the number
//! of buckets at anytime will be prime.
//!
HashTable::HashTable(diff_t diff, hash_t hash, unsigned int capacity, double bucketCap):
Growable(Prime(capacity).asU32(), -1 /*growBy*/)
{
    unsigned int numBuckets = Growable::capacity();
    bucketCap_ = (bucketCap < 1.0)? 1.0: bucketCap;
    diff_ = diff;
    hash_ = hash;
    maxItems_ = static_cast<unsigned int>(numBuckets * bucketCap_ + 0.5);
    numEmptyBuckets_ = numBuckets;
    numItems_ = 0;
    peakBucketSize_ = 0;
    usagePeak_ = 0;

    bucket_ = new node_t*[numBuckets];
    bucketSize_ = new unsigned int[numBuckets];
    memset(bucket_, 0, numBuckets * sizeof(*bucket_));
    memset(bucketSize_, 0, numBuckets * sizeof(*bucketSize_));
}


//!
//! Construct a duplicate instance of the given table.
//!
HashTable::HashTable(const HashTable& table):
Growable(table)
{
    bucketCap_ = table.bucketCap_;
    diff_ = table.diff_;
    hash_ = table.hash_;
    maxItems_ = table.maxItems_;
    numEmptyBuckets_ = table.numEmptyBuckets_;
    numItems_ = table.numItems_;
    peakBucketSize_ = 0;
    usagePeak_ = numItems_;

    // Allocate space for buckets. Copy buckets.
    size_t numBuckets = capacity();
    bucket_ = new node_t*[numBuckets];
    bucketSize_ = new unsigned int[numBuckets];
    copy(&table);
}


HashTable::~HashTable()
{
    delete[] bucketSize_;

    for (size_t i = 0, numBuckets = capacity(); i < numBuckets; ++i)
    {
        const node_t* next;
        for (const node_t* p = bucket_[i]; p != 0; next = p->next, delete p, p = next);
    }

    delete[] bucket_;
}


const HashTable& HashTable::operator =(const HashTable& table)
{

    // Prevent self assignment.
    if (this != &table)
    {
        reset();
        ((capacity() == table.capacity()) && (hash_ == table.hash_))?
            copy(&table):
            copy(table.bucket_, table.capacity());
    }

    // Return reference to self.
    return *this;
}


//!
//! Add given item to the table, even if the same item already exists
//! in the table. The table's hash function dictates where (in which
//! bucket) this new item will reside. Behavior is unpredictable if the
//! hash function does not behave. Return true if successful. Return
//! false otherwise (table is full and can no longer grow).
//!
bool HashTable::add(item_t item)
{
    bool ok;
    if ((numItems_ < maxItems_) || grow())
    {
        size_t numBuckets = capacity();
        size_t i = hash_(item, numBuckets);
        node_t* node = new node_t;
        node->item = item;
        node->next = bucket_[i];
        bucket_[i] = node;
        if (node->next == 0)
        {
            --numEmptyBuckets_;
        }
        incrementWmark(numItems_, usagePeak_);
        incrementWmark(bucketSize_[i], peakBucketSize_);
        ok = true;
    }

    else
    {
        ok = false;
    }

    return ok;
}


//!
//! Add given item to the table. If the same item already exists in the
//! table, replace it. The table's hash function dictates where (in which
//! bucket) this new item will reside. Behavior is unpredictable if the
//! hash function does not behave. Return true if successful. Return
//! false otherwise (table is full and can no longer grow).
//!
bool HashTable::add(item_t item, item_t& replacedItem)
{

    // Replace if found.
    size_t numBuckets = capacity();
    size_t i = hash_(item, numBuckets);
    for (node_t* p = bucket_[i]; p != 0; p = p->next)
    {
        if (diff_(item, p->item) == 0)
        {
            replacedItem = p->item;
            p->item = item;
            bool ok = true;
            return ok;
        }
    }

    // Add hashed item.
    replacedItem = 0;
    bool ok = addHere(i, item);
    return ok;
}


bool HashTable::addHere(size_t index, item_t item)
{

    // Return immediately if table is full.
    // Need to perform the hash function again if growth occurs.
    if (numItems_ >= maxItems_)
    {
        if (!grow())
        {
            bool ok = false;
            return ok;
        }
        size_t numBuckets = capacity();
        index = hash_(item, numBuckets);
    }

    // Add item.
    node_t* node = new node_t;
    node->item = item;
    node->next = bucket_[index];
    bucket_[index] = node;
    if (node->next == 0)
    {
        --numEmptyBuckets_;
    }
    incrementWmark(numItems_, usagePeak_);
    incrementWmark(bucketSize_[index], peakBucketSize_);

    // Return true to indicate success.
    bool ok = true;
    return ok;
}


//!
//! Add given item to the table only if the same item doesn't exist.
//! The table's hash function dictates where (in which bucket) this
//! new item will reside. Behavior is unpredictable if the hash
//! function does not behave. Return true if successful. Return
//! false otherwise (table is full or item already exists). Return
//! found item in foundItem if item already exists. Return zero in
//! foundItem otherwise.
//!
bool HashTable::addIfNotFound(item_t item, item_t& foundItem)
{

    // Return immediately if item already exists.
    foundItem = 0;
    size_t numBuckets = capacity();
    size_t i = hash_(item, numBuckets);
    for (const node_t* p = bucket_[i]; p != 0; p = p->next)
    {
        if (diff_(item, p->item) == 0)
        {
            foundItem = p->item;
            bool ok = false;
            return ok;
        }
    }

    // Add hashed item.
    bool ok = addHere(i, item);
    return ok;
}


//!
//! Apply callback to all entries. The callback should return true to continue
//! iterating and should return false to abort iterating. Return false if the
//! callback aborted the iterating. Return true otherwise.
//!
bool HashTable::apply(cb0_t cb, void* arg) const
{
    bool ok = true;
    for (size_t i = 0, numBuckets = capacity(); i < numBuckets; ++i)
    {
        for (const node_t* p = bucket_[i]; p != 0; p = p->next)
        {
            if (!cb(arg, p->item))
            {
                ok = false;
                i = numBuckets - 1; //terminate outer loop
                break;
            }
        }
    }

    return ok;
}


//!
//! Locate given item. Behavior is unpredictable if the hash function
//! does not behave. Return true if found (also return the found item
//! in foundItem). Return false otherwise. Use given compatible comparison
//! function for this search.
//!
bool HashTable::find(const void* item, diff_t diff, item_t& foundItem) const
{
    bool found = false;
    size_t numBuckets = capacity();
    size_t i = hash_(item, numBuckets);
    for (const node_t* p = bucket_[i]; p != 0; p = p->next)
    {
        if (diff(item, p->item) == 0)
        {
            foundItem = p->item;
            found = true;
            break;
        }
    }

    return found;
}


//!
//! Grow by adding more buckets and rehashing. Return true.
//!
bool HashTable::grow()
{
    return resize(nextCap());
}


//!
//! Resize table. Return true if successful.
//!
bool HashTable::resize(unsigned int newCap)
{
    bool ok;
    newCap = Prime(newCap).asU32();
    unsigned int oldCap = capacity();
    if (newCap == 0)
    {
        ok = false;
    }
    else
    {
        ok = true;
        if (newCap != oldCap)
        {
            node_t** oldBucket = bucket_;
            const unsigned int* oldBucketSize = bucketSize_;
            setCapacity(newCap);
            bucket_ = new node_t*[newCap];
            bucketSize_ = new unsigned int[newCap];
            memset(bucket_, 0, newCap * sizeof(*bucket_));
            memset(bucketSize_, 0, newCap * sizeof(*bucketSize_));
            maxItems_ = static_cast<unsigned int>(newCap * bucketCap_ + 0.5);
            numEmptyBuckets_ = newCap;
            rehash(oldBucket, oldCap);
            delete[] oldBucketSize;
            delete[] oldBucket;
        }
    }

    return ok;
}


//!
//! Locate given item. Use given compatible comparison function. Behavior is
//! unpredictable if the hash function does not behave. If found, remove it
//! from the table and return true (also return the removed item in removedItem).
//! Return false otherwise.
//!
bool HashTable::rm(const void* item, diff_t diff, item_t& removedItem)
{
    bool ok = false;
    node_t* prev = 0;
    size_t numBuckets = capacity();
    size_t i = hash_(item, numBuckets);
    for (node_t* p = bucket_[i]; p != 0; prev = p, p = p->next)
    {
        if (diff(item, p->item) == 0)
        {
            removedItem = p->item;
            --numItems_;
            --bucketSize_[i];
            if (prev == 0)
            {
                if (p->next == 0)
                {
                    ++numEmptyBuckets_;
                }
                bucket_[i] = p->next;
            }
            else
            {
                prev->next = p->next;
            }
            delete p;
            ok = true;
            break;
        }
    }

    return ok;
}


//!
//! Manage growth. The table grows by doubling and cannot be customized.
//! That is, the growth factor is negative and cannot be changed. Return
//! true if given growth factor is negative. Return false otherwise.
//!
bool HashTable::setGrowth(int growBy)
{
    return (growBy < 0);
}


//!
//! Apply callback to all entries.
//!
void HashTable::apply(cb1_t cb, void* arg) const
{
    for (size_t i = 0, numBuckets = capacity(); i < numBuckets; ++i)
    {
        for (const node_t* p = bucket_[i]; p != 0; p = p->next)
        {
            cb(arg, p->item);
        }
    }
}


//
// Copy items from given hash table. Metadata has already been copy.
// Since this table and source table has same capacity and same hash
// function, items are not rehashed.
//
void HashTable::copy(const HashTable* that)
{
    const node_t* const* bucket = that->bucket_;
    const unsigned int* bucketSize = that->bucketSize_;
    for (size_t i = 0, numBuckets = capacity(); i < numBuckets; ++i)
    {
        const node_t* src = bucket[i];
        if (src == 0)
        {
            bucket_[i] = 0;
        }
        else
        {
            node_t* dst = new node_t;
            dst->item = src->item;
            bucket_[i] = dst;
            while ((src = src->next) != 0)
            {
                dst = dst->next = new node_t;
                dst->item = src->item;
            }
            dst->next = 0;
        }
        if ((bucketSize_[i] = bucketSize[i]) > peakBucketSize_)
        {
            peakBucketSize_ = bucketSize_[i];
        }
    }
}


//
// Copy items from given hash table. Metadata has already been copy.
// Since this table and source table has different capacity and/or
// different hash function, items needs to be rehashed.
//
void HashTable::copy(const node_t* const* from, size_t bucketCount)
{
    const node_t* const* bucket = from;
    const node_t* const* bucketEnd = bucket + bucketCount;
    for (; bucket < bucketEnd; ++bucket)
    {
        for (const node_t* src = *bucket; src != 0; src = src->next)
        {
            add(src->item);
        }
    }
}


void HashTable::rehash(node_t* const* old, unsigned int oldCap)
{
    size_t numBuckets = capacity();
    node_t* const* bucket = old;
    for (const node_t* const* bucketEnd = bucket + oldCap; bucket < bucketEnd; ++bucket)
    {
        node_t* next;
        for (node_t* node = *bucket; node != 0; node = next)
        {
            next = node->next;
            size_t i = hash_(node->item, numBuckets);
            node->next = bucket_[i];
            if (node->next == 0)
            {
                --numEmptyBuckets_;
            }
            bucket_[i] = node;
            incrementWmark(bucketSize_[i], peakBucketSize_);
        }
    }
}


//!
//! Reset the table by removing all items.
//!
void HashTable::reset()
{
    if (numItems_ > 0)
    {
        size_t numBuckets = capacity();
        for (size_t i = 0; i < numBuckets; ++i)
        {
            const node_t* next;
            for (const node_t* p = bucket_[i]; p != 0; next = p->next, delete p, p = next);
            bucket_[i] = 0;
            bucketSize_[i] = 0;
        }
        numItems_ = 0;
        numEmptyBuckets_ = static_cast<unsigned int>(numBuckets);
    }
}

END_NAMESPACE1
