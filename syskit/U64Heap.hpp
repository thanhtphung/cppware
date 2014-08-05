/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_U64_HEAP_HPP
#define SYSKIT_U64_HEAP_HPP

#include "syskit/Growable.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! heap of 64-bit numbers
class U64Heap: public Growable
    //!
    //! A class representing a heap of 64-bit numbers. In a heap, the item with
    //! the largest key resides at the top. The heap has an initial capacity of
    //! capacity() items. The capacity can grow as needed if the growth factor
    //! is set to non-zero when constructed or afterwards using setGrowth(). If
    //! the heap is growable, growth can occur when items are added. This heap
    //! supports two basic operations: insert item into heap, and remove the top
    //! item.
    //!
{

public:
    enum
    {
        DefaultCap = 256
    };

    typedef unsigned long long item_t;

    // Constructors.
    U64Heap(const U64Heap& heap);
    U64Heap(unsigned int capacity = DefaultCap, int growBy = 0);

    // Operators.
    const U64Heap& operator =(const U64Heap& heap);

    // Heap management.
    bool add(item_t item);
    bool rm(item_t& topItem);
    bool rmFromIndex(size_t index);
    bool rmFromIndex(size_t index, item_t& removedItem);
    void reset();

    // Getters.
    bool peekAtTop(item_t& topItem) const;
    item_t peek(size_t index) const;
    unsigned int numItems() const;

    // Override Growable.
    virtual ~U64Heap();
    virtual bool resize(unsigned int newCap);

    static void sort(item_t* item, size_t numItems, bool reverseOrder = false);

private:
    item_t* item_;
    unsigned int numItems_;

    U64Heap(item_t*, size_t);

    void heapifyDown(size_t);
    void heapifyUp(size_t);

};

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid. Index
//! zero can be used to peek at the top item. There's no deterministic
//! order of items in the heap.
inline U64Heap::item_t U64Heap::peek(size_t index) const
{
    return item_[index + 1];
}

//! Peek at the top item. Return true if successful (i.e., heap is not empty).
inline bool U64Heap::peekAtTop(item_t& topItem) const
{
    return (numItems_ > 0)? ((topItem = item_[1]), true): (false);
}

//! Remove item from given index. Return true if successful (i.e., given index is valid).
inline bool U64Heap::rmFromIndex(size_t index)
{
    item_t removedItem;
    bool ok = rmFromIndex(index, removedItem);
    return ok;
}

//! Return the current number of items in the heap.
inline unsigned int U64Heap::numItems() const
{
    return numItems_;
}

//! Reset the heap by removing all items.
inline void U64Heap::reset()
{
    numItems_ = 0;
}

END_NAMESPACE1

#endif
