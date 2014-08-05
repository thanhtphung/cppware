/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/Lifo.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//!
//! Construct a duplicate instance of the given queue.
//!
Lifo::Lifo(const Lifo& q):
Fifo(q)
{
}


//!
//! Construct an empty queue with initial capacity of capacity items. The
//! queue does not grow if growBy is zero, exponentially grows by doubling
//! if growBy is negative, and grows by growBy items otherwise.
//!
Lifo::Lifo(unsigned int capacity, int growBy):
Fifo(capacity, growBy)
{
}


Lifo::~Lifo()
{
}


//!
//! Copy the queue contents from given queue. This queue might grow to
//! accomodate the source. If it cannot grow, items at the bottom will
//! be dropped.
//!
const Lifo& Lifo::operator =(const Lifo& q)
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
        Fifo::operator =(q);
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
