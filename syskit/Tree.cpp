/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/Tree.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)

Tree::Node* const Tree::NOT_FOUND = (Node*)(0x0badcafeUL); //must be non-zero and must be an invalid address


//!
//! Construct instance using guts from that. That is, move tree contents from
//! that into this. Also, use the same comparison function.
//!
Tree::Tree(Tree* that)
{
    compare_ = that->compare_;
    numItems_ = that->numItems_;
    root_ = that->root_;
    that->numItems_ = 0;
    that->root_ = new Node0;
}


//!
//! Construct an empty tree. When items are compared, the given comparison
//! function will be used. A primitive comparison function comparing opaque
//! items by their values will be used if compare is zero.
//!
Tree::Tree(compare_t compare)
{
    compare_ = (compare == 0)? Tree::compare: compare;
    numItems_ = 0;
    root_ = new Node0;
}


//!
//! Construct a duplicate instance of the given tree.
//!
Tree::Tree(const Tree& tree)
{
    compare_ = tree.compare_;
    numItems_ = tree.numItems_;
    root_ = tree.root_->clone();
}


//!
//! Destruct tree.
//! Nodes are deleted recursively, children before parent.
//!
Tree::~Tree()
{
    delete root_;
}


//!
//! Return true if this tree equals given tree. That is, if both have the
//! same number of items and items in one can be found in the other.
//!
bool Tree::operator ==(const Tree& tree) const
{
    bool answer;
    if (numItems_ != tree.numItems_)
    {
        answer = false;
    }

    // Two trees are equal even if they don't have the same shape. To decide,
    // iterate all items in one and see if they can be found in the other and
    // also check the reverse.
    else
    {
        void* p[2];
        p[0] = const_cast<Tree*>(&tree);
        p[1] = (void*)(compare_);
        answer = root_->applyParentFirst(isEqual, p);
    }

    return answer;
}


//!
//! Reset and move the tree contents from that into this. Assume the trees
//! are compatible. That is, items unique in that are also unique in this.
//! Behavior is unpredictable if the trees are incompatible.
//!
const Tree& Tree::operator =(Tree* that)
{

    // No-op if operating against the same tree.
    if (this != that)
    {
        delete root_;
        numItems_ = that->numItems_;
        root_ = that->root_;
        that->numItems_ = 0;
        that->root_ = new Node0;
    }

    // Return reference to self.
    return *this;
}


//!
//! Reset and copy the tree contents from given tree. If necessary,
//! drop items which are inappropriate for this tree (i.e., distinct
//! items from one tree might be duplicates in another).
//!
const Tree& Tree::operator =(const Tree& tree)
{

    // Prevent self assignment.
    if (this != &tree)
    {
        if (compare_ == tree.compare_)
        {
            delete root_;
            numItems_ = tree.numItems_;
            root_ = tree.root_->clone();
        }
        else
        {
            reset();
            root_->applyParentFirst(addItem, this);
        }
    }

    // Return reference to self.
    return *this;
}


//!
//! Look at tree as a sorted collection. Peek at given index and return the residing item.
//! Don't do any error checking. Behavior is unpredictable if given index is invalid. This
//! is an inefficient way of iterating the tree and is provided mainly for unit testing.
//!
Tree::item_t Tree::peek(size_t index) const
{
    void* p[2];
    unsigned int k = static_cast<unsigned int>(index + 1);
    item_t itemAtIndex = NOT_FOUND;
    p[0] = &k;
    p[1] = &itemAtIndex;
    root_->apply(peek, p);
    return itemAtIndex;
}


//!
//! Add given item to the tree. Return true if successful. Return false
//! and also return the duplicate item otherwise (item already exists).
//!
bool Tree::add(item_t item, item_t& foundItem)
{

    // Item added successfully? Addition might have caused the old 3-item
    // root node to split and morph. A NodeX node is used to split a 3-item
    // child and exists only momentarily to allow the parent to adopt the
    // item moving up due to splitting.
    bool ok;
    Node* morph = root_->add(compare_, item, foundItem);
    if (morph != 0)
    {
        if (morph->isReal())
        {
            root_ = morph;
        }
        else
        {
            NodeX* x = static_cast<NodeX*>(morph);
            root_ = new Node1(x->orphan(), x->left(), x->right());
            delete x;
        }
        ++numItems_;
        ok = true;
    }

    // Item already exists?
    else
    {
        ok = false;
    }

    // Return true if successful.
    return ok;
}


//
// Look at arg as a vector of two pointers. The first points to tree1, and
// the second points to the comparison function used by tree0. Return true
// if tree1 contains the given item from tree0 and if tree0 contains the
// corresponding item from tree1. Return false otherwise.
//
bool Tree::isEqual(void* arg, item_t item)
{
    item_t foundItem;
    void** p = static_cast<void**>(arg);
    const Tree* tree = static_cast<const Tree*>(p[0]);
    compare_t compare = reinterpret_cast<compare_t>(p[1]);
    bool keepGoing = tree->find(item, foundItem) && (compare(foundItem, item) == 0);
    return keepGoing;
}


//!
//! Locate and return the maximum item. Return true if successful (also
//! set maxItem to the found item). Return false if the tree has no items.
//!
bool Tree::findMax(item_t& maxItem) const
{
    bool ok;
    if (numItems_ > 0)
    {
        item_t item;
        const Node* p = root_;
        while ((p = p->goRight(item)) != 0);
        maxItem = item;
        ok = true;
    }
    else
    {
        ok = false;
    }

    // Return true if successful.
    return ok;
}


//!
//! Locate and return the minimum item. Return true if successful (also
//! set minItem to the found item). Return false if the tree has no items.
//!
bool Tree::findMin(item_t& minItem) const
{
    bool ok;
    if (numItems_ > 0)
    {
        item_t item;
        const Node* p = root_;
        while ((p = p->goLeft(item)) != 0);
        minItem = item;
        ok = true;
    }
    else
    {
        ok = false;
    }

    // Return true if successful.
    return ok;
}


//!
//! Locate given item in tree. Use given compatible comparison function.
//! If found, remove it from the tree and return true (also set removedItem
//! to the removed item). Return false otherwise. Frequent item removal can
//! cause a tree to become somewhat unbalanced. A suspected unbalanced tree
//! can be rebalanced using the rebalance() method.
//!
bool Tree::rm(const void* item, compare_t compare, item_t& removedItem)
{

    // Removal might have caused the old 1-item root node to disappear.
    // Make sure a Node0 instance is used in an empty tree.
    bool ok;
    Node* morph = root_->rm(compare, item, removedItem);
    if (morph == NOT_FOUND)
    {
        ok = false;
    }
    else
    {
        root_ = (morph == 0)? new Node0: morph;
        --numItems_;
        ok = true;
    }

    // Return true if successful.
    return ok;
}


