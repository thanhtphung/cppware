/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <string.h>

#include "syskit-pch.h"
#include "syskit/Heap.hpp"
#include "syskit/Vec.hpp"
#include "syskit/macros.h"

const unsigned int INVALID_CAP = 0xffffffffU;

BEGIN_NAMESPACE1(syskit)


//!
//! Construct instance using guts from that. That is, move vector contents from
//! that into this. That instance is now disabled and must not be used further.
//!
Vec::Vec(Vec* that):
Growable(*that)
{
    item_ = that->detachRaw(numItems_);
}


//!
//! Construct a duplicate instance of the given vector.
//!
Vec::Vec(const Vec& vec):
Growable(vec)
{
    item_ = new item_t[capacity()];
    numItems_ = vec.numItems_;

    // Copy the utilized items only. Don't care about the
    // unused ones. Unused items are initialized when used.
    memcpy(item_, vec.item_, numItems_ * sizeof(*item_));
}


//!
//! Construct instance with given subset (itemCount items starting at startAt).
//!
Vec::Vec(const Vec& vec, size_t startAt, size_t itemCount):
Growable(vec)
{
    item_ = new item_t[capacity()];
    numItems_ = static_cast<unsigned int>(itemCount);

    // Copy the utilized items only. Don't care about the
    // unused ones. Unused items are initialized when used.
    memcpy(item_, vec.item_ + startAt, numItems_ * sizeof(*item_));
}


//!
//! Construct an empty vector with initial capacity of capacity items.
//! The vector does not grow if growBy is zero, exponentially grows by
//! doubling if growBy is negative, and grows by growBy items otherwise.
//!
Vec::Vec(unsigned int capacity, int growBy):
Growable(capacity, growBy)
{

    // Allocate all items. Initialize each item when used.
    item_ = new item_t[Vec::capacity()];
    numItems_ = 0;
}


Vec::~Vec()
{
    delete[] item_;
}


//!
//! Reset and move the vector contents including guts from that into this. That
//! instance is now disabled and must not be used further.
//!
const Vec& Vec::operator =(Vec* that)
{

    // No-op if operating against the same tree.
    if (this != that)
    {
        delete[] item_;
        Growable::operator =(*that);
        item_ = that->detachRaw(numItems_);
    }

    // Return reference to self.
    return *this;
}


//!
//! Copy the vector contents from given vector. This vector might grow to
//! accomodate the source. If it cannot grow, items at the tail end will
//! be dropped.
//!
const Vec& Vec::operator =(const Vec& vec)
{

    // Prevent self assignment.
    if (this == &vec)
    {
        return *this;
    }

    // Might need to grow to accomodate source.
    // Might need to truncate if not growable.
    unsigned int curCap = capacity();
    unsigned int minCap = vec.numItems_;
    if (minCap > curCap)
    {
        if (canGrow())
        {
            delete[] item_;
            item_ = new item_t[setNextCap(minCap)];
        }
        else
        {
            minCap = curCap; //truncate copy
        }
    }

    // Initialize used items.
    memcpy(item_, vec.item_, minCap * sizeof(*item_));
    numItems_ = minCap;

    // Return reference to self.
    return *this;
}


//!
//! Find and return the kth smallest item. Given items can be arbitrarily rearranged.
//! Behavior is unpredictable if k is not less than the number of items. Use given
//! comparison function.
//!
Vec::item_t Vec::findKthSmallest(item_t* item, size_t itemCount, size_t k, compare_t compare)
{
    for (size_t left = 0, right = itemCount - 1; left < right;)
    {
        item_t kthItem = item[k];
        size_t i = left;
        size_t j = right;
        do
        {
            for (; compare(item[i], kthItem) < 0; ++i);
            for (; compare(item[j], kthItem) > 0; --j);
            if (i <= j)
            {
                item_t tmp = item[i];
                item[i++] = item[j];
                item[j--] = tmp;
            }
        } while (i <= j);
        if (j < k) left = i;
        if (i > k) right = j;
    }

    return item[k];
}


