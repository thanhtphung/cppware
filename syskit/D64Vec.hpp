/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_D64_VEC_HPP
#define SYSKIT_D64_VEC_HPP

#include "syskit/Growable.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! vector of 64-bit doubles
class D64Vec: public Growable
    //!
    //! A class representing an unsorted vector of 64-bit doubles. The vector
    //! has an initial capacity of capacity() items. The capacity can grow
    //! as needed if the growth factor is set to non-zero when constructed or
    //! afterwards using setGrowth(). If the vector is growable, growth can
    //! occur when items are added. Items are added using various forms of
    //! the add() method and removed using various forms of the rm() method.
    //! Updates can be expensive if many items need to be shifted up or down.
    //! For best update performance, items should be added to the tail end,
    //! and items should be removed without maintaining the current ordering.
    //! Example:
    //!\code
    //! D64Vec vec;
    //! vec.add(item0);
    //! vec.addAtIndex(1 /*index*/, item1, false /*maintainOrder*/);
    //! vec.addAtIndex(0 /*index*/, item2, true /*maintainOrder*/);
    //! vec.rmFromIndex(1 /*index*/, false /*maintainOrder*/);
    //! vec.rmFromIndex(0 /*index*/, true /*maintainOrder*/);
    //!\endcode
    //!
{

public:
    enum
    {
        DefaultCap = 64
    };

    typedef double item_t;

    // Constructors.
    D64Vec(const D64Vec& vec);
    D64Vec(const D64Vec& vec, size_t startAt, size_t itemCount);
    D64Vec(unsigned int capacity = DefaultCap, int growBy = 0);

    // Operators.
    const D64Vec& operator =(const D64Vec& vec);
    bool operator !=(const D64Vec& vec) const;
    bool operator ==(const D64Vec& vec) const;
    item_t operator [](size_t index) const;
    item_t& operator [](size_t index);

    // Vector operations.
    bool add(item_t item);
    bool addAtIndex(size_t index, item_t item, bool maintainOrder = true);
    bool addIfNotFound(item_t item);
    bool find(item_t item) const;
    bool find(item_t item, size_t& foundIndex) const;
    bool rm(item_t item, bool maintainOrder = false);
    bool rmFromIndex(size_t index, bool maintainOrder = false);
    bool rmFromIndex(size_t index, item_t& removedItem, bool maintainOrder = false);
    bool rmTail();
    bool rmTail(item_t& removedItem);
    bool truncate(size_t numItems);
    unsigned int add(const D64Vec& vec);
    unsigned int add(const D64Vec& vec, size_t startAt, size_t itemCount);
    unsigned int add(const item_t* raw, size_t itemCount);
    unsigned int add(size_t count, item_t item);
    unsigned int findIndex(item_t item) const;
    unsigned int reset(const D64Vec& vec, size_t startAt, size_t itemCount);
    void reset();
    void setItem(size_t index, item_t item);
    void setItem(size_t index, item_t item, item_t& replacedItem);

    // Getters.
    const item_t* raw() const;
    item_t peek(size_t index) const;
    item_t* detachRaw();
    item_t* detachRaw(unsigned int& numItems);
    unsigned int numItems() const;

    // Sort and search.
    bool search(item_t item) const;
    bool search(item_t item, size_t& foundIndex) const;
    void sort(bool reverseOrder = false);
    void sort(D64Vec& sorted, bool reverseOrder = false) const;

    // Override Growable.
    virtual ~D64Vec();
    virtual bool resize(unsigned int newCap);

    static item_t findKthSmallest(item_t* item, size_t itemCount, size_t k);

private:
    item_t* item_;
    unsigned int numItems_;

};

END_NAMESPACE1

#include "syskit/D64Heap.hpp"

