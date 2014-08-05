/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <string.h>

#include "syskit-pch.h"
#include "syskit/Trie.hpp"
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(syskit)

Trie::Node* const Trie::NODE_ADDED = (Node*)(0x900dcafeUL); //must be non-zero and must be an invalid address


//!
//! Construct a duplicate instance of the given trie.
//!
Trie::Trie(const Trie& trie)
{
    maxDigit_ = trie.maxDigit_;
    numKvPairs_ = trie.numKvPairs_;
    numNodes_ = trie.numNodes_;
    root_ = trie.root_->clone();
}


//!
//! Construct an empty trie with given digit range.
//!
Trie::Trie(unsigned char maxDigit)
{
    maxDigit_ = maxDigit;
    numKvPairs_ = 0;
    numNodes_ = 1;
    root_ = new Node0;
}


//!
//! Destruct trie. Nodes are deleted recursively, children before parent.
//!
Trie::~Trie()
{
    delete root_;
}


//!
//! Return true if this trie equals given trie. That is,
//! if both have the same number of key-value pairs and
//! if corresponding key-value pairs are equal.
//!
bool Trie::operator ==(const Trie& trie) const
{
    bool answer;
    if ((numKvPairs_ != trie.numKvPairs_) || (numNodes_ != trie.numNodes_))
    {
        answer = false;
    }
    else
    {
        unsigned char key[1 + MaxKeyLength];
        key[0] = 0;
        answer = root_->applyChildFirst(key, isEqual, const_cast<Trie*>(&trie));
    }

    return answer;
}


//!
//! Reset and copy the trie contents from given trie. Do not change
//! the digit range. If necessary, drop key-value pairs which are
//! inappropriate for this trie.
//!
const Trie& Trie::operator =(const Trie& trie)
{

    // Prevent self assignment.
    if (this != &trie)
    {

        // Populate this trie with key-value pairs from given trie.
        // If necessary, drop key-value pairs which are inappropriate
        // for this trie.
        if (maxDigit_ == trie.maxDigit_)
        {
            delete root_;
            numKvPairs_ = trie.numKvPairs_;
            numNodes_ = trie.numNodes_;
            root_ = trie.root_->clone();
        }
        else
        {
            reset();
            unsigned char key[1 + MaxKeyLength];
            key[0] = 0;
            trie.root_->applyChildFirst(key, addNode, this);
        }
    }

    // Return reference to self.
    return *this;
}


//
// Add given key-value pair. Both key and value must be non-null.
// Return NODE_ADDED if successful. Return zero if given key is invalid.
// Return the existing node in the trie if given key already exists.
//
Trie::Node* Trie::addNode(const unsigned char* k, void* v)
{

    // Locate given key starting from the root. If not found,
    // make leaf node and intermediate nodes (if any) for this
    // key-value pair. Return true to indicate success.
    const unsigned char* p = k + 1;
    const unsigned char* pEnd = p + k[0];
    Node* child = 0;
    Node* grandparent = 0;
    Node* parent = root_;
    for (; p < pEnd; ++p, grandparent = parent, parent = child)
    {
        child = parent->child(*p);
        if (child == 0)
        {

            // Invalid key?
            child = mkNodes(p, pEnd, v);
            if (child == 0)
            {
                break;
            }

            // Add new nodes to trie.
            Node* morph = parent->setChild(*p, child, maxDigit_);
            if (morph != parent)
            {
                (grandparent != 0)? grandparent->setChild(*(p - 1), morph, maxDigit_): root_ = morph;
            }
            ++numKvPairs_;
            numNodes_ += static_cast<unsigned int>(pEnd - p);
            return NODE_ADDED;
        }
    }

    // Key node already exists or invalid key.
    return child;
}


//
// Find the node holding given subkey. Return the root node if not found.
// Note that the root node's value can never be non-zero.
//
Trie::Node* Trie::findNode(const unsigned char* subkey) const
{
    Node* found = root_;
    if ((subkey != 0) && (subkey[0] != 0))
    {
        const unsigned char* p = subkey + 1;
        const unsigned char* pEnd = p + subkey[0];
        for (; p < pEnd; ++p)
        {
            found = found->child(*p);
            if (found == 0)
            {
                found = root_;
                break;
            }
        }
    }

    return found;
}


//
// Create nodes for given value. Return the root of the created nodes.
// Return zero if given key is invalid.
//
Trie::Node* Trie::mkNodes(const unsigned char* p, const unsigned char* pEnd, void* v) const
{

    // Create leaf node first to hold value.
    // Then create the intermediate parent nodes.
    bool ok = true;
    Node* child = new Node0(v);
    for (const unsigned char* d = pEnd - 1; d > p; --d)
    {
        if (*d <= maxDigit_)
        {
            Node* parent = new Node1(0, *d, child);
            child = parent;
        }
        else
        {
            ok = false;
            break;
        }
    }

    // Invalid key. Recursively delete the just created nodes.
    if ((!ok) || (*p > maxDigit_))
    {
        delete child;
        child = 0;
    }

    // Return the root of the created nodes.
    return child;
}


//
// Add given key-value pair. Both key and value must be non-null.
// Return true if successful. Return false otherwise (invalid key
// or key already exists).
//
bool Trie::addKv(const unsigned char* k, void* v, void*& foundV)
{
    foundV = 0;
    bool ok = false;

    // Add new node.
    Node* child = addNode(k, v);
    if (child == NODE_ADDED)
    {
        ok = true;
    }

    // Set value in existing key node if it does not have a value.
    else if (child != 0)
    {
        if (child->v() == 0)
        {
            ++numKvPairs_;
            child->setV(v);
            ok = true;
        }
        else //key already exists
        {
            foundV = child->v();
        }
    }

    // Return true if successful.
    return ok;
}


//!
//! Recursively iterate tree. Children first. Invoke callback at each key-value
//! pair. The callback should return true to continue iterating and should return
//! false to abort iterating. Return false if the callback aborted the iterating.
//! Return true otherwise.
//!
bool Trie::applyChildFirst(cb0_t cb, void* arg) const
{
    unsigned char k[1 + MaxKeyLength];
    k[0] = 0;
    bool ok = root_->applyChildFirst(k, cb, arg);
    return ok;
}


//!
//! Recursively iterate tree. Parent first. Invoke callback at each key-value
//! pair. The callback should return true to continue iterating and should return
//! false to abort iterating. Return false if the callback aborted the iterating.
//! Return true otherwise.
//!
bool Trie::applyParentFirst(cb0_t cb, void* arg) const
{
    unsigned char k[1 + MaxKeyLength];
    k[0] = 0;
    bool ok = root_->applyParentFirst(k, cb, arg);
    return ok;
}


