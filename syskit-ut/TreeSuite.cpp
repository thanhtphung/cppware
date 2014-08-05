#include "appkit/U32.hpp"
#include "syskit/Tree.hpp"

#include "syskit-ut-pch.h"
#include "TreeSuite.hpp"

using namespace appkit;
using namespace syskit;

typedef union
{
    unsigned long* item;
    void* opaque;
} item_t;

const char ITEMS[] = "aRandomStringUsedForTreePopulation!!!~@#$%^&*()_+=-`{}|][:;'<>?/.,";
const size_t NUM_ITEMS = sizeof(ITEMS) - 1;


TreeSuite::TreeSuite()
{
}


TreeSuite::~TreeSuite()
{
}


//
// This method is invoke when iterating items in order. Given argument
// is a pointer to an unsigned long for temporary use to check if the
// iterated items are in the expected ascending order.
//
bool TreeSuite::checkItem(void* arg, void* item)
{
    unsigned long lo = *static_cast<unsigned long*>(arg);
    unsigned long hi = *static_cast<unsigned long*>(item);
    *static_cast<unsigned long*>(arg) = hi;
    return (lo < hi);
}


bool TreeSuite::rmItem(void* arg, void* item)
{
    Tree& tree = *static_cast<Tree*>(arg);
    size_t numItems = tree.numItems() - 1;
    void* removedItem = 0;
    bool keepGoing = (tree.rm(item, removedItem) &&
        (item == removedItem) &&
        (tree.numItems() == numItems) &&
        (!tree.find(item)));

    return keepGoing;
}


void TreeSuite::deleteItem(void* /*arg*/, void* item)
{
    delete static_cast<unsigned long*>(item);
}


