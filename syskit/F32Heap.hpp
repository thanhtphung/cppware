/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_F32_HEAP_HPP
#define SYSKIT_F32_HEAP_HPP

#include "syskit/Growable.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! heap of 32-bit floats
class F32Heap: public Growable
    //!
    //! A class representing a heap of 32-bit floats. In a heap, the item with
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

    typedef float item_t;

    // Constructors.
    F32Heap(const F32Heap& heap);
    F32Heap(unsigned int capacity = DefaultCap, int growBy = 0);

    // Operators.
    const F32Heap& operator =(const F32Heap& heap);

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
    virtual ~F32Heap();
    virtual bool resize(unsigned int newCap);

    static void sort(item_t* item, size_t numItems, bool reverseOrder = false);

private:
    item_t* item_;
    unsigned int numItems_;

    F32Heap(item_t*, size_t);

    void heapifyDown(size_t);
    void heapifyUp(size_t);

};

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid. Index
//! zero can be used to peek at the top item. There's no deterministic
//! order of items in the heap.
inline F32Heap::item_t F32Heap::peek(size_t index) const
{
    return item_[index + 1];
}

//! Peek at the top item. Return true if successful (i.e., heap is not empty).
inline bool F32Heap::peekAtTop(item_t& topItem) const
{
    return (numItems_ > 0)? ((topItem = item_[1]), true): (false);
}

//! Remove item from given index. Return true if successful (i.e., given index is valid).
inline bool F32Heap::rmFromIndex(size_t index)
{
    item_t removedItem;
    bool ok = rmFromIndex(index, removedItem);
    return ok;
}

//! Return the current number of items in the heap.
inline unsigned int F32Heap::numItems() const
{
    return numItems_;
}

//! Reset the heap by removing all items.
inline void F32Heap::reset()
{
    numItems_ = 0;
}

END_NAMESPACE1

#endif
