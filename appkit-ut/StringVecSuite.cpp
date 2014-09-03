#include "appkit/DelimitedTxt.hpp"
#include "appkit/StringVec.hpp"
#include "appkit/U32.hpp"

#include "appkit-ut-pch.h"
#include "StringVecSuite.hpp"

using namespace appkit;

const char ITEM[] = "aRandomStringUsedForStringVecPopulation!!!";

BEGIN_NAMESPACE


class Sample0: public StringVec
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
StringVec(32 /*capacity*/, -1 /*growBy*/)
{
    String item;
    for (const char* p = ITEM; *p; ++p)
    {
        size_t count = (*p % 3) + 1;
        item.reset(count, *p);
        add(item);
    }
}

Sample0::~Sample0()
{
}


class Sample1: public StringVec
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
StringVec(21 /*capacity*/, 13 /*growBy*/)
{
    String item;
    for (unsigned int i = 1; i <= 32; i += 2)
    {
        item = U32(i);
        add(item);
    }
}

Sample1::~Sample1()
{
}

END_NAMESPACE


StringVecSuite::StringVecSuite()
{
}


StringVecSuite::~StringVecSuite()
{
}


int StringVecSuite::compareNumericStrings(const void* item0, const void* item1)
{
    const String* s0 = static_cast<const String*>(item0);
    const String* s1 = static_cast<const String*>(item1);
    unsigned int k0 = U32(*s0);
    unsigned int k1 = U32(*s1);
    int rc = (k0 < k1)? (-1): ((k0 > k1)? 1: 0);
    return rc;
}


