/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <string.h>

#include "syskit-pch.h"
#include "syskit/HeapX.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)

const HeapX::handle_t HeapX::INVALID_HANDLE = 0U;


//!
//! Construct an empty heap with initial capacity of capacity items. The
//! heap does not grow if growBy is zero, exponentially grows by doubling
//! if growBy is negative, and grows by growBy items otherwise. When items
//! are compared, the given comparison function will be used. A primitive
//! comparison function comparing opaque items by their values will be used
//! if compare is zero.
//!
HeapX::HeapX(compare_t compare, unsigned int capacity, int growBy):
Growable(capacity, growBy),
handleIsAvail_(Growable::capacity(), true /*initialVal*/)
{
    compare_ = (compare == 0)? HeapX::compare: compare;
    handleSeed_ = BitVec::INVALID_BIT;
    numItems_ = 0;

    // Make node_ a one-based array instead of a zero-based array. node_[i/2]
    // refers to the mom of node_[i], for 2 <= i <= capacity. node_[1]
    // refers to the root node.
    node_ = new node_t[HeapX::capacity()];
    --node_;

    // Make nodeNum_ a one-based array instead of a zero-based
    // array. nodeNum_[handle] gives the node number associated
    // with the handle <handle>.
    nodeNum_ = new unsigned int[HeapX::capacity()];
    --nodeNum_;
}


//!
//! Construct a duplicate instance of the given heap.
//!
HeapX::HeapX(const HeapX& heap):
Growable(heap),
handleIsAvail_(heap.handleIsAvail_)
{
    compare_ = heap.compare_;
    handleSeed_ = heap.handleSeed_;
    numItems_ = heap.numItems_;

    // Make node_ a one-based array instead of a zero-based array. node_[i/2]
    // refers to the mom of node_[i], for 2 <= i <= capacity. node_[1]
    // refers to the root node. Copy the utilized items only. Don't care about
    // the unused ones. Unused items are initialized when used.
    size_t capacity = heap.capacity();
    node_ = new node_t[capacity];
    memcpy(node_, heap.node_ + 1, numItems_ * sizeof(*node_));
    --node_;

    // Make nodeNum_ a one-based array instead of a zero-based
    // array. nodeNum_[handle] gives the node number associated
    // with that handle.
    nodeNum_ = new unsigned int[capacity];
    memcpy(nodeNum_, heap.nodeNum_ + 1, capacity * sizeof(*nodeNum_));
    --nodeNum_;
}


HeapX::~HeapX()
{

    // nodeNum_ and node_ are one-based.
    delete[](nodeNum_ + 1);
    delete[](node_ + 1);
}


//!
//! Assignment operator. This heap might grow to accomodate the source.
//! If it cannot grow or if the comparison functions are not identical,
//! the assignment fails and nothing is copied. If failures occurs,
//! this heap will be reset upon return (old contents, if any, would
//! be lost). To avoid failures, make sure this operation is used only
//! if the comparison functions are identical and only if this heap's
//! capacity is the same or higher than the source heap's capacity.
//!
const HeapX& HeapX::operator =(const HeapX& heap)
{

    // Prevent self assignment.
    if (this == &heap)
    {
        return *this;
    }

    // Might need to grow to accomodate source.
    unsigned int curCap = capacity();
    unsigned int minCap = heap.numItems_;
    if (minCap > curCap)
    {
        if (canGrow())
        {
            delete[](nodeNum_ + 1);
            delete[](node_ + 1);
            curCap = setNextCap(minCap);
            bool initialVal = true;
            handleIsAvail_.resize(curCap, initialVal);
            node_ = new node_t[curCap];
            --node_;
            nodeNum_ = new unsigned int[curCap];
            --nodeNum_;
        }
    }

    // Copy all items by copying memory if the comparison functions are
    // identical and if source heap is not too big.
    if ((minCap <= curCap) && (compare_ == heap.compare_))
    {
        handleIsAvail_ = heap.handleIsAvail_;
        handleSeed_ = heap.handleSeed_;
        numItems_ = heap.numItems_;
        memcpy(node_ + 1, heap.node_ + 1, numItems_ * sizeof(*node_));
        memcpy(nodeNum_ + 1, heap.nodeNum_ + 1, minCap * sizeof(*nodeNum_));
    }

    // Reset heap to indicate failure (comparison functions are not
    // identical or source heap is too big).
    else
    {
        reset();
    }

    // Return reference to self.
    return *this;
}


