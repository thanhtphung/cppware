/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_GROWABLE_HPP
#define SYSKIT_GROWABLE_HPP

#include <string.h>
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


#if _WIN32
#pragma pack(push,4)
#endif

//! growable container
class Growable
    //!
    //! An abstract base class for a container which might grow. By default,
    //! most containers do not grow. Use setGrowth() to manage growth. Use
    //! resize() to adjust the container's capacity on demand. Example:
    //!\code
    //! // If growth might have occurred, use initial capacity if possible.
    //! if (growable.canGrow())
    //! {
    //!   unsigned int initialCap = growable.initialCap();
    //!   growable.resize(initialCap);
    //! }
    //!\endcode
    //!
{

public:
    static const unsigned int INVALID_INDEX;

    bool canGrow() const;
    bool canGrow(int& growBy) const;
    int growthFactor() const;
    unsigned int capacity() const;
    unsigned int initialCap() const;

    virtual ~Growable();
    virtual bool resize(unsigned int newCap);
    virtual bool setGrowth(int growBy);

protected:

    // Constructors.
    Growable(const Growable& growable);
    Growable(unsigned int capacity, int growBy = 0);

    // Operators.
    const Growable& operator =(const Growable& growable);

    unsigned int increaseCap(unsigned int delta);
    unsigned int nextCap() const;
    unsigned int nextCap(unsigned int minCap) const;
    unsigned int setNextCap(unsigned int minCap);
    void setCapacity(unsigned int capacity);

    virtual bool grow();

private:
    int growBy_;
    unsigned int capacity_;
    unsigned int initialCap_;

};

#if _WIN32
#pragma pack(pop)
#endif

//! Return true if this container can grow.
inline bool Growable::canGrow() const
{
    return growBy_ != 0;
}

//! Return true if this container can grow. Also return the growBy growth
//! factor. A container does not grow if growBy is zero, exponentially grows
//! by doubling if growBy is negative, and linearly grows by growBy units
//! otherwise.
inline bool Growable::canGrow(int& growBy) const
{
    growBy = growBy_;
    return growBy != 0;
}

//! Return the growBy growth factor. A container does not grow if growBy is zero,
//! exponentially grows by doubling if growBy is negative, and linearly grows by
//! growBy units otherwise.
inline int Growable::growthFactor() const
{
    return growBy_;
}

//! Return the container's capacity.
inline unsigned int Growable::capacity() const
{
    return capacity_;
}

//! Return the container's initial capacity when first constructed.
inline unsigned int Growable::initialCap() const
{
    return initialCap_;
}

//! Increase the container's capacity by delta.
//! Return the new capacity.
inline unsigned int Growable::increaseCap(unsigned int delta)
{
    capacity_ += delta;
    return capacity_;
}

//! The container is growable and needs to grow. Grow enough to be able
//! to handle the given minimal capacity. Given capacity must be greater
//! than current capacity. Return the new capacity.
inline unsigned int Growable::setNextCap(unsigned int minCap)
{
    capacity_ = nextCap(minCap);
    return capacity_;
}

//! Reset the container's capacity to capacity.
inline void Growable::setCapacity(unsigned int capacity)
{
    capacity_ = ((growBy_ < 0) && (capacity == 0))? 1: capacity;
}

END_NAMESPACE1

#include "syskit/win/link-with-syskit.h"
#endif