//!
//! Detach raw vector from instance. The returned vector is allocated
//! from the heap and is to be freed by the caller using the delete[]
//! operator when done. Instance is now disabled and must not be used
//! further.
//!
Vec::item_t* Vec::detachRaw()
{
    item_t* raw = item_;
    item_ = 0;
    numItems_ = 0;
    setCapacity(INVALID_CAP);
    return raw;
}


//!
//! Add given item to the tail end. Return true if successful (i.e.,
//! vector is not full). Return false otherwise.
//!
bool Vec::add(item_t item)
{

    // Vector is not full.
    bool ok;
    if ((numItems_ < capacity()) || grow())
    {
        item_[numItems_++] = item;
        ok = true;
    }

    // Vector is full.
    else
    {
        ok = false;
    }

    // return true if successful.
    return ok;
}


//!
//! Add given item at given index. If maintainOrder is true, items are shifted
//! down. Otherwise, the existing item at given index, if any, moves to the end.
//! Return true if successful (i.e., vector is not full). Return false otherwise.
//!
bool Vec::addAtIndex(size_t index, item_t item, bool maintainOrder)
{

    // Vector is not full.
    bool ok;
    if ((numItems_ < capacity()) || grow())
    {
        if (index < numItems_)
        {
            if (maintainOrder)
            {
                memmove(item_ + index + 1, item_ + index, (numItems_ - index) * sizeof(*item_));
            }
            else
            {
                item_[numItems_] = item_[index];
            }
        }
        item_[index] = item;
        ++numItems_;
        ok = true;
    }

    // Vector is full.
    else
    {
        ok = false;
    }

    // Return true if successful.
    return ok;
}


//!
//! Return true if this vector equals given vector. That is, if both
//! have the same number of items and corresponding items are identical.
//!
bool Vec::equals(const Vec& vec, diff_t diff) const
{
    bool eq;
    if (numItems_ != vec.numItems_)
    {
        eq = false;
    }
    else
    {
        eq = true;
        const void* const* p0 = item_;
        const void* const* p1 = vec.item_;
        for (size_t i = 0; i < numItems_; ++i)
        {
            if (diff(p0[i], p1[i]) != 0)
            {
                eq = false;
                break;
            }
        }
    }

    return eq;
}


//!
//! Locate given item using linear search. Return true if found (also return
//! the located index in foundIndex). Return false and INVALID_INDEX otherwise.
//!
bool Vec::find(const void* item, diff_t diff, size_t& foundIndex) const
{
    bool found = false;
    void* const* p = item_;
    void* const* pEnd = p + numItems_;
    for (foundIndex = INVALID_INDEX; p < pEnd; ++p)
    {
        if (diff(item, *p) == 0)
        {
            found = true;
            foundIndex = p - item_;
            break;
        }
    }

    return found;
}


//!
//! Locate given item using linear search. Return true if found (also return
//! the located index in foundIndex). Return false and INVALID_INDEX otherwise.
//!
bool Vec::find(const void* item, size_t& foundIndex) const
{
    bool found = false;
    void* const* p = item_;
    void* const* pEnd = p + numItems_;
    for (foundIndex = INVALID_INDEX; p < pEnd; ++p)
    {
        if (item == *p)
        {
            found = true;
            foundIndex = p - item_;
            break;
        }
    }

    return found;
}


//!
//! Resize vector. Given new capacity must not be less than the current vector
//! size. Return true if successful.
//!
bool Vec::resize(unsigned int newCap)
{
    bool ok;
    if (numItems_ > newCap)
    {
        ok = false;
    }

    else
    {
        ok = true;
        if (newCap != capacity())
        {
            item_t* item = new item_t[newCap];
            memcpy(item, item_, numItems_ * sizeof(*item_));
            delete[] item_;
            item_ = item;
            setCapacity(newCap);
        }
    }

    return ok;
}