//
// Associate given key-value pair. Both key and value must be non-null.
// Return true if successful. Return false otherwise (invalid key).
//
bool Trie::associateKv(const unsigned char* k, void* v, void*& oldV)
{

    // Add new node.
    bool ok;
    Node* child = addNode(k, v);
    if (child == NODE_ADDED)
    {
        ok = true;
    }

    // Key node already exists.
    else if (child != 0)
    {
        oldV = child->v();
        if (oldV == 0)
        {
            ++numKvPairs_;
        }
        child->setV(v);
        ok = true;
    }

    // Invalid key.
    else
    {
        ok = false;
    }

    // Return true if successful.
    return ok;
}


//!
//! Locate given key. Return true if found (also return the located
//! value in foundValue). Return false otherwise.
//!
bool Trie::find(const unsigned char* k, void*& foundV) const
{
    const Node* p = findNode(k);
    bool found = (p->v() != 0)? (foundV = p->v(), true): (false);
    return found;
}


//
// Look at arg as a trie pointer. Return true if the trie
// contains the given key-value pair. Return false otherwise.
//
bool Trie::isEqual(void* arg, const unsigned char* k, void* v)
{
    const Trie& trie = *static_cast<const Trie*>(arg);
    void* foundV;
    bool eq = trie.find(k, foundV) && (foundV == v);
    return eq;
}


//!
//! Return true if given key is a valid key for this trie. A key is valid for
//! this trie if it's non-null and if it contains at least one digit and if its
//! digits are within the valid range (i.e., not larger than maxDigit()).
//!
bool Trie::isValid(const unsigned char* k) const
{

    // A null pointer or an empty key is invalid.
    bool ok;
    if ((k == 0) || (k[0] == 0))
    {
        ok = false;
    }

    // A valid key must contain at least one digit and must
    // contain only valid digits. A valid digit must not be
    // larger than maxDigit_.
    else
    {
        ok = true;
        const unsigned char* p = k + 1;
        const unsigned char* pEnd = p + k[0];
        for (; p < pEnd; ++p)
        {
            if (*p > maxDigit_)
            {
                ok = false;
                break;
            }
        }
    }

    // Return true if given key is valid.
    return ok;
}


//!
//! Locate given key. If found, remove the key-value pair from
//! the trie and return true (also return the removed value in
//! removedV). Return false otherwise.
//!
bool Trie::rm(const unsigned char* k, void*& removedV)
{

    // Sanity check.
    if ((k == 0) || (k[0] == 0))
    {
        return false;
    }

    // Locate key. Keep track of the surviving nodes so they
    // are not removed. A parent node survives the removal if
    // it holds a value or if it has another child.
    const unsigned char* p = k + 1;
    const unsigned char* pEnd = p + k[0];
    const unsigned char* deadLink = p;
    Node* child = 0;
    Node* grandparent = 0;
    Node* parent = root_;
    Node* survivor = root_;
    Node* survivorParent = 0;
    for (; p < pEnd; ++p, grandparent = parent, parent = child)
    {
        child = parent->child(*p);
        if (child == 0)
        {
            break;
        }
        if ((parent->v() != 0) || (parent->numChildren() > 1))
        {
            survivor = parent;
            survivorParent = grandparent;
            deadLink = p;
        }
    }

    // Key found. Removing a non-leaf node is quite simple. Just set
    // the value to zero and update the key-value pair count. Removing
    // a leaf node is a little more complex. Make sure the surviving
    // nodes are not removed. Also be careful due to morphing. A parent
    // node might morph into a leaf node when its only child is removed.
    bool ok;
    if ((child != 0) && (child->v() != 0))
    {
        ok = true;
        removedV = child->v();
        if (child->isLeaf())
        {
            Node* removedChild;
            Node* morph = survivor->rmChild(*deadLink, removedChild);
            if (morph != survivor)
            {
                (survivorParent != 0)?
                    survivorParent->setChild(*(deadLink - 1), morph, maxDigit_):
                    root_ = morph;
            }
            delete removedChild;
            numNodes_ -= static_cast<unsigned int>(pEnd - deadLink);
        }
        else
        {
            child->setV(0);
        }
        --numKvPairs_;
    }

    // Key not found.
    else
    {
        ok = false;
    }

    // Return true if successful.
    return ok;
}


//!
//! Count and return the number of key-value pairs this subtrie
//! holds. A subtrie is identified by a subkey which can be a key
//! or a non-emtpy key prefix.
//!
unsigned int Trie::countKvPairs(const unsigned char* subkey) const
{
    const Node* p = findNode(subkey);
    size_t cumKvPairs = 0;
    p->countNodes(cumKvPairs);
    return static_cast<unsigned int>(cumKvPairs);
}


//!
//! Remove all key-value pairs held in given subtrie. A subtrie is
//! identified by a subkey which can be a key or a non-empty key
//! prefix. Return the number of removed key-value pairs.
//!
unsigned int Trie::rmAll(const unsigned char* subkey)
{

    // Sanity check.
    if ((subkey == 0) || (subkey[0] == 0))
    {
        return 0;
    }

    // Locate subkey. Keep track of the surviving nodes so they
    // are not removed. A parent node survives the removal if it
    // holds a value or if it has another child.
    const unsigned char* p = subkey + 1;
    const unsigned char* pEnd = p + subkey[0];
    const unsigned char* deadLink = p;
    Node* child = 0;
    Node* grandparent = 0;
    Node* parent = root_;
    Node* survivor = root_;
    Node* survivorParent = 0;
    for (; p < pEnd; ++p, grandparent = parent, parent = child)
    {
        child = parent->child(*p);
        if (child == 0)
        {
            break;
        }
        if ((parent->v() != 0) || (parent->numChildren() > 1))
        {
            survivor = parent;
            survivorParent = grandparent;
            deadLink = p;
        }
    }

    // Subkey found. Remove located node and everything below and
    // possibly a few unnecessary nodes above. Make sure the surviving
    // nodes are not removed. Also be careful due to morphing. A parent
    // node might morph into a leaf node when its only child is removed.
    unsigned int numKvPairsRemoved = 0;
    if (child != 0)
    {
        Node* removedChild;
        Node* morph = survivor->rmChild(*deadLink, removedChild);
        if (morph != survivor)
        {
            (survivorParent != 0)?
                survivorParent->setChild(*(deadLink - 1), morph, maxDigit_):
                root_ = morph;
        }
        size_t pairCount = 0;
        numNodes_ -= removedChild->countNodes(pairCount);
        numKvPairsRemoved = static_cast<unsigned int>(pairCount);
        numKvPairs_ -= numKvPairsRemoved;
        delete removedChild;
    }

    // Return the number of removed key-value pairs.
    return numKvPairsRemoved;
}


