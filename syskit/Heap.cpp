/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <string.h>

#include "syskit-pch.h"
#include "syskit/Heap.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//!
//! Construct an empty heap with initial capacity of capacity items. The
//! heap does not grow if growBy is zero, exponentially grows by doubling
//! if growBy is negative, and grows by growBy items otherwise. When items
//! are compared, the given comparison function will be used. A primitive
//! comparison function comparing opaque items by their values will be used
//! if compare is zero.
//!
Heap::Heap(compare_t compare, unsigned int capacity, int growBy):
Growable(capacity, growBy)
{

    // Allocate all items. Initialize each item when used.
    compare_ = (compare == 0)? Heap::compare: compare;
    numItems_ = 0;
    item_ = new item_t[Heap::capacity()];

    // Make item_ a one-based array instead of a zero-based array. item_[i/2]
    // refers to the mom of item_[i], for 2 <= i <= numItems_. item_[1] refers
    // to the root node.
    --item_;
}


//!
//! Construct a duplicate instance of the given heap.
//!
Heap::Heap(const Heap& heap):
Growable(heap)
{

    // Allocate all items.
    compare_ = heap.compare_;
    numItems_ = heap.numItems_;
    item_ = new item_t[capacity()];

    // Make item_ a one-based array instead of a zero-based array. item_[i/2]
    // refers to the mom of item_[i], for 2 <= i < numItems_. item_[1] refers
    // to the root node. Copy the utilized items only. Don't care about the
    // unused ones. Unused items are initialized when used.
    memcpy(item_, heap.item_ + 1, numItems_ * sizeof(*item_));
    --item_;
}


//
// Private constructor allowing use of caller's item space.
// Use w/ care as we don't want the destructor to delete this memory.
//
Heap::Heap(compare_t compare, item_t* item, size_t numItems):
Growable(static_cast<unsigned int>(numItems), 0 /*growBy*/)
{

    // Bottom-up heap construction takes linear time.
    // This is a slight performance improvement compared to the top-down ordering.
    compare_ = (compare == 0)? Heap::compare: compare;
    item_ = item - 1;
    numItems_ = static_cast<unsigned int>(numItems);
    for (size_t i = numItems >> 1; i > 0; heapifyDown(i--));
}


Heap::~Heap()
{

    // item_ is one-based.
    delete[](item_ + 1);
}


//!
//! Assignment operator. This heap might grow to accomodate the source.
//! If it cannot grow, smaller items from the source heap will be dropped.
//!
const Heap& Heap::operator =(const Heap& heap)
{

    // Prevent self assignment.
    if (this == &heap)
    {
        return *this;
    }

    // Might need to grow to accomodate source.
    // Might need to truncate if not growable.
    unsigned int curCap = capacity();
    unsigned int minCap = heap.numItems_;
    if (minCap > curCap)
    {
        if (canGrow())
        {
            delete[](item_ + 1);
            item_ = new item_t[setNextCap(minCap)];
            --item_;
        }
        else
        {
            minCap = curCap; //truncate copy
        }
    }

    // Don't do much if source heap is empty or if this heap has zero capacity.
    if (minCap == 0)
    {
        numItems_ = 0;
    }

    // Copy all items by copying memory if the comparison functions are
    // identical and if no truncation is required.
    else if ((minCap == heap.numItems_) && (compare_ == heap.compare_))
    {
        numItems_ = minCap;
        memcpy(item_ + 1, heap.item_ + 1, numItems_ * sizeof(*item_));
    }

    // Copy items one at a time. If source heap has too many items, drop
    // the smaller items.
    else
    {
        Heap tmp(heap);
        item_t item = 0;
        reset();
        for (size_t i = minCap; i > 0; --i)
        {
            tmp.rm(item);
            add(item);
        }
    }

    // Return reference to self.
    return *this;
}


//!
//! Add given item to the heap. Return true if successful.
//!
bool Heap::add(item_t item)
{

    // Is heap full?
    bool ok;
    if ((numItems_ == capacity()) && (!grow()))
    {
        ok = false;
    }

    // Add given item.
    else
    {
        ok = true;
        item_[++numItems_] = item;
        heapifyUp(numItems_);
    }

    // Return true if successful.
    return ok;
}