//!
//! Locate and remove given item using linear search. Return true if found.
//! Return false otherwise. Item removal might affect vector ordering. If
//! maintainOrder is true, appropriate items would be shifted up. Otherwise,
//! the last item would move up to replace the removed item.
//!
bool Vec::rm(const void* item, bool maintainOrder)
{
    size_t foundIndex;
    bool found = find(item, foundIndex);
    if (found)
    {
        item_t removedItem;
        rmFromIndex(foundIndex, removedItem, maintainOrder);
    }

    return found;
}


//!
//! Locate and remove given item using linear search. Return true if found
//! (also return the removed item). Return false otherwise. Item removal
//! might affect vector ordering. If maintainOrder is true, appropriate
//! items would be shifted up. Otherwise, the last item would move up to
//! replace the removed item.
//!
bool Vec::rm(const void* item, diff_t diff, item_t& removedItem, bool maintainOrder)
{
    size_t foundIndex;
    bool found = find(item, diff, foundIndex);
    if (found)
    {
        rmFromIndex(foundIndex, removedItem, maintainOrder);
    }

    return found;
}


//!
//! Remove item from given index. If given index is valid, return true (also set
//! removedItem to the removed item). Return false otherwise. If maintainOrder is
//! true, appropriate items will be shifted up. Otherwise, the last item will
//! move up to replace the removed item.
//!
bool Vec::rmFromIndex(size_t index, item_t& removedItem, bool maintainOrder)
{

    // Shift items up if order is to be maintained. Move just the
    // last item up if order does not have to be maintained.
    bool ok;
    if (index < numItems_)
    {
        removedItem = item_[index];
        if (index < --numItems_)
        {
            if (maintainOrder)
            {
                memmove(item_ + index, item_ + index + 1, (numItems_ - index) * sizeof(*item_));
            }
            else
            {
                item_[index] = item_[numItems_];
            }
        }
        ok = true;
    }

    // Given index is valid.
    else
    {
        ok = false;
    }

    // Return true if successful.
    return ok;
}


//!
//! Remove given items. If given indices are valid, return true. Return false
//! otherwise. If maintainOrder is true, appropriate items will be shifted up.
//! Otherwise, the last items will move up to replace the removed items.
//!
bool Vec::rmFromIndex(size_t startAt, size_t itemCount, bool maintainOrder)
{

    // Shift items up if order is to be maintained. Move just the
    // last item up if order does not have to be maintained.
    bool ok;
    size_t endAt = startAt + itemCount - 1;
    if (endAt < numItems_)
    {
        size_t numItems = numItems_ - itemCount;
        if (numItems > startAt)
        {
            if (maintainOrder)
            {
                memmove(item_ + startAt, item_ + endAt + 1, (numItems - startAt) * sizeof(*item_));
            }
            else
            {
                size_t j = numItems_;
                for (size_t i = startAt, iEnd = (endAt < numItems)? endAt: (numItems - 1); i <= iEnd; item_[i++] = item_[--j]);
            }
        }
        numItems_ = static_cast<unsigned int>(numItems);
        ok = true;
    }

    // Given indices are valid.
    else
    {
        ok = false;
    }

    // Return true if successful.
    return ok;
}


//!
//! Assume given vector (arg.numItems items in arg.raw) is sorted, locate
//! given item (arg.item) using binary search. When items are compared, use
//! the given comparison function (arg.compare). Return true if found (also
//! return the located index in foundIndex). Return false otherwise (also
//! return the nearest item in foundIndex). Behavior is unpredictable if
//! vector is unsorted.
//!
bool Vec::search(const searchArg_t& arg, size_t& foundIndex)
{

    // Extract arguments.
    compare_t compare = arg.compare;
    const void* item = arg.item;
    const void* const* raw = arg.raw;
    size_t numItems = arg.numItems;

    // Assume not found.
    bool found = false;

    // Locate item using binary search.
    int loI = 0;
    int hiI = static_cast<int>(numItems - 1);
    unsigned int midI = 0;
    while (loI <= hiI)
    {
        midI = (loI + hiI) >> 1;
        int rc = compare(item, raw[midI]);
        if (rc > 0)
        {
            loI = midI + 1;
        }
        else if (rc < 0)
        {
            hiI = midI - 1;
        }
        else
        {
            found = true;
            break;
        }
    }

    // Return true if found.
    foundIndex = midI;
    return found;
}


