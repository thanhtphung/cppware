/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/RefCounted.hpp"
#include "syskit/RefVec.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//!
//! Construct a duplicate instance of the given vector. Items are copied
//! by incrementing reference counts. To make deep copies, use cloneAll()
//! afterwards.
//!
RefVec::RefVec(const RefVec& vec):
Vec(vec)
{
    lockItems();
}


//!
//! Construct instance with given subset (itemCount items starting at startAt).
//! Items are copied by incrementing reference counts. To make deep copies, use
//! cloneAll() afterwards.
//!
RefVec::RefVec(const RefVec& vec, size_t startAt, size_t itemCount):
Vec(vec, startAt, itemCount)
{
    lockItems();
}


//!
//! Construct an empty vector with initial capacity of capacity items.
//! The vector does not grow if growBy is zero, exponentially grows by
//! doubling if growBy is negative, and grows by growBy items otherwise.
//!
RefVec::RefVec(unsigned int capacity, int growBy):
Vec(capacity, growBy)
{
}


RefVec::~RefVec()
{
    unlockItems();
}


//!
//! Copy the vector contents from given vector. This vector might grow to
//! accomodate the source. If it cannot grow, items at the tail end will
//! be dropped. Items are copied by incrementing reference counts. To make
//! deep copies, use cloneAll() afterwards.
//!
const RefVec& RefVec::operator =(const RefVec& vec)
{
    if (this != &vec)
    {
        reset();
        Vec::operator =(vec);
        lockItems();
    }

    return *this;
}


//!
//! Add given item to the tail end. Return true if successful (i.e.,
//! vector is not full). Return false otherwise.
//!
bool RefVec::add(item_t item)
{
    item->addRef();
    bool ok = Vec::add(item);
    if (!ok)
    {
        item->rmRef();
    }

    return ok;
}


//!
//! Add given item at given index. If maintainOrder is true, items are shifted
//! down. Otherwise, the existing item at given index, if any, moves to the end.
//! Return true if successful (i.e., vector is not full). Return false otherwise.
//!
bool RefVec::addAtIndex(size_t index, item_t item, bool maintainOrder)
{
    item->addRef();
    bool ok = Vec::addAtIndex(index, item, maintainOrder);
    if (!ok)
    {
        item->rmRef();
    }

    return ok;
}


//!
//! Locate and remove given item using linear search. Return true if found.
//! Return false otherwise. Item removal might affect vector ordering. If
//! maintainOrder is true, appropriate items would be shifted up. Otherwise,
//! the last item would move up to replace the removed item.
//!
bool RefVec::rm(const RefCounted* item, bool maintainOrder)
{
    bool ok = Vec::rm(item, maintainOrder);
    if (ok)
    {
        item->rmRef();
    }

    return ok;
}


//!
//! Locate and remove given item using linear search. Return true if found
//! (also return the removed item). Return false otherwise. Item removal
//! might affect vector ordering. If maintainOrder is true, appropriate
//! items would be shifted up. Otherwise, the last item would move up to
//! replace the removed item.
//!
bool RefVec::rm(const RefCounted* item, diff_t diff, item_t& removedItem, bool maintainOrder)
{
    void* removed;
    bool ok = Vec::rm(item, diff, removed, maintainOrder);
    if (ok)
    {
        item_t p = static_cast<item_t>(removed);
        p->rmRef();
        removedItem = p;
    }

    return ok;
}


//!
//! Remove item from given index. If given index is valid, return true (also set
//! removedItem to the removed item). Return false otherwise. If maintainOrder is
//! true, appropriate items will be shifted up. Otherwise, the last item will
//! move up to replace the removed item.
//!
bool RefVec::rmFromIndex(size_t index, item_t& removedItem, bool maintainOrder)
{
    void* removed;
    bool ok = Vec::rmFromIndex(index, removed, maintainOrder);
    if (ok)
    {
        item_t p = static_cast<item_t>(removed);
        p->rmRef();
        removedItem = p;
    }

    return ok;
}


//!
//! Remove given items. If given indices are valid, return true. Return false
//! otherwise. If maintainOrder is true, appropriate items will be shifted up.
//! Otherwise, the last items will move up to replace the removed items.
//!
bool RefVec::rmFromIndex(size_t startAt, size_t itemCount, bool maintainOrder)
{
    bool ok;
    size_t endAt = startAt + itemCount - 1;
    if (endAt < numItems())
    {
        for (size_t i = startAt; i <= endAt; peek(i++)->rmRef());
        Vec::rmFromIndex(startAt, itemCount, maintainOrder);
        ok = true;
    }
    else
    {
        ok = false;
    }

    return ok;
}


