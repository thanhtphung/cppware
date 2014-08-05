#include "appkit/U32.hpp"
#include "syskit/Vec.hpp"

#include "syskit-ut-pch.h"
#include "VecSuite.hpp"

using namespace appkit;
using namespace syskit;

const char ITEM[] = "aRandomStringUsedForVecPopulation!!!";

BEGIN_NAMESPACE


class Sample0: public Vec
{
public:
    Sample0();
    ~Sample0();
private:
    Sample0(const Sample0&); //prohibit usage
    const Sample0& operator =(const Sample0&); //prohibit usage
};

Sample0::Sample0():
Vec(64 /*capacity*/, 0 /*growBy*/)
{
}

Sample0::~Sample0()
{
    for (item_t item; rmTail(item); delete static_cast<unsigned long*>(item));
}


class Sample1: public Vec
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
Vec(21 /*capacity*/, 13 /*growBy*/)
{
    for (unsigned long i = 1; i <= 32; i += 2)
    {
        unsigned long* p = new unsigned long(i);
        add(p);
    }
}

Sample1::~Sample1()
{
    for (item_t item; rmTail(item); delete static_cast<unsigned long*>(item));
}

END_NAMESPACE


VecSuite::VecSuite()
{
}


VecSuite::~VecSuite()
{
}