//
// Look at arg as a trie pointer. Add given key-value pair
// to the trie. Ignore errors (i.e., it's okay if a key-value pair
// is not added successfully due to the key's digit range).
//
void Trie::addNode(void* arg, const unsigned char* k, void* v)
{
    void* foundV;
    Trie& trie = *static_cast<Trie*>(arg);
    trie.addKv(k, v, foundV);
}


//!
//! Recursively iterate trie. Children first.
//! Invoke callback at each key-value pair.
//!
void Trie::applyChildFirst(cb1_t cb, void* arg) const
{
    unsigned char k[1 + MaxKeyLength];
    k[0] = 0;
    root_->applyChildFirst(k, cb, arg);
}


//!
//! Recursively iterate trie. Parent first.
//! Invoke callback at each key-value pair.
//!
void Trie::applyParentFirst(cb1_t cb, void* arg) const
{
    unsigned char k[1 + MaxKeyLength];
    k[0] = 0;
    root_->applyParentFirst(k, cb, arg);
}


//!
//! Reset the trie by recursively removing all key-value pairs.
//!
void Trie::reset()
{
    if (numKvPairs_ > 0)
    {
        delete root_;
        numKvPairs_ = 0;
        numNodes_ = 1;
        root_ = new Node0;
    }
}


//
// Construct a node with given value.
//
Trie::Node::Node(void* v)
{
    v_ = v;
}


Trie::Node::~Node()
{
}


//
// Construct a leaf node with given value.
//
Trie::Node0::Node0(void* v):
Node(v)
{
}


Trie::Node0::~Node0()
{
}


//
// Return the child at given digit. Return zero if none. Since a leaf
// node has no children, this method is a no-op, and zero is returned.
//
Trie::Node* Trie::Node0::child(unsigned char /*digit*/) const
{
    return 0;
}


Trie::Node* Trie::Node0::clone() const
{
    Node* cloned = new Node0(v());
    return cloned;
}


//
// Remove the child at given digit. Return self or the new image
// if morphed. Since a leaf node has no children, this method is
// invalid.
//
Trie::Node* Trie::Node0::rmChild(unsigned char /*digit*/, Node*& /*removedChild*/)
{
    return 0;
}


//
// Add/Replace given child at given digit. Return self or the new
// image if morphed. Since a leaf node has no children, it will
// morph into a Node1 instance to accomodate the request, and the
// new image is returned.
//
Trie::Node* Trie::Node0::setChild(unsigned char digit, Node* child, unsigned char /*maxDigit*/)
{
    Node* morph = new Node1(v(), digit, child);
    delete this;
    return morph;
}


//
// Recursively invoke callback at each key-value pair. Iterate
// children before parent. The callback returns true to continue
// iterating and returns false to abort iterating. Return false
// if the callback aborted the iterating. Return true otherwise.
//
bool Trie::Node0::applyChildFirst(unsigned char* k, cb0_t cb, void* arg) const
{
    void* v = Node::v();
    bool ok = (v != 0)? cb(arg, k, v): true;
    return ok;
}


//
// Recursively invoke callback at each key-value pair. Iterate
// parent before children. The callback returns true to continue
// iterating and returns false to abort iterating. Return false
// if the callback aborted the iterating. Return true otherwise.
//
bool Trie::Node0::applyParentFirst(unsigned char* k, cb0_t cb, void* arg) const
{
    void* v = Node::v();
    bool ok = (v != 0)? cb(arg, k, v): true;
    return ok;
}


//
// Return true if this is a leaf node.
//
bool Trie::Node0::isLeaf() const
{
    return true;
}


//
// Recursively count the number of nodes and accumulate the number of
// key-value pairs this subtrie holds. Maintain the cumulative number
// of key-value pairs in cumKvPairs. Return the number of nodes
// in subtrie including self.
//
unsigned int Trie::Node0::countNodes(size_t& cumKvPairs) const
{
    if (v() != 0)
    {
        ++cumKvPairs;
    }

    return 1;
}


//
// Return the number of children this node has.
//
unsigned int Trie::Node0::numChildren() const
{
    return 0;
}


//
// Recursively invoke callback at each key-value pair.
// Iterate children before parent.
//
void Trie::Node0::applyChildFirst(unsigned char* k, cb1_t cb, void* arg) const
{
    void* v = Node::v();
    if (v != 0)
    {
        cb(arg, k, v);
    }
}


//
// Recursively invoke callback at each key-value pair.
// Iterate parent before children.
//
void Trie::Node0::applyParentFirst(unsigned char* k, cb1_t cb, void* arg) const
{
    void* v = Node::v();
    if (v != 0)
    {
        cb(arg, k, v);
    }
}


//
// Construct a 1-child node. The given child resides at the given digit.
//
Trie::Node1::Node1(void* v, unsigned char digit, Node* child):
Node(v)
{
    child_[0] = child;
    digit_[0] = digit;
}


//
// Destruct node.
//
Trie::Node1::~Node1()
{
    delete child_[0];
}


//
// Return the child at given digit. Return zero if none.
//
Trie::Node* Trie::Node1::child(unsigned char digit) const
{
    return (digit == digit_[0])? child_[0]: 0;
}


Trie::Node* Trie::Node1::clone() const
{
    Node* child0 = child_[0]->clone();
    Node* cloned = new Node1(v(), digit_[0], child0);
    return cloned;
}


//
// Remove the child at given digit. Return self or the new image
// if morphed. This node will morph into a Node0 instance, and
// the new image is returned.
//
Trie::Node* Trie::Node1::rmChild(unsigned char /*digit*/, Node*& removedChild)
{
    Node* morph = new Node0(v());
    removedChild = child_[0];
    child_[0] = 0;
    delete this;
    return morph;
}


//
// Add/Replace given child at given digit. Return self or the new
// image if morphed. To accomodate the add request, this node will
// morph into a Node4 instance, and the new image is returned.
//
Trie::Node* Trie::Node1::setChild(unsigned char digit, Node* child, unsigned char /*maxDigit*/)
{

    // Replace only child?
    if (digit == digit_[0])
    {
        child_[0] = child;
        return this;
    }

    // Morph into a Node4 instance to add new child.
    Node* morph = (digit > digit_[0])?
        new Node4(v(), digit_[0], child_[0], digit, child):
        new Node4(v(), digit, child, digit_[0], child_[0]);
    child_[0] = 0;
    delete this;
    return morph;
}