BEGIN_NAMESPACE1(syskit)

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline D64Vec::item_t D64Vec::operator [](size_t index) const
{
    return item_[index];
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline D64Vec::item_t& D64Vec::operator [](size_t index)
{
    return item_[index];
}

//! Return true if this vector does not equal given vector.
inline bool D64Vec::operator !=(const D64Vec& vec) const
{
    bool ne = (numItems_ != vec.numItems_) || (memcmp(item_, vec.item_, numItems_ * sizeof(*item_)) != 0);
    return ne;
}

//! Return true if this vector equals given vector. That is, if both
//! have the same number of items and corresponding items are identical.
inline bool D64Vec::operator ==(const D64Vec& vec) const
{
    bool eq = (numItems_ == vec.numItems_) && (memcmp(item_, vec.item_, numItems_ * sizeof(*item_)) == 0);
    return eq;
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline D64Vec::item_t D64Vec::peek(size_t index) const
{
    return item_[index];
}

//! Detach raw vector from instance. The returned vector is allocated
//! from the heap and is to be freed by the caller using the delete[]
//! operator when done. Instance is now disabled and must not be used
//! further.
inline D64Vec::item_t* D64Vec::detachRaw(unsigned int& numItems)
{
    numItems = numItems_;
    item_t* raw = detachRaw();
    return raw;
}

//! Add given item to the tail end only if it's unique. Return true if
//! item was added successful. Return false otherwise (vector is full
//! or item already exists).
inline bool D64Vec::addIfNotFound(item_t item)
{
    size_t foundIndex;
    bool ok = find(item, foundIndex)? false: add(item);
    return ok;
}

//! Locate given item using linear search. Return true if found.
inline bool D64Vec::find(item_t item) const
{
    size_t foundIndex;
    bool found = find(item, foundIndex);
    return found;
}

//! Remove item from given index. Return true if successful (i.e., given
//! index is valid). Return false otherwise. If maintainOrder is true,
//! appropriate items will be shifted up. Otherwise, the last item will
//! move up to replace the removed item.
inline bool D64Vec::rmFromIndex(size_t index, bool maintainOrder)
{
    item_t removedItem;
    bool ok = rmFromIndex(index, removedItem, maintainOrder);
    return ok;
}

//! Remove the item from the tail end. Return true if successful. Return
//! false otherwise (i.e., vector is empty).
inline bool D64Vec::rmTail()
{
    bool ok = (numItems_ > 0)? (--numItems_, true): false;
    return ok;
}

//! Remove the item from the tail end. Return true (also set removedItem to the
//! removed item) if successful. Return false otherwise (i.e., vector is empty).
inline bool D64Vec::rmTail(item_t& removedItem)
{
    bool ok = (numItems_ > 0)? ((removedItem = item_[--numItems_]), true): false;
    return ok;
}

//! Assume vector is sorted, locate given item using binary search.
//! Return true if found. Behavior is unpredictable if vector is unsorted.
inline bool D64Vec::search(item_t item) const
{
    size_t foundIndex;
    bool found = search(item, foundIndex);
    return found;
}

//! Truncate vector if it has more than numItems items.
//! Return true if truncation occured.
inline bool D64Vec::truncate(size_t numItems)
{
    bool truncated = (numItems_ > numItems)? ((numItems_ = static_cast<unsigned int>(numItems)), true): (false);
    return truncated;
}

//! Return the internal raw vector. To be used with extra care.
inline const D64Vec::item_t* D64Vec::raw() const
{
    return item_;
}

//! Add given items. Return number of items successfully added. This
//! number can be less than the given item count if growth is required
//! but this vector cannot grow.
inline unsigned int D64Vec::add(const D64Vec& vec)
{
    unsigned int numAdds = add(vec.item_, vec.numItems_);
    return numAdds;
}

//! Add given subset (itemCount items starting at startAt). Return number
//! of items successfully added. This number can be less than itemCount
//! if growth is required but this vector cannot grow.
inline unsigned int D64Vec::add(const D64Vec& vec, size_t startAt, size_t itemCount)
{
    unsigned int numAdds = add(vec.item_ + startAt, itemCount);
    return numAdds;
}

//! Locate the index of given item using linear search. Return located
//! index if found. Return INVALID_INDEX otherwise.
inline unsigned int D64Vec::findIndex(item_t item) const
{
    size_t foundIndex;
    find(item, foundIndex);
    return static_cast<unsigned int>(foundIndex);
}

//! Return the current number of items in the vector.
inline unsigned int D64Vec::numItems() const
{
    return numItems_;
}

//! Reset with given subset (itemCount items starting at startAt).
//! Return number of items successfully added. This number can be
//! less than itemCount if growth is required but this vector cannot
//! grow.
inline unsigned int D64Vec::reset(const D64Vec& vec, size_t startAt, size_t itemCount)
{
    numItems_ = 0;
    unsigned int numAdds = add(vec.item_ + startAt, itemCount);
    return numAdds;
}

//! Reset the vector by removing all items.
inline void D64Vec::reset()
{
    numItems_ = 0;
}

//! Replace the item at given index. Don't do any error checking.
//! Behavior is unpredictable if given index is invalid.
inline void D64Vec::setItem(size_t index, item_t item)
{
    item_[index] = item;
}

//! Replace the item at given index (also return the replaced item
//! in replacedItem). Don't do any error checking. Behavior is
//! unpredictable if given index is invalid.
inline void D64Vec::setItem(size_t index, item_t item, item_t& replacedItem)
{
    replacedItem = item_[index];
    item_[index] = item;
}

//! Sort vector. Order is ascending if reverseOrder is false and is descending
//! if reverseOrder is true.
inline void D64Vec::sort(bool reverseOrder)
{
    D64Heap::sort(item_, numItems_, reverseOrder);
}

//! Sort vector and save results in sorted. Order is ascending if reverseOrder
//! is false and is descending if reverseOrder is true.
inline void D64Vec::sort(D64Vec& sorted, bool reverseOrder) const
{
    sorted = *this;
    D64Heap::sort(sorted.item_, sorted.numItems_, reverseOrder);
}

END_NAMESPACE1

#endif