//!
//! Add count entries to the tail end. Use item for the new values.
//! Return number of entries successfully added. This number can be
//! less than count if growth is required but the vector cannot grow.
//!
unsigned int Vec::add(size_t count, item_t item)
{
    if (count == 0)
    {
        unsigned int numAdds = 0;
        return numAdds;
    }

    if (count == 1)
    {
        bool ok = add(item);
        unsigned int numAdds = ok? 1: 0;
        return numAdds;
    }

    // Might need to grow to accomodate new entries.
    // Not all new entries can be accomodated if not growable.
    unsigned int curCap = capacity();
    unsigned int minCap = static_cast<unsigned int>(numItems_ + count);
    if (minCap > curCap)
    {
        if (canGrow())
        {
            resize(minCap);
        }
        else
        {
            count = curCap - numItems_;
        }
    }

    size_t iEnd = numItems_ + count;
    for (size_t i = numItems_; i < iEnd; item_[i++] = item);
    numItems_ = static_cast<unsigned int>(iEnd);
    unsigned int numAdds = static_cast<unsigned int>(count);
    return numAdds;
}


//!
//! Add given items. Return number of items successfully added. This
//! number can be less than the given item count if growth is required
//! but this vector cannot grow.
//!
unsigned int Vec::add(void* const* raw, size_t itemCount)
{
    if (itemCount == 0)
    {
        unsigned int numAdds = 0;
        return numAdds;
    }

    if (itemCount == 1)
    {
        bool ok = add(raw[0]);
        unsigned int numAdds = ok? 1: 0;
        return numAdds;
    }

    // Might need to grow to accomodate new entries.
    // Not all new entries can be accomodated if not growable.
    unsigned int curCap = capacity();
    unsigned int minCap = static_cast<unsigned int>(numItems_ + itemCount);
    if (minCap > curCap)
    {
        if (canGrow())
        {
            resize(minCap);
        }
        else
        {
            itemCount = curCap - numItems_;
        }
    }

    memcpy(item_ + numItems_, raw, itemCount * sizeof(*item_));
    unsigned int numAdds = static_cast<unsigned int>(itemCount);
    numItems_ += numAdds;
    return numAdds;
}


//!
//! Sort vector and save results in sorted. Use given comparison function.
//!
void Vec::sort(compare_t compare, Vec& sorted, bool reverseOrder) const
{
    int growBy = 0;
    Heap heap(compare, numItems_, growBy);

    void* const* p = item_;
    void* const* pEnd = p + numItems_;
    for (; p < pEnd; heap.add(*p++));

    item_t item;
    sorted.reset();
    sorted.resize(numItems_);
    if (reverseOrder)
    {
        for (item_t* p = sorted.item_; heap.rm(item); *p++ = item);
    }
    else
    {
        for (item_t* p = sorted.item_ + numItems_; heap.rm(item); *--p = item);
    }
    sorted.numItems_ = numItems_;
}


//!
//! Sort vector (numItems items in raw) using given comparison function.
//!
void Vec::sort(item_t* raw, size_t numItems, compare_t compare, bool reverseOrder)
{
    int growBy = 0;
    unsigned int capacity = static_cast<unsigned int>(numItems);
    Heap heap(compare, capacity, growBy);

    void* const* p = raw;
    void* const* pEnd = p + numItems;
    for (; p < pEnd; heap.add(*p++));

    item_t item;
    if (reverseOrder)
    {
        for (item_t* p = raw; heap.rm(item); *p++ = item);
    }
    else
    {
        for (item_t* p = raw + numItems; heap.rm(item); *--p = item);
    }
}

END_NAMESPACE1