//!
//! Remove the item from the tail end. Return true (also set removedItem to the
//! removed item) if successful. Return false otherwise (i.e., vector is empty).
//!
bool RefVec::rmTail(item_t& removedItem)
{
    void* removed;
    bool ok = Vec::rmTail(removed);
    if (ok)
    {
        removedItem = static_cast<item_t>(removed);
        removedItem->rmRef();
    }

    return ok;
}


//!
//! Truncate vector if it has more than numItems items.
//! Return true if truncation occured.
//!
bool RefVec::truncate(size_t numItems)
{
    const RefCounted* const* p = raw();
    for (size_t i = Vec::numItems(); i > numItems; p[--i]->rmRef());
    bool truncated = Vec::truncate(numItems);
    return truncated;
}


//!
//! Add given items. Return number of items successfully added. This
//! number can be less than the given item count if growth is required
//! but this vector cannot grow.
//!
unsigned int RefVec::add(const RefVec& vec)
{
    size_t startAt = Vec::numItems();
    unsigned int itemCount = Vec::add(vec);
    lockItems(startAt, itemCount);
    return itemCount;
}


//!
//! Add given subset (itemCount items starting at startAt). Return number
//! of items successfully added. This number can be less than itemCount
//! if growth is required but this vector cannot grow.
//!
unsigned int RefVec::add(const RefVec& vec, size_t startAt, size_t itemCount)
{
    size_t i0 = Vec::numItems();
    unsigned int itemCount32 = Vec::add(vec, startAt, itemCount);
    lockItems(i0, itemCount32);
    return itemCount32;
}


//!
//! Add count entries to the tail end. Use item for the new values.
//! Return number of entries successfully added. This number can be
//! less than count if growth is required but the vector cannot grow.
//!
unsigned int RefVec::add(size_t count, item_t item)
{
    size_t startAt = Vec::numItems();
    unsigned int itemCount = Vec::add(count, item);
    lockItems(startAt, itemCount);
    return itemCount;
}


//!
//! Reset with given subset (itemCount items starting at startAt).
//! Return number of items successfully added. This number can be
//! less than itemCount if growth is required but this vector cannot
//! grow.
//!
unsigned int RefVec::reset(const RefVec& vec, size_t startAt, size_t itemCount)
{
    reset();
    unsigned int itemCount32 = Vec::reset(vec, startAt, itemCount);
    lockItems();
    return itemCount32;
}


//!
//! Make deep copies of all members using clone().
//!
void RefVec::cloneAll()
{
    const RefCounted* const* p = raw();
    for (size_t i = 0, numItems = RefVec::numItems(); i < numItems; ++i)
    {
        const RefCounted* item = p[i];
        Vec::setItem(i, item->clone());
        item->rmRef();
    }
}


void RefVec::lockItems() const
{
    const RefCounted* const* p = raw();
    for (size_t i = numItems(); i > 0; p[--i]->addRef());
}


void RefVec::lockItems(size_t startAt, size_t itemCount) const
{
    const RefCounted* const* p = raw();
    for (size_t i = startAt, iEnd = startAt + itemCount; i < iEnd; p[i++]->addRef());
}


//!
//! Reset the vector by removing all items.
//!
void RefVec::reset()
{
    unlockItems();
    Vec::reset();
}


//!
//! Replace the item at given index (also return the replaced item
//! in replacedItem). Don't do any error checking. Behavior is
//! unpredictable if given index is invalid.
//!
void RefVec::setItem(size_t index, item_t item, item_t& replacedItem)
{
    void* replaced;
    item->addRef();
    Vec::setItem(index, item, replaced);
    replacedItem = static_cast<item_t>(replaced);
    replacedItem->rmRef();
}


//!
//! Sort vector and save results in sorted. Use given comparison function.
//!
void RefVec::sort(compare_t compare, RefVec& sorted, bool reverseOrder) const
{
    sorted.reset();
    Vec::sort(compare, sorted, reverseOrder);
    sorted.lockItems();
}


void RefVec::unlockItems() const
{
    const RefCounted* const* p = raw();
    for (size_t i = numItems(); i > 0; p[--i]->rmRef());
}

END_NAMESPACE1