//
// Assume at least one handle is available, allocate and return next
// available handle.
//
HeapX::handle_t HeapX::allocateHandle()
{
    size_t bit = handleIsAvail_.nextSetBit(handleSeed_);
    if (bit == BitVec::INVALID_BIT)
    {
        bit = handleIsAvail_.nextSetBit(BitVec::INVALID_BIT);
    }

    handleIsAvail_.clear(bit);
    handleSeed_ = bit;
    handle_t h = static_cast<handle_t>(handleSeed_ + 1);
    return h;
}


//!
//! Add given item to the heap. Return true if successful. Also
//! return the item's handle. The item's handle is required for
//! some extended operations.
//!
bool HeapX::add(item_t item, handle_t& handle)
{

    // Is heap full?
    bool ok;
    if ((numItems_ == capacity()) && (!grow()))
    {
        ok = false;
        handle = INVALID_HANDLE;
    }

    // Add given item
    else
    {
        ok = true;
        node_t& newNode = node_[++numItems_];
        newNode.handle = allocateHandle();
        newNode.item = item;
        handle = newNode.handle;
        nodeNum_[newNode.handle] = numItems_;
        heapifyUp(numItems_);
    }

    // Return true if successful.
    return ok;
}


//!
//! Given an item handle, locate the associated item. Return true
//! if found (i.e., given item handle is valid). If found, also
//! return the associated item.
//!
bool HeapX::getItem(handle_t handle, item_t& item) const
{

    // Given handle seems valid.
    bool ok;
    unsigned int nodeNum;
    if (isValid(handle, nodeNum))
    {
        ok = true;
        item = node_[nodeNum].item;
    }

    // Given handle cannot be interpreted.
    else
    {
        ok = false;
    }

    // Return true if found.
    return ok;
}


//!
//! Peek at the top item. Return true if successful (i.e., heap is not empty).
//! Also return the item's handle. The item's handle is required for some
//! extended operations.
//!
bool HeapX::peekAtTop(item_t& topItem, handle_t& handle) const
{
    bool ok;
    if (numItems_ > 0)
    {
        ok = true;
        topItem = node_[1].item;
        handle = node_[1].handle;
    }

    else
    {
        ok = false;
    }

    return ok;
}


//
// Return true if the given handle appears to be valid. If it's valid,
// also return the associated node number.
//
bool HeapX::isValid(handle_t handle, unsigned int& nodeNum) const
{
    bool ok;
    if ((handle > 0) && (handle <= capacity()))
    {
        nodeNum = nodeNum_[handle];
        ok = ((nodeNum > 0) && (nodeNum <= numItems_));
    }
    else
    {
        ok = false;
    }

    return ok;
}


//!
//! Replace given item in the heap. Given item is identified
//! by its handle determined when added. New value is given in
//! item. Return true if successful. Return false otherwise
//! (e.g., given handle cannot be interpreted).
//!
bool HeapX::replace(handle_t handle, item_t item)
{

    // Given handle seems valid.
    bool ok;
    unsigned int nodeNum;
    if (isValid(handle, nodeNum))
    {
        ok = true;
        node_t& node = node_[nodeNum];
        node.item = item;
        ((nodeNum == 1) || (compare_(node.item, node_[nodeNum >> 1].item) < 0))?
            heapifyDown(nodeNum):
            heapifyUp(nodeNum);
    }

    // Given handle cannot be interpreted.
    else
    {
        ok = false;
    }

    // Return true if successful.
    return ok;
}


//!
//! Resize heap. Given new capacity must not be less than the current heap
//! size. Return true if successful.
//!
bool HeapX::resize(unsigned int newCap)
{
    bool ok;
    if (numItems_ > newCap)
    {
        ok = false;
    }

    else
    {
        ok = true;
        size_t curCap = capacity();
        if (newCap != curCap)
        {

            // Resize handle allocator.
            bool initialVal = true;
            handleIsAvail_.resize(newCap, initialVal);

            // Resize nodes.
            node_t* node = new node_t[newCap];
            memcpy(node, node_ + 1, numItems_ * sizeof(*node_));
            delete[](node_ + 1);
            node_ = --node;

            // Resize node numbers.
            unsigned int* nodeNum = new unsigned int[newCap];
            memcpy(nodeNum, nodeNum_ + 1, curCap * sizeof(*nodeNum_));
            delete[](nodeNum_ + 1);
            nodeNum_ = --nodeNum;

            // Update capacity.
            setCapacity(newCap);
        }
    }

    // Return true if successful.
    return ok;
}