//
// Recursively invoke callback at each key-value pair. Iterate
// children before parent. The callback returns true to continue
// iterating and returns false to abort iterating. Return false
// if the callback aborted the iterating. Return true otherwise.
//
bool Trie::Node1::applyChildFirst(unsigned char* k, cb0_t cb, void* arg) const
{
    unsigned int numDigits = ++*k;
    k[numDigits] = digit_[0];
    bool ok = child_[0]->applyChildFirst(k, cb, arg);
    --*k;

    void* v = Node::v();
    if (ok && (v != 0))
    {
        ok = cb(arg, k, v);
    }

    return ok;
}


//
// Recursively invoke callback at each key-value pair. Iterate
// parent before children. The callback returns true to continue
// iterating and returns false to abort iterating. Return false
// if the callback aborted the iterating. Return true otherwise.
//
bool Trie::Node1::applyParentFirst(unsigned char* k, cb0_t cb, void* arg) const
{
    void* v = Node::v();
    if (v != 0)
    {
        if (!cb(arg, k, v))
        {
            return false;
        }
    }

    unsigned int numDigits = ++*k;
    k[numDigits] = digit_[0];
    bool ok = child_[0]->applyParentFirst(k, cb, arg);
    --*k;

    return ok;
}


//
// Return true if this is a leaf node.
//
bool Trie::Node1::isLeaf() const
{
    return false;
}


//
// Recursively count the number of nodes and accumulate the number of
// key-value pairs this subtrie holds. Maintain the cumulative number
// of key-value pairs in cumKvPairs. Return the number of nodes
// in subtrie including self.
//
unsigned int Trie::Node1::countNodes(size_t& cumKvPairs) const
{
    unsigned int numNodes = child_[0]->countNodes(cumKvPairs);
    if (v() != 0)
    {
        ++cumKvPairs;
    }

    return numNodes + 1;
}


//
// Return the number of children this node has.
//
unsigned int Trie::Node1::numChildren() const
{
    return 1;
}


//
// Recursively invoke callback at each key-value pair.
// Iterate children before parent.
//
void Trie::Node1::applyChildFirst(unsigned char* k, cb1_t cb, void* arg) const
{
    unsigned int numDigits = ++*k;
    k[numDigits] = digit_[0];
    child_[0]->applyChildFirst(k, cb, arg);
    --*k;

    void* v = Node::v();
    if (v != 0)
    {
        cb(arg, k, v);
    }
}


//
// Recursively invoke callback at each key-value pair.
// Iterate parent before children.
//
void Trie::Node1::applyParentFirst(unsigned char* k, cb1_t cb, void* arg) const
{
    void* v = Node::v();
    if (v != 0)
    {
        cb(arg, k, v);
    }

    unsigned int numDigits = ++*k;
    k[numDigits] = digit_[0];
    child_[0]->applyParentFirst(k, cb, arg);
    --*k;
}


//
// Construct a node with at least 2 and up to 4 children.
// Start with the given four children.
//
Trie::Node4::Node4(void* v, unsigned char const digit[4], Node* const child[4]):
Node(v)
{
    child_[0] = child[0];
    child_[1] = child[1];
    child_[2] = child[2];
    child_[3] = child[3];
    digit_[0] = digit[0];
    digit_[1] = digit[1];
    digit_[2] = digit[2];
    digit_[3] = digit[3];
}


//
// Construct a node with at least 2 and up to 4 children.
// Start with the given two children.
//
Trie::Node4::Node4(void* v, unsigned char digit0, Node* child0, unsigned char digit1, Node* child1):
Node(v)
{
    child_[0] = child0;
    child_[1] = child1;
    child_[2] = 0;
    child_[3] = 0;
    digit_[0] = digit0;
    digit_[1] = digit1;
    digit_[2] = 0;
    digit_[3] = 0;
}


Trie::Node4::~Node4()
{
    delete child_[3];
    delete child_[2];
    delete child_[1];
    delete child_[0];
}


//
// Return the child at given digit. Return zero if none.
//
Trie::Node* Trie::Node4::child(unsigned char digit) const
{
    if (digit <= digit_[0]) return (digit == digit_[0])? child_[0]: 0;
    if (digit <= digit_[1]) return (digit == digit_[1])? child_[1]: 0;
    if (digit <= digit_[2]) return (digit == digit_[2])? child_[2]: 0;
    return (digit == digit_[3])? child_[3]: 0;
}


Trie::Node* Trie::Node4::clone() const
{
    Node* child[4];
    child[0] = child_[0]->clone();
    child[1] = child_[1]->clone();
    child[2] = (child_[2] != 0)? child_[2]->clone(): 0;
    child[3] = (child_[3] != 0)? child_[3]->clone(): 0;
    Node* cloned = new Node4(v(), digit_, child);
    return cloned;
}


//
// Remove the child at given digit. Return self or the new image if
// morphed. This node might morph into a Node1 instance, and the new
// image would be returned.
//
Trie::Node* Trie::Node4::rmChild(unsigned char digit, Node*& removedChild)
{

    // Three children remain.
    if (child_[3] != 0)
    {
        if (digit == digit_[0])
        {
            removedChild = child_[0];
            child_[0] = child_[1];
            digit_[0] = digit_[1];
            child_[1] = child_[2];
            digit_[1] = digit_[2];
            child_[2] = child_[3];
            digit_[2] = digit_[3];
        }
        else if (digit == digit_[1])
        {
            removedChild = child_[1];
            child_[1] = child_[2];
            digit_[1] = digit_[2];
            child_[2] = child_[3];
            digit_[2] = digit_[3];
        }
        else if (digit == digit_[2])
        {
            removedChild = child_[2];
            child_[2] = child_[3];
            digit_[2] = digit_[3];
        }
        else
        {
            removedChild = child_[3];
        }
        child_[3] = 0;
        digit_[3] = 0;
        return this;
    }

    // Two children remain.
    if (child_[2] != 0)
    {
        if (digit == digit_[0])
        {
            removedChild = child_[0];
            child_[0] = child_[1];
            digit_[0] = digit_[1];
            child_[1] = child_[2];
            digit_[1] = digit_[2];
        }
        else if (digit == digit_[1])
        {
            removedChild = child_[1];
            child_[1] = child_[2];
            digit_[1] = digit_[2];
        }
        else
        {
            removedChild = child_[2];
        }
        child_[2] = 0;
        digit_[2] = 0;
        return this;
    }

    // One child remains.
    // Morph into a Node1 instance.
    Node* morph;
    if (digit == digit_[0])
    {
        removedChild = child_[0];
        morph = new Node1(v(), digit_[1], child_[1]);
    }
    else
    {
        removedChild = child_[1];
        morph = new Node1(v(), digit_[0], child_[0]);
    }
    child_[0] = 0;
    child_[1] = 0;
    delete this;
    return morph;
}


