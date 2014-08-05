/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <string.h>

#include "syskit-pch.h"
#include "syskit/U64Fifo.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//!
//! Construct a duplicate instance of the given queue.
//!
U64Fifo::U64Fifo(const U64Fifo& q):
Growable(q)
{
    item_ = new item_t[capacity()];
    construct(q);

    numAdds_ = numItems_;
    numFails_ = 0;
    numRms_ = 0;
    usagePeak_ = numItems_;
}


//!
//! Construct an empty queue with initial capacity of capacity items. The
//! queue does not grow if growBy is zero, exponentially grows by doubling
//! if growBy is negative, and grows by growBy items otherwise.
//!
U64Fifo::U64Fifo(unsigned int capacity, int growBy):
Growable(capacity, growBy)
{
    head_ = 0;
    numItems_ = 0;
    tail_ = 0;
    resetStat();

    // Allocate for all items now. Initialize each item when used.
    item_ = new item_t[U64Fifo::capacity()];
}


U64Fifo::~U64Fifo()
{
    delete[] item_;
}


//!
//! Copy the queue contents from given queue. This queue might grow to
//! accomodate the source. If it cannot grow, items at the head will be
//! dropped.
//!
const U64Fifo& U64Fifo::operator =(const U64Fifo& q)
{

    // Prevent self assignment.
    if (this == &q)
    {
        return *this;
    }

    // Might need to grow to accomodate source.
    unsigned int minCap = q.numItems_;
    if (minCap > capacity())
    {
        if (canGrow())
        {
            delete[] item_;
            item_ = new item_t[setNextCap(minCap)];
        }
    }

    // Copy all items.
    unsigned int curCap = capacity();
    numRms_ += numItems_;
    if (minCap <= curCap)
    {
        construct(q);
        numAdds_ += numItems_;
        if (numItems_ > usagePeak_)
        {
            usagePeak_ = numItems_;
        }
    }

    // Source queue has too many items. Drop items at the head.
    else if (curCap > 0)
    {
        reset();
        for (size_t i = q.numItems_ - curCap, iEnd = q.numItems_; i < iEnd; ++i)
        {
            item_t item = q.peek(i);
            U64Fifo::add(item);
        }
    }

    // Return reference to self.
    return *this;
}


//!
//! Add given item to the tail of queue. Return true if successful.
//! Return false otherwise (queue is full).
//!
bool U64Fifo::add(item_t item)
{

    // Queue is not full.
    bool ok;
    if ((numItems_ < capacity()) || grow())
    {
        ++numAdds_;
        if (++numItems_ > usagePeak_)
        {
            usagePeak_ = numItems_;
        }
        item_[tail_] = item;
        if (++tail_ == capacity())
        {
            tail_ = 0;
        }
        ok = true;
    }

    // Queue is full.
    else
    {
        ++numFails_;
        ok = false;
    }

    // Return true if successful.
    return ok;
}


//!
//! Add given item to the head of the queue. Return true if successful.
//! Return false otherwise (queue is full).
//!
bool U64Fifo::addAtHead(item_t item)
{

    // Queue is not full.
    bool ok;
    if ((numItems_ < capacity()) || grow())
    {
        unsigned int capacity = U64Fifo::capacity();
        if (--head_ >= capacity)
        {
            head_ = capacity - 1;
        }
        ++numAdds_;
        if (++numItems_ > usagePeak_)
        {
            usagePeak_ = numItems_;
        }
        item_[head_] = item;
        ok = true;
    }

    // Queue is full.
    else
    {
        ++numFails_;
        ok = false;
    }

    // Return true if successful.
    return ok;
}


//!
//! Remove item from the head of the queue. Return true if successful (also
//! return removed item in item). Return false otherwise (queue is empty).
//!
bool U64Fifo::rm(item_t& item)
{

    // Queue is not empty.
    bool ok;
    if (numItems_ > 0)
    {
        ++numRms_;
        --numItems_;
        item = item_[head_];
        if (++head_ == capacity())
        {
            head_ = 0;
        }
        ok = true;
    }

    // Queue is empty.
    else
    {
        ok = false;
    }

    // Return true if successful.
    return ok;
}


//!
//! Remove item from the tail of the queue. Return true if successful (also
//! return removed item in item). Return false otherwise (queue is empty).
//!
bool U64Fifo::rmFromTail(item_t& item)
{

    // Queue is not empty.
    bool ok;
    if (numItems_ > 0)
    {
        ++numRms_;
        --numItems_;
        unsigned int capacity = U64Fifo::capacity();
        if (--tail_ >= capacity)
        {
            tail_ = capacity - 1;
        }
        item = item_[tail_];
        ok = true;
    }

    // Queue is empty.
    else
    {
        ok = false;
    }

    // Return true if successful.
    return ok;
}


//!
//! Resize queue. Given new capacity must not be less than the current
//! queue size. Return true if successful.
//!
bool U64Fifo::resize(unsigned int newCap)
{
    bool ok;
    unsigned int curCap = capacity();
    if (numItems_ > newCap)
    {
        ok = false;
    }

    // No-op if capacity does not change.
    else if (newCap == curCap)
    {
        ok = true;
    }

    // Resize.
    else
    {

        // Items are contiguous.
        item_t* item = new item_t[newCap];
        if (head_ < tail_)
        {
            memcpy(item, item_ + head_, numItems_ * sizeof(*item_));
        }

        // Items wrap.
        else if (numItems_ > 0)
        {
            size_t part0Count = curCap - head_;
            memcpy(item, item_ + head_, part0Count * sizeof(*item_));
            size_t part1Count = tail_;
            memcpy(item + part0Count, item_, part1Count * sizeof(*item_));
        }

        head_ = 0;
        tail_ = numItems_;
        delete[] item_;
        item_ = item;
        setCapacity(newCap);
        ok = true;
    }

    // Return true if successful.
    return ok;
}


//
// Construct from given queue. Before invoking this method,
// the queue memory must have been properly allocated and
// the capacity must have already been set.
//
void U64Fifo::construct(const U64Fifo& q)
{

    // Copy the utilized items only. Don't care about the
    // unused ones. Unused items are initialized when used.
    head_ = 0;
    numItems_ = q.numItems_;
    tail_ = numItems_;

    // Items from the source are contiguous.
    if (q.head_ < q.tail_)
    {
        memcpy(item_, q.item_ + q.head_, numItems_ * sizeof(*item_));
    }

    // Items from the source wrap.
    else if (numItems_ > 0)
    {
        size_t part0Count = q.capacity() - q.head_;
        memcpy(item_, q.item_ + q.head_, part0Count * sizeof(*item_));
        size_t part1Count = q.tail_;
        memcpy(item_ + part0Count, q.item_, part1Count * sizeof(*item_));
    }
}


//!
//! Reset instance with statistics from given queue.
//!
void U64Fifo::Stat::reset(const U64Fifo& q)
{
    numAdds_ = q.numAdds_;
    numFails_ = q.numFails_;
    numRms_ = q.numRms_;
    usagePeak_ = q.usagePeak_;
}

END_NAMESPACE1