//!
//! Remove the top item. Return true if successful (i.e., heap is no empty).
//!
bool HeapX::rm(item_t& topItem)
{

    // Heap is not empty.
    bool ok;
    if (numItems_ > 0)
    {
        ok = true;
        topItem = node_[1].item;
        freeHandle(node_[1].handle);
        node_t& lastNode = node_[numItems_--];
        nodeNum_[lastNode.handle] = 1;
        node_[1] = lastNode;
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


//!
//! Remove item at given index. Return true if successful (i.e.,
//! given index is valid).
//!
bool HeapX::rmFromIndex(size_t index, item_t& removedItem)
{

    // Exchange with high-index item and heapify.
    bool ok;
    if (index < numItems_)
    {
        ok = true;
        const node_t& removedNode = node_[++index];
        removedItem = removedNode.item;
        freeHandle(removedNode.handle);
        const node_t& lastNode = node_[numItems_--];
        if (index <= numItems_)
        {
            nodeNum_[lastNode.handle] = static_cast<unsigned int>(index);
            node_[index] = lastNode;
            ((index == 1) || (compare_(node_[index].item, node_[index >> 1].item) < 0))?
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
//! Remove the given item from the heap. Given item is identified
//! by its handle determined when added. Return true if successful.
//! Return false otherwise (e.g., given handle cannot be interpreted).
//!
bool HeapX::rmItem(handle_t handle)
{

    // Given handle seems valid.
    bool ok;
    unsigned int nodeNum;
    if (isValid(handle, nodeNum))
    {
        ok = true;
        freeHandle(node_[nodeNum].handle);
        node_t& lastNode = node_[numItems_--];
        nodeNum_[lastNode.handle] = nodeNum;
        node_[nodeNum] = lastNode;
        ((nodeNum == 1) || (compare_(node_[nodeNum].item, node_[nodeNum >> 1].item) < 0))?
            heapifyDown(nodeNum):
            heapifyUp(nodeNum);
    }

    // Given handle cannot be interpreted.
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
int HeapX::compare(const void* item0, const void* item1)
{
    return (item0 < item1)? (-1): ((item0>item1)? 1: 0);
}


//
// Start at node i, move downward to restore the heap condition.
//
void HeapX::heapifyDown(size_t i)
{
    size_t mom = i;
    node_t* momNode = node_ + mom;
    size_t kid = (mom << 1);
    node_t* kidNode = node_ + kid;
    while (kid <= numItems_)
    {

        // Determine the larger kid. Be careful since there might be only one kid.
        size_t sib = kid + 1;
        node_t* sibNode = node_ + sib;
        if ((sib <= numItems_) && (compare_(kidNode->item, sibNode->item) < 0))
        {
            kid = sib;
            kidNode = sibNode;
        }

        // Done if mom is larger than the larger kid.
        if (compare_(momNode->item, kidNode->item) >= 0)
        {
            break;
        }

        // Swap mom with the larger kid and keep going.
        nodeNum_[kidNode->handle] = static_cast<unsigned int>(mom);
        nodeNum_[momNode->handle] = static_cast<unsigned int>(kid);
        node_t tmp = *kidNode;
        *kidNode = *momNode;
        *momNode = tmp;
        mom = kid;
        momNode = kidNode;
        kid = (mom << 1);
        kidNode = node_ + kid;
    }
}


//
// Start at node i, move upward to restore the heap condition.
//
void HeapX::heapifyUp(size_t i)
{
    size_t kid = i;
    node_t* kidNode = node_ + kid;
    size_t mom = (kid >> 1);
    node_t* momNode = node_ + mom;
    while ((mom > 0) && (compare_(momNode->item, kidNode->item) < 0))
    {
        nodeNum_[momNode->handle] = static_cast<unsigned int>(kid);
        nodeNum_[kidNode->handle] = static_cast<unsigned int>(mom);
        node_t tmp = *momNode;
        *momNode = *kidNode;
        *kidNode = tmp;
        kid = mom;
        kidNode = momNode;
        mom = (kid >> 1);
        momNode = node_ + mom;
    }
}

END_NAMESPACE1