//
// Look at arg as a vector of two pointers. The first p0 points to a counter k,
// and the second p1 points to an item. Decrement the counter k. Return true
// if decremented counter is non-zero. Return false otherwise (also return
// current item in p1). Effectively, the return code aborts the tree iterating
// at the 1-based k-th item in the tree.
//
bool Tree::peek(void* arg, item_t item)
{
    void** p = static_cast<void**>(arg);
    unsigned int* k = reinterpret_cast<unsigned int*>(p[0]);
    bool keepGoing = (--*k == 0)? (*reinterpret_cast<item_t*>(p[1]) = item, false): (true);
    return keepGoing;
}


bool Tree::peekAny(void* arg, item_t item)
{
    void** anyItem = static_cast<void**>(arg);
    *anyItem = item;
    bool keepGoing = false;
    return keepGoing;
}


//
// Primitive comparison function comparing opaque items by their values.
// Return a negative value if item0<item1, a positive value if item 0>item 1, and zero otherwise.
//
int Tree::compare(const void* item0, const void* item1)
{
    return (item0 < item1)? (-1): ((item0>item1)? 1: 0);
}


//
// Look at arg as a tree pointer. Add given item to the tree.
// Ignore errors (i.e., it's okay if an item is not added
// successfully because it's considered a duplicate).
//
void Tree::addItem(void* arg, item_t item)
{
    Tree& tree = *static_cast<Tree*>(arg);
    tree.add(item);
}


//!
//! Item removal can cause a tree to become somewhat unbalanced.
//! This method can be used to rebalance a suspected unbalanced tree.
//!
void Tree::rebalance()
{

    // Rebalance by reinserting all items.
    if (numItems_ > 0)
    {
        Node* oldRoot = root_;
        numItems_ = 0;
        root_ = new Node0;
        oldRoot->applyParentFirst(addItem, this);
        delete oldRoot;
    }
}


//!
//! Reset the tree by recursively removing all items.
//!
void Tree::reset()
{
    if (numItems_ > 0)
    {
        delete root_;
        numItems_ = 0;
        root_ = new Node0;
    }
}


Tree::Node::Node()
{
}


Tree::Node::~Node()
{
}


Tree::Node* Tree::Node::add(compare_t /*compare*/, item_t /*item*/, item_t& /*foundItem*/)
{
    return 0;
}


Tree::Node* Tree::Node::clone() const
{
    return 0;
}


Tree::Node* Tree::Node::goLeft(item_t& leftItem) const
{
    leftItem = 0;
    return 0;
}


Tree::Node* Tree::Node::goRight(item_t& rightItem) const
{
    rightItem = 0;
    return 0;
}


Tree::Node* Tree::Node::rm(compare_t /*compare*/, const void* /*item*/, item_t& /*removedItem*/)
{
    return NOT_FOUND;
}


bool Tree::Node::apply(cb0_t /*cb*/, void* /*arg*/) const
{
    bool ok = true;
    return ok;
}


bool Tree::Node::applyChildFirst(cb0_t /*cb*/, void* /*arg*/) const
{
    bool ok = true;
    return ok;
}


bool Tree::Node::applyParentFirst(cb0_t /*cb*/, void* /*arg*/) const
{
    bool ok = true;
    return ok;
}


bool Tree::Node::find(compare_t /*compare*/, const void* /*item*/, item_t& /*foundItem*/) const
{
    bool found = false;
    return found;
}


//
// Return true if real.
// All nodes are real except for NodeX instances.
//
bool Tree::Node::isReal() const
{
    return true;
}


void Tree::Node::apply(cb1_t /*cb*/, void* /*arg*/) const
{
}


void Tree::Node::applyChildFirst(cb1_t /*cb*/, void* /*arg*/) const
{
}


void Tree::Node::applyParentFirst(cb1_t /*cb*/, void* /*arg*/) const
{
}


Tree::Node0::Node0():
Node()
{
}


Tree::Node0::~Node0()
{
}


//
// Use given comparison function, add given item to this subtree.
// Return zero if item already exists. Return self or the new
// Node1 image if morphed.
//
Tree::Node* Tree::Node0::add(compare_t /*compare*/, item_t item, item_t& /*foundItem*/)
{

    // Add to this node by morphing into a 1-item node.
    Node* morph = new Node1(item, 0, 0);
    delete this;
    return morph;
}


Tree::Node* Tree::Node0::clone() const
{
    return new Node0;
}


Tree::Node1::Node1(item_t item0, Node* link0, Node* link1):
Node()
{
    item_[0] = item0;
    link_[0] = link0;
    link_[1] = link1;
}


Tree::Node1::~Node1()
{
    delete link_[1];
    delete link_[0];
}


//
// Use given comparison function, add given item to this subtree.
// Return zero if item already exists. Return self or the new
// Node2 image if morphed.
//
Tree::Node* Tree::Node1::add(compare_t compare, item_t item, item_t& foundItem)
{

    // Does item already exist?
    int rc = compare(item, item_[0]);
    if (rc == 0)
    {
        foundItem = item_[0];
        return 0;
    }

    Node* morph;
    if (rc < 0)
    {

        // Add to left child, if any. This might cause the left child to morph
        // into something else (Node1 --> Node2, Node2 --> Node3, Node3 --> NodeX).
        // A NodeX node is used to split a 3-item child and exists only momentarily
        // to allow this parent to adopt the item moving up due to splitting. The
        // adopted item would be leftmost. Any item added to this node requires
        // morphing into a 2-item node.
        if (link_[0] != 0)
        {
            morph = link_[0]->add(compare, item, foundItem);
            if (morph == 0) return 0;
            if (morph->isReal()) return link_[0] = morph, this;
            NodeX* x = static_cast<NodeX*>(morph);
            morph = new Node2(x->orphan(), item_[0], x->left(), x->right(), link_[1]);
            link_[0] = 0;
            delete x;
        }
        else
        {
            morph = new Node2(item, item_[0], 0, 0, link_[1]);
        }
        link_[1] = 0;
    }

    else //rc > 0
    {

        // Add to right child, if any. This might cause the right child to morph
        // into something else (Node1 --> Node2, Node2 --> Node3, Node3 --> NodeX).
        // A NodeX node is used to split a 3-item child and exists only momentarily
        // to allow this parent to adopt the item moving up due to splitting. The
        // adopted item would be rightmost. Any item added to this node requires
        // morphing into a 2-item node.
        if (link_[1] != 0)
        {
            morph = link_[1]->add(compare, item, foundItem);
            if (morph == 0) return 0;
            if (morph->isReal()) return link_[1] = morph, this;
            NodeX* x = static_cast<NodeX*>(morph);
            morph = new Node2(item_[0], x->orphan(), link_[0], x->left(), x->right());
            link_[1] = 0;
            delete x;
        }
        else
        {
            morph = new Node2(item_[0], item, link_[0], 0, 0);
        }
        link_[0] = 0;
    }

    // Addition required morphing into a 2-item node.
    // Return the new Node2 image.
    delete this;
    return morph;
}