//
// Add/Replace given child at given digit. Return self or the new
// image if morphed. To accomodate the add request, this node might
// morph into a highest-capacity instance, and the new image would
// be returned.
//
Trie::Node* Trie::Node4::setChild(unsigned char digit, Node* child, unsigned char maxDigit)
{

    // Replace child at given digit.
    unsigned int i = 4;
    if (digit == digit_[0]) i = 0;
    else if (digit == digit_[1]) i = 1;
    else if (digit == digit_[2]) i = 2;
    else if (digit == digit_[3]) i = 3;
    if ((i != 4) && (child_[i] != 0))
    {
        child_[i] = child;
        return this;
    }

    // Morph into a highest-capacity instance since this node is full.
    if (child_[3] != 0)
    {
        Node* morph = new NodeN(v(), digit_, child_, digit, child, maxDigit);
        child_[0] = 0;
        child_[1] = 0;
        child_[2] = 0;
        child_[3] = 0;
        delete this;
        return morph;
    }

    // Add third child.
    if (child_[2] == 0)
    {
        if (digit < digit_[0])
        {
            child_[2] = child_[1];
            digit_[2] = digit_[1];
            child_[1] = child_[0];
            digit_[1] = digit_[0];
            child_[0] = child;
            digit_[0] = digit;
        }
        else if (digit < digit_[1])
        {
            child_[2] = child_[1];
            digit_[2] = digit_[1];
            child_[1] = child;
            digit_[1] = digit;
        }
        else
        {
            child_[2] = child;
            digit_[2] = digit;
        }
        return this;
    }

    // Add fourth child.
    if (digit < digit_[0])
    {
        child_[3] = child_[2];
        digit_[3] = digit_[2];
        child_[2] = child_[1];
        digit_[2] = digit_[1];
        child_[1] = child_[0];
        digit_[1] = digit_[0];
        child_[0] = child;
        digit_[0] = digit;
    }
    else if (digit < digit_[1])
    {
        child_[3] = child_[2];
        digit_[3] = digit_[2];
        child_[2] = child_[1];
        digit_[2] = digit_[1];
        child_[1] = child;
        digit_[1] = digit;
    }
    else if (digit < digit_[2])
    {
        child_[3] = child_[2];
        digit_[3] = digit_[2];
        child_[2] = child;
        digit_[2] = digit;
    }
    else
    {
        child_[3] = child;
        digit_[3] = digit;
    }
    return this;
}


//
// Recursively invoke callback at each key-value pair. Iterate
// children before parent. The callback returns true to continue
// iterating and returns false to abort iterating. Return false
// if the callback aborted the iterating. Return true otherwise.
//
bool Trie::Node4::applyChildFirst(unsigned char* k, cb0_t cb, void* arg) const
{
    unsigned int numDigits = ++*k;
    k[numDigits] = digit_[0];
    bool ok = child_[0]->applyChildFirst(k, cb, arg);
    if (ok)
    {
        k[numDigits] = digit_[1];
        ok = child_[1]->applyChildFirst(k, cb, arg);
        if (ok && (child_[2] != 0))
        {
            k[numDigits] = digit_[2];
            ok = child_[2]->applyChildFirst(k, cb, arg);
            if (ok && (child_[3] != 0))
            {
                k[numDigits] = digit_[3];
                ok = child_[3]->applyChildFirst(k, cb, arg);
            }
        }
    }
    --*k;

    void* v = Node::v();
    if (ok && (v != 0))
    {
        ok = cb(arg, k, v);
    }

    return ok;
}


//
// Recursively invoke callback at each key-value pair. Iterate
// parent before children. The callback returns true to continue
// iterating and returns false to abort iterating. Return false
// if the callback aborted the iterating. Return true otherwise.
//
bool Trie::Node4::applyParentFirst(unsigned char* k, cb0_t cb, void* arg) const
{
    void* v = Node::v();
    if (v != 0)
    {
        if (!cb(arg, k, v))
        {
            return false;
        }
    }

    unsigned int numDigits = ++*k;
    k[numDigits] = digit_[0];
    bool ok = child_[0]->applyParentFirst(k, cb, arg);
    if (ok)
    {
        k[numDigits] = digit_[1];
        ok = child_[1]->applyParentFirst(k, cb, arg);
        if (ok && (child_[2] != 0))
        {
            k[numDigits] = digit_[2];
            ok = child_[2]->applyParentFirst(k, cb, arg);
            if (ok && (child_[3] != 0))
            {
                k[numDigits] = digit_[3];
                ok = child_[3]->applyParentFirst(k, cb, arg);
            }
        }
    }
    --*k;

    return ok;
}


//
// Return true if this is a leaf node.
//
bool Trie::Node4::isLeaf() const
{
    return false;
}


//
// Recursively count the number of nodes and accumulate the number of
// key-value pairs this subtrie holds. Maintain the cumulative number
// of key-value pairs in cumKvPairs. Return the number
// of nodes in subtrie including self.
//
unsigned int Trie::Node4::countNodes(size_t& cumKvPairs) const
{
    unsigned int numNodes = child_[0]->countNodes(cumKvPairs);
    numNodes += child_[1]->countNodes(cumKvPairs);
    if (child_[2] != 0)
    {
        numNodes += child_[2]->countNodes(cumKvPairs);
        if (child_[3] != 0)
        {
            numNodes += child_[3]->countNodes(cumKvPairs);
        }
    }

    if (v() != 0)
    {
        ++cumKvPairs;
    }

    return numNodes + 1;
}


//
// Return the number of children this node has.
//
unsigned int Trie::Node4::numChildren() const
{
    if (child_[3] != 0) return 4;
    if (child_[2] != 0) return 3;
    return 2;
}


//
// Recursively invoke callback at each key-value pair.
// Iterate children before parent.
//
void Trie::Node4::applyChildFirst(unsigned char* k, cb1_t cb, void* arg) const
{
    unsigned int numDigits = ++*k;
    k[numDigits] = digit_[0];
    child_[0]->applyChildFirst(k, cb, arg);
    k[numDigits] = digit_[1];
    child_[1]->applyChildFirst(k, cb, arg);
    if (child_[2] != 0)
    {
        k[numDigits] = digit_[2];
        child_[2]->applyChildFirst(k, cb, arg);
        if (child_[3] != 0)
        {
            k[numDigits] = digit_[3];
            child_[3]->applyChildFirst(k, cb, arg);
        }
    }
    --*k;

    void* v = Node::v();
    if (v != 0)
    {
        cb(arg, k, v);
    }
}


