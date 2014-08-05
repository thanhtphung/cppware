#include "syskit/U32Vec.hpp"

#include "syskit-ut-pch.h"
#include "U32VecSuite.hpp"

using namespace syskit;

const char ITEM[] = "aRandomStringUsedForU32VecPopulation!!!";

BEGIN_NAMESPACE


class Sample0: public U32Vec
{
public:
    Sample0();
    ~Sample0();
private:
    Sample0(const Sample0&); //prohibit usage
    const Sample0& operator =(const Sample0&); //prohibit usage
};

// Growable.
// Initially populated with numbers from ITEM.
Sample0::Sample0():
U32Vec(32 /*capacity*/, -1 /*growBy*/)
{
    for (const char* p = ITEM; *p; ++p)
    {
        add(*p);
    }
}

Sample0::~Sample0()
{
}


class Sample1: public U32Vec
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
U32Vec(21 /*capacity*/, 13 /*growBy*/)
{
    for (item_t i = 1; i <= 32; i += 2)
    {
        add(i);
    }
}

Sample1::~Sample1()
{
}

END_NAMESPACE


U32VecSuite::U32VecSuite()
{
}


U32VecSuite::~U32VecSuite()
{
}


void U32VecSuite::testAdd00()
{
    U32Vec vec0(64 /*capacity*/, 0 /*growBy*/);

    // Add random items to a fixed-capacity empty vector.
    bool ok = true;
    for (const char* p = ITEM; *p; ++p)
    {
        U32Vec::item_t item = *p;
        if (vec0.find(item))
        {
            if (vec0.addIfNotFound(item))
            {
                ok = false;
                break;
            }
        }
        else if ((!vec0.addIfNotFound(item)) || (!vec0.find(item)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void U32VecSuite::testAdd01()
{
    Sample1 vec1;

    // Add random items using addAtIndex().
    const char* p;
    bool ok = true;
    for (p = ITEM; *p; ++p)
    {
        U32Vec::item_t item = *p;
        if ((!vec1.addAtIndex(0, item, false)) || (vec1.peek(0) != item))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    // Undo.
    for (--p; p >= ITEM; --p)
    {
        if (!vec1.rmFromIndex(0, false))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = (vec1 == Sample1());
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - unsigned long U32Vec::add(size_t count, item_t item);
//
void U32VecSuite::testAdd02()
{
    unsigned long capacity = 3;
    int growBy = 0;
    U32Vec vec(capacity, growBy);
    unsigned long count = 0;
    U32Vec::item_t item = 12;
    unsigned long numAdds = vec.add(count, item);
    bool ok = ((numAdds == 0) && (vec.numItems() == 0));
    CPPUNIT_ASSERT(ok);
    count = 1;
    numAdds = vec.add(count, item);
    ok = ((numAdds == 1) && (vec.numItems() == 1) && (vec[0] == 12));
    CPPUNIT_ASSERT(ok);

    vec.reset();
    count = 5;
    item = 1234;
    numAdds = vec.add(count, item);
    ok = ((numAdds == 3) && (vec.numItems() == 3));
    CPPUNIT_ASSERT(ok);
    ok = ((vec[0] == 1234) && (vec[1] == 1234) && (vec[2] == 1234));
    CPPUNIT_ASSERT(ok);
    count = 1;
    numAdds = vec.add(count, item);
    ok = (numAdds == 0);
    CPPUNIT_ASSERT(ok);

    vec.reset();
    count = 2;
    item = 12345;
    numAdds = vec.add(count, item);
    ok = ((numAdds == 2) && (vec.numItems() == 2));
    CPPUNIT_ASSERT(ok);
    ok = ((vec[0] == 12345) && (vec[1] == 12345));
    CPPUNIT_ASSERT(ok);
    item = 123;
    numAdds = vec.add(count, item);
    ok = ((numAdds == 1) && (vec.numItems() == 3));
    CPPUNIT_ASSERT(ok);
    ok = ((vec[0] == 12345) && (vec[1] == 12345) && (vec[2] == 123));
    CPPUNIT_ASSERT(ok);

    growBy = -1;
    vec.setGrowth(growBy);
    item = 12345;
    numAdds = vec.add(count, item);
    ok = ((numAdds == 2) && (vec.numItems() == 5));
    CPPUNIT_ASSERT(ok);
    ok = ((vec[0] == 12345) && (vec[1] == 12345) && (vec[2] == 123) && (vec[3] == 12345) && (vec[4] == 12345));
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - unsigned long U32Vec::add(const item_t* raw, size_t itemCount);
//
void U32VecSuite::testAdd03()
{
    unsigned long capacity = 3;
    int growBy = 0;
    U32Vec vec(capacity, growBy);
    unsigned long count = 0;
    U32Vec::item_t item[] = {12, 34, 56, 78, 99};
    unsigned long numAdds = vec.add(item, count);
    bool ok = ((numAdds == 0) && (vec.numItems() == 0));
    CPPUNIT_ASSERT(ok);
    count = 1;
    numAdds = vec.add(item, count);
    ok = ((numAdds == 1) && (vec.numItems() == 1) && (vec[0] == item[0]));
    CPPUNIT_ASSERT(ok);

    vec.reset();
    count = 5;
    numAdds = vec.add(item, count);
    ok = ((numAdds == 3) && (vec.numItems() == 3));
    CPPUNIT_ASSERT(ok);
    ok = ((vec[0] == item[0]) && (vec[1] == item[1]) && (vec[2] == item[2]));
    CPPUNIT_ASSERT(ok);
    count = 1;
    numAdds = vec.add(item, count);
    ok = (numAdds == 0);
    CPPUNIT_ASSERT(ok);

    vec.reset();
    count = 2;
    numAdds = vec.add(item, count);
    ok = ((numAdds == 2) && (vec.numItems() == 2));
    CPPUNIT_ASSERT(ok);
    ok = ((vec[0] == item[0]) && (vec[1] == item[1]));
    CPPUNIT_ASSERT(ok);
    numAdds = vec.add(item, count);
    ok = ((numAdds == 1) && (vec.numItems() == 3));
    CPPUNIT_ASSERT(ok);
    ok = ((vec[0] == item[0]) && (vec[1] == item[1]) && (vec[2] == item[0]));
    CPPUNIT_ASSERT(ok);

    growBy = -1;
    vec.setGrowth(growBy);
    numAdds = vec.add(item, count);
    ok = ((numAdds == 2) && (vec.numItems() == 5));
    CPPUNIT_ASSERT(ok);
    ok = ((vec[0] == item[0]) && (vec[1] == item[1]) && (vec[2] == item[0]) && (vec[3] == item[0]) && (vec[4] == item[1]));
    CPPUNIT_ASSERT(ok);
}


void U32VecSuite::testAdd04()
{
    Sample0 vec0;

    U32Vec vec;
    size_t startAt = 0;
    size_t itemCount = 1;
    unsigned long numAdds = vec.add(vec0, startAt, itemCount);
    bool ok = (numAdds == itemCount);
    CPPUNIT_ASSERT(ok);
    startAt = 1;
    itemCount = vec0.numItems() - 1;
    numAdds = vec.add(vec0, startAt, itemCount);
    ok = (numAdds == itemCount) && (vec == vec0);
    CPPUNIT_ASSERT(ok);

    vec.reset();
    numAdds = vec.add(vec0);
    ok = (numAdds == vec0.numItems()) && (vec == vec0);
    CPPUNIT_ASSERT(ok);
    numAdds = vec.reset(vec0, 0, vec0.numItems());
    ok = (numAdds == vec0.numItems()) && (vec == vec0);
    CPPUNIT_ASSERT(ok);

    int growBy = 0;
    vec.setGrowth(growBy);
    vec.resize(vec.numItems());
    ok = (!vec.add(vec0.peek(5)));
    CPPUNIT_ASSERT(ok);
    ok = (!vec.addAtIndex(0, vec0.peek(5)));
    CPPUNIT_ASSERT(ok);
}


//
// Default constructor.
//
void U32VecSuite::testCtor00()
{
    U32Vec vec;
    bool ok = ((!vec.canGrow()) &&
        (vec.growthFactor() == 0) &&
        (vec.numItems() == 0) &&
        (vec.capacity() == U32Vec::DefaultCap));
    CPPUNIT_ASSERT(ok);
}


void U32VecSuite::testFind00()
{
    Sample1 vec1;

    bool ok = true;
    for (U32Vec::item_t i = 0, item = 1; item <= 32; ++i, item += 2)
    {
        if (!vec1.find(item))
        {
            ok = false;
            break;
        }
        size_t index;
        if ((!vec1.find(item, index)) || (index != i))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - U32Vec::item_t* U32Vec::detachRaw(unsigned long& numItems);
// - U32Vec::item_t U32Vec::findKthSmallest(item_t* item, size_t itemCount, size_t k);
// - const U32Vec::item_t* U32Vec::raw() const;
//
void U32VecSuite::testFindKthSmallest00()
{
    Sample0 vec0;

    unsigned long capacity = vec0.numItems();
    int growBy = 0;
    U32Vec vec1(capacity, growBy);
    bool reverseOrder = false;
    vec0.sort(vec1, reverseOrder);
    const U32Vec::item_t* p = vec0.raw();
    unsigned int itemCount;
    U32Vec::item_t* item = vec0.detachRaw(itemCount);
    bool ok = (item == p) && (itemCount == capacity);
    CPPUNIT_ASSERT(ok);

    for (unsigned long k = 0; k < capacity; ++k)
    {
        U32Vec::item_t kth = U32Vec::findKthSmallest(item, itemCount, k);
        if (kth != vec1.peek(k))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    delete[] item;
}


//
// Assignment operator. No growing required.
//
void U32VecSuite::testOp00()
{
    Sample1 vec1;

    U32Vec vec(vec1.numItems(), 0 /*growBy*/);
    vec = vec1;
    bool ok = (vec.initialCap() == vec.capacity());
    CPPUNIT_ASSERT(ok);
    ok = (vec == vec1);
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator. Exponential growth required.
//
void U32VecSuite::testOp01()
{
    Sample1 vec1;

    U32Vec vec(3 /*capacity*/, -1 /*growBy*/);
    vec = vec1;
    bool ok = ((vec.initialCap() == 3) && (vec.capacity() == 24));
    CPPUNIT_ASSERT(ok);
    ok = (vec == vec1);
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator. Linear growth required.
//
void U32VecSuite::testOp02()
{
    Sample0 vec0;
    Sample1 vec1;

    U32Vec vec(3 /*capacity*/, 3 /*growBy*/);
    vec = vec1;
    bool ok = ((vec.initialCap() == 3) && (vec.capacity() == 18));
    CPPUNIT_ASSERT(ok);
    ok = (vec == vec1);
    CPPUNIT_ASSERT(ok);
    ok = (!(vec0 == vec1));
    CPPUNIT_ASSERT(ok);

    // Also check setItem() and reset() here.
    vec.setItem(5, 0);
    ok = (vec.peek(5) == 0) && (vec != vec1);
    CPPUNIT_ASSERT(ok);
    U32Vec::item_t replacedItem;
    vec.setItem(9, 0, replacedItem);
    ok = (vec.peek(9) == 0) && (replacedItem == vec1.peek(9));
    CPPUNIT_ASSERT(ok);
    vec.reset();
    ok = (vec.numItems() == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator. Truncation required.
//
void U32VecSuite::testOp03()
{
    Sample1 vec1;

    U32Vec vec(3 /*capacity*/, 0 /*growBy*/);
    vec = vec1;
    vec = vec; //no-op
    bool ok = (vec.numItems() == 3);
    CPPUNIT_ASSERT(ok);

    ok = vec1.truncate(3) && (vec1 == vec) && (vec1 == U32Vec(Sample1(), 0, 3));
    CPPUNIT_ASSERT(ok);
}


void U32VecSuite::testResize00()
{
    U32Vec vec0(64 /*capacity*/, 0 /*growBy*/);
    Sample1 vec1;

    // no-op
    bool ok = vec0.resize(vec0.capacity());
    CPPUNIT_ASSERT(ok);

    // No truncation allowed.
    ok = (!vec1.resize(vec1.numItems() - 1));
    CPPUNIT_ASSERT(ok);

    U32Vec vec(vec1);
    ok = vec.resize(vec.numItems());
    CPPUNIT_ASSERT(ok);
    ok = (vec == vec1);
    CPPUNIT_ASSERT(ok);
}


//
// Remove various items.
//
void U32VecSuite::testRm00()
{
    Sample1 vec1;
    U32Vec vec(vec1);

    // Last item.
    U32Vec::item_t item = vec1.peek(vec1.numItems() - 1);
    U32Vec::item_t removedItem = 0;
    bool ok = (vec.rmTail(removedItem) && (removedItem == item) && (!vec.find(item)));
    CPPUNIT_ASSERT(ok);

    // First item.
    item = vec1.peek(0);
    ok = (vec.rmFromIndex(0, removedItem, false) && (removedItem == item) && (!vec.find(item)));
    CPPUNIT_ASSERT(ok);

    // Other item.
    // Removed item.
    // Non-existent item.
    item = vec1.peek(3);
    ok = (vec.rm(item, true) && (!vec.find(item)));
    CPPUNIT_ASSERT(ok);
    ok = (!vec.rm(item));
    CPPUNIT_ASSERT(ok);
    ok = (!vec.rm(23456));
    CPPUNIT_ASSERT(ok);

    // Invalid index.
    ok = ((!vec.rmFromIndex(9999)) && (vec.numItems() == (vec1.numItems() - 3)));
    CPPUNIT_ASSERT(ok);
}


void U32VecSuite::testSort00()
{
    Sample1 vec1;

    U32Vec vec(vec1.numItems(), 0);
    bool reverseOrder = false;
    vec1.sort(vec, reverseOrder);
    bool ok = (vec.numItems() == vec1.numItems());
    CPPUNIT_ASSERT(ok);
    ok = (vec == vec1);
    CPPUNIT_ASSERT(ok);

    ok = vec.search(vec1.peek(0));
    CPPUNIT_ASSERT(ok);

    U32Vec::item_t prevItem = vec.peek(0);
    for (size_t i = 1, numItems = vec.numItems(); i<numItems; ++i)
    {
        U32Vec::item_t item = vec.peek(i);
        if ((prevItem > item) || (!vec.search(vec1.peek(i))))
        {
            ok = false;
            break;
        }
        prevItem = item;
    }
    CPPUNIT_ASSERT(ok);
}


void U32VecSuite::testSort01()
{
    Sample1 vec1;

    U32Vec vec(vec1.numItems(), 0);
    bool reverseOrder = true;
    vec1.sort(vec, reverseOrder);
    bool ok = (vec.numItems() == vec1.numItems());
    CPPUNIT_ASSERT(ok);

    ok = vec.find(vec1.peek(0));
    CPPUNIT_ASSERT(ok);

    U32Vec::item_t prevItem = vec.peek(0);
    for (size_t i = 1, numItems = vec.numItems(); i < numItems; ++i)
    {
        U32Vec::item_t item = vec.peek(i);
        if ((prevItem < item) || (!vec.find(vec1.peek(i))))
        {
            ok = false;
            break;
        }
        prevItem = item;
    }
    CPPUNIT_ASSERT(ok);
}