Tree::Node* Tree::Node1::clone() const
{
    Node* link0 = (link_[0] != 0)? link_[0]->clone(): 0;
    Node* link1 = (link_[1] != 0)? link_[1]->clone(): 0;
    return new Node1(item_[0], link0, link1);
}


Tree::Node* Tree::Node1::goLeft(item_t& leftItem) const
{
    leftItem = item_[0];
    return link_[0];
}


Tree::Node* Tree::Node1::goRight(item_t& rightItem) const
{
    rightItem = item_[0];
    return link_[1];
}


//
// Use given comparison function, remove given item from this subtree.
// Return NOT_FOUND if item does not exist. Return self or the new image
// if morphed.
//
Tree::Node* Tree::Node1::rm(compare_t compare, const void* item, item_t& removedItem)
{

    // Item found here?
    int rc = compare(item, item_[0]);
    if (rc == 0)
    {

        // If this node has no left child, the node can go away and be replaced
        // with the right subtree (if any).
        removedItem = item_[0];
        if (link_[0] == 0)
        {
            Node* morph = link_[1];
            link_[1] = 0;
            delete this;
            return morph;
        }

        // If this node has no right child, the node can go away and be replaced
        // with the left subtree.
        if (link_[1] == 0)
        {
            Node* morph = link_[0];
            link_[0] = 0;
            delete this;
            return morph;
        }

        // Locate the minimum item in the right subtree. This item has no left
        // child and can be easily removed (no need to check for NOT_FOUND since
        // this item was already found, no need to worry about morphing as this
        // node will not need to morph). Remove the minimum item from where it
        // is and use it to replace the removed item in this node.
        item_t minItem;
        for (const Node* p = link_[1]; (p = p->goLeft(minItem)) != 0;);
        link_[1] = link_[1]->rm(compare, minItem, minItem);
        item_[0] = minItem;
        return this;
    }

    // Item not found here, but might be in left subtree.
    else if (rc < 0)
    {
        if (link_[0] != 0)
        {
            Node* morph = link_[0]->rm(compare, item, removedItem);
            if (morph != NOT_FOUND)
            {
                link_[0] = morph;
                return this;
            }
        }
    }

    // Item not found here, but might be in right subtree.
    else //rc > 0
    {
        if (link_[1] != 0)
        {
            Node* morph = link_[1]->rm(compare, item, removedItem);
            if (morph != NOT_FOUND)
            {
                link_[1] = morph;
                return this;
            }
        }
    }

    return NOT_FOUND;
}


//
// Recursively invoke callback at each item in order. That is,
// iterate left subtree, then node, then right subtree. The callback
// returns true to continue iterating and returns false to abort
// iterating. Return false if the callback aborted the iterating.
// Return true otherwise.
//
bool Tree::Node1::apply(cb0_t cb, void* arg) const
{
    bool ok = (link_[0] != 0)? link_[0]->apply(cb, arg): true;
    if (ok)
    {
        ok = cb(arg, item_[0]);
        if (ok)
        {
            if (link_[1] != 0) ok = link_[1]->apply(cb, arg);
        }
    }

    return ok;
}


//
// Recursively invoke callback at each item. Iterate children before
// parent. The callback returns true to continue iterating and returns
// false to abort iterating. Return false if the callback aborted the
// iterating. Return true otherwise.
//
bool Tree::Node1::applyChildFirst(cb0_t cb, void* arg) const
{
    bool ok = (link_[0] != 0)? link_[0]->applyChildFirst(cb, arg): true;
    if (ok)
    {
        if (link_[1] != 0) ok = link_[1]->applyChildFirst(cb, arg);
        if (ok)
        {
            ok = cb(arg, item_[0]);
        }
    }

    return ok;
}


//
// Recursively invoke callback at each item. Iterate parent before
// children. The callback returns true to continue iterating and returns
// false to abort iterating. Return false if the callback aborted the
// iterating. Return true otherwise.
//
bool Tree::Node1::applyParentFirst(cb0_t cb, void* arg) const
{
    bool ok = cb(arg, item_[0]);
    if (ok)
    {
        if (link_[0] != 0) ok = link_[0]->applyParentFirst(cb, arg);
        if (ok)
        {
            if (link_[1] != 0) ok = link_[1]->applyParentFirst(cb, arg);
        }
    }

    return ok;
}


//
// Use given comparison function, recursively locate the node containing
// given item. Return true if found (also set foundItem to the found item).
// Return false if not found.
//
bool Tree::Node1::find(compare_t compare, const void* item, item_t& foundItem) const
{
    int rc = compare(item, item_[0]);
    if (rc == 0)
    {
        foundItem = item_[0];
        bool found = true;
        return found;
    }

    // Keep looking, if possible.
    Node* p = (rc < 0)? link_[0]: link_[1];
    bool found = (p != 0)? p->find(compare, item, foundItem): false;
    return found;
}


//
// Recursively invoke callback at each item in order.
// That is, iterate left subtree, then node, then right subtree.
//
void Tree::Node1::apply(cb1_t cb, void* arg) const
{
    if (link_[0] != 0) link_[0]->apply(cb, arg);
    cb(arg, item_[0]);
    if (link_[1] != 0) link_[1]->apply(cb, arg);
}


//
// Recursively invoke callback at each item.
// Iterate children before parent.
//
void Tree::Node1::applyChildFirst(cb1_t cb, void* arg) const
{
    if (link_[0] != 0) link_[0]->applyChildFirst(cb, arg);
    if (link_[1] != 0) link_[1]->applyChildFirst(cb, arg);
    cb(arg, item_[0]);
}