//
// Recursively invoke callback at each key-value pair.
// Iterate parent before children.
//
void Trie::Node4::applyParentFirst(unsigned char* k, cb1_t cb, void* arg) const
{
    void* v = Node::v();
    if (v != 0)
    {
        cb(arg, k, v);
    }

    unsigned int numDigits = ++*k;
    k[numDigits] = digit_[0];
    child_[0]->applyParentFirst(k, cb, arg);
    k[numDigits] = digit_[1];
    child_[1]->applyParentFirst(k, cb, arg);
    if (child_[2] != 0)
    {
        k[numDigits] = digit_[2];
        child_[2]->applyParentFirst(k, cb, arg);
        if (child_[3] != 0)
        {
            k[numDigits] = digit_[3];
            child_[3]->applyParentFirst(k, cb, arg);
        }
    }
    --*k;
}


Trie::NodeN::NodeN(const NodeN& node):
Node(node.v())
{
    maxDigit_ = node.maxDigit_;
    numChildren_ = node.numChildren_;

    const Node* const* src = node.child_;
    child_ = new Node*[maxDigit_ + 1];
    for (unsigned int i = 0; i <= maxDigit_; ++i)
    {
        child_[i] = (src[i] != 0)? src[i]->clone(): 0;
    }
}


//
// Construct a highest-capacity node capable of (maxDigit+1) children.
// Start with the given five children.
//
Trie::NodeN::NodeN(void* v,
    unsigned char const digit[4],
    Node* const child[4],
    unsigned char digit4,
    Node* child4,
    unsigned char maxDigit):
    Node(v)
{
    unsigned int maxChildren = maxDigit + 1;
    child_ = new Node*[maxChildren];
    memset(child_, 0, sizeof(*child_) * maxChildren);
    child_[digit[0]] = child[0];
    child_[digit[1]] = child[1];
    child_[digit[2]] = child[2];
    child_[digit[3]] = child[3];
    child_[digit4] = child4;

    maxDigit_ = maxDigit;
    numChildren_ = 5;
}


Trie::NodeN::~NodeN()
{

    // A NodeN instance can be destructed when it has no children (e.g., all
    // children have been transferred to a Node4 instance in a remove operation).
    if (numChildren_ > 0)
    {
        for (const Node* const* pp = child_;; ++pp)
        {
            const Node* p = *pp;
            if (p != 0)
            {
                delete p;
                if (--numChildren_ == 0)
                {
                    break;
                }
            }
        }
    }

    delete[] child_;
}


//
// Return the child at given digit. Return zero if none.
//
Trie::Node* Trie::NodeN::child(unsigned char digit) const
{
    return (digit <= maxDigit_)? child_[digit]: 0;
}


Trie::Node* Trie::NodeN::clone() const
{
    Node* cloned = new NodeN(*this);
    return cloned;
}


//
// Remove the child at given digit. Return self or the new image if
// morphed. This node might morph into a Node4 instance, and the new
// image would be returned.
//
Trie::Node* Trie::NodeN::rmChild(unsigned char digit, Node*& removedChild)
{
    removedChild = child_[digit];
    if (--numChildren_ == 4)
    {
        child_[digit] = 0;
        Node* child[4];
        unsigned char digit[4];
        unsigned int i = 0;
        for (Node** pp = child_;; ++pp)
        {
            Node* p = *pp;
            if (p != 0)
            {
                digit[i] = static_cast<unsigned char>(pp - child_);
                child[i] = p;
                ++i;
                *pp = 0;
                if (--numChildren_ == 0)
                {
                    break;
                }
            }
        }
        Node* morph = new Node4(v(), digit, child);
        delete this;
        return morph;
    }

    child_[digit] = 0;
    return this;
}


//
// Add/Replace given child at given digit. Return self or the new
// image if morphed. Since this is already a highest-capacity node,
// no morphing will occur.
//
Trie::Node* Trie::NodeN::setChild(unsigned char digit, Node* child, unsigned char /*maxDigit*/)
{
    if (child_[digit] == 0)
    {
        ++numChildren_;
    }

    child_[digit] = child;
    return this;
}


//
// Recursively invoke callback at each key-value pair. Iterate
// children before parent. The callback returns true to continue
// iterating and returns false to abort iterating. Return false
// if the callback aborted the iterating. Return true otherwise.
//
bool Trie::NodeN::applyChildFirst(unsigned char* k, cb0_t cb, void* arg) const
{
    bool ok = true;
    unsigned int numDigits = ++*k;
    unsigned int childrenCount = numChildren_;
    for (Node* const* pp = child_;; ++pp)
    {
        const Node* p = *pp;
        if (p != 0)
        {
            k[numDigits] = static_cast<unsigned char>(pp - child_);
            ok = p->applyChildFirst(k, cb, arg);
            if ((!ok) || (--childrenCount == 0))
            {
                break;
            }
        }
    }
    --*k;

    void* v = Node::v();
    if (ok && (v != 0))
    {
        ok = cb(arg, k, v);
    }

    return ok;
}


//
// Recursively invoke callback at each key-value pair. Iterate
// parent before children. The callback returns true to continue
// iterating and returns false to abort iterating. Return false
// if the callback aborted the iterating. Return true otherwise.
//
bool Trie::NodeN::applyParentFirst(unsigned char* k, cb0_t cb, void* arg) const
{
    void* v = Node::v();
    if (v != 0)
    {
        if (!cb(arg, k, v))
        {
            return false;
        }
    }

    bool ok = true;
    unsigned int numDigits = ++*k;
    unsigned int childrenCount = numChildren_;
    for (Node* const* pp = child_;; ++pp)
    {
        const Node* p = *pp;
        if (p != 0)
        {
            k[numDigits] = static_cast<unsigned char>(pp - child_);
            ok = p->applyParentFirst(k, cb, arg);
            if ((!ok) || (--childrenCount == 0))
            {
                break;
            }
        }
    }
    --*k;

    return ok;
}


//
// Return true if this is a leaf node.
//
bool Trie::NodeN::isLeaf() const
{
    return false;
}