//
// Interfaces under test:
// - bool Tree::add(void* item);
// - bool Tree::apply(cb0_t cb, void* arg=0) const;
// - bool Tree::find(void* item) const;
// - bool Tree::find(void* item, void*& foundItem) const;
// - bool Tree::findMax(void*& maxItem) const;
// - bool Tree::findMin(void*& minItem) const;
// - void Tree::applyChildFirst(cb1_t cb, void* arg=0) const;
//
void TreeSuite::testAdd00()
{
    unsigned long maxItem = 0UL;
    unsigned long minItem = 0xffffffffUL;

    // Add random items.
    // This sample should cover all node configurations.
    Tree tree(U32::compareP);
    bool ok = (tree.cmpFunc() == U32::compareP);
    CPPUNIT_ASSERT(ok);
    size_t numItems = 0;
    for (const char* p = ITEMS; *p != 0; ++p)
    {
        for (unsigned long u32 = *p; u32 != 0; u32 <<= 1)
        {
            void* foundItem = 0;
            unsigned long* item = new unsigned long(u32);
            ++numItems;
            if (tree.find(item))
            {
                delete item;
                --numItems;
            }
            else if ((!tree.add(item)) ||
                (tree.numItems() != numItems) ||
                (!tree.find(item, foundItem)) ||
                (item != foundItem))
            {
                ok = false;
                p = ITEMS + NUM_ITEMS - 1; //terminate outer loop
                break;
            }
            else
            {
                if (u32 > maxItem) maxItem = u32;
                if (u32 < minItem) minItem = u32;
            }
        }
    }
    CPPUNIT_ASSERT(ok);

    void* foundItem;
    ok = (tree.findMax(foundItem) && (*static_cast<unsigned long*>(foundItem) == maxItem));
    CPPUNIT_ASSERT(ok);
    ok = (tree.findMin(foundItem) && (*static_cast<unsigned long*>(foundItem) == minItem));
    CPPUNIT_ASSERT(ok);

    unsigned long v = 0;
    ok = tree.apply(checkItem, &v);
    CPPUNIT_ASSERT(ok);

    tree.applyChildFirst(deleteItem, 0 /*arg*/);
    ok = (tree.numItems() == numItems);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - bool Tree::add(void* item, void*& foundItem);
// - bool Tree::apply(cb0_t cb, void* arg=0) const;
// - void Tree::applyParentFirst(cb1_t cb, void* arg=0) const;
// - void Tree::reset();
//
void TreeSuite::testAdd01()
{

    // Add random items.
    // If an add() fails, make sure the found item looks reasonable.
    Tree tree(U32::compareP);
    bool ok = true;
    for (const char* p = ITEMS; *p != 0; ++p)
    {
        for (unsigned long u32 = *p; u32 != 0; u32 <<= 1)
        {
            unsigned long* item = new unsigned long(u32);
            item_t found;
            if (!tree.add(item, found.opaque))
            {
                if (*found.item != *item)
                {
                    ok = false;
                    p = ITEMS + NUM_ITEMS - 1; //terminate outer loop
                    break;
                }
                delete item;
            }
        }
    }
    CPPUNIT_ASSERT(ok);

    unsigned long v = 0;
    ok = tree.apply(checkItem, &v);
    CPPUNIT_ASSERT(ok);

    tree.applyParentFirst(deleteItem, 0 /*arg*/);
    tree.reset();
    ok = (tree.numItems() == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - Tree::item_t Tree::peek(size_t index) const;
// - void Tree::apply(cb1_t cb, void* arg=0) const;
// - void Tree::rebalance();
//
void TreeSuite::testApply00()
{

    // Add random items.
    Tree tree0(U32::compareP);
    for (const char* p = ITEMS + NUM_ITEMS - 1; p >= ITEMS; --p)
    {
        for (unsigned long u32 = *p; u32 != 0; u32 <<= 1)
        {
            unsigned long* item = new unsigned long(u32);
            if (!tree0.add(item))
            {
                delete item;
            }
        }
    }

    Tree tree1(tree0);
    for (unsigned long i = 0; i < 10; ++i)
    {
        void* item = tree0.peek(0);
        tree0.rm(item);
        tree0.add(item);
    }
    tree0.rebalance();
    bool ok = (tree0 == tree1);
    CPPUNIT_ASSERT(ok);

    unsigned long numItems = tree0.numItems();
    for (unsigned long i = 0, item = 0; i < numItems; ++i)
    {
        unsigned long item0 = *static_cast<unsigned long*>(tree0.peek(i));
        unsigned long item1 = *static_cast<unsigned long*>(tree1.peek(i));
        if ((item0 != item1) || (item >= item0))
        {
            ok = false;
            break;
        }
        item = item0;
    }
    CPPUNIT_ASSERT(ok);

    tree0.apply(deleteItem, 0 /*arg*/);
    ok = (tree0.numItems() == numItems);
    CPPUNIT_ASSERT(ok);
    tree1.reset();
    tree0.reset();
    tree0 = tree1;
    ok = (tree0 == tree1);
    CPPUNIT_ASSERT(ok);
}


void TreeSuite::testCtor00()
{
    Tree::compare_t compare = 0;
    Tree tree(compare);
    compare = tree.cmpFunc();
    bool ok = (compare != 0);
    CPPUNIT_ASSERT(ok);

    unsigned long item[3] = {3, 1, 2};
    ok = (compare(&item[0], &item[0]) == 0);
    CPPUNIT_ASSERT(ok);
    ok = (compare(&item[0], &item[1]) < 0);
    CPPUNIT_ASSERT(ok);
    ok = (compare(&item[2], &item[1]) > 0);
    CPPUNIT_ASSERT(ok);

    Tree::item_t any = tree.any();
    ok = (any == 0);
    CPPUNIT_ASSERT(ok);

    tree.add(this);
    any = tree.any();
    ok = (any == this);
    CPPUNIT_ASSERT(ok);
    tree.reset();

    Tree::cb0_t cb0 = 0;
    Tree::cb1_t cb1 = 0;
    tree.apply(cb1, 0);
    tree.applyChildFirst(cb1, 0);
    tree.applyParentFirst(cb1, 0);
    ok = tree.apply(cb0, 0) && tree.applyChildFirst(cb0, 0) && tree.applyParentFirst(cb0, 0);
    CPPUNIT_ASSERT(ok);
}


void TreeSuite::testCtor01()
{

    // Add random items.
    Tree tree0(U32::compareP);
    for (const char* p = ITEMS + NUM_ITEMS - 1; p >= ITEMS; --p)
    {
        for (unsigned long u32 = *p; u32 != 0; u32 <<= 1)
        {
            unsigned long* item = new unsigned long(u32);
            if (!tree0.add(item))
            {
                delete item;
            }
        }
    }

    Tree tree1(tree0);
    Tree tree2(&tree1);
    bool ok = (tree2 == tree0) && (tree1.numItems() == 0);
    CPPUNIT_ASSERT(ok);

    tree1 = &tree2;
    ok = (tree1 == tree0) && (tree2.numItems() == 0);
    CPPUNIT_ASSERT(ok);
    tree1 = &tree1; //no-op
    ok = (tree1 == tree0);
    CPPUNIT_ASSERT(ok);

    tree1 = &tree0;
    tree1.apply(deleteItem, 0 /*arg*/);
    tree1.reset();
    ok = (tree1.numItems() == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - void Tree::operator delete(void* p, size_t size);
// - void* Tree::operator new(size_t size);
// - void* Tree::operator new(size_t size, void* buf);
//
void TreeSuite::testNew00()
{
    Tree* tree = new Tree(U32::compareP);
    bool ok = (tree != 0);
    CPPUNIT_ASSERT(ok);
    delete tree;

    unsigned char buf[sizeof(*tree)];
    tree = new(buf)Tree(U32::compareP);
    ok = (reinterpret_cast<unsigned char*>(tree) == buf);
    CPPUNIT_ASSERT(ok);
    tree->Tree::~Tree();
}


//
// Interfaces under test:
// - bool Tree::apply(cb0_t cb, void* arg=0) const;
// - bool Tree::applyChildFirst(cb0_t cb, void* arg=0) const;
// - bool Tree::applyParentFirst(cb0_t cb, void* arg=0) const;
// - bool Tree::rm(void* item, void*& removedItem);
//
void TreeSuite::testRm00()
{

    // Add random items.
    Tree tree0(U32::compareP);
    for (const char* p = ITEMS + NUM_ITEMS - 1; p >= ITEMS; --p)
    {
        for (unsigned long u32 = *p; u32 != 0; u32 <<= 1)
        {
            unsigned long* item = new unsigned long(u32);
            if (!tree0.add(item))
            {
                delete item;
            }
        }
    }

    Tree tree1(tree0);
    bool ok = (tree1 == tree0);
    CPPUNIT_ASSERT(ok);

    // Remove all items. Children first.
    tree0.applyChildFirst(rmItem, &tree1);
    ok = (tree1.numItems() == 0);
    CPPUNIT_ASSERT(ok);

    // Remove all items. Parent first.
    tree1 = tree0;
    ok = (tree1 == tree0);
    CPPUNIT_ASSERT(ok);
    tree0.applyParentFirst(rmItem, &tree1);
    ok = (tree1.numItems() == 0);
    CPPUNIT_ASSERT(ok);

    // Remove all items. In order.
    tree1 = tree0;
    tree0.apply(rmItem, &tree1);
    ok = (tree1.numItems() == 0);
    CPPUNIT_ASSERT(ok);

    // Remove and delete all items in a slightly "random" order.
    // All these removal patterns should cover all node configurations.
    const char* p0 = ITEMS + 23;
    for (const char* p = p0;;)
    {
        unsigned long u32a = *p << 5;
        for (unsigned long u32 = u32a;;)
        {
            void *removedItem;
            if (tree0.rm(&u32, removedItem))
            {
                delete static_cast<unsigned long*>(removedItem);
            }
            if ((u32 <<= 1) == 0) { u32 = *p; continue; }
            if (u32 == u32a) break;
        }
        if (*++p == 0) { p = ITEMS; continue; }
        if (p == p0) break;
    }
    ok = (tree0.numItems() == 0);
    CPPUNIT_ASSERT(ok);
}


void TreeSuite::testSize00()
{
    bool ok = (sizeof(Tree) == sizeof(void*) * 3) &&
        (sizeof(Tree::Node0) == sizeof(void*)) &&
        (sizeof(Tree::Node1) == sizeof(void*) * 4) &&
        (sizeof(Tree::Node2) == sizeof(void*) * 6) &&
        (sizeof(Tree::Node3) == sizeof(void*) * 8) &&
        (sizeof(Tree::NodeX) == sizeof(void*) * 4);
    CPPUNIT_ASSERT(ok);
}