//
// Add random items to the end.
//
void VecSuite::testAdd00()
{
    Sample0 vec0;

    // Add random items to a fixed-capacity empty vector.
    bool ok = true;
    size_t i = 0;
    for (const char* p = ITEM; *p != 0; ++p, ++i)
    {
        unsigned long* item = new unsigned long(*p);
        if ((!vec0.add(item)) || (vec0.numItems() != (i + 1)) || (vec0.peek(i) != item))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = (!vec0.addIfNotFound(vec0.peek(0), U32::compareP));
    CPPUNIT_ASSERT(ok);
    unsigned long* item = new unsigned long(0UL);
    ok = (vec0.findIndex(item) == Vec::INVALID_INDEX);
    CPPUNIT_ASSERT(ok);
    ok = vec0.addIfNotFound(item, U32::compareP);
    CPPUNIT_ASSERT(ok);
}


void VecSuite::testAdd01()
{
    Sample1 vec1;

    // Add at.
    bool ok = (vec1.addAtIndex(0, 0, true /*maintainOrder*/) && (vec1.peek(0) == 0));
    CPPUNIT_ASSERT(ok);
    ok = vec1.rmFromIndex(0, true /*maintainOrder*/);
    CPPUNIT_ASSERT(ok);
    ok = (vec1.addAtIndex(2, 0, false) && (vec1.peek(2) == 0));
    CPPUNIT_ASSERT(ok);
    ok = vec1.rmFromIndex(2, false);
    CPPUNIT_ASSERT(ok);

    // Validate original contents.
    size_t i = 0;
    for (size_t item = 1; item <= 31; item += 2)
    {
        const unsigned long* p = static_cast<const unsigned long*>(vec1.peek(i++));
        if (*p != item)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    // Full vector.
    Vec vec(0, 0);
    ok = ((!vec.add(0)) && (!vec.addAtIndex(0, 0)));
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - size_t Vec::add(size_t, void*);
//
void VecSuite::testAdd02()
{
    unsigned long capacity = 3;
    int growBy = 0;
    Vec vec(capacity, growBy);
    size_t count = 0;
    Vec::item_t item = &vec;
    size_t numAdds = vec.add(count, item);
    bool ok = ((numAdds == 0) && (vec.numItems() == 0));
    CPPUNIT_ASSERT(ok);
    count = 1;
    numAdds = vec.add(count, item);
    ok = ((numAdds == 1) && (vec.numItems() == 1) && (vec[0] == &vec));
    CPPUNIT_ASSERT(ok);

    vec.reset();
    count = 5;
    item = &item;
    numAdds = vec.add(count, item);
    ok = ((numAdds == 3) && (vec.numItems() == 3));
    CPPUNIT_ASSERT(ok);
    ok = ((vec[0] == &item) && (vec[1] == &item) && (vec[2] == &item));
    CPPUNIT_ASSERT(ok);
    count = 1;
    numAdds = vec.add(count, item);
    ok = (numAdds == 0);
    CPPUNIT_ASSERT(ok);

    vec.reset();
    count = 2;
    item = 0;
    numAdds = vec.add(count, item);
    ok = ((numAdds == 2) && (vec.numItems() == 2));
    CPPUNIT_ASSERT(ok);
    ok = ((vec[0] == 0) && (vec[1] == 0));
    CPPUNIT_ASSERT(ok);
    item = this;
    numAdds = vec.add(count, item);
    ok = ((numAdds == 1) && (vec.numItems() == 3));
    CPPUNIT_ASSERT(ok);
    ok = ((vec[0] == 0) && (vec[1] == 0) && (vec[2] == this));
    CPPUNIT_ASSERT(ok);

    growBy = -1;
    vec.setGrowth(growBy);
    item = 0;
    numAdds = vec.add(count, item);
    ok = ((numAdds == 2) && (vec.numItems() == 5));
    CPPUNIT_ASSERT(ok);
    ok = ((vec[0] == 0) && (vec[1] == 0) && (vec[2] == this) && (vec[3] == 0) && (vec[4] == 0));
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - unsigned long Vec::add(void* const* raw, size_t itemCount);
//
void VecSuite::testAdd03()
{
    unsigned long capacity = 3;
    int growBy = 0;
    Vec vec(capacity, growBy);
    unsigned long count = 0;
    Vec::item_t item[] =
    {
        new unsigned long(12),
        new unsigned long(34),
        new unsigned long(56),
        new unsigned long(78),
        new unsigned long(99)
    };
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

    vec.reset();
    for (unsigned long i = 5; i > 0; delete static_cast<unsigned long*>(item[--i]));
}


void VecSuite::testAdd04()
{
    Sample0 vec0;
    for (const char* p = ITEM; *p != 0; ++p)
    {
        unsigned long* item = new unsigned long(*p);
        vec0.add(item);
    }

    Vec vec;
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
void VecSuite::testCtor00()
{
    Vec vec;
    bool ok = ((!vec.canGrow()) &&
        (vec.growthFactor() == 0) &&
        (vec.numItems() == 0) &&
        (vec.capacity() == Vec::DefaultCap));
    CPPUNIT_ASSERT(ok);
}


void VecSuite::testCtor01()
{
    Sample1 vec1;

    Vec vec(vec1);
    bool ok = (vec.initialCap() == vec.capacity());
    CPPUNIT_ASSERT(ok);
    size_t numItems = vec1.numItems();
    ok = ((vec.numItems() == numItems) && (memcmp(vec.raw(), vec1.raw(), numItems * sizeof(Vec::item_t)) == 0));
    CPPUNIT_ASSERT(ok);

    Vec vec2;
    vec2 = &vec; //disable vec and move its guts to vec2
    vec2 = vec2; //no-op
    ok = (vec.capacity() == 0UL - 1) &&
        ((vec2.numItems() == numItems) && (memcmp(vec2.raw(), vec1.raw(), numItems * sizeof(Vec::item_t)) == 0));
    CPPUNIT_ASSERT(ok);
}


//
// bool Vec::find(void* item, compare_t compare, size_t& foundIndex) const;
//
void VecSuite::testFind00()
{
    Sample1 vec1;

    size_t foundIndex = 0;
    unsigned long key = 5;
    bool ok = (vec1.find(&key, U32::compareP, foundIndex) && (foundIndex == 2));
    CPPUNIT_ASSERT(ok);

    key = 99;
    ok = (!vec1.find(&key, U32::compareP, foundIndex));
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - Vec::item_t* Vec::detachRaw(unsigned long& numItems);
// - Vec::item_t Vec::findKthSmallest(item_t* item, size_t itemCount, size_t k, compare_t compare);
// - const Vec::item_t* Vec::raw() const;
//
void VecSuite::testFindKthSmallest00()
{
    Sample0 vec0;
    for (const char* p = ITEM; *p != 0; ++p)
    {
        unsigned long* item = new unsigned long(*p);
        vec0.add(item);
    }

    unsigned long capacity = vec0.numItems();
    int growBy = 0;
    Vec vec1(capacity, growBy);
    bool reverseOrder = false;
    vec0.sort(U32::compareP, vec1, reverseOrder);
    const Vec::item_t* p = vec0.raw();
    unsigned int itemCount = 999;
    Vec::item_t* item = vec0.detachRaw(itemCount);
    bool ok = (item == p) && (itemCount == capacity);
    CPPUNIT_ASSERT(ok);

    for (unsigned long k = 0; k < capacity; ++k)
    {
        Vec::item_t kth = Vec::findKthSmallest(item, itemCount, k, U32::compareP);
        if (U32::compareP(kth, vec1.peek(k)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    for (unsigned long k = capacity; k > 0; delete static_cast<unsigned long*>(item[--k]));
    delete[] item;
}


void VecSuite::testNew00()
{
    Vec* vec = new Vec;
    bool ok = (vec != 0);
    CPPUNIT_ASSERT(ok);
    delete vec;

    unsigned char buf[sizeof(*vec)];
    vec = new(buf)Vec;
    ok = (reinterpret_cast<unsigned char*>(vec) == buf);
    CPPUNIT_ASSERT(ok);
    vec->Vec::~Vec();
}


//
// Assignment operator. No growing required.
//
void VecSuite::testOp00()
{
    Sample1 vec1;

    Vec vec(vec1.numItems(), 0 /*growBy*/);
    vec = vec1;
    bool ok = (vec.initialCap() == vec.capacity());
    CPPUNIT_ASSERT(ok);
    size_t numItems = vec1.numItems();
    ok = ((vec.numItems() == numItems) && (memcmp(vec.raw(), vec1.raw(), numItems * sizeof(Vec::item_t)) == 0));
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator. Exponential growth required.
//
void VecSuite::testOp01()
{
    Sample1 vec1;

    Vec vec(3 /*capacity*/, -1 /*growBy*/);
    vec = vec1;
    bool ok = ((vec.initialCap() == 3) && (vec.capacity() == 24));
    CPPUNIT_ASSERT(ok);
    size_t numItems = vec1.numItems();
    ok = ((vec.numItems() == numItems) && (memcmp(vec.raw(), vec1.raw(), numItems * sizeof(Vec::item_t)) == 0));
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator. Linear growth required.
//
void VecSuite::testOp02()
{
    Sample0 vec0;
    Sample1 vec1;

    Vec vec(3 /*capacity*/, 3 /*growBy*/);
    vec = vec1;
    bool ok = ((vec.initialCap() == 3) && (vec.capacity() == 18));
    CPPUNIT_ASSERT(ok);
    ok = (vec == vec1) && (vec.equals(vec1, U32::compareP));
    CPPUNIT_ASSERT(ok);
    ok = (!(vec0 == vec1)) && (!vec0.equals(vec1, U32::compareP));
    CPPUNIT_ASSERT(ok);
    size_t numItems = vec1.numItems();
    ok = ((vec.numItems() == numItems) && (memcmp(vec.raw(), vec1.raw(), numItems * sizeof(Vec::item_t)) == 0));
    CPPUNIT_ASSERT(ok);

    // Also check setItem() and reset() here.
    Vec::item_t item = new unsigned long(0);
    vec.setItem(5, item);
    ok = (vec.peek(5) == item) && (vec != vec1) && (!vec.equals(vec1, U32::compareP));
    CPPUNIT_ASSERT(ok);
    Vec::item_t replacedItem;
    vec.setItem(9, item, replacedItem);
    ok = (vec.peek(9) == item) && (replacedItem == vec1.peek(9));
    CPPUNIT_ASSERT(ok);
    vec.reset();
    ok = (vec.numItems() == 0);
    CPPUNIT_ASSERT(ok);

    delete static_cast<unsigned long*>(item);
}


//
// Assignment operator. Truncation required.
//
void VecSuite::testOp03()
{
    Sample1 vec1;

    Vec vec(3 /*capacity*/, 0 /*growBy*/);
    vec = vec1;
    vec = vec; //no-op
    bool ok = ((vec.numItems() == 3) && (memcmp(vec.raw(), vec1.raw(), 3 * sizeof(Vec::item_t)) == 0));
    CPPUNIT_ASSERT(ok);

    Vec vec2(vec1);
    ok = vec2.truncate(3) && vec2.equals(vec, U32::compareP) && vec2.equals(Vec(vec1, 0, 3), U32::compareP);
    CPPUNIT_ASSERT(ok);

    // Disable vec2 and move its guts to vec3.
    Vec vec3(&vec2);
    ok = (vec2.capacity() == 0UL - 1) &&
        vec3.equals(vec, U32::compareP) && vec3.equals(Vec(vec1, 0, 3), U32::compareP);
    CPPUNIT_ASSERT(ok);
}


void VecSuite::testResize00()
{
    Sample0 vec0;
    Sample1 vec1;

    // no-op
    bool ok = vec0.resize(vec0.capacity());
    CPPUNIT_ASSERT(ok);

    // No truncation allowed.
    ok = (!vec1.resize(vec1.numItems() - 1));
    CPPUNIT_ASSERT(ok);

    Vec vec(vec1);
    ok = vec.resize(vec.numItems());
    CPPUNIT_ASSERT(ok);
    size_t numItems = vec1.numItems();
    ok = ((vec.numItems() == numItems) && (memcmp(vec.raw(), vec1.raw(), numItems * sizeof(Vec::item_t)) == 0));
    CPPUNIT_ASSERT(ok);
}


//
// Remove various items.
//
void VecSuite::testRm00()
{
    Sample1 vec1;
    Vec vec(vec1);

    // Last item.
    Vec::item_t removedItem = 0;
    bool ok = (vec.rmTail(removedItem) && (removedItem == vec1.peek(vec1.numItems() - 1)));
    CPPUNIT_ASSERT(ok);

    // First item.
    ok = (vec.rmFromIndex(0, removedItem, false) && (removedItem == vec1.peek(0)));
    CPPUNIT_ASSERT(ok);

    // Other item.
    // Removed item.
    // Non-existent item.
    Vec::item_t item = vec1.peek(3);
    ok = (vec.rm(item, U32::compareP, removedItem, true) && (removedItem == item) && (!vec.find(item, U32::compareP)));
    CPPUNIT_ASSERT(ok);
    ok = (!vec.rm(item, U32::compareP));
    CPPUNIT_ASSERT(ok);
    unsigned long k = 0;
    ok = (!vec.rm(&k, U32::compareP));
    CPPUNIT_ASSERT(ok);

    // Invalid index.
    ok = ((!vec.rmFromIndex(9999)) && (vec.numItems() == (vec1.numItems() - 3)));
    CPPUNIT_ASSERT(ok);
}


void VecSuite::testRm01()
{
    Sample1 vec1;
    Vec vec(vec1);

    // First items.
    bool maintainOrder = true;
    size_t itemCount = 3;
    size_t startAt = 0;
    bool ok = vec.rmFromIndex(startAt, itemCount, maintainOrder) && (vec == Vec(vec1, 3, vec1.numItems() - 3));
    CPPUNIT_ASSERT(ok);

    // Last items.
    itemCount = 5;
    startAt = vec.numItems() - 5;
    ok = vec.rmFromIndex(startAt, itemCount, maintainOrder) && (vec == Vec(vec1, 3, vec1.numItems() - 3 - 5));
    CPPUNIT_ASSERT(ok);

    // Middle items.
    Vec::item_t itemB = vec.peek(vec.numItems() - 1);
    Vec::item_t itemA = vec.peek(0);
    itemCount = vec.numItems() - 2;
    startAt = 1;
    ok = vec.rmFromIndex(startAt, itemCount, maintainOrder) && (vec.numItems() == 2) && (vec[0] == itemA) && (vec[1] == itemB);
    CPPUNIT_ASSERT(ok);

    // Invalid indices.
    vec = vec1;
    itemCount = 9999;
    ok = (!vec.rmFromIndex(startAt, itemCount, maintainOrder));
    CPPUNIT_ASSERT(ok);
    startAt = 999;
    ok = (!vec.rmFromIndex(startAt, itemCount, maintainOrder));
    CPPUNIT_ASSERT(ok);
}


void VecSuite::testRm02()
{
    Sample1 vec1;
    Vec vec(vec1);

    // Just 1.
    Vec::item_t itemA = vec.peek(vec.numItems() - 1);
    bool maintainOrder = false;
    size_t itemCount = 1;
    size_t startAt = 7;
    bool ok = vec.rmFromIndex(startAt, itemCount, maintainOrder) && (vec.numItems() == vec1.numItems() - 1) && (vec[7] == itemA);

    // All but 2.
    Vec::item_t itemB = vec.peek(vec.numItems() - 1);
    itemA = vec.peek(0);
    itemCount = vec.numItems() - 2;
    startAt = 1;
    ok = vec.rmFromIndex(startAt, itemCount, maintainOrder) && (vec.numItems() == 2) && (vec[0] == itemA) && (vec[1] == itemB);
    CPPUNIT_ASSERT(ok);

    // All.
    itemCount = vec.numItems();
    startAt = 0;
    ok = vec.rmFromIndex(startAt, itemCount, maintainOrder) && (vec.numItems() == 0);
    CPPUNIT_ASSERT(ok);

    // Invalid indices.
    vec = vec1;
    itemCount = 9999;
    ok = (!vec.rmFromIndex(startAt, itemCount, maintainOrder));
    CPPUNIT_ASSERT(ok);
    startAt = 999;
    ok = (!vec.rmFromIndex(startAt, itemCount, maintainOrder));
    CPPUNIT_ASSERT(ok);
}


void VecSuite::testSize00()
{
    bool ok = (sizeof(Growable) == (sizeof(void*) + 12)) && //Win32:16 x64:20
        (sizeof(Vec) == (sizeof(Growable) + sizeof(void*) + 4)); //Win32:24 x64:32
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - void Vec::sort(compare_t compare, Vec& sorted, bool reverseOrder=false) const;
//
void VecSuite::testSort00()
{
    Sample0 vec0;
    for (const char* p = ITEM; *p != 0; ++p)
    {
        unsigned long* item = new unsigned long(*p);
        vec0.add(item);
    }

    unsigned long capacity = vec0.numItems();
    int growBy = 0;
    Vec vec1(capacity, growBy);
    Vec vec2(capacity, growBy);
    bool reverseOrder = false;
    vec0.sort(U32::compareP, vec1, reverseOrder);
    reverseOrder = true;
    vec0.sort(U32::compareP, vec2, reverseOrder);

    bool ok = true;
    unsigned long item = 0;
    for (unsigned long i = 0, numItems = vec1.numItems(), j = numItems - 1; i < numItems; ++i, --j)
    {
        unsigned long item1 = *static_cast<unsigned long*>(vec1.peek(i));
        unsigned long item2 = *static_cast<unsigned long*>(vec2.peek(j));
        if ((item1 != item2) || (item >= item1))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    vec0.sort(U32::compareP, reverseOrder);
    ok = vec0.equals(vec2, U32::compareP);
    CPPUNIT_ASSERT(ok);

    reverseOrder = false;
    vec0.sort(U32::compareP, reverseOrder);
    ok = vec0.equals(vec1, U32::compareP);
    CPPUNIT_ASSERT(ok);

    vec2.reset();
    vec1.reset();
}