//
// Recursively count the number of nodes and accumulate the number of
// key-value pairs this subtrie holds. Maintain the cumulative number
// of key-value pairs in cumKvPairs. Return the number of nodes
// in subtrie including self.
//
unsigned int Trie::NodeN::countNodes(size_t& cumKvPairs) const
{
    unsigned int numNodes = 0;
    unsigned int childrenCount = numChildren_;
    for (const Node* const* pp = child_;; ++pp)
    {
        const Node* p = *pp;
        if (p != 0)
        {
            numNodes += p->countNodes(cumKvPairs);
            if (--childrenCount == 0)
            {
                break;
            }
        }
    }

    if (v() != 0)
    {
        ++cumKvPairs;
    }

    return numNodes + 1;
}


//
// Return the number of children this node has.
//
unsigned int Trie::NodeN::numChildren() const
{
    return numChildren_;
}


//
// Recursively invoke callback at each key-value pair.
// Iterate children before parent.
//
void Trie::NodeN::applyChildFirst(unsigned char* k, cb1_t cb, void* arg) const
{
    unsigned int numDigits = ++*k;
    unsigned int childrenCount = numChildren_;
    for (Node* const* pp = child_;; ++pp)
    {
        const Node* p = *pp;
        if (p != 0)
        {
            k[numDigits] = static_cast<unsigned char>(pp - child_);
            p->applyChildFirst(k, cb, arg);
            if (--childrenCount == 0)
            {
                break;
            }
        }
    }
    --*k;

    void* v = Node::v();
    if (v != 0)
    {
        cb(arg, k, v);
    }
}


//
// Recursively invoke callback at each key-value pair.
// Iterate parent before children.
//
void Trie::NodeN::applyParentFirst(unsigned char* k, cb1_t cb, void* arg) const
{
    void* v = Node::v();
    if (v != 0)
    {
        cb(arg, k, v);
    }

    unsigned int numDigits = ++*k;
    unsigned int childrenCount = numChildren_;
    for (Node* const* pp = child_;; ++pp)
    {
        const Node* p = *pp;
        if (p != 0)
        {
            k[numDigits] = static_cast<unsigned char>(pp - child_);
            p->applyParentFirst(k, cb, arg);
            if (--childrenCount == 0)
            {
                break;
            }
        }
    }
    --*k;
}


//
// For type Hex or HEX, encoding is case-insensitive, but
// decoding can return a lower or uppercase hexadecimal string.
Trie::StrKey::decoder_t Trie::StrKey::decode_[] =
{
    &Trie::StrKey::decodeAscii,
    &Trie::StrKey::decodeDec,
    &Trie::StrKey::decodeHEX,
    &Trie::StrKey::decodeHex
};

Trie::StrKey::encoder_t Trie::StrKey::encode_[] =
{
    &Trie::StrKey::encodeAscii,
    &Trie::StrKey::encodeDec,
    &Trie::StrKey::encodeHex,
    &Trie::StrKey::encodeHex
};


//!
//! Construct a duplicate instance of the given key.
//!
Trie::StrKey::StrKey(const StrKey& key)
{
    unsigned int keySize = key.k_[0] + 1;
    k_ = new unsigned char[keySize];
    memcpy(k_, key.k_, keySize);
}


//!
//! Construct instance w/ given native key key. For type Hex or HEX, encoding
//! is case-insensitive. For example, "abc123" and "ABc123" would result in an
//! identical normalized key.
//!
Trie::StrKey::StrKey(type_e type, const char* key)
{
    unsigned char n = static_cast<unsigned char>(strlen(key));
    k_ = new unsigned char[n + 1];
    encode_[type](k_, key, n);
}


//!
//! Construct instance w/ given native key (n characters starting at key).
//! For type Hex or HEX, encoding is case-insensitive. For example, "abc123"
//! and "ABc123" would result in an identical normalized key.
//!
Trie::StrKey::StrKey(type_e type, const char* key, unsigned char n)
{
    k_ = new unsigned char[n + 1];
    encode_[type](k_, key, n);
}


Trie::StrKey::~StrKey()
{
    delete[] k_;
}


const Trie::StrKey& Trie::StrKey::operator =(const StrKey& key)
{

    // Prevent self assignment.
    if (this != &key)
    {
        if (k_[0] != key.k_[0])
        {
            delete[] k_;
            k_ = new unsigned char[key.k_[0] + 1];
        }
        memcpy(k_, key.k_, key.k_[0] + 1);
    }

    // Return reference to self.
    return *this;
}


//
// Decode normalized key k. Save result in native key key. Return the
// native key key. Allow the same buffer to be used for the keys.
//
char* Trie::StrKey::decodeAscii(char* key, const unsigned char* k)
{
    unsigned int n = k[0];
    memmove(key, k + 1, n);
    key[n] = 0;
    return key;
}


//
// Decode normalized key k. Save result in native key key. Return the
// native key key. Allow the same buffer to be used for the keys.
//
char* Trie::StrKey::decodeDec(char* key, const unsigned char* k)
{
    char* s = key;
    char* sEnd = s + k[0];
    const unsigned char* p = k + 1;
    for (; s < sEnd; ++s, ++p)
    {
        *s = *p + '0';
    }
    *s = 0;
    return key;
}


//
// Decode normalized key k. Save result in native key key. Return the
// native key key. Allow the same buffer to be used for the keys.
//
char* Trie::StrKey::decodeHEX(char* key, const unsigned char* k)
{
    char* s = key;
    char* sEnd = s + k[0];
    const unsigned char* p = k + 1;
    for (; s < sEnd; ++s, ++p)
    {
        *s = syskit::XDIGIT[*p];
    }
    *s = 0;
    return key;
}


//
// Decode normalized key k. Save result in native key key. Return the
// native key key. Allow the same buffer to be used for the keys.
//
char* Trie::StrKey::decodeHex(char* key, const unsigned char* k)
{
    char* s = key;
    char* sEnd = s + k[0];
    const unsigned char* p = k + 1;
    for (; s < sEnd; ++s, ++p)
    {
        *s = syskit::xdigit[*p];
    }
    *s = 0;
    return key;
}


//
// Encode native key (n characters starting at key). Save result in
// normalized key k. Return the normalized key k. Allow the same buffer
// to be used for the keys.
//
unsigned char* Trie::StrKey::encodeAscii(unsigned char* k, const char* key, unsigned char n)
{
    memmove(k + 1, key, n);
    k[0] = n;
    return k;
}


//
// Encode native key (n characters starting at key). Save result in
// normalized key k. Return the normalized key k. Allow the same buffer
// to be used for the keys.
//
unsigned char* Trie::StrKey::encodeDec(unsigned char* k, const char* key, unsigned char n)
{
    const char* s = key + n - 1;
    unsigned char* p = k + n;
    for (; p > k; --p, --s)
    {
        *p = *s - '0';
    }
    *p = n;
    return k;
}