//!
//! Resize heap. Given new capacity must not be less than the current heap
//! size. Return true if successful.
//!
bool Heap::resize(unsigned int newCap)
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
            memcpy(item, item_ + 1, numItems_ * sizeof(*item_));
            delete[](item_ + 1);
            item_ = --item;
            setCapacity(newCap);
        }
    }

    return ok;
}


//!
//! Remove item at given index. Return true if successful (i.e.,
//! given index is valid).
//!
bool Heap::rmFromIndex(size_t index, item_t& removedItem)
{

    // Exchange with high-index item and heapify.
    bool ok;
    if (index < numItems_)
    {
        ok = true;
        removedItem = item_[++index];
        item_[index] = item_[numItems_--];
        if (index <= numItems_)
        {
            ((index == 1) || (compare_(item_[index], item_[index >> 1]) < 0))?
                heapifyDown(index):
                heapifyUp(index);
        }
    }

    // Invalid index.
    else
    {
        ok = false;
    }

    // Return true if successful.
    return ok;
}


//!
//! Remove the top item. Return true if successful (i.e., heap is no empty).
//!
bool Heap::rm(item_t& topItem)
{

    // Heap is not empty.
    bool ok;
    if (numItems_ > 0)
    {
        ok = true;
        topItem = item_[1];
        item_[1] = item_[numItems_--];
        heapifyDown(1);
    }

    // Heap is empty.
    else
    {
        ok = false;
    }

    // Return true if successful.
    return ok;
}


//
// Primitive comparison function comparing opaque items by their values.
// Return a negative value if item0<item1, a positive value if item 0>item 1, and zero otherwise.
//
int Heap::compare(const void* item0, const void* item1)
{
    return (item0 < item1)? (-1): ((item0>item1)? 1: 0);
}


//
// Start at node i, move downward to restore the heap condition.
//
void Heap::heapifyDown(size_t i)
{
    item_t momV;
    for (size_t mom = i;;)
    {

        // Be more careful with the last mom as there might be only one kid.
        size_t kid = (mom << 1);
        size_t sib = kid + 1;
        if (sib > numItems_)
        {
            if ((kid <= numItems_) && (compare_((momV = item_[mom]), item_[kid]) < 0))
            {
                item_[mom] = item_[kid];
                item_[kid] = momV;
            }
            break;
        }

        // Swap mom with the larger kid and keep going.
        if (compare_(item_[kid], item_[sib]) < 0) kid = sib;
        if (compare_((momV = item_[mom]), item_[kid]) < 0)
        {
            item_[mom] = item_[kid];
            item_[kid] = momV;
            mom = kid;
            continue;
        }

        // Done if both kids are larger than the mom.
        break;
    }
}


//
// Start at node i, move upward to restore the heap condition.
//
void Heap::heapifyUp(size_t i)
{
    for (size_t kid = i;;)
    {
        item_t momV;
        size_t mom = (kid >> 1);
        if ((mom > 0) && (compare_((momV = item_[mom]), item_[kid]) < 0))
        {
            item_[mom] = item_[kid];
            item_[kid] = momV;
            kid = mom;
            continue;
        }
        break;
    }
}


//!
//! Sort given items in-place.
//!
void Heap::sort(item_t* item, size_t itemCount, compare_t compare)
{

    // Construct a temporary heap utilizing the same item array. Then remove
    // all items from the temporary heap and store them back in the same item
    // array so that it is sorted in ascending order.
    Heap heap(compare, item, itemCount);
    for (item_t* dst = item + itemCount - 1; dst >= item; --dst)
    {
        item_t next = *dst;
        *dst = heap.item_[1];
        heap.item_[1] = next;
        --heap.numItems_;
        heap.heapifyDown(1);
    }

    // No-op the destructor.
    heap.item_ = static_cast<item_t*>(0) - 1;
}

END_NAMESPACE1
