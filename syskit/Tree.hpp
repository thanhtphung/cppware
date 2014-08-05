/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_TREE_HPP
#define SYSKIT_TREE_HPP

#include <new>
#include <sys/types.h>
#include "syskit/macros.h"

class TreeSuite;

BEGIN_NAMESPACE1(syskit)


//! 2-3-4 tree of opaque items
class Tree
    //!
    //! A class representing a 2-3-4 tree of opaque items. A 2-3-4 tree
    //! is a balanced tree with nodes having 2 or 3 or 4 links. A 2-link
    //! node holds 1 item. A 3-link node holds 2 items. A 4-link node holds
    //! 3 items. For comparison, a binary tree is a tree with 2-link nodes,
    //! and a red-black tree is a binary tree representation of a 2-3-4
    //! tree.
    //!
{

public:
    typedef void* item_t;
    typedef bool(*cb0_t)(void* arg, item_t item);

    //! Return a negative value if item0<item1, a positive value if item 0>item 1, and zero otherwise.
    typedef int(*compare_t)(const void* item0, const void* item1);

    typedef void(*cb1_t)(void* arg, item_t item);

    // Constructors and destructor.
    Tree(Tree* that);
    Tree(compare_t compare);
    Tree(const Tree& tree);
    ~Tree();

    // Operators.
    bool operator !=(const Tree& tree) const;
    bool operator ==(const Tree& tree) const;
    const Tree& operator =(Tree* that);
    const Tree& operator =(const Tree& tree);
    static void operator delete(void* p, size_t size);
    static void operator delete(void* p, void* buf);
    static void* operator new(size_t size);
    static void* operator new(size_t size, void* buf);

    // Tree operations.
    bool add(item_t item);
    bool add(item_t item, item_t& foundItem);
    bool find(const void* item) const;
    bool find(const void* item, compare_t compare) const;
    bool find(const void* item, compare_t compare, item_t& foundItem) const;
    bool find(const void* item, item_t& foundItem) const;
    bool findMax(item_t& maxItem) const;
    bool findMin(item_t& minItem) const;
    bool rm(const void* item);
    bool rm(const void* item, compare_t compare);
    bool rm(const void* item, compare_t compare, item_t& removedItem);
    bool rm(const void* item, item_t& removedItem);
    item_t any() const;
    item_t peek(size_t index) const;
    void rebalance();
    void reset();

    // Getters.
    compare_t cmpFunc() const;
    unsigned int numItems() const;

    // Iterator support.
    bool apply(cb0_t cb, void* arg = 0) const;
    bool applyChildFirst(cb0_t cb, void* arg = 0) const;
    bool applyParentFirst(cb0_t cb, void* arg = 0) const;
    void apply(cb1_t cb, void* arg = 0) const;
    void applyChildFirst(cb1_t cb, void* arg = 0) const;
    void applyParentFirst(cb1_t cb, void* arg = 0) const;

private:

    //
    // ABC for all nodes.
    //
    class Node
    {
    public:
        virtual ~Node();
        virtual Node* add(compare_t compare, item_t item, item_t& foundItem);
        virtual Node* clone() const;
        virtual Node* goLeft(item_t& leftItem) const;
        virtual Node* goRight(item_t& rightItem) const;
        virtual Node* rm(compare_t compare, const void* item, item_t& removedItem);
        virtual bool apply(cb0_t cb, void* arg) const;
        virtual bool applyChildFirst(cb0_t cb, void* arg) const;
        virtual bool applyParentFirst(cb0_t cb, void* arg) const;
        virtual bool find(compare_t compare, const void* item, item_t& foundItem) const;
        virtual bool isReal() const;
        virtual void apply(cb1_t cb, void* arg) const;
        virtual void applyChildFirst(cb1_t cb, void* arg) const;
        virtual void applyParentFirst(cb1_t cb, void* arg) const;
        static void operator delete(void* p, size_t size);
        static void* operator new(size_t size);
    protected:
        Node();
    private:
        Node(const Node&); //prohibit usage
        const Node& operator =(const Node&); //prohibit usage
    };

    //
    // 0-item node. Used as root node in empty tree.
    //
    class Node0: public Node
    {
    public:
        Node0();
        virtual ~Node0();
        virtual Node* add(compare_t compare, item_t item, item_t& foundItem);
        virtual Node* clone() const;
    private:
        Node0(const Node0&); //prohibit usage
        const Node0& operator =(const Node0&); //prohibit usage
    };

    //
    // 1-item (aka 2-link) node.
    //
    class Node1: public Node
    {
    public:
        Node1(item_t item0, Node* link0, Node* link1);
        virtual ~Node1();
        virtual Node* add(compare_t compare, item_t item, item_t& foundItem);
        virtual Node* clone() const;
        virtual Node* goLeft(item_t& leftItem) const;
        virtual Node* goRight(item_t& rightItem) const;
        virtual Node* rm(compare_t compare, const void* item, item_t& removedItem);
        virtual bool applyChildFirst(cb0_t cb, void* arg) const;
        virtual bool apply(cb0_t cb, void* arg) const;
        virtual bool applyParentFirst(cb0_t cb, void* arg) const;
        virtual bool find(compare_t compare, const void* item, item_t& foundItem) const;
        virtual void applyChildFirst(cb1_t cb, void* arg) const;
        virtual void apply(cb1_t cb, void* arg) const;
        virtual void applyParentFirst(cb1_t cb, void* arg) const;
    private:
        Node* link_[2];
        item_t item_[1];
        Node1(const Node1&); //prohibit usage
        const Node1& operator =(const Node1&); //prohibit usage
    };

    //
    // 2-item (aka 3-link) node.
    //
    class Node2: public Node
    {
    public:
        Node2(item_t item0, item_t item1, Node* link0, Node* link1, Node* link2);
        virtual ~Node2();
        virtual Node* add(compare_t compare, item_t item, item_t& foundItem);
        virtual Node* clone() const;
        virtual Node* goLeft(item_t& leftItem) const;
        virtual Node* goRight(item_t& rightItem) const;
        virtual Node* rm(compare_t compare, const void* item, item_t& removedItem);
        virtual bool applyChildFirst(cb0_t cb, void* arg) const;
        virtual bool apply(cb0_t cb, void* arg) const;
        virtual bool applyParentFirst(cb0_t cb, void* arg) const;
        virtual bool find(compare_t compare, const void* item, item_t& foundItem) const;
        virtual void applyChildFirst(cb1_t cb, void* arg) const;
        virtual void apply(cb1_t cb, void* arg) const;
        virtual void applyParentFirst(cb1_t cb, void* arg) const;
    private:
        Node* link_[3];
        item_t item_[2];
        Node2(const Node2&); //prohibit usage
        const Node2& operator =(const Node2&); //prohibit usage
    };

    //
    // 3-item (aka 4-link) node.
    //
    class Node3: public Node
    {
    public:
        Node3(item_t item0, item_t item1, item_t item2, Node* link0, Node* link1, Node* link2, Node* link3);
        virtual ~Node3();
        virtual Node* add(compare_t compare, item_t item, item_t& foundItem);
        virtual Node* clone() const;
        virtual Node* goLeft(item_t& leftItem) const;
        virtual Node* goRight(item_t& rightItem) const;
        virtual Node* rm(compare_t compare, const void* item, item_t& removedItem);
        virtual bool apply(cb0_t cb, void* arg) const;
        virtual bool applyChildFirst(cb0_t cb, void* arg) const;
        virtual bool applyParentFirst(cb0_t cb, void* arg) const;
        virtual bool find(compare_t compare, const void* item, item_t& foundItem) const;
        virtual void apply(cb1_t cb, void* arg) const;
        virtual void applyChildFirst(cb1_t cb, void* arg) const;
        virtual void applyParentFirst(cb1_t cb, void* arg) const;
    private:
        Node* link_[4];
        item_t item_[3];
        Node3(const Node3&); //prohibit usage
        const Node3& operator =(const Node3&); //prohibit usage
    };

    //
    // Temporary node used in splitting. Splitting occurs when adding
    // an item to a 3-item node. The 3-item node is split into two nodes
    // with the middle item moving up to balance the tree.
    //
    class NodeX: public Node
    {
    public:
        NodeX(item_t orphan, Node* left, Node* right);
        Node* left() const;
        Node* right() const;
        void reset(item_t orphan, Node* left, Node* right);
        item_t orphan() const;
        virtual ~NodeX();
        virtual bool isReal() const;
    private:
        Node* link_[2];
        item_t item_[1];
        NodeX(const NodeX&); //prohibit usage
        const NodeX& operator =(const NodeX&); //prohibit usage
    };

    Node* root_;
    compare_t compare_;
    unsigned int numItems_;

    static Node* const NOT_FOUND;

    static bool isEqual(void*, item_t);
    static bool peek(void*, item_t);
    static bool peekAny(void*, item_t);
    static int compare(const void*, const void*);
    static void addItem(void*, item_t);

    friend class ::TreeSuite;

};

