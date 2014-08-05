/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <string.h>

#include "syskit-pch.h"
#include "syskit/Bst.hpp"
#include "syskit/Heap.hpp"
#include "syskit/Vec.hpp"
#include "syskit/macros.h"

const unsigned int INVALID_CAP = 0xffffffffU;

BEGIN_NAMESPACE1(syskit)


//!
//! Take over ownership of given raw table. The raw table holds numItems opaque items
//! already in order. Delete the raw table using the delete[] operator when appropriate.
//! This instance is constructed with initial capacity of numItems. The table does not
//! grow if growBy is zero, grows by doubling if growBy is negative, and grows by growBy
//! items otherwise. When items are compared, the given comparison function will be used.
//! A primitive comparison function comparing opaque items by their values will be used
//! if compare is zero.
//!
Bst::Bst(compare_t compare, item_t* item, size_t numItems, int growBy):
Growable(static_cast<unsigned int>(numItems), growBy)
{
    compare_ = (compare == 0)? Bst::compare: compare;
    item_ = item;
    numItems_ = static_cast<unsigned int>(numItems);
}


//!
//! Construct an empty table with initial capacity of capacity items. The table does not
//! grow if growBy is zero, grows by doubling if growBy is negative, and grows by growBy
//! items otherwise. When items are compared, the given comparison function will be used.
//! A primitive comparison function comparing opaque items by their values will be used
//! if compare is zero.
//!
Bst::Bst(compare_t compare, unsigned int capacity, int growBy):
Growable(capacity, growBy)
{

    // Allocate all items now. Initialize each item when used.
    compare_ = (compare == 0)? Bst::compare: compare;
    item_ = new item_t[Bst::capacity()];
    numItems_ = 0;
}


//!
//! Construct a duplicate instance of the given table.
//!
Bst::Bst(const Bst& bst):
Growable(bst)
{

    // Allocate all items now. Initialize each item when used.
    compare_ = bst.compare_;
    item_ = new item_t[capacity()];
    numItems_ = bst.numItems_;

    // Copy the utilized items only.
    memcpy(item_, bst.item_, numItems_ * sizeof(*item_));
}


//!
//! Construct instance from the given unsorted vector using heap sort. When items are
//! compared, the given comparison function will be used. A primitive comparison function
//! comparing opaque items by their values will be used if compare is zero.
//!
Bst::Bst(const Vec& vec, compare_t compare):
Growable(vec)
{

    // Allocate all items now. Initialize each item when used.
    compare_ = (compare == 0)? Bst::compare: compare;
    item_ = new item_t[capacity()];
    numItems_ = 0;

    // Copy the utilized items only.
    copyFrom(vec.raw(), vec.numItems());
}


Bst::~Bst()
{
    delete[] item_;
}


//!
//! Return true if this table equals given table.
//! That is, if both have the same number of entries
//! and if corresponding entries are equal.
//!
bool Bst::operator ==(const Bst& bst) const
{
    bool answer;
    if (numItems_ != bst.numItems_)
    {
        answer = false;
    }

    else
    {
        answer = true;
        void* const* p0 = item_;
        void* const* pEnd = p0 + numItems_;
        for (void* const* p1 = bst.item_; p0 < pEnd; ++p0, ++p1)
        {
            if (compare_(*p0, *p1) != 0)
            {
                answer = false;
                break;
            }
        }
    }

    return answer;
}


//!
//! Copy the table contents from given table. This table might
//! grow to accomodate the source. If it cannot grow, items at
//! the tail end from the source table will be dropped.
//!
const Bst& Bst::operator =(const Bst& bst)
{

    // Prevent self assignment.
    if (this == &bst)
    {
        return *this;
    }

    // Might need to grow to accomodate source.
    // Might need to truncate if not growable.
    unsigned int curCap = capacity();
    unsigned int minCap = bst.numItems_;
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

    // Don't do much if source table is empty or if this table has zero capacity.
    if (minCap == 0)
    {
        numItems_ = minCap;
    }

    // Copy items by copying memory if the comparison functions are identical.
    else if (compare_ == bst.compare_)
    {
        numItems_ = minCap;
        memcpy(item_, bst.item_, numItems_ * sizeof(*item_));
    }

    // Copy items, then sort.
    else
    {
        reset();
        copyFrom(bst.item_, minCap);
    }

    // Return reference to self.
    return *this;
}


//!
//! Copy the vector contents from given unsorted vector using
//! heap sort. This table might grow to accomodate the source.
//! If it cannot grow, items at the tail end will be dropped.
//!
const Bst& Bst::operator =(const Vec& vec)
{

    // Might need to grow to accomodate source.
    // Might need to truncate if not growable.
    unsigned int curCap = capacity();
    unsigned int minCap = vec.numItems();
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
    copyFrom(vec.raw(), minCap);

    // Return reference to self.
    return *this;
}