//
// Recursively invoke callback at each item.
// Iterate parent before children.
//
void Tree::Node1::applyParentFirst(cb1_t cb, void* arg) const
{
    cb(arg, item_[0]);
    if (link_[0] != 0) link_[0]->applyParentFirst(cb, arg);
    if (link_[1] != 0) link_[1]->applyParentFirst(cb, arg);
}


Tree::Node2::Node2(item_t item0, item_t item1, Node* link0, Node* link1, Node* link2):
Node()
{
    item_[0] = item0;
    item_[1] = item1;
    link_[0] = link0;
    link_[1] = link1;
    link_[2] = link2;
}


Tree::Node2::~Node2()
{
    delete link_[2];
    delete link_[1];
    delete link_[0];
}


//
// Use given comparison function, add given item to this subtree.
// Return zero if item already exists. Return self or the new
// Node3 image if morphed.
//
Tree::Node* Tree::Node2::add(compare_t compare, item_t item, item_t& foundItem)
{

    // Does item already exist as left item?
    int rc = compare(item, item_[0]);
    if (rc == 0)
    {
        foundItem = item_[0];
        return 0;
    }

    Node* morph;
    if (rc < 0)
    {

        // Add to left child, if any. This might cause the left child to morph
        // into something else (Node1 --> Node2, Node2 --> Node3, Node3 --> NodeX).
        // A NodeX node is used to split a 3-item child and exists only momentarily
        // to allow this parent to adopt the item moving up due to splitting. The
        // adopted item would be leftmost. Any item added to this node requires
        // morphing into a 3-item node.
        if (link_[0] != 0)
        {
            morph = link_[0]->add(compare, item, foundItem);
            if (morph == 0) return 0;
            if (morph->isReal()) return link_[0] = morph, this;
            NodeX* x = static_cast<NodeX*>(morph);
            morph = new Node3(x->orphan(), item_[0], item_[1], x->left(), x->right(), link_[1], link_[2]);
            link_[0] = 0;
            delete x;
        }
        else
        {
            morph = new Node3(item, item_[0], item_[1], 0, 0, link_[1], link_[2]);
        }
        link_[1] = 0;
        link_[2] = 0;
    }

    // Does item already exist as right item?
    else if ((rc = compare(item, item_[1])) == 0)
    {
        foundItem = item_[1];
        return 0;
    }

    else if (rc > 0)
    {

        // Add to right child, if any. This might cause the right child to morph
        // into something else (Node1 --> Node2, Node2 --> Node3, Node3 --> NodeX).
        // A NodeX node is used to split a 3-item child and exists only momentarily
        // to allow this parent to adopt the item moving up due to splitting. The
        // adopted item would be rightmost. Any item added to this node requires
        // morphing into a 3-item node.
        if (link_[2] != 0)
        {
            morph = link_[2]->add(compare, item, foundItem);
            if (morph == 0) return 0;
            if (morph->isReal()) return link_[2] = morph, this;
            NodeX* x = static_cast<NodeX*>(morph);
            morph = new Node3(item_[0], item_[1], x->orphan(), link_[0], link_[1], x->left(), x->right());
            link_[2] = 0;
            delete x;
        }
        else
        {
            morph = new Node3(item_[0], item_[1], item, link_[0], link_[1], 0, 0);
        }
        link_[0] = 0;
        link_[1] = 0;
    }

    else //rc < 0
    {

        // Add to middle child, if any. This might cause the middle child to morph
        // into something else (Node1 --> Node2, Node2 --> Node3, Node3 --> NodeX).
        // A NodeX node is used to split a 3-item child and exists only momentarily
        // to allow this parent to adopt the item moving up due to splitting. The
        // adopted item would be in the middle. Any item added to this node requires
        // morphing into a 3-item node.
        if (link_[1] != 0)
        {
            morph = link_[1]->add(compare, item, foundItem);
            if (morph == 0) return 0;
            if (morph->isReal()) return link_[1] = morph, this;
            NodeX* x = static_cast<NodeX*>(morph);
            morph = new Node3(item_[0], x->orphan(), item_[1], link_[0], x->left(), x->right(), link_[2]);
            link_[1] = 0;
            delete x;
        }
        else
        {
            morph = new Node3(item_[0], item, item_[1], link_[0], 0, 0, link_[2]);
        }
        link_[0] = 0;
        link_[2] = 0;
    }

    // Addition required morphing into a 3-item node.
    // Return the new Node3 image.
    delete this;
    return morph;
}


Tree::Node* Tree::Node2::clone() const
{
    Node* link0 = (link_[0] != 0)? link_[0]->clone(): 0;
    Node* link1 = (link_[1] != 0)? link_[1]->clone(): 0;
    Node* link2 = (link_[2] != 0)? link_[2]->clone(): 0;
    return new Node2(item_[0], item_[1], link0, link1, link2);
}


Tree::Node* Tree::Node2::goLeft(item_t& leftItem) const
{
    leftItem = item_[0];
    return link_[0];
}


Tree::Node* Tree::Node2::goRight(item_t& rightItem) const
{
    rightItem = item_[1];
    return link_[2];
}


