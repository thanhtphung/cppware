/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_HEAP_X_HPP
#define SYSKIT_HEAP_X_HPP

#include "syskit/BitVec.hpp"
#include "syskit/Growable.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! heap of opaque items - w/ extended features
class HeapX: public Growable
    //!
    //! A class representing a heap of opaque items. In a heap, the item with
    //! the largest key resides at the top. The heap has an initial capacity of
    //! capacity() items. The capacity can grow as needed if the growth factor
    //! is set to non-zero when constructed or afterwards using setGrowth(). If
    //! the heap is growable, growth can occur when items are added. This heap
    //! supports four main operations: insert item, remove the top item, remove
    //! an item, and replace an item. If there's no need for extended operations
    //! such as remove and replace any item, consider using the basic Heap class.
    //!
{

public:
    enum
    {
        DefaultCap = 256
    };

    //! Return a negative value if item0<item1, a positive value if item 0>item 1, and zero otherwise.
    typedef int(*compare_t)(const void* item0, const void* item1);
    typedef unsigned int handle_t;
    typedef void* item_t;

    static const handle_t INVALID_HANDLE;

    // Constructors.
    HeapX(compare_t compare, unsigned int capacity = DefaultCap, int growBy = 0);
    HeapX(const HeapX& heap);

    // Operators.
    const HeapX& operator =(const HeapX& heap);

    // Heap management.
    bool add(item_t item);
    bool add(item_t item, handle_t& handle);
    bool getItem(handle_t handle, item_t& item) const;
    bool replace(handle_t handle, item_t item);
    bool rm(item_t& topItem);
    bool rmFromIndex(size_t index);
    bool rmFromIndex(size_t index, item_t& removedItem);
    bool rmItem(handle_t handle);
    void reset();

    // Getters.
    bool peekAtTop(item_t& topItem) const;
    bool peekAtTop(item_t& topItem, handle_t& handle) const;
    compare_t cmpFunc() const;
    item_t peek(size_t index) const;
    unsigned int numItems() const;

    // Override Growable.
    virtual ~HeapX();
    virtual bool resize(unsigned int newCap);

private:
    typedef struct
    {
        item_t item;
        unsigned int handle;
    } node_t;

    BitVec handleIsAvail_;
    compare_t compare_;
    node_t* node_;
    unsigned int* nodeNum_;
    size_t handleSeed_;
    unsigned int numItems_;

    bool isValid(handle_t, unsigned int&) const;
    handle_t allocateHandle();
    void freeHandle(handle_t);
    void heapifyDown(size_t);
    void heapifyUp(size_t);

    static int compare(const void*, const void*);

};

//! Return the utilized comparison function.
inline HeapX::compare_t HeapX::cmpFunc() const
{
    return compare_;
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid. Index
//! zero can be used to peek at the top item. There's no deterministic
//! order of items in the heap.
inline HeapX::item_t HeapX::peek(size_t index) const
{
    return node_[index + 1].item;
}

//! Add given item to the heap. Return true if successful.
inline bool HeapX::add(item_t item)
{
    handle_t handle;
    bool ok = add(item, handle);
    return ok;
}

//! Peek at the top item. Return true if successful (i.e., heap is not empty).
inline bool HeapX::peekAtTop(item_t& topItem) const
{
    return (numItems_ > 0)? ((topItem = node_[1].item), true): (false);
}

//! Remove item from given index. Return true if successful (i.e., given index is valid).
inline bool HeapX::rmFromIndex(size_t index)
{
    item_t removedItem;
    bool ok = rmFromIndex(index, removedItem);
    return ok;
}

//! Return the current number of items in the heap.
inline unsigned int HeapX::numItems() const
{
    return numItems_;
}

// Free given handle.
inline void HeapX::freeHandle(handle_t handle)
{
    size_t bit = handle - 1;
    handleIsAvail_.set(bit);
}

//! Reset the heap by removing all items.
inline void HeapX::reset()
{
    numItems_ = 0;
    handleIsAvail_.setAll();
    handleSeed_ = BitVec::INVALID_BIT;
}

END_NAMESPACE1

#endif
