/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_REF_VEC_HPP
#define SYSKIT_REF_VEC_HPP

#include "syskit/Vec.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)

class RefCounted;


//! vector of reference-counted items
class RefVec: private Vec
    //!
    //! A class representing an unsorted vector of reference-counted items. The
    //! vector has an initial capacity of capacity() items. The capacity can
    //! grow as needed if the growth factor is set to non-zero when constructed
    //! or afterwards using setGrowth(). If the vector is growable, growth can
    //! occur when items are added. Items are added using various forms of
    //! the add() method and removed using various forms of the rm() method.
    //! Updates can be expensive if many items need to be shifted up or down.
    //! For best update performance, items should be added to the tail end,
    //! and items should be removed without maintaining the current ordering.
    //! In general, upon vector entrance, an item's reference count is incremented;
    //! and upon vector exit, the item's reference count is decremented. Example:
    //!\code
    //! RefVec vec;
    //! vec.add(item0);
    //! vec.addAtIndex(1 /*index*/, item1);
    //! vec.rm(item2, false /*maintainOrder*/);
    //! vec.rm(item3, true /*maintainOrder*/);
    //! vec.rmFromIndex(1 /*index*/, false /*maintainOrder*/);
    //! vec.rmFromIndex(0 /*index*/, true /*maintainOrder*/);
    //!\endcode
    //!
{

public:
    typedef RefCounted* item_t;

    // From Growable.
    using Growable::INVALID_INDEX;
    using Growable::canGrow;
    using Growable::capacity;
    using Growable::growthFactor;
    using Growable::initialCap;

    // From Vec.
    using Vec::DefaultCap;
    using Vec::compare_t;
    using Vec::diff_t;
    using Vec::operator delete;
    using Vec::operator new;
    using Vec::empty;
    using Vec::find;
    using Vec::findIndex;
    using Vec::numItems;
    using Vec::resize;
    using Vec::setGrowth;

    // Constructors.
    RefVec(const RefVec& vec);
    RefVec(const RefVec& vec, size_t startAt, size_t itemCount);
    RefVec(unsigned int capacity = DefaultCap, int growBy = -1);

    // Operators.
    const RefVec& operator =(const RefVec& vec);
    item_t operator [](size_t index) const;

    // Vector operations.
    bool add(item_t item);
    bool addAtIndex(size_t index, item_t item, bool maintainOrder = true);
    bool addIfNotFound(item_t item);
    bool addIfNotFound(item_t item, diff_t diff);
    bool rm(const RefCounted* item, bool maintainOrder = false);
    bool rm(const RefCounted* item, diff_t diff, bool maintainOrder = false);
    bool rm(const RefCounted* item, diff_t diff, item_t& removedItem, bool maintainOrder = false);
    bool rmFromIndex(size_t index, bool maintainOrder = false);
    bool rmFromIndex(size_t index, item_t& removedItem, bool maintainOrder = false);
    bool rmFromIndex(size_t startAt, size_t itemCount, bool maintainOrder = true);
    bool rmTail();
    bool rmTail(item_t& removedItem);
    bool truncate(size_t numItems);
    unsigned int add(const RefVec& vec);
    unsigned int add(const RefVec& vec, size_t startAt, size_t itemCount);
    unsigned int add(size_t count, item_t item);
    unsigned int reset(const RefVec& vec, size_t startAt, size_t itemCount);
    void cloneAll();
    void reset();
    void setItem(size_t index, item_t item);
    void setItem(size_t index, item_t item, item_t& replacedItem);

    // Getters.
    item_t peek(size_t index) const;

    // Sort and search.
    bool search(const RefCounted* item, compare_t compare) const;
    bool search(const RefCounted* item, compare_t compare, size_t& foundIndex) const;
    void sort(compare_t compare, RefVec& sorted, bool reverseOrder = false) const;
    void sort(compare_t compare, bool reverseOrder = false);

    // Override Vec.
    virtual ~RefVec();

private:
    RefCounted* const* raw() const;
    void lockItems() const;
    void lockItems(size_t, size_t) const;
    void unlockItems() const;

};

inline RefCounted* const* RefVec::raw() const
{
    RefCounted* const* p = reinterpret_cast<RefCounted* const*>(Vec::raw());
    return p;
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline RefVec::item_t RefVec::operator [](size_t index) const
{
    item_t p = static_cast<item_t>(Vec::operator [](index));
    return p;
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline RefVec::item_t RefVec::peek(size_t index) const
{
    item_t p = static_cast<item_t>(Vec::peek(index));
    return p;
}

//! Add given item to the tail end only if it's unique. Return true if item
//! was added successful. Return false otherwise (vector is full or item
//! already exists).
inline bool RefVec::addIfNotFound(item_t item)
{
    size_t foundIndex;
    bool ok = find(item, foundIndex)? false: add(item);
    return ok;
}

//! Add given item to the tail end only if it's unique. Use given comparison
//! function to compare items. Return true if item was added successful. Return
//! false otherwise (vector is full or item already exists).
inline bool RefVec::addIfNotFound(item_t item, diff_t diff)
{
    size_t foundIndex;
    bool ok = find(item, diff, foundIndex)? false: add(item);
    return ok;
}

//! Locate and remove given item using linear search. Return true if found.
//! Return false otherwise. Item removal might affect vector ordering. If
//! maintainOrder is true, appropriate items would be shifted up. Otherwise,
//! the last item would move up to replace the removed item.
inline bool RefVec::rm(const RefCounted* item, diff_t diff, bool maintainOrder)
{
    item_t removedItem;
    bool found = rm(item, diff, removedItem, maintainOrder);
    return found;
}

//! Remove item from given index. If given index is valid, return true.
//! Return false otherwise. If maintainOrder is true, appropriate items
//! will be shifted up. Otherwise, the last item will move up to replace
//! the removed item.
inline bool RefVec::rmFromIndex(size_t index, bool maintainOrder)
{
    item_t removedItem;
    bool ok = rmFromIndex(index, removedItem, maintainOrder);
    return ok;
}

//! Remove the item from the tail end. Return true if successful. Return
//! false otherwise (i.e., vector is empty).
inline bool RefVec::rmTail()
{
    item_t removedItem;
    bool ok = rmTail(removedItem);
    return ok;
}

//! Assume vector is sorted, locate given item using binary search.
//! Return true if found. Behavior is unpredictable if vector is unsorted.
inline bool RefVec::search(const RefCounted* item, compare_t compare) const
{
    bool found = Vec::search(item, compare);
    return found;
}

//! Assume vector is sorted, locate given item using binary search.
//! Return true if found (also return the located index in foundIndex).
//! Return false otherwise (also return the nearest item in foundIndex).
//! Behavior is unpredictable if vector is unsorted.
inline bool RefVec::search(const RefCounted* item, compare_t compare, size_t& foundIndex) const
{
    bool found = Vec::search(item, compare, foundIndex);
    return found;
}

//! Replace the item at given index. Don't do any error checking.
//! Behavior is unpredictable if given index is invalid.
inline void RefVec::setItem(size_t index, item_t item)
{
    item_t replacedItem;
    setItem(index, item, replacedItem);
}

//! Sort vector. Use given comparison function.
inline void RefVec::sort(compare_t compare, bool reverseOrder)
{
    Vec::sort(compare, reverseOrder);
}

END_NAMESPACE1

#endif
