/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <string.h>

#include "syskit-pch.h"
#include "syskit/F32Vec.hpp"
#include "syskit/macros.h"

const unsigned int INVALID_CAP = 0xffffffffU;

BEGIN_NAMESPACE1(syskit)


//!
//! Construct a duplicate instance of the given vector.
//!
F32Vec::F32Vec(const F32Vec& vec):
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
F32Vec::F32Vec(const F32Vec& vec, size_t startAt, size_t itemCount):
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
F32Vec::F32Vec(unsigned int capacity, int growBy):
Growable(capacity, growBy)
{

    // Allocate all items. Initialize each item when used.
    item_ = new item_t[F32Vec::capacity()];
    numItems_ = 0;
}


F32Vec::~F32Vec()
{
    delete[] item_;
}


//!
//! Copy the vector contents from given vector. This vector might grow to
//! accomodate the source. If it cannot grow, items at the tail end will
//! be dropped.
//!
const F32Vec& F32Vec::operator =(const F32Vec& vec)
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
//! Behavior is unpredictable if k is not less than the number of items. The original
//! code is in public domain (algorithm by N. Wirth, implementation by N. Devillard).
//!
F32Vec::item_t F32Vec::findKthSmallest(item_t* item, size_t itemCount, size_t k)
{
    for (size_t left = 0, right = itemCount - 1; left < right;)
    {
        item_t kthItem = item[k];
        size_t i = left;
        size_t j = right;
        do
        {
            for (; item[i] < kthItem; ++i);
            for (; item[j] > kthItem; --j);
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
F32Vec::item_t* F32Vec::detachRaw()
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
bool F32Vec::add(item_t item)
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
bool F32Vec::addAtIndex(size_t index, item_t item, bool maintainOrder)
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
//! Locate given item using linear search. Return true if found (also return
//! the located index in foundIndex). Return false and INVALID_INDEX otherwise.
//!
bool F32Vec::find(item_t item, size_t& foundIndex) const
{
    bool found = false;
    const item_t* p = item_;
    const item_t* pEnd = p + numItems_;
    for (foundIndex = INVALID_INDEX; p < pEnd; ++p)
    {
        if (*p == item)
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
bool F32Vec::resize(unsigned int newCap)
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
bool F32Vec::rm(item_t item, bool maintainOrder)
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
//! Remove item from given index. If given index is valid, return true (also set
//! removedItem to the removed item). Return false otherwise. If maintainOrder is
//! true, appropriate items will be shifted up. Otherwise, the last item will
//! move up to replace the removed item.
//!
bool F32Vec::rmFromIndex(size_t index, item_t& removedItem, bool maintainOrder)
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
//! Assume vector is sorted, locate given item using binary search.
//! Return true if found (also return the located index in foundIndex).
//! Return false otherwise (also return the nearest item in foundIndex).
//! Behavior is unpredictable if vector is unsorted.
//!
bool F32Vec::search(item_t item, size_t& foundIndex) const
{

    // Assume not found.
    bool found = false;

    // Locate item using binary search.
    int loI = 0;
    int hiI = static_cast<int>(numItems_ - 1);
    unsigned int midI = 0;
    while (loI <= hiI)
    {
        midI = (loI + hiI) >> 1;
        if (item > item_[midI])
        {
            loI = midI + 1;
        }
        else if (item < item_[midI])
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
unsigned int F32Vec::add(size_t count, item_t item)
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
unsigned int F32Vec::add(const item_t* raw, size_t itemCount)
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

END_NAMESPACE1
