/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_PADDR_VEC_HPP
#define NETKIT_PADDR_VEC_HPP

#include "syskit/U64Vec.hpp"
#include "syskit/macros.h"

DECLARE_CLASS1(appkit, StringVec)

BEGIN_NAMESPACE1(netkit)

class Paddr;


//! vector of Paddr instances
class PaddrVec: private syskit::U64Vec
{

public:
    typedef Paddr item_t;

    // From Growable.
    using U64Vec::INVALID_INDEX;
    using U64Vec::canGrow;
    using U64Vec::capacity;
    using U64Vec::growthFactor;
    using U64Vec::initialCap;

    // From U64Vec.
    using U64Vec::DefaultCap;
    using U64Vec::add;
    using U64Vec::addAtIndex;
    using U64Vec::addIfNotFound;
    using U64Vec::find;
    using U64Vec::findIndex;
    using U64Vec::numItems;
    using U64Vec::resize;
    using U64Vec::rm;
    using U64Vec::search;
    using U64Vec::setGrowth;
    using U64Vec::truncate;

    // Constructors.
    PaddrVec(const PaddrVec& vec);
    PaddrVec(unsigned int capacity = DefaultCap, int growBy = -1);

    // Operators.
    bool operator !=(const PaddrVec& vec) const;
    bool operator ==(const PaddrVec& vec) const;
    const PaddrVec& operator =(const PaddrVec& vec);
    const item_t& operator [](size_t index) const;

    // Vector operations.
    bool reset(const appkit::StringVec& vec);
    bool rmFromIndex(size_t index, bool maintainOrder = false);
    bool rmFromIndex(size_t index, item_t& removedItem, bool maintainOrder = false);
    bool rmTail();
    bool rmTail(item_t& removedItem);
    unsigned int add(const PaddrVec& vec);
    unsigned int add(const PaddrVec& vec, size_t startAt, size_t itemCount);
    unsigned int add(size_t count, const item_t& item);
    unsigned int reset(const PaddrVec& vec, size_t startAt, size_t itemCount);
    void reset();
    void setItem(size_t index, const item_t& item);
    void setItem(size_t index, const item_t& item, item_t& replacedItem);

    // Getters.
    const item_t& peek(size_t index) const;

    // Sort and search.
    void sort(bool reverseOrder = false);
    void sort(PaddrVec& sorted, bool reverseOrder = false) const;

    virtual ~PaddrVec();

private:
    void toH64();
    void toN64();

};

END_NAMESPACE1

#include "netkit/Paddr.hpp"

BEGIN_NAMESPACE1(netkit)

//! Return true if this vector does not equal given vector.
inline bool PaddrVec::operator !=(const PaddrVec& vec) const
{
    bool ne = U64Vec::operator !=(vec);
    return ne;
}

//! Return true if this vector equals given vector. That is, if both
//! have the same number of items and corresponding items are identical.
inline bool PaddrVec::operator ==(const PaddrVec& vec) const
{
    bool eq = U64Vec::operator ==(vec);
    return eq;
}

//! Copy the vector contents from given vector. This vector might grow to
//! accomodate the source. If it cannot grow, items at the tail end will
//! be dropped.
inline const PaddrVec& PaddrVec::operator =(const PaddrVec& vec)
{
    U64Vec::operator =(vec);
    return *this;
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline const PaddrVec::item_t& PaddrVec::operator [](size_t index) const
{
    const item_t& item = *reinterpret_cast<const item_t*>(U64Vec::raw() + index);
    return item;
}

//! Remove item from given index. Return true if successful (i.e., given
//! index is valid). Return false otherwise. If maintainOrder is true,
//! appropriate items will be shifted up. Otherwise, the last item will
//! move up to replace the removed item.
inline bool PaddrVec::rmFromIndex(size_t index, bool maintainOrder)
{
    bool ok = U64Vec::rmFromIndex(index, maintainOrder);
    return ok;
}

//! Remove item from given index. If given index is valid, return true (also set
//! removedItem to the removed item). Return false otherwise. If maintainOrder is
//! true, appropriate items will be shifted up. Otherwise, the last item will
//! move up to replace the removed item.
inline bool PaddrVec::rmFromIndex(size_t index, item_t& removedItem, bool maintainOrder)
{
    U64Vec::item_t item64;
    bool ok = U64Vec::rmFromIndex(index, item64, maintainOrder)? ((removedItem = item64), true): (false);
    return ok;
}

//! Remove the item from the tail end. Return true if successful. Return
//! false otherwise (i.e., vector is empty).
inline bool PaddrVec::rmTail()
{
    bool ok = U64Vec::rmTail();
    return ok;
}

//! Remove the item from the tail end. Return true (also set removedItem to the
//! removed item) if successful. Return false otherwise (i.e., vector is empty).
inline bool PaddrVec::rmTail(item_t& removedItem)
{
    U64Vec::item_t item64;
    bool ok = U64Vec::rmTail(item64)? ((removedItem = item64), true): (false);
    return ok;
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline const PaddrVec::item_t& PaddrVec::peek(size_t index) const
{
    const item_t& item = *reinterpret_cast<const item_t*>(U64Vec::raw() + index);
    return item;
}

//! Add given items. Return number of items successfully added. This
//! number can be less than the given item count if growth is required
//! but this vector cannot grow.
inline unsigned int PaddrVec::add(const PaddrVec& vec)
{
    unsigned int numAdds = U64Vec::add(vec);
    return numAdds;
}

//! Add given subset (itemCount items starting at startAt). Return number
//! of items successfully added. This number can be less than itemCount
//! if growth is required but this vector cannot grow.
inline unsigned int PaddrVec::add(const PaddrVec& vec, size_t startAt, size_t itemCount)
{
    unsigned int numAdds = U64Vec::add(vec, startAt, itemCount);
    return numAdds;
}

//! Add count entries to the tail end. Use item for the new values.
//! Return number of entries successfully added. This number can be
//! less than count if growth is required but the vector cannot grow.
inline unsigned int PaddrVec::add(size_t count, const item_t& item)
{
    unsigned int numAdds = U64Vec::add(count, item);
    return numAdds;
}

//! Reset with given subset (itemCount items starting at startAt).
//! Return number of items successfully added. This number can be
//! less than itemCount if growth is required but this vector cannot
//! grow.
inline unsigned int PaddrVec::reset(const PaddrVec& vec, size_t startAt, size_t itemCount)
{
    unsigned int numAdds = U64Vec::reset(vec, startAt, itemCount);
    return numAdds;
}

//! Reset the vector by removing all items.
inline void PaddrVec::reset()
{
    U64Vec::reset();
}

//! Replace the item at given index. Don't do any error checking.
//! Behavior is unpredictable if given index is invalid.
inline void PaddrVec::setItem(size_t index, const item_t& item)
{
    U64Vec::setItem(index, item);
}

//! Replace the item at given index (also return the replaced item
//! in replacedItem). Don't do any error checking. Behavior is
//! unpredictable if given index is invalid.
inline void PaddrVec::setItem(size_t index, const item_t& item, item_t& replacedItem)
{
    U64Vec::item_t item64;
    U64Vec::setItem(index, item, item64);
    replacedItem = item64;
}

//! Sort vector and save results in sorted. Order is ascending if reverseOrder
//! is false and is descending if reverseOrder is true.
inline void PaddrVec::sort(PaddrVec& sorted, bool reverseOrder) const
{
    sorted.U64Vec::operator =(*this);
    sorted.sort(reverseOrder);
}

END_NAMESPACE1

#endif