void StringVecSuite::testAdd00()
{
    StringVec vec0(64 /*capacity*/, 0 /*growBy*/);

    // Add random items to a fixed-capacity empty vector.
    bool ok = true;
    String item;
    for (const char* p = ITEM; *p; ++p)
    {
        size_t count = (*p % 3) + 1;
        item.reset(count, *p);
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


void StringVecSuite::testAdd01()
{
    Sample1 vec1;

    // Add random items using addAtIndex().
    const char* p;
    bool ok = true;
    String item;
    for (p = ITEM; *p; ++p)
    {
        size_t count = (*p % 3) + 1;
        item.reset(count, *p);
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
// - unsigned int StringVec::add(size_t count, const String& item);
//
void StringVecSuite::testAdd02()
{
    unsigned int capacity = 3;
    int growBy = 0;
    StringVec vec(capacity, growBy);
    unsigned int count = 0;
    String item;
    item = U32(12);
    unsigned int numAdds = vec.add(count, item);
    bool ok = ((numAdds == 0) && (vec.numItems() == 0));
    CPPUNIT_ASSERT(ok);
    count = 1;
    numAdds = vec.add(count, item);
    ok = ((numAdds == 1) && (vec.numItems() == 1) && (vec[0] == "12"));
    CPPUNIT_ASSERT(ok);

    vec.reset();
    count = 5;
    item = U32(1234);
    numAdds = vec.add(count, item);
    ok = ((numAdds == 3) && (vec.numItems() == 3));
    CPPUNIT_ASSERT(ok);
    ok = ((vec[0] == "1234") && (vec[1] == "1234") && (vec[2] == "1234"));
    CPPUNIT_ASSERT(ok);
    count = 1;
    numAdds = vec.add(count, item);
    ok = (numAdds == 0);
    CPPUNIT_ASSERT(ok);

    vec.reset();
    count = 2;
    item = U32(12345);
    numAdds = vec.add(count, item);
    ok = ((numAdds == 2) && (vec.numItems() == 2));
    CPPUNIT_ASSERT(ok);
    ok = ((vec[0] == "12345") && (vec[1] == "12345"));
    CPPUNIT_ASSERT(ok);
    item = U32(123);
    numAdds = vec.add(count, item);
    ok = ((numAdds == 1) && (vec.numItems() == 3));
    CPPUNIT_ASSERT(ok);
    ok = ((vec[0] == "12345") && (vec[1] == "12345") && (vec[2] == "123"));
    CPPUNIT_ASSERT(ok);

    growBy = -1;
    vec.setGrowth(growBy);
    item = U32(12345);
    numAdds = vec.add(count, item);
    ok = ((numAdds == 2) && (vec.numItems() == 5));
    CPPUNIT_ASSERT(ok);
    ok = ((vec[0] == "12345") && (vec[1] == "12345") && (vec[2] == "123") && (vec[3] == "12345") && (vec[4] == "12345"));
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - unsigned int StringVec::add(const String* array, size_t itemCount);
//
void StringVecSuite::testAdd03()
{
    unsigned int capacity = 3;
    int growBy = 0;
    StringVec vec(capacity, growBy);
    unsigned int count = 0;
    String item[] = {"12", "34", "56", "78", "99"};
    unsigned int numAdds = vec.add(item, count);
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


void StringVecSuite::testAdd04()
{
    Sample0 vec0;

    StringVec vec;
    size_t startAt = 0;
    size_t itemCount = 1;
    unsigned int numAdds = vec.add(vec0, startAt, itemCount);
    bool ok = (numAdds == itemCount);
    CPPUNIT_ASSERT(ok);

    startAt = 1;
    itemCount = vec0.numItems() - 1;
    numAdds = vec.add(vec0, startAt, itemCount);
    ok = (numAdds == itemCount) && (vec == vec0);
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
void StringVecSuite::testCtor00()
{
    StringVec vec;
    bool ok = (vec.canGrow() &&
        (vec.growthFactor() < 0) &&
        (vec.numItems() == 0) &&
        (vec.capacity() == StringVec::DefaultCap));
    CPPUNIT_ASSERT(ok);
}


void StringVecSuite::testCtor01()
{
    Sample1 vec1;

    const char* delim = ";";
    String s(vec1.stringify(delim));
    bool ok = (s == "1;3;5;7;9;11;13;15;17;19;21;23;25;27;29;31");
    CPPUNIT_ASSERT(ok);

    bool makeCopy = false;
    DelimitedTxt txt(s, makeCopy, *delim);
    bool trimLines = true;
    StringVec vec(txt, trimLines);
    ok = (vec == vec1);
    CPPUNIT_ASSERT(ok);

    trimLines = false;
    {
        StringVec tmp(txt, trimLines);
        vec = &tmp; //move guts to vec
    }
    ok = (vec.numItems() == 16) && (vec[0] == "1;") && (vec[15] == "31");
    CPPUNIT_ASSERT(ok);

    trimLines = true;
    unsigned int capacity = 6;
    int growBy = 0;
    {
        StringVec tmp(txt, trimLines, capacity, growBy);
        vec = &tmp; //move guts to vec
    }
    ok = (vec.numItems() == 6) && (vec.stringify() == "1357911");
    CPPUNIT_ASSERT(ok);

    vec.truncate(5);
    bool reverseOrder = true;
    vec.sort(reverseOrder);
    ok = (vec.stringify() == "97531");
    CPPUNIT_ASSERT(ok);
    reverseOrder = false;
    vec.sort(reverseOrder);
    ok = (vec.stringify() == "13579");
    CPPUNIT_ASSERT(ok);

    StringVec vec2(&vec);
    ok = (vec2.stringify() == "13579") && (vec.capacity() == 0U - 1);
    CPPUNIT_ASSERT(ok);
}


void StringVecSuite::testFind00()
{
    Sample1 vec1;

    bool ok = true;
    String item;
    for (unsigned int i = 0, u32 = 1; u32 <= 32; ++i, u32 += 2)
    {
        item = U32(u32);
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


void StringVecSuite::testFindMaxLength00()
{
    Sample0 vec0;

    bool useByteSize = false;
    unsigned int length = vec0.findMaxLength(useByteSize);
    bool ok = (length == 3);
    CPPUNIT_ASSERT(ok);
    useByteSize = true;
    length = vec0.findMaxLength(useByteSize);
    ok = (length == 4);
    CPPUNIT_ASSERT(ok);
}


void StringVecSuite::testFindMinLength00()
{
    Sample0 vec0;

    bool useByteSize = false;
    unsigned int length = vec0.findMinLength(useByteSize);
    bool ok = (length == 1);
    CPPUNIT_ASSERT(ok);
    useByteSize = true;
    length = vec0.findMinLength(useByteSize);
    ok = (length == 2);
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator. No growing required.
//
void StringVecSuite::testOp00()
{
    Sample1 vec1;

    StringVec vec(vec1.numItems(), 0 /*growBy*/);
    vec = vec1;
    bool ok = (vec.initialCap() == vec.capacity());
    CPPUNIT_ASSERT(ok);
    ok = (vec == vec1);
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator. Exponential growth required.
//
void StringVecSuite::testOp01()
{
    Sample1 vec1;

    StringVec vec(3 /*capacity*/, -1 /*growBy*/);
    vec = vec1;
    bool ok = ((vec.initialCap() == 3) && (vec.capacity() >= vec1.numItems()));
    CPPUNIT_ASSERT(ok);
    ok = (vec == vec1);
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator. Linear growth required.
//
void StringVecSuite::testOp02()
{
    Sample0 vec0;
    Sample1 vec1;

    StringVec vec(3 /*capacity*/, 3 /*growBy*/);
    vec = vec1;
    bool ok = ((vec.initialCap() == 3) && (vec.capacity() >= vec1.numItems()));
    CPPUNIT_ASSERT(ok);
    ok = (vec == vec1);
    CPPUNIT_ASSERT(ok);
    ok = (!(vec0 == vec1));
    CPPUNIT_ASSERT(ok);

    // Also check setItem() and reset() here.
    String item;
    unsigned int v = 0U;
    item = U32(v);
    vec.setItem(5, item);
    ok = (vec.peek(5) == item) && (vec != vec1);
    CPPUNIT_ASSERT(ok);
    item = vec1.stringify();
    String replacedItem;
    vec.setItem(9, item, replacedItem);
    ok = (vec.peek(9) == "135791113151719212325272931") && (replacedItem == vec1.peek(9));
    CPPUNIT_ASSERT(ok);
    vec.reset();
    ok = (vec.numItems() == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator. Truncation required.
//
void StringVecSuite::testOp03()
{
    Sample1 vec1;

    StringVec vec(3 /*capacity*/, 0 /*growBy*/);
    vec = vec1;
    vec = vec; //no-op
    bool ok = (vec.numItems() == 3);
    CPPUNIT_ASSERT(ok);

    ok = vec1.truncate(3) && (vec1 == vec) && (vec1 == StringVec(Sample1(), 0, 3));
    CPPUNIT_ASSERT(ok);
}


void StringVecSuite::testResize00()
{
    StringVec vec0(64 /*capacity*/, 0 /*growBy*/);
    Sample1 vec1;

    // no-op
    bool ok = vec0.resize(vec0.capacity());
    CPPUNIT_ASSERT(ok);

    // No truncation allowed.
    ok = (!vec1.resize(vec1.numItems() - 1));
    CPPUNIT_ASSERT(ok);

    StringVec vec(vec1);
    ok = vec.resize(vec.numItems());
    CPPUNIT_ASSERT(ok);
    ok = (vec == vec1);
    CPPUNIT_ASSERT(ok);
}


//
// Remove various items.
//
void StringVecSuite::testRm00()
{
    Sample1 vec1;
    StringVec vec(vec1);

    // Last item.
    String item(vec1.peek(vec1.numItems() - 1));
    String removedItem;
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
    item = U32(23456);
    ok = (!vec.rm(item));
    CPPUNIT_ASSERT(ok);

    // Invalid index.
    ok = ((!vec.rmFromIndex(9999)) && (vec.numItems() == (vec1.numItems() - 3)));
    CPPUNIT_ASSERT(ok);
}


//
// Remove various items.
//
void StringVecSuite::testRm01()
{
    Sample1 vec1;
    StringVec vec(vec1);

    // Last item.
    String item(vec1.peek(vec1.numItems() - 1));
    bool ok = (vec.rm(item.ascii(), true) && (!vec.find(item)));
    CPPUNIT_ASSERT(ok);

    // First item.
    item = vec1.peek(0);
    ok = (vec.rm(item.ascii(), false) && (!vec.find(item)));
    CPPUNIT_ASSERT(ok);

    // Other item.
    // Removed item.
    // Non-existent item.
    String removedItem;
    item = vec1.peek(3);
    ok = (vec.rm(item.ascii(), true) && (!vec.find(item)));
    CPPUNIT_ASSERT(ok);
    ok = (!vec.rm(item.ascii()));
    CPPUNIT_ASSERT(ok);
    item = U32(23456);
    ok = (!vec.rm(item.ascii()));
    CPPUNIT_ASSERT(ok);
}


void StringVecSuite::testSort00()
{
    Sample1 vec1;

    StringVec vec(vec1.numItems(), 0);
    bool reverseOrder = false;
    vec1.sort(compareNumericStrings, vec, reverseOrder);
    bool ok = (vec.numItems() == vec1.numItems());
    CPPUNIT_ASSERT(ok);
    ok = (vec == vec1);
    CPPUNIT_ASSERT(ok);

    ok = vec.search(vec1.peek(0));
    CPPUNIT_ASSERT(ok);

    String prevItem(vec.peek(0));
    U32 prevValue(prevItem);
    for (size_t i = 1, numItems = vec.numItems(); i<numItems; ++i)
    {
        String item(vec.peek(i));
        U32 value(item);
        if ((prevValue > value) || (!vec.search(compareNumericStrings, vec1.peek(i))))
        {
            ok = false;
            break;
        }
        prevValue = value;
    }
    CPPUNIT_ASSERT(ok);
}


void StringVecSuite::testSort01()
{
    Sample1 vec1;

    StringVec vec(vec1.numItems(), 0);
    bool reverseOrder = true;
    vec1.sort(compareNumericStrings, vec, reverseOrder);
    bool ok = (vec.numItems() == vec1.numItems());
    CPPUNIT_ASSERT(ok);

    ok = vec.find(vec1.peek(0));
    CPPUNIT_ASSERT(ok);

    String prevItem(vec.peek(0));
    U32 prevValue(prevItem);
    for (size_t i = 1, numItems = vec.numItems(); i < numItems; ++i)
    {
        String item(vec.peek(i));
        U32 value(item);
        if ((prevValue < value) || (!vec.find(vec1.peek(i))))
        {
            ok = false;
            break;
        }
        prevValue = value;
    }
    CPPUNIT_ASSERT(ok);
}