//
// Encode native key (n characters starting at key). Save result in
// normalized key k. Return the normalized key k. Allow the same buffer
// to be used for the keys.
//
unsigned char* Trie::StrKey::encodeHex(unsigned char* k, const char* key, unsigned char n)
{
    const char* s = key + n - 1;
    unsigned char* p = k + n;
    for (; p > k; --p, --s)
    {
        *p = syskit::NIBBLE[*s - '0'];
    }
    *p = n;
    return k;
}


//!
//! Reset instance w/ given native key. For type Hex or HEX, encoding is
//! case-insensitive. For example, "abc123" and "ABc123" would result in
//! an identical normalized key.
//!
void Trie::StrKey::reset(type_e type, const char* key)
{
    unsigned char n = static_cast<unsigned char>(strlen(key));
    if (n != k_[0])
    {
        delete[] k_;
        k_ = new unsigned char[n + 1];
    }

    encode_[type](k_, key, n);
}


//!
//! Reset instance w/ given native key (n characters starting at key).
//! For type Hex or HEX, encoding is case-insensitive. For example,
//! "abc123" and "ABc123" would result in an identical normalized key.
//!
void Trie::StrKey::reset(type_e type, const char* key, unsigned char n)
{
    if (n != k_[0])
    {
        delete[] k_;
        k_ = new unsigned char[n + 1];
    }

    encode_[type](k_, key, n);
}


//!
//! Normalize the native key. Save result in the normalized key k.
//! Return the normalized key k.
//!
unsigned char* Trie::U16Key::encode(unsigned char k[1 + 4], unsigned short key)
{
    unsigned char* p = k + 4;
    for (; p > k; key >>= 4, --p)
    {
        *p = static_cast<unsigned char>(key & 0x0fUL);
    }
    *p = 4;
    return k;
}


//!
//! Decode the normalized key k which was formed using U64Key.
//! Return the native key.
//!
unsigned short Trie::U16Key::decode(const unsigned char* k)
{
    unsigned int key = (static_cast<unsigned int>(k[1]) << 12) |
        (static_cast<unsigned int>(k[2]) << 8) |
        (static_cast<unsigned int>(k[3]) << 4) |
        (static_cast<unsigned int>(k[4]));
    return static_cast<unsigned short>(key);
}


//!
//! Normalize the native key. Save result in the normalized key k.
//! Return the normalized key k.
//!
unsigned char* Trie::U28Key::encode(unsigned char k[1 + 7], unsigned int key)
{
    unsigned char* p = k + 7;
    for (; p > k; key >>= 4, --p)
    {
        *p = static_cast<unsigned char>(key & 0x0fUL);
    }
    *p = 7;
    return k;
}


//!
//! Decode the normalized key k which was formed using U64Key.
//! Return the native key.
//!
unsigned int Trie::U28Key::decode(const unsigned char* k)
{
    unsigned int key = (static_cast<unsigned int>(k[1]) << 24) |
        (static_cast<unsigned int>(k[2]) << 20) |
        (static_cast<unsigned int>(k[3]) << 16) |
        (static_cast<unsigned int>(k[4]) << 12) |
        (static_cast<unsigned int>(k[5]) << 8) |
        (static_cast<unsigned int>(k[6]) << 4) |
        (static_cast<unsigned int>(k[7]));
    return key;
}


//!
//! Normalize the native key. Save result in the normalized key k.
//! Return the normalized key k.
//!
unsigned char* Trie::U32Key::encode(unsigned char k[1 + 8], unsigned int key)
{
    unsigned char* p = k + 8;
    for (; p > k; key >>= 4, --p)
    {
        *p = static_cast<unsigned char>(key & 0x0fUL);
    }
    *p = 8;
    return k;
}


//!
//! Decode the normalized key k which was formed using U32Key.
//! Return the native key.
//!
unsigned int Trie::U32Key::decode(const unsigned char* k)
{
    unsigned int key = (static_cast<unsigned int>(k[1]) << 28) |
        (static_cast<unsigned int>(k[2]) << 24) |
        (static_cast<unsigned int>(k[3]) << 20) |
        (static_cast<unsigned int>(k[4]) << 16) |
        (static_cast<unsigned int>(k[5]) << 12) |
        (static_cast<unsigned int>(k[6]) << 8) |
        (static_cast<unsigned int>(k[7]) << 4) |
        (static_cast<unsigned int>(k[8]));
    return key;
}


//!
//! Normalize the native key. Save result in the normalized key k.
//! Return the normalized key k.
//!
unsigned char* Trie::U56Key::encode(unsigned char k[1 + 7], unsigned long long key)
{
    unsigned char* p = k + 7;
    for (; p > k; key >>= 8, --p)
    {
        *p = static_cast<unsigned char>(key);
    }
    *p = 7;
    return k;
}


//!
//! Decode the normalized key k which was formed using U56Key.
//! Return the native key.
//!
unsigned long long Trie::U56Key::decode(const unsigned char* k)
{
    unsigned long long key = (static_cast<unsigned long long>(k[1]) << 48) |
        (static_cast<unsigned long long>(k[2]) << 40) |
        (static_cast<unsigned long long>(k[3]) << 32) |
        (static_cast<unsigned long long>(k[4]) << 24) |
        (static_cast<unsigned long long>(k[5]) << 16) |
        (static_cast<unsigned long long>(k[6]) << 8) |
        (static_cast<unsigned long long>(k[7]));
    return key;
}


//!
//! Normalize the native key. Save result in the normalized key k.
//! Return the normalized key k.
//!
unsigned char* Trie::U64Key::encode(unsigned char k[1 + 8], unsigned long long key)
{
    unsigned char* p = k + 8;
    for (; p > k; key >>= 8, --p)
    {
        *p = static_cast<unsigned char>(key);
    }
    *p = 8;
    return k;
}


//!
//! Decode the normalized key k which was formed using U64Key.
//! Return the native key.
//!
unsigned long long Trie::U64Key::decode(const unsigned char* k)
{
    unsigned long long key = (static_cast<unsigned long long>(k[1]) << 56) |
        (static_cast<unsigned long long>(k[2]) << 48) |
        (static_cast<unsigned long long>(k[3]) << 40) |
        (static_cast<unsigned long long>(k[4]) << 32) |
        (static_cast<unsigned long long>(k[5]) << 24) |
        (static_cast<unsigned long long>(k[6]) << 16) |
        (static_cast<unsigned long long>(k[7]) << 8) |
        (static_cast<unsigned long long>(k[8]));
    return key;
}

END_NAMESPACE1
