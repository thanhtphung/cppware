/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/Growable.hpp"
#include "syskit/macros.h"

const unsigned long long MAX_CAP = 0x00000000ffffffffULL;

BEGIN_NAMESPACE1(syskit)

const unsigned int Growable::INVALID_INDEX = 0xffffffffU;


//!
//! Construct a container with given capacity and growth factor. A container
//! does not grow if growBy is zero, exponentially grows by doubling if growBy
//! is negative, and linearly grows by growBy units otherwise. If the given
//! growth factor is negative and if the given capacity is zero, then one will
//! be used instead.
//!
Growable::Growable(unsigned int capacity, int growBy)
{
    growBy_ = growBy;

    setCapacity(capacity);
    initialCap_ = capacity_;
}


//!
//! Construct a duplicate instance of the given container.
//!
Growable::Growable(const Growable& growable)
{
    growBy_ = growable.growBy_;

    capacity_ = growable.capacity_;
    initialCap_ = growable.capacity_;
}


//!
//! Destruct container.
//!
Growable::~Growable()
{
}


//!
//! Copy everything except the initial capacity from given instance.
//!
const Growable& Growable::operator =(const Growable& growable)
{

    // Prevent self assignment.
    // Do not copy initial capacity.
    if (this != &growable)
    {
        growBy_ = growable.growBy_;
        capacity_ = growable.capacity_;
    }

    // Return reference to self.
    return *this;
}


//!
//! Default implementation. Grow if growable. Return true if grown.
//!
bool Growable::grow()
{
    bool grown = false;
    if (canGrow())
    {
        unsigned int newCap = nextCap();
        if (newCap > capacity_)
        {
            grown = resize(newCap);
        }
    }

    return grown;
}


//!
//! Default implementation. By default, a container cannot be resized. Return
//! false to indicate failure. In most cases, this implementation is not useful,
//! and the derived class should provide its own implementation.
//!
bool Growable::resize(unsigned int /*newCap*/)
{
    return false;
}


//!
//! Manage growth. A container does not grow if growBy is zero, exponentially
//! grows by doubling if growBy is negative, and linearly grows by growBy units
//! otherwise. Return true if successful. Return false otherwise (this collection
//! prohibits growth or this collection has zero-capacity and cannot grow by
//! doubling).
//!
bool Growable::setGrowth(int growBy)
{
    return ((growBy >= 0) || capacity_)? ((growBy_ = growBy), true): (false);
}


//!
//! The container is growable and needs to grow. Compute and return
//! the next capacity if growth occurs. Return zero if container is
//! too big.
//!
unsigned int Growable::nextCap() const
{
    unsigned long long newCap = capacity_;
    (growBy_ < 0)? (newCap <<= 1ULL): (newCap += growBy_);
    return (newCap <= MAX_CAP)? static_cast<unsigned int>(newCap): 0;
}


//!
//! The container is growable and needs to grow to or beyond the given minimal
//! capacity. Compute and return the required capacity if growth occurs. Return
//! zero if container is too big.
//!
unsigned int Growable::nextCap(unsigned int minCap) const
{
    unsigned int nextCap = capacity_;
    if (minCap > nextCap)
    {
        unsigned long long newCap = nextCap;
        if (growBy_ < 0)
        {
            while ((newCap <<= 1) < minCap);
        }
        else
        {
            newCap += ((minCap - capacity_ - 1ULL) / growBy_ + 1ULL) * growBy_;
        }
        nextCap = (newCap <= MAX_CAP)? static_cast<unsigned int>(newCap): 0;
    }

    // Return the required capacity if growth occurs.
    return nextCap;
}

END_NAMESPACE1