//
// Use given comparison function, remove given item from this subtree.
// Return NOT_FOUND if item does not exist. Return self or the new image
// if morphed.
//
Tree::Node* Tree::Node2::rm(compare_t compare, const void* item, item_t& removedItem)
{

    // Item found here as left item?
    int rc = compare(item, item_[0]);
    if (rc == 0)
    {

        // If this node has no left child, the item can be removed by morphing
        // into a 1-item node containing the right item and the other children.
        removedItem = item_[0];
        if (link_[0] == 0)
        {
            Node* morph = new Node1(item_[1], link_[1], link_[2]);
            link_[1] = 0;
            link_[2] = 0;
            delete this;
            return morph;
        }

        // If this node has no middle child, the item can be removed by morphing
        // into a 1-item node containing the right item and the other children.
        if (link_[1] == 0)
        {
            Node* morph = new Node1(item_[1], link_[0], link_[2]);
            link_[0] = 0;
            link_[2] = 0;
            delete this;
            return morph;
        }

        // Locate the minimum item in the middle subtree. This item has no left
        // child and can be easily removed (no need to check for NOT_FOUND since
        // this item was already found, no need to worry about morphing as this
        // node will not need to morph). Remove the minimum item from where it
        // is and use it to replace the removed item in this node.
        item_t minItem;
        for (const Node* p = link_[1]; (p = p->goLeft(minItem)) != 0;);
        link_[1] = link_[1]->rm(compare, minItem, minItem);
        item_[0] = minItem;
        return this;
    }

    // Item not found here, but might be in left subtree.
    else if (rc < 0)
    {
        if (link_[0] != 0)
        {
            Node* morph = link_[0]->rm(compare, item, removedItem);
            if (morph != NOT_FOUND)
            {
                link_[0] = morph;
                return this;
            }
        }
    }

    // Item found here as right item?
    else if ((rc = compare(item, item_[1])) == 0)
    {

        // If this node has no right child, the item can be removed by morphing
        // into a 1-item node containing the left item and the other children.
        removedItem = item_[1];
        if (link_[2] == 0)
        {
            Node* morph = new Node1(item_[0], link_[0], link_[1]);
            link_[0] = 0;
            link_[1] = 0;
            delete this;
            return morph;
        }

        // If this node has no middle child, the item can be removed by morphing
        // into a 1-item node containing the left item and the other children.
        if (link_[1] == 0)
        {
            Node* morph = new Node1(item_[0], link_[0], link_[2]);
            link_[0] = 0;
            link_[2] = 0;
            delete this;
            return morph;
        }

        // Locate the maximum item in the middle subtree. This item has no right
        // child and can be easily removed (no need to check for NOT_FOUND since
        // this item was already found, no need to worry about morphing as this
        // node will not need to morph). Remove the maximum item from where it
        // is and use it to replace the removed item in this node.
        item_t maxItem;
        for (const Node* p = link_[1]; (p = p->goRight(maxItem)) != 0;);
        link_[1] = link_[1]->rm(compare, maxItem, maxItem);
        item_[1] = maxItem;
        return this;
    }

    // Item not found here, but might be in right subtree.
    else if (rc > 0)
    {
        if (link_[2] != 0)
        {
            Node* morph = link_[2]->rm(compare, item, removedItem);
            if (morph != NOT_FOUND)
            {
                link_[2] = morph;
                return this;
            }
        }
    }

    // Item not found here, but might be in middle subtree.
    else //rc < 0
    {
        if (link_[1] != 0)
        {
            Node* morph = link_[1]->rm(compare, item, removedItem);
            if (morph != NOT_FOUND)
            {
                link_[1] = morph;
                return this;
            }
        }
    }

    return NOT_FOUND;
}


//
// Recursively invoke callback at each item in order. That is,
// iterate left subtree, then node, then right subtree. The callback
// returns true to continue iterating and returns false to abort
// iterating. Return false if the callback aborted the iterating.
// Return true otherwise.
//
bool Tree::Node2::apply(cb0_t cb, void* arg) const
{
    bool ok = (link_[0] != 0)? link_[0]->apply(cb, arg): true;
    if (ok)
    {
        ok = cb(arg, item_[0]);
        if (ok)
        {
            if (link_[1] != 0) ok = link_[1]->apply(cb, arg);
            if (ok)
            {
                ok = cb(arg, item_[1]);
                if (ok)
                {
                    if (link_[2] != 0) ok = link_[2]->apply(cb, arg);
                }
            }
        }
    }

    return ok;
}


//
// Recursively invoke callback at each item. Iterate children before
// parent. The callback returns true to continue iterating and returns
// false to abort iterating. Return false if the callback aborted the
// iterating. Return true otherwise.
//
bool Tree::Node2::applyChildFirst(cb0_t cb, void* arg) const
{
    bool ok = (link_[0] != 0)? link_[0]->applyChildFirst(cb, arg): true;
    if (ok)
    {
        if (link_[1] != 0) ok = link_[1]->applyChildFirst(cb, arg);
        if (ok)
        {
            if (link_[2] != 0) ok = link_[2]->applyChildFirst(cb, arg);
            if (ok)
            {
                ok = cb(arg, item_[0]);
                if (ok)
                {
                    ok = cb(arg, item_[1]);
                }
            }
        }
    }

    return ok;
}


//
// Recursively invoke callback at each item. Iterate parent before
// children. The callback returns true to continue iterating and returns
// false to abort iterating. Return false if the callback aborted the
// iterating. Return true otherwise.
//
bool Tree::Node2::applyParentFirst(cb0_t cb, void* arg) const
{
    bool ok = cb(arg, item_[0]);
    if (ok)
    {
        ok = cb(arg, item_[1]);
        if (ok)
        {
            if (link_[0] != 0) ok = link_[0]->applyParentFirst(cb, arg);
            if (ok)
            {
                if (link_[1] != 0) ok = link_[1]->applyParentFirst(cb, arg);
                if (ok)
                {
                    if (link_[2] != 0) ok = link_[2]->applyParentFirst(cb, arg);
                }
            }
        }
    }

    return ok;
}


//
// Use given comparison function, recursively locate the node containing
// given item. Return true if found (also set foundItem to the found item).
// Return false if not found.
//
bool Tree::Node2::find(compare_t compare, const void* item, item_t& foundItem) const
{

    // Found as left item?
    int rc = compare(item, item_[0]);
    if (rc == 0)
    {
        foundItem = item_[0];
        bool found = true;
        return found;
    }

    // Look left.
    Node* p;
    if (rc < 0)
    {
        p = link_[0];
    }

    // Found as right item?
    else if ((rc = compare(item, item_[1])) == 0)
    {
        foundItem = item_[1];
        bool found = true;
        return found;
    }

    // Look down or right.
    else
    {
        p = (rc < 0)? link_[1]: link_[2];
    }

    // Keep looking, if possible.
    bool found = (p != 0)? p->find(compare, item, foundItem): false;
    return found;
}


//
// Recursively invoke callback at each item in order.
// That is, iterate left subtree, then node, then right subtree.
//
void Tree::Node2::apply(cb1_t cb, void* arg) const
{
    if (link_[0] != 0) link_[0]->apply(cb, arg);
    cb(arg, item_[0]);
    if (link_[1] != 0) link_[1]->apply(cb, arg);
    cb(arg, item_[1]);
    if (link_[2] != 0) link_[2]->apply(cb, arg);
}


//
// Recursively invoke callback at each item.
// Iterate children before parent.
//
void Tree::Node2::applyChildFirst(cb1_t cb, void* arg) const
{
    if (link_[0] != 0) link_[0]->applyChildFirst(cb, arg);
    if (link_[1] != 0) link_[1]->applyChildFirst(cb, arg);
    if (link_[2] != 0) link_[2]->applyChildFirst(cb, arg);
    cb(arg, item_[0]);
    cb(arg, item_[1]);
}


