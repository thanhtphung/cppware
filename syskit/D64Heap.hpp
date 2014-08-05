/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_D64_HEAP_HPP
#define SYSKIT_D64_HEAP_HPP

#include "syskit/Growable.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! heap of 64-bit doubles
class D64Heap: public Growable
    //!
    //! A class representing a heap of 64-bit doubles. In a heap, the item with
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

    typedef double item_t;

    // Constructors.
    D64Heap(const D64Heap& heap);
    D64Heap(unsigned int capacity = DefaultCap, int growBy = 0);

    // Operators.
    const D64Heap& operator =(const D64Heap& heap);

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
    virtual ~D64Heap();
    virtual bool resize(unsigned int newCap);

    static void sort(item_t* item, size_t numItems, bool reverseOrder = false);

private:
    item_t* item_;
    unsigned int numItems_;

    D64Heap(item_t*, size_t);

    void heapifyDown(size_t);
    void heapifyUp(size_t);

};

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid. Index
//! zero can be used to peek at the top item. There's no deterministic
//! order of items in the heap.
inline D64Heap::item_t D64Heap::peek(size_t index) const
{
    return item_[index + 1];
}

//! Peek at the top item. Return true if successful (i.e., heap is not empty).
inline bool D64Heap::peekAtTop(item_t& topItem) const
{
    return (numItems_ > 0)? ((topItem = item_[1]), true): (false);
}

//! Remove item from given index. Return true if successful (i.e., given index is valid).
inline bool D64Heap::rmFromIndex(size_t index)
{
    item_t removedItem;
    bool ok = rmFromIndex(index, removedItem);
    return ok;
}

//! Return the current number of items in the heap.
inline unsigned int D64Heap::numItems() const
{
    return numItems_;
}

//! Reset the heap by removing all items.
inline void D64Heap::reset()
{
    numItems_ = 0;
}

END_NAMESPACE1

#endif
