#include "appkit/U32.hpp"
#include "syskit/BitVec.hpp"
#include "syskit/Bst.hpp"
#include "syskit/Vec.hpp"

#include "syskit-ut-pch.h"
#include "BstSuite.hpp"

using namespace appkit;
using namespace syskit;

const char ITEM[] = "aRandomStringUsedForBstPopulation!!!";

BEGIN_NAMESPACE


class Sample0: public Bst
{
public:
    Sample0();
    ~Sample0();
private:
    Sample0(const Sample0&); //prohibit usage
    const Sample0& operator =(const Sample0&); //prohibit usage
};

Sample0::Sample0():
Bst(U32::compareP, 64 /*capacity*/, 0 /*growBy*/)
{
}

Sample0::~Sample0()
{
    for (size_t i = numItems(); i > 0; delete static_cast<unsigned long*>(peek(--i)));
}


class Sample1: public Bst
{
public:
    Sample1();
    ~Sample1();
private:
    Sample1(const Sample1&); //prohibit usage
    const Sample1& operator =(const Sample1&); //prohibit usage
};

// Growable.
// Initially populated with odd numbers from 1 to 32.
Sample1::Sample1():
Bst(U32::compareP, 21 /*capacity*/, 13 /*growBy*/)
{
    for (unsigned long i = 1; i <= 32; i += 2)
    {
        unsigned long* p = new unsigned long(i);
        add(p);
    }
}

Sample1::~Sample1()
{
    for (size_t i = numItems(); i > 0; delete static_cast<unsigned long*>(peek(--i)));
}


class Sample2: public Vec
{
public:
    Sample2();
    ~Sample2();
private:
    Sample2(const Sample2&); //prohibit usage
    const Sample2& operator =(const Sample2&); //prohibit usage
};

Sample2::Sample2():
Vec(64 /*capacity*/, 0 /*growBy*/)
{
    BitVec added(256 /*maxBits*/, false /*initialValue*/);
    for (const char* p = ITEM; *p; ++p)
    {
        if (!added.isSet(*p))
        {
            added.set(*p);
            unsigned long* item = new unsigned long(*p);
            add(item);
        }
    }
}

Sample2::~Sample2()
{
    for (item_t item; rmTail(item); delete static_cast<unsigned long*>(item));
}

END_NAMESPACE


BstSuite::BstSuite()
{
}


BstSuite::~BstSuite()
{
}