//
// Recursively invoke callback at each item.
// Iterate parent before children.
//
void Tree::Node2::applyParentFirst(cb1_t cb, void* arg) const
{
    cb(arg, item_[0]);
    cb(arg, item_[1]);
    if (link_[0] != 0) link_[0]->applyParentFirst(cb, arg);
    if (link_[1] != 0) link_[1]->applyParentFirst(cb, arg);
    if (link_[2] != 0) link_[2]->applyParentFirst(cb, arg);
}


Tree::Node3::Node3(item_t item0, item_t item1, item_t item2, Node* link0, Node* link1, Node* link2, Node* link3):
Node()
{
    item_[0] = item0;
    item_[1] = item1;
    item_[2] = item2;
    link_[0] = link0;
    link_[1] = link1;
    link_[2] = link2;
    link_[3] = link3;
}


Tree::Node3::~Node3()
{
    delete link_[3];
    delete link_[2];
    delete link_[1];
    delete link_[0];
}


//
// Use given comparison function, add given item to this subtree.
// Return zero if item already exists. Return self or the new
// NodeX image if morphed.
//
Tree::Node* Tree::Node3::add(compare_t compare, item_t item, item_t& foundItem)
{

    // Does item already exist as left item?
    int rc = compare(item, item_[0]);
    if (rc == 0)
    {
        foundItem = item_[0];
        return 0;
    }

    Node* morph;
    if (rc < 0)
    {

        // Add to left child, if any. This might cause the left child to morph
        // into something else (Node1 --> Node2, Node2 --> Node3, Node3 --> NodeX).
        // A NodeX node is used to split a 3-item child and exists only momentarily
        // to allow this parent to adopt the item moving up due to splitting. The
        // adopted item would be leftmost. Any item added to this node requires
        // splitting into 3 parts: 2-item left, 1-item right, and one item moving up.
        if (link_[0] != 0)
        {
            morph = link_[0]->add(compare, item, foundItem);
            if (morph == 0) return 0;
            if (morph->isReal()) return link_[0] = morph, this;
            NodeX* x = static_cast<NodeX*>(morph);
            Node* left = new Node2(x->orphan(), item_[0], x->left(), x->right(), link_[1]);
            Node* right = new Node1(item_[2], link_[2], link_[3]);
            x->reset(item_[1], left, right);
            morph = x;
            link_[0] = 0;
        }
        else
        {
            Node* left = new Node2(item, item_[0], 0, 0, link_[1]);
            Node* right = new Node1(item_[2], link_[2], link_[3]);
            morph = new NodeX(item_[1], left, right);
        }
        link_[1] = 0;
        link_[2] = 0;
        link_[3] = 0;
    }

    // Does item already exist as right item?
    else if ((rc = compare(item, item_[2])) == 0)
    {
        foundItem = item_[2];
        return 0;
    }

    else if (rc > 0)
    {

        // Add to right child, if any. This might cause the right child to morph
        // into something else (Node1 --> Node2, Node2 --> Node3, Node3 --> NodeX).
        // A NodeX node is used to split a 3-item child and exists only momentarily
        // to allow this parent to adopt the item moving up due to splitting. The
        // adopted item would be rightmost. Any item added to this node requires
        // splitting into 3 parts: 1-item left, 2-item right, and one item moving up.
        if (link_[3] != 0)
        {
            morph = link_[3]->add(compare, item, foundItem);
            if (morph == 0) return 0;
            if (morph->isReal()) return link_[3] = morph, this;
            NodeX* x = static_cast<NodeX*>(morph);
            Node* left = new Node1(item_[0], link_[0], link_[1]);
            Node* right = new Node2(item_[2], x->orphan(), link_[2], x->left(), x->right());
            x->reset(item_[1], left, right);
            morph = x;
            link_[3] = 0;
        }
        else
        {
            Node* left = new Node1(item_[0], link_[0], link_[1]);
            Node* right = new Node2(item_[2], item, link_[2], 0, 0);
            morph = new NodeX(item_[1], left, right);
        }
        link_[0] = 0;
        link_[1] = 0;
        link_[2] = 0;
    }

    // Does item already exist as middle item?
    else if ((rc = compare(item, item_[1])) == 0)
    {
        foundItem = item_[1];
        return 0;
    }

    else if (rc < 0)
    {

        // Add to midleft child, if any. This might cause the midleft child to morph
        // into something else (Node1 --> Node2, Node2 --> Node3, Node3 --> NodeX).
        // A NodeX node is used to split a 3-item child and exists only momentarily
        // to allow this parent to adopt the item moving up due to splitting. The
        // adopted item would be midleft. Any item added to this node requires
        // splitting into 3 parts: 2-item left, 1-item right, and one item moving up.
        if (link_[1] != 0)
        {
            morph = link_[1]->add(compare, item, foundItem);
            if (morph == 0) return 0;
            if (morph->isReal()) return link_[1] = morph, this;
            NodeX* x = static_cast<NodeX*>(morph);
            Node* left = new Node2(item_[0], x->orphan(), link_[0], x->left(), x->right());
            Node* right = new Node1(item_[2], link_[2], link_[3]);
            x->reset(item_[1], left, right);
            morph = x;
            link_[1] = 0;
        }
        else
        {
            Node* left = new Node2(item_[0], item, link_[0], 0, 0);
            Node* right = new Node1(item_[2], link_[2], link_[3]);
            morph = new NodeX(item_[1], left, right);
        }
        link_[0] = 0;
        link_[2] = 0;
        link_[3] = 0;
    }

    else //rc > 0
    {

        // Add to midright child, if any. This might cause the midright child to morph
        // into something else (Node1 --> Node2, Node2 --> Node3, Node3 --> NodeX).
        // A NodeX node is used to split a 3-item child and exists only momentarily
        // to allow this parent to adopt the item moving up due to splitting. The
        // adopted item would be midright. Any item added to this node requires
        // splitting into 3 parts: 1-item left, 2-item right, and one item moving up.
        if (link_[2] != 0)
        {
            morph = link_[2]->add(compare, item, foundItem);
            if (morph == 0) return 0;
            if (morph->isReal()) return link_[2] = morph, this;
            NodeX* x = static_cast<NodeX*>(morph);
            Node* left = new Node1(item_[0], link_[0], link_[1]);
            Node* right = new Node2(x->orphan(), item_[2], x->left(), x->right(), link_[3]);
            x->reset(item_[1], left, right);
            morph = x;
            link_[2] = 0;
        }
        else
        {
            Node* left = new Node1(item_[0], link_[0], link_[1]);
            Node* right = new Node2(item, item_[2], 0, 0, link_[3]);
            morph = new NodeX(item_[1], left, right);
        }
        link_[0] = 0;
        link_[1] = 0;
        link_[3] = 0;
    }

    // Addition required morphing into a temporary node.
    // Return the temporary image.
    delete this;
    return morph;
}


