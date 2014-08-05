/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/U64Lifo.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//!
//! Construct a duplicate instance of the given queue.
//!
U64Lifo::U64Lifo(const U64Lifo& q):
U64Fifo(q)
{
}


//!
//! Construct an empty queue with initial capacity of capacity items. The
//! queue does not grow if growBy is zero, exponentially grows by doubling
//! if growBy is negative, and grows by growBy items otherwise.
//!
U64Lifo::U64Lifo(unsigned int capacity, int growBy):
U64Fifo(capacity, growBy)
{
}


U64Lifo::~U64Lifo()
{
}


//!
//! Copy the queue contents from given queue. This queue might grow to
//! accomodate the source. If it cannot grow, items at the bottom will
//! be dropped.
//!
const U64Lifo& U64Lifo::operator =(const U64Lifo& q)
{

    // Prevent self assignment.
    if (this == &q)
    {
        return *this;
    }

    // Might need to grow to accomodate source.
    unsigned int minCap = q.numItems();
    if (minCap > capacity())
    {
        if (canGrow())
        {
            reset();
            resize(nextCap(minCap));
        }
    }

    // Copy all items.
    unsigned int curCap = capacity();
    if (minCap <= curCap)
    {
        U64Fifo::operator =(q);
    }

    // Drop bottom items.
    else if (curCap > 0)
    {
        reset();
        for (size_t i = 0; i < curCap; ++i)
        {
            item_t item = q.peek(i);
            add(item);
        }
    }

    // Return reference to self.
    return *this;
}

END_NAMESPACE1