END_NAMESPACE1

#include "syskit/BufPool.hpp"

BEGIN_NAMESPACE1(syskit)

//! Return true if this tree does not equal given tree. That is, if both
//! do not have the same number of items or if some item in one cannot be
//! found in the other.
inline bool Tree::operator !=(const Tree& tree) const
{
    return !(operator ==(tree));
}

inline void Tree::operator delete(void* p, size_t size)
{
    BufPool::freeBuf(p, size);
}

inline void Tree::operator delete(void* /*p*/, void* /*buf*/)
{
}

inline void* Tree::operator new(size_t size)
{
    void* buf = BufPool::allocateBuf(size);
    return buf;
}

inline void* Tree::operator new(size_t /*size*/, void* buf)
{
    return buf;
}

//! Return the comparison function used when items are compared.
inline Tree::compare_t Tree::cmpFunc() const
{
    return compare_;
}

//! Return any item in the tree. Returned value is zero for an empty tree.
inline Tree::item_t Tree::any() const
{
    item_t anyItem = 0;
    root_->applyParentFirst(peekAny, &anyItem);
    return anyItem;
}

//! Add given item to the tree. Return true if successful. Return false
//! otherwise (item already exists).
inline bool Tree::add(item_t item)
{
    item_t foundItem;
    return add(item, foundItem);
}