Tree::Node* Tree::Node3::clone() const
{
    Node* link0 = (link_[0] != 0)? link_[0]->clone(): 0;
    Node* link1 = (link_[1] != 0)? link_[1]->clone(): 0;
    Node* link2 = (link_[2] != 0)? link_[2]->clone(): 0;
    Node* link3 = (link_[3] != 0)? link_[3]->clone(): 0;
    return new Node3(item_[0], item_[1], item_[2], link0, link1, link2, link3);
}


Tree::Node* Tree::Node3::goLeft(item_t& leftItem) const
{
    leftItem = item_[0];
    return link_[0];
}


Tree::Node* Tree::Node3::goRight(item_t& rightItem) const
{
    rightItem = item_[2];
    return link_[3];
}


//
// Use given comparison function, remove given item from this subtree.
// Return NOT_FOUND if item does not exist. Return self or the new image
// if morphed.
//
Tree::Node* Tree::Node3::rm(compare_t compare, const void* item, item_t& removedItem)
{

    // Item found here as left item?
    int rc = compare(item, item_[0]);
    if (rc == 0)
    {

        // If this node has no left child, the item can be removed by morphing
        // into a 2-item node containing the right items and the other children.
        removedItem = item_[0];
        if (link_[0] == 0)
        {
            Node* morph = new Node2(item_[1], item_[2], link_[1], link_[2], link_[3]);
            link_[1] = 0;
            link_[2] = 0;
            link_[3] = 0;
            delete this;
            return morph;
        }

        // If this node has no midleft child, the item can be removed by morphing
        // into a 2-item node containing the right items and the other children.
        if (link_[1] == 0)
        {
            Node* morph = new Node2(item_[1], item_[2], link_[0], link_[2], link_[3]);
            link_[0] = 0;
            link_[2] = 0;
            link_[3] = 0;
            delete this;
            return morph;
        }

        // Locate the minimum item in the midleft subtree. This item has no left
        // child and can be easily removed (no need to check for NOT_FOUND since
        // this item was already found, no need to worry about morphing as this
        // node will not need to morph). Remove the minimum item from where it
        // is and use it to replace the removed item in this node.
        item_t minItem;
        for (const Node* p = link_[1]; (p = p->goLeft(minItem)) != 0;);
        link_[1] = link_[1]->rm(compare, minItem, minItem);
        item_[0] = minItem;
        return this;
    }

    // Item not found here, but might be in left subtree.
    else if (rc < 0)
    {
        if (link_[0] != 0)
        {
            Node* morph = link_[0]->rm(compare, item, removedItem);
            if (morph != NOT_FOUND)
            {
                link_[0] = morph;
                return this;
            }
        }
    }

    // Item found here as right item?
    else if ((rc = compare(item, item_[2])) == 0)
    {

        // If this node has no right child, the item can be removed by morphing
        // into a 2-item node containing the left items and the other children.
        removedItem = item_[2];
        if (link_[3] == 0)
        {
            Node* morph = new Node2(item_[0], item_[1], link_[0], link_[1], link_[2]);
            link_[0] = 0;
            link_[1] = 0;
            link_[2] = 0;
            delete this;
            return morph;
        }

        // If this node has no midright child, the item can be removed by morphing
        // into a 2-item node containing the goLeft items and the other children.
        if (link_[2] == 0)
        {
            Node* morph = new Node2(item_[0], item_[1], link_[0], link_[1], link_[3]);
            link_[0] = 0;
            link_[1] = 0;
            link_[3] = 0;
            delete this;
            return morph;
        }

        // Locate the maximum item in the midright subtree. This item has no right
        // child and can be easily removed (no need to check for NOT_FOUND since
        // this item was already found, no need to worry about morphing as this
        // node will not need to morph). Remove the maximum item from where it
        // is and use it to replace the removed item in this node.
        item_t maxItem;
        for (const Node* p = link_[2]; (p = p->goRight(maxItem)) != 0;);
        link_[2] = link_[2]->rm(compare, maxItem, maxItem);
        item_[2] = maxItem;
        return this;
    }

    // Item not found here, but might be in right subtree.
    else if (rc > 0)
    {
        if (link_[3] != 0)
        {
            Node* morph = link_[3]->rm(compare, item, removedItem);
            if (morph != NOT_FOUND)
            {
                link_[3] = morph;
                return this;
            }
        }
    }

    // Item found here as middle item?
    else if ((rc = compare(item, item_[1])) == 0)
    {

        // If this node has no midright child, the item can be removed by morphing
        // into a 2-item node containing the other items and the other children.
        removedItem = item_[1];
        if (link_[2] == 0)
        {
            Node* morph = new Node2(item_[0], item_[2], link_[0], link_[1], link_[3]);
            link_[0] = 0;
            link_[1] = 0;
            link_[3] = 0;
            delete this;
            return morph;
        }

        // If this node has no midleft child, the item can be removed by morphing
        // into a 2-item node containing the other items and the other children.
        if (link_[1] == 0)
        {
            Node* morph = new Node2(item_[0], item_[2], link_[0], link_[2], link_[3]);
            link_[0] = 0;
            link_[2] = 0;
            link_[3] = 0;
            delete this;
            return morph;
        }

        // Locate the maximum item in the midleft subtree. This item has no right
        // child and can be easily removed (no need to check for NOT_FOUND since
        // this item was already found, no need to worry about morphing as this
        // node will not need to morph). Remove the maximum item from where it
        // is and use it to replace the removed item in this node.
        item_t maxItem;
        for (const Node* p = link_[1]; (p = p->goRight(maxItem)) != 0;);
        link_[1] = link_[1]->rm(compare, maxItem, maxItem);
        item_[1] = maxItem;
        return this;
    }

    // Item not found here, but might be in midleft subtree.
    else if (rc < 0)
    {
        if (link_[1] != 0)
        {
            Node* morph = link_[1]->rm(compare, item, removedItem);
            if (morph != NOT_FOUND)
            {
                link_[1] = morph;
                return this;
            }
        }
    }

    // Item not found here, but might be in midright subtree.
    else //rc > 0
    {
        if (link_[2] != 0)
        {
            Node* morph = link_[2]->rm(compare, item, removedItem);
            if (morph != NOT_FOUND)
            {
                link_[2] = morph;
                return this;
            }
        }
    }

    return NOT_FOUND;
}