//!
//! Detach raw table from instance. The returned table is allocated
//! from the heap and is to be freed by the caller using the delete[]
//! operator when done. Instance is now disabled and must not be used
//! further.
//!
Bst::item_t* Bst::detachRaw()
{
    item_t* raw = item_;
    item_ = 0;
    numItems_ = 0;
    setCapacity(INVALID_CAP);
    return raw;
}


//!
//! Add given item to the table. Return true if successful (also set
//! addedAtIndex to the index of the added item). Return false
//! otherwise (i.e., table is full or item already exists). Use
//! allowDuplicates to specify uniqueness requirement.
//!
bool Bst::add(item_t item, size_t& addedAtIndex, bool allowDuplicates)
{

    // Is table full?
    bool ok;
    size_t index;
    if ((numItems_ == capacity()) && (!grow()))
    {
        ok = false;
    }

    // Does item already exist?
    else if (find(item, index) && (!allowDuplicates))
    {
        ok = false;
    }

    // Add new item.
    else
    {

        // Current table is not empty. Might need to shift bottom part down.
        ok = true;
        if (numItems_)
        {
            if (compare_(item, item_[index]) > 0)
            {
                ++index;
            }
            memmove(item_ + index + 1, item_ + index, (numItems_ - index) * sizeof(*item_));
        }

        // Current table is empty. New item is added at index 0.
        else
        {
            index = 0;
        }

        // Add new item at the determined index.
        addedAtIndex = index;
        item_[index] = item;
        ++numItems_;
    }

    // Return true if successful.
    return ok;
}


//!
//! Locate given item using binary search. Use given compatible comparison function.
//! Return true if found (also set foundIndex to the index of the located item).
//! Return false otherwise (also set foundIndex to the nearest item).
//!
bool Bst::find(const void* item, compare_t compare, size_t& foundIndex) const
{

    // Assume not found.
    bool found = false;

    // Locate item.
    int loI = 0;
    int hiI = numItems_ - 1;
    unsigned int midI = 0;
    while (loI <= hiI)
    {
        midI = (loI + hiI) >> 1;
        int rc = compare(item, item_[midI]);
        if (rc < 0)
        {
            hiI = midI - 1; //look in lower half
        }
        else if (rc > 0)
        {
            loI = midI + 1; //look in upper half
        }
        else
        {
            found = true;
            break;
        }
    }

    // Return true if found. Return false otherwise.
    foundIndex = midI;
    return found;
}


//!
//! Resize table. Given new capacity must not be less than the current table
//! size. Return true if successful.
//!
bool Bst::resize(unsigned int newCap)
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
//! Locate given item using binary search. Use given compatible comparison function.
//! If found, remove it from the table and return true (also set removedItem to the
//! removed item). Return false otherwise.
//!
bool Bst::rm(const void* item, compare_t compare, item_t& removedItem)
{

    // Assume not found.
    bool found = false;

    // Locate and remove given item.
    int loI = 0;
    int hiI = numItems_ - 1;
    while (loI <= hiI)
    {
        unsigned int midI = (loI + hiI) >> 1;
        int rc = compare(item, item_[midI]);
        if (rc < 0)
        {
            hiI = midI - 1; //look in lower half
        }
        else if (rc > 0)
        {
            loI = midI + 1; //look in upper half
        }
        else
        {
            found = true;
            removedItem = item_[midI];
            if (midI < --numItems_)
            {
                memmove(item_ + midI, item_ + midI + 1, (numItems_ - midI) * sizeof(*item_));
            }
            break;
        }
    }

    // Return true if found. Return false otherwise.
    return found;
}


//!
//! Remove item at given index. If given index is valid, return true
//! (also set removedItem to the removed item). Return false
//! otherwise.
//!
bool Bst::rmFromIndex(size_t index, item_t& removedItem)
{

    // Make sure given index is valid.
    bool ok;
    if (index >= numItems_)
    {
        ok = false;
    }

    // Shift items up.
    else
    {
        ok = true;
        removedItem = item_[index];
        if (index < --numItems_)
        {
            memmove(item_ + index, item_ + index + 1, (numItems_ - index) * sizeof(*item_));
        }
    }

    // Return true if successful.
    return ok;
}


//
// Primitive comparison function comparing opaque items by their values.
// Return a negative value if item0<item1, a positive value if item 0>item 1, and zero otherwise.
//
int Bst::compare(const void* item0, const void* item1)
{
    return (item0 < item1)? (-1): ((item0>item1)? 1: 0);
}


//
// Copy numItems from given vector using heap sort.
//
void Bst::copyFrom(void* const* raw, size_t numItems)
{
    int growBy = 0;
    unsigned int capacity = static_cast<unsigned int>(numItems);
    Heap heap(compare_, capacity, growBy);

    void* const* p = raw;
    void* const* pEnd = p + numItems;
    for (; p < pEnd; heap.add(*p++));

    for (item_t* p = item_ + numItems - 1; heap.rm(*p); --p);
    numItems_ = static_cast<unsigned int>(numItems);
}

END_NAMESPACE1