//! Recursively iterate tree in order. Invoke callback at each item. The
//! callback should return true to continue iterating and should return
//! false to abort iterating. Return false if the callback aborted the
//! iterating. Return true otherwise.
inline bool Tree::apply(cb0_t cb, void* arg) const
{
    bool ok = root_->apply(cb, arg);
    return ok;
}

//! Recursively iterate tree. Children first. Invoke callback at each item.
//! The callback should return true to continue iterating and should return
//! false to abort iterating. Return false if the callback aborted the
//! iterating. Return true otherwise.
inline bool Tree::applyChildFirst(cb0_t cb, void* arg) const
{
    bool ok = root_->applyChildFirst(cb, arg);
    return ok;
}

//! Recursively iterate tree. Parent first. Invoke callback at each item.
//! The callback should return true to continue iterating and should return
//! false to abort iterating. Return false if the callback aborted the
//! iterating. Return true otherwise.
inline bool Tree::applyParentFirst(cb0_t cb, void* arg) const
{
    bool ok = root_->applyParentFirst(cb, arg);
    return ok;
}

//! Locate given item in tree. Return true if found. Return false otherwise.
inline bool Tree::find(const void* item) const
{
    item_t foundItem;
    bool found = root_->find(compare_, item, foundItem);
    return found;
}

//! Locate given item in tree. Return true if found. Return false otherwise.
//! Use given compatible comparison function for this search.
inline bool Tree::find(const void* item, compare_t compare) const
{
    item_t foundItem;
    bool found = root_->find(compare, item, foundItem);
    return found;
}

//! Locate given item. Return true if found (also set foundItem to the found
//! item). Return false otherwise. Use given compatible comparison function
//! for this search.
inline bool Tree::find(const void* item, compare_t compare, item_t& foundItem) const
{
    bool found = root_->find(compare, item, foundItem);
    return found;
}

//! Locate given item. Return true if found (also set foundItem to the found
//! item). Return false otherwise.
inline bool Tree::find(const void* item, item_t& foundItem) const
{
    bool found = root_->find(compare_, item, foundItem);
    return found;
}

//! Locate given item in tree. If found, remove it from the tree and return
//! true. Return false otherwise.
inline bool Tree::rm(const void* item)
{
    item_t removedItem;
    bool found = rm(item, compare_, removedItem);
    return found;
}

//! Locate given item in tree. Use given compatible comparison function.
//! If found, remove it from the tree and return true. Return false otherwise.
inline bool Tree::rm(const void* item, compare_t compare)
{
    item_t removedItem;
    bool found = rm(item, compare, removedItem);
    return found;
}

//! Locate given item in tree. If found, remove it from the tree and return
//! true (also set removedItem to the removed item). Return false otherwise.
//! Frequent item removal can cause a tree to become somewhat unbalanced. A
//! suspected unbalanced tree can be rebalanced using the rebalance() method.
inline bool Tree::rm(const void* item, item_t& removedItem)
{
    bool found = rm(item, compare_, removedItem);
    return found;
}

//! Return the current number of items in the tree.
inline unsigned int Tree::numItems() const
{
    return numItems_;
}

//! Recursively iterate tree in order. Invoke callback at each item.
inline void Tree::apply(cb1_t cb, void* arg) const
{
    root_->apply(cb, arg);
}

//! Recursively iterate tree. Children first. Invoke callback at each item.
inline void Tree::applyChildFirst(cb1_t cb, void* arg) const
{
    root_->applyChildFirst(cb, arg);
}

//! Recursively iterate tree. Parent first. Invoke callback at each item.
//! The callback should return true to continue iterating and should return
//! false to abort iterating. Return false if the callback aborted the
//! iterating. Return true otherwise.
inline void Tree::applyParentFirst(cb1_t cb, void* arg) const
{
    root_->applyParentFirst(cb, arg);
}

inline void Tree::Node::operator delete(void* p, size_t size)
{
    BufPool::freeBuf(p, size);
}

inline void* Tree::Node::operator new(size_t size)
{
    void* buf = BufPool::allocateBuf(size);
    return buf;
}

inline Tree::Node* Tree::NodeX::left() const
{
    return link_[0];
}

inline Tree::Node* Tree::NodeX::right() const
{
    return link_[1];
}

inline Tree::item_t Tree::NodeX::orphan() const
{
    return item_[0];
}

inline void Tree::NodeX::reset(item_t orphan, Node* left, Node* right)
{
    item_[0] = orphan;
    link_[0] = left;
    link_[1] = right;
}

END_NAMESPACE1

#endif