//
// Recursively invoke callback at each item in order. That is,
// iterate left subtree, then node, then right subtree. The callback
// returns true to continue iterating and returns false to abort
// iterating. Return false if the callback aborted the iterating.
// Return true otherwise.
//
bool Tree::Node3::apply(cb0_t cb, void* arg) const
{
    bool ok = (link_[0] != 0)? link_[0]->apply(cb, arg): true;
    if (ok)
    {
        ok = cb(arg, item_[0]);
        if (ok)
        {
            if (link_[1] != 0) ok = link_[1]->apply(cb, arg);
            if (ok)
            {
                ok = cb(arg, item_[1]);
                if (ok)
                {
                    if (link_[2] != 0) ok = link_[2]->apply(cb, arg);
                    if (ok)
                    {
                        ok = cb(arg, item_[2]);
                        if (ok)
                        {
                            if (link_[3] != 0) ok = link_[3]->apply(cb, arg);
                        }
                    }
                }
            }
        }
    }

    return ok;
}


//
// Recursively invoke callback at each item. Iterate children before
// parent. The callback returns true to continue iterating and returns
// false to abort iterating. Return false if the callback aborted the
// iterating. Return true otherwise.
//
bool Tree::Node3::applyChildFirst(cb0_t cb, void* arg) const
{
    bool ok = (link_[0] != 0)? link_[0]->applyChildFirst(cb, arg): true;
    if (ok)
    {
        if (link_[1] != 0) ok = link_[1]->applyChildFirst(cb, arg);
        if (ok)
        {
            if (link_[2] != 0) ok = link_[2]->applyChildFirst(cb, arg);
            if (ok)
            {
                if (link_[3] != 0) ok = link_[3]->applyChildFirst(cb, arg);
                if (ok)
                {
                    ok = cb(arg, item_[0]);
                    if (ok)
                    {
                        ok = cb(arg, item_[1]);
                        if (ok)
                        {
                            ok = cb(arg, item_[2]);
                        }
                    }
                }
            }
        }
    }

    return ok;
}


//
// Recursively invoke callback at each item. Iterate parent before
// children. The callback returns true to continue iterating and returns
// false to abort iterating. Return false if the callback aborted the
// iterating. Return true otherwise.
//
bool Tree::Node3::applyParentFirst(cb0_t cb, void* arg) const
{
    bool ok = cb(arg, item_[0]);
    if (ok)
    {
        ok = cb(arg, item_[1]);
        if (ok)
        {
            ok = cb(arg, item_[2]);
            if (ok)
            {
                if (link_[0] != 0) ok = link_[0]->applyParentFirst(cb, arg);
                if (ok)
                {
                    if (link_[1] != 0) ok = link_[1]->applyParentFirst(cb, arg);
                    if (ok)
                    {
                        if (link_[2] != 0) ok = link_[2]->applyParentFirst(cb, arg);
                        if (ok)
                        {
                            if (link_[3] != 0) ok = link_[3]->applyParentFirst(cb, arg);
                        }
                    }
                }
            }
        }
    }

    return ok;
}


//
// Use given comparison function, recursively locate the node containing
// given item. Return true if found (also set foundItem to the found item).
// Return false if not found.
//
bool Tree::Node3::find(compare_t compare, const void* item, item_t& foundItem) const
{

    // Found as left item?
    int rc = compare(item, item_[0]);
    if (rc == 0)
    {
        foundItem = item_[0];
        bool found = true;
        return found;
    }

    // Look left.
    Node* p;
    if (rc < 0)
    {
        p = link_[0];
    }

    // Look right.
    else
    {

        // Found as right item?
        rc = compare(item, item_[2]);
        if (rc == 0)
        {
            foundItem = item_[2];
            bool found = true;
            return found;
        }

        // Look right.
        if (rc > 0)
        {
            p = link_[3];
        }

        // Found as middle item?
        else if ((rc = compare(item, item_[1])) == 0)
        {
            foundItem = item_[1];
            bool found = true;
            return found;
        }

        // Look down.
        else
        {
            p = (rc < 0)? link_[1]: link_[2];
        }
    }

    // Keep looking, if possible.
    bool found = (p != 0)? p->find(compare, item, foundItem): false;
    return found;
}


//
// Recursively invoke callback at each item in order.
// That is, iterate left subtree, then node, then right subtree.
//
void Tree::Node3::apply(cb1_t cb, void* arg) const
{
    if (link_[0] != 0) link_[0]->apply(cb, arg);
    cb(arg, item_[0]);
    if (link_[1] != 0) link_[1]->apply(cb, arg);
    cb(arg, item_[1]);
    if (link_[2] != 0) link_[2]->apply(cb, arg);
    cb(arg, item_[2]);
    if (link_[3] != 0) link_[3]->apply(cb, arg);
}


//
// Recursively invoke callback at each item.
// Iterate children before parent.
//
void Tree::Node3::applyChildFirst(cb1_t cb, void* arg) const
{
    if (link_[0] != 0) link_[0]->applyChildFirst(cb, arg);
    if (link_[1] != 0) link_[1]->applyChildFirst(cb, arg);
    if (link_[2] != 0) link_[2]->applyChildFirst(cb, arg);
    if (link_[3] != 0) link_[3]->applyChildFirst(cb, arg);
    cb(arg, item_[0]);
    cb(arg, item_[1]);
    cb(arg, item_[2]);
}


//
// Recursively invoke callback at each item.
// Iterate parent before children.
//
void Tree::Node3::applyParentFirst(cb1_t cb, void* arg) const
{
    cb(arg, item_[0]);
    cb(arg, item_[1]);
    cb(arg, item_[2]);
    if (link_[0] != 0) link_[0]->applyParentFirst(cb, arg);
    if (link_[1] != 0) link_[1]->applyParentFirst(cb, arg);
    if (link_[2] != 0) link_[2]->applyParentFirst(cb, arg);
    if (link_[3] != 0) link_[3]->applyParentFirst(cb, arg);
}


Tree::NodeX::NodeX(item_t orphan, Node* left, Node* right):
Node()
{
    item_[0] = orphan;
    link_[0] = left;
    link_[1] = right;
}


Tree::NodeX::~NodeX()
{
}


//
// Return true if real.
// All nodes are real except for NodeX instances.
//
bool Tree::NodeX::isReal() const
{
    return false;
}

END_NAMESPACE1