//
// Add random items.
//
void BstSuite::testAdd00()
{
    Sample0 bst0;

    // Add random items to a fixed-capacity empty table.
    bool ok = true;
    size_t numItems = 0;
    for (const char* p = ITEM; *p != 0; ++p, ++numItems)
    {
        unsigned long* item = new unsigned long(*p);
        if (bst0.find(item))
        {
            delete item;
            --numItems;
        }
        else if ((!bst0.add(item)) || (!bst0.find(item)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = (!bst0.addIfNotFound(bst0[0]));
    CPPUNIT_ASSERT(ok);
    unsigned long* item = new unsigned long(0UL);
    ok = (bst0.findIndex(item) == Bst::INVALID_INDEX);
    CPPUNIT_ASSERT(ok);
    ok = bst0.addIfNotFound(item);
    CPPUNIT_ASSERT(ok);

    validateBst(bst0, numItems + 1);
}


void BstSuite::testAdd01()
{
    Sample0 bst0;
    bst0.resize(0);
    bool ok = (!bst0.add(0));
    CPPUNIT_ASSERT(ok);
}


//
// Default constructor.
//
void BstSuite::testCtor00()
{
    Bst bst;
    bool ok = ((!bst.canGrow()) &&
        (bst.growthFactor() == 0) &&
        (bst.numItems() == 0) &&
        (bst.capacity() == Bst::DefaultCap));
    CPPUNIT_ASSERT(ok);

    Bst::compare_t compare = bst.cmpFunc();
    ok = (compare != 0);
    CPPUNIT_ASSERT(ok);

    unsigned long item[3] = {3, 1, 2};
    ok = (compare(&item[0], &item[0]) == 0);
    CPPUNIT_ASSERT(ok);
    ok = (compare(&item[0], &item[1]) < 0);
    CPPUNIT_ASSERT(ok);
    ok = (compare(&item[2], &item[1]) > 0);
    CPPUNIT_ASSERT(ok);
}


//
// Construct w/ unsorted vector.
//
void BstSuite::testCtor01()
{
    Sample2 vec2;
    Bst bst0(vec2, U32::compareP);
    validateBst(bst0, vec2, vec2.numItems());

    const Bst::item_t* p = bst0.raw();
    unsigned int itemCount = 999;
    Bst::item_t* item = bst0.detachRaw(itemCount);
    bool ok = (item == p) && (itemCount == vec2.numItems());
    CPPUNIT_ASSERT(ok);

    Bst bst1(U32::compareP, item, itemCount, 0 /*growBy*/);
    validateBst(bst1, vec2, vec2.numItems());
}


//
// Make sure all items can be found.
//
void BstSuite::testFind00()
{
    Sample1 bst1;

    unsigned long i = 0;
    bool ok = true;
    for (unsigned long item = 1; item <= 32; item += 2, ++i)
    {
        if (!bst1.find(&item))
        {
            ok = false;
            break;
        }
        size_t foundIndex = 999;
        if ((!bst1.find(&item, foundIndex)) || (foundIndex != i))
        {
            ok = false;
            break;
        }
        Bst::item_t foundItem = 0;
        if ((!bst1.find(&item, foundItem)) || (foundItem == 0))
        {
            ok = false;
            break;
        }
        foundIndex = 999;
        foundItem = 0;
        if ((!bst1.find(&item, foundItem, foundIndex)) || (foundItem == 0) || (foundIndex != i))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    unsigned long nonExistent = 0;
    size_t foundIndex = 999;
    Bst::item_t foundItem = 0;
    ok = (!bst1.find(&nonExistent)) &&
        (!bst1.find(&nonExistent, foundIndex)) && (foundIndex == 0) &&
        (!bst1.find(&nonExistent, foundItem)) && (foundItem == 0);
    CPPUNIT_ASSERT(ok);

    nonExistent = 999;
    ok = (!bst1.find(&nonExistent, foundIndex)) && (foundIndex == (bst1.numItems() - 1));
    CPPUNIT_ASSERT(ok);
    foundIndex = 999;
    ok = (!bst1.find(&nonExistent, foundItem, foundIndex)) && (foundItem == 0) && (foundIndex == (bst1.numItems() - 1));
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator. No growing required.
//
void BstSuite::testOp00()
{
    Sample0 bst0;
    Sample1 bst1;
    Bst bst(bst1.cmpFunc(), bst1.numItems(), 0 /*growBy*/);
    bst = bst; //no-op
    bst = bst1;
    bool ok = (bst == bst1) && (bst.capacity() == bst.initialCap());
    CPPUNIT_ASSERT(ok);

    validateBst(bst, bst1.numItems());
    bst = bst0;
    ok = (bst == bst0);
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator w/ unsorted vector. No growing required.
//
void BstSuite::testOp01()
{
    Sample2 vec2;
    Bst bst(U32::compareP, vec2.numItems(), 0 /*growBy*/);
    bst = vec2;
    bool ok = (bst.capacity() == bst.initialCap());
    CPPUNIT_ASSERT(ok);

    validateBst(bst, vec2, vec2.numItems());
}


//
// Assignment operator. Exponential growth required.
//
void BstSuite::testOp02()
{
    Sample1 bst1;
    Bst bst(bst1.cmpFunc(), 3 /*capacity*/, -1 /*growBy*/);
    bst = bst1;
    bool ok = (bst == bst1) && (bst.capacity() == 24);
    CPPUNIT_ASSERT(ok);

    validateBst(bst, bst1.numItems());
}


//
// Assignment operator w/ unsorted vector. Exponential growth required.
//
void BstSuite::testOp03()
{
    Sample2 vec2;
    Bst bst(U32::compareP, 3 /*capacity*/, -1 /*growBy*/);
    bst = vec2;
    bool ok = (bst.capacity() == 24);
    CPPUNIT_ASSERT(ok);

    validateBst(bst, vec2, vec2.numItems());
}


//
// Assignment operator. Truncation required.
//
void BstSuite::testOp04()
{
    Sample1 bst1;
    Bst bst(bst1.cmpFunc(), 5 /*capacity*/, 0 /*growBy*/);
    bst = bst1;
    validateBst(bst, 5);
}


//
// Assignment operator w/ unsorted vector. Truncation required.
//
void BstSuite::testOp05()
{
    Sample2 vec2;
    Bst bst(U32::compareP, 5 /*capacity*/, 0 /*growBy*/);
    bst = vec2;
    validateBst(bst, vec2, 5);
}


//
// Assignment operator. Different comparison functions.
//
void BstSuite::testOp06()
{
    Sample1 bst1;

    Bst bst(U32::compareP, bst1.numItems(), 0 /*growBy*/);
    Bst bstR(U32::comparePR, bst1.numItems(), 0 /*growBy*/);
    bstR = bst1;
    bool ok = (bstR.numItems() == bst1.numItems()) && (bstR != bst1);
    CPPUNIT_ASSERT(ok);
    validateBst(bstR, bst1.numItems());

    bst = bstR;
    ok = (bst == bst1);
    CPPUNIT_ASSERT(ok);
}


void BstSuite::testResize00()
{
    Sample0 bst0;
    Sample1 bst1;

    // no-op
    bool ok = bst0.resize(bst0.capacity());
    CPPUNIT_ASSERT(ok);

    // No truncation allowed.
    ok = (!bst1.resize(bst1.numItems() - 1));
    CPPUNIT_ASSERT(ok);

    Bst bst(bst1);
    ok = bst.resize(bst.numItems());
    CPPUNIT_ASSERT(ok);
    size_t numItems = bst1.numItems();
    ok = ((bst.numItems() == numItems) && (memcmp(bst.raw(), bst1.raw(), numItems * sizeof(Bst::item_t)) == 0));
    CPPUNIT_ASSERT(ok);
}


//
// Remove various items.
//
void BstSuite::testRm00()
{
    Sample1 bst1;
    Bst bst(bst1);

    // First item.
    Bst::item_t item = bst.peek(0);
    bool ok = bst.rm(item) && (!bst.find(item)) && (bst != bst1);
    CPPUNIT_ASSERT(ok);

    // Last item.
    unsigned long i = bst.numItems() - 1;
    item = bst.peek(i);
    Bst::item_t removedItem = 0;
    ok = bst.rm(item, removedItem) && (!bst.find(item) && (removedItem == item));
    CPPUNIT_ASSERT(ok);

    // Other item.
    item = bst[3];
    ok = bst.rmFromIndex(3) && (!bst.rmFromIndex(999));
    CPPUNIT_ASSERT(ok);
    item = bst[6];
    ok = bst.rmFromIndex(6, removedItem) && (removedItem == item);
    CPPUNIT_ASSERT(ok);

    // Non-existent item.
    unsigned long nonExistent = 0;
    ok = (!bst.rm(&nonExistent));
    CPPUNIT_ASSERT(ok);
}


//
// Validate given table.
// Table should contain numItems items.
//
void BstSuite::validateBst(const Bst& bst, size_t numItems)
{
    bool ok = (bst.numItems() == numItems);
    CPPUNIT_ASSERT(ok);

    Bst::compare_t compare = bst.cmpFunc();
    Bst::item_t prevItem = (numItems == 0)? 0: bst.peek(0);
    for (size_t i = 1; i < numItems; ++i)
    {
        Bst::item_t item = bst[i];
        if (compare(prevItem, item) > 0)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


//
// Validate given table.
// Table was formed using numItems items from given vector.
//
void BstSuite::validateBst(const Bst& bst, const Vec& vec, size_t numItems)
{
    validateBst(bst, numItems);

    bool ok = true;
    for (size_t i = numItems; i > 0;)
    {
        Bst::item_t item = vec.peek(--i);
        if (!bst.find(item))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}
