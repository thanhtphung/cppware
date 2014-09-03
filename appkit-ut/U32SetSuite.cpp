#include <string>
#include <strstream>
#include "appkit/U32Set.hpp"

#include "appkit-ut-pch.h"
#include "U32SetSuite.hpp"

using namespace appkit;

const char ITEM[] = "aRandomStringUsedForU32SetPopulation!!!";
const size_t NUM_ITEMS = sizeof(ITEM) - 1;


U32SetSuite::U32SetSuite()
{
}


U32SetSuite::~U32SetSuite()
{
}


//
// Add random keys to empty set.
//
void U32SetSuite::testAdd00()
{
    U32Set set;

    bool ok = true;
    for (const char* p = ITEM; *p; ++p)
    {
        U32Set::key_t key = *p;
        set.add(key);
        if (!set.contains(key))
        {
            ok = false;
            break;
        }
        validateSet(set);
    }
    CPPUNIT_ASSERT(ok);
}


//
// Add even keys to set of odd keys.
//
void U32SetSuite::testAdd01()
{
    U32Set set;
    for (U32Set::key_t i = 1; i <= 99; set.add(i), i += 2);

    // Add even keys to set of odd keys.
    bool ok = true;
    for (U32Set::key_t i = 0; i < 100; i += 2)
    {
        if ((!set.add(i)) || (!set.contains(i)) || (set.findIndex(i) != 0))
        {
            ok = false;
            break;
        }
        validateSet(set);
    }
    CPPUNIT_ASSERT(ok);

    ok = (set.numKeys() == 100) && (set.numRanges() == 1);
    CPPUNIT_ASSERT(ok);
}


//
// Add overlapped keys.
//
void U32SetSuite::testAdd02()
{
    U32Set set;
    for (U32Set::key_t i = 1; i <= 99; set.add(i), i += 2);
    U32Set::key_t lo = 0;
    U32Set::key_t hi = 99;
    bool ok = set.add(lo, hi);
    CPPUNIT_ASSERT(ok);
    validateSet(set);

    ok = (set.numKeys() == 100) && (set.numRanges() == 1);
    CPPUNIT_ASSERT(ok);
}


//
// Add partially valid key ranges.
//
void U32SetSuite::testAdd03()
{
    U32Set::key_t validMin = 10;
    U32Set::key_t validMax = 99;
    U32Set tmp(validMin, validMax);
    U32Set::key_t lo = 5;
    U32Set::key_t hi = 13;
    bool ok = tmp.add(5, 13);
    CPPUNIT_ASSERT(ok);
    validateSet(tmp);
    ok = (tmp.numKeys() == 4) && (tmp.numRanges() == 1);
    CPPUNIT_ASSERT(ok);

    tmp.reset();
    lo = 81;
    hi = 1000;
    ok = tmp.add(lo, hi);
    CPPUNIT_ASSERT(ok);
    validateSet(tmp);
    ok = (tmp.numKeys() == 19) && (tmp.numRanges() == 1);
    CPPUNIT_ASSERT(ok);

    tmp.reset();
    ok = tmp.add("5-13, 81-1000");
    validateSet(tmp);
    CPPUNIT_ASSERT(ok);
    ok = (tmp.numKeys() == 23) && (tmp.numRanges() == 2);
    CPPUNIT_ASSERT(ok);

    tmp.reset();
    ok = tmp.add(U32Set("5-13,\r\n81-1000"));
    CPPUNIT_ASSERT(ok);
    validateSet(tmp);
    ok = (tmp.numKeys() == 23) && (tmp.numRanges() == 2);
    CPPUNIT_ASSERT(ok);
}


//
// Add invalid keys and invalid key ranges.
//
void U32SetSuite::testAdd04()
{
    U32Set::key_t validMin = 10;
    U32Set::key_t validMax = 99;
    U32Set tmp(validMin, validMax);
    bool ok = (!tmp.add(5)) && (!tmp.add(1, 9)) && (!tmp.add(100, 999)) && (!tmp.add(555));
    CPPUNIT_ASSERT(ok);

    ok = (!tmp.add("5")) && (!tmp.add("1-9")) && (!tmp.add("100-999")) && (!tmp.add("555"));
    CPPUNIT_ASSERT(ok);

    ok = (!tmp.add(U32Set("5"))) &&
        (!tmp.add(U32Set("1-9"))) &&
        (!tmp.add(U32Set("100-999"))) &&
        (!tmp.add(U32Set("555")));
    CPPUNIT_ASSERT(ok);
}


void U32SetSuite::testAdd05()
{

    // Add all keys to an empty set.
    U32Set tmp;
    U32Set::key_t lo = 0;
    U32Set::key_t hi = 0xffffffffU;
    bool ok = tmp.add(lo, hi) && (tmp.numKeys() == 0x100000000ULL) && (tmp.numRanges() == 1);
    CPPUNIT_ASSERT(ok);

    // Adding to a full set.
    ok = (!tmp.add(tmp)) && (!tmp.add("1")) && (!tmp.add(lo) && (!tmp.add(lo, hi)));
    CPPUNIT_ASSERT(ok);

    // Add all keys to a non-empty set.
    tmp.reset();
    for (U32Set::key_t i = 0; i < 100; tmp.add(i), i += 2);
    ok = tmp.add(lo, hi) && (tmp.numKeys() == 0x100000000ULL) && (tmp.numRanges() == 1);
    CPPUNIT_ASSERT(ok);
}


//
// Try the contains() methods.
//
void U32SetSuite::testCmp00()
{
    U32Set set0;
    U32Set set1;
    for (U32Set::key_t i = 1; i <= 99; set1.add(i), i += 2);
    U32Set set2;
    for (U32Set::key_t i = 0; i < 100; set2.add(i), i += 2);

    U32Set tmp;
    bool ok = (!tmp.contains(set1));
    CPPUNIT_ASSERT(ok);

    tmp.add(1);
    tmp.add(9);
    ok = set1.contains(tmp) && tmp.contains(set0) && (!set2.contains(tmp)) && (!tmp.contains(set1));
    CPPUNIT_ASSERT(ok);
}


//
// Try the overlaps() methods.
//
void U32SetSuite::testCmp01()
{
    U32Set set0;
    U32Set set1;
    for (U32Set::key_t i = 1; i <= 99; set1.add(i), i += 2);
    U32Set set2;
    for (U32Set::key_t i = 0; i < 100; set2.add(i), i += 2);

    // One set is empty.
    U32Set tmp0;
    tmp0.add(1);
    tmp0.add(9);
    bool ok = (!set0.overlaps(tmp0)) && (!tmp0.overlaps(set0));
    CPPUNIT_ASSERT(ok);

    // Given sets cannot overlap.
    U32Set tmp1;
    tmp1.add(111);
    tmp1.add(999);
    ok = (!tmp0.overlaps(tmp1)) && (!tmp1.overlaps(tmp0));
    CPPUNIT_ASSERT(ok);

    // Given sets might overlap.
    ok = set1.overlaps(tmp0) && tmp0.overlaps(set1) && (!set2.overlaps(tmp0)) && (!tmp0.overlaps(set2));
    CPPUNIT_ASSERT(ok);
}


//
// Try the compareP() method.
//
void U32SetSuite::testCmp02()
{
    U32Set set0;
    U32Set set1;
    for (U32Set::key_t i = 1; i <= 99; set1.add(i), i += 2);
    U32Set set2;
    for (U32Set::key_t i = 0; i < 100; set2.add(i), i += 2);

    U32Set set(set1);
    set.rm(1, 1);
    bool ok = ((U32Set::compareP(&set0, &set1) < 0) &&
        (U32Set::compareP(&set, &set1) > 0) &&
        (U32Set::compareP(&set2, &set1) < 0));
    CPPUNIT_ASSERT(ok);

    ok = ((U32Set::compareP(&set1, &set0) > 0) &&
        (U32Set::compareP(&set1, &set) < 0) &&
        (U32Set::compareP(&set1, &set2) > 0));
    CPPUNIT_ASSERT(ok);

    set = set1;
    ok = (U32Set::compareP(&set, &set1) == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Default constructor.
//
void U32SetSuite::testCtor00()
{
    U32Set set;
    bool ok = set.canGrow() &&
        (set.growthFactor() == -1) &&
        (set.numKeys() == 0) &&
        (set.numRanges() == 0) &&
        (set.capacity() == U32Set::DefaultCap) &&
        (set.validMin() == U32Set::VALID_MIN) &&
        (set.validMax() == U32Set::VALID_MAX);
    CPPUNIT_ASSERT(ok);

    int growBy = 0;
    ok = (!set.setGrowth(growBy));
    CPPUNIT_ASSERT(ok);
    growBy = -1;
    ok = set.setGrowth(growBy);
    CPPUNIT_ASSERT(ok);
}


//
// Construct with initial members.
//
void U32SetSuite::testCtor01()
{
    U32Set set0("1,1,3,5,7,9,1000-1999,9990-9994,9992,9999,11,13,15,17,19,19");
    bool ok = (set0.numKeys() == 1016) && (set0.numRanges() == 13);
    CPPUNIT_ASSERT(ok);
    validateSet(set0);

    U32Set set1("1;1;3;5;7;9;1000-1999;9990-9994;9992;9999;11;13;15;17;19;19", ';');
    ok = (set1.numKeys() == 1016) && (set1.numRanges() == 13);
    CPPUNIT_ASSERT(ok);
    validateSet(set1);

    U32Set set2("1 1 3 5 7 9 1000-1999 9990-9994 9992 9999 11 13 15 17 19 19", ' ');
    ok = (set2.numKeys() == 1016) && (set2.numRanges() == 13);
    CPPUNIT_ASSERT(ok);
    validateSet(set2);
}


//
// Construct with empty string.
//
void U32SetSuite::testCtor02()
{
    U32Set set0;
    String s(set0.toString());
    U32Set set(s.ascii());
    bool ok = (set == set0) && (set0 == set);
    CPPUNIT_ASSERT(ok);
}


//
// Construct with non-empty string.
//
void U32SetSuite::testCtor03()
{
    U32Set set0;
    for (const char* p = ITEM; *p; set0.add(*p++));

    std::ostringstream oss;
    oss << set0;
    String str(oss.str().c_str());
    U32Set set1(str.ascii());
    bool ok = (set1 == set0) && (set0 == set1);
    CPPUNIT_ASSERT(ok);

    str = set0.toString(";");
    U32Set set2(str.ascii(), ';');
    ok = (set2 == set0) && (set0 == set2);
    CPPUNIT_ASSERT(ok);

    str = set0.toString(" ");
    U32Set set3(str.ascii(), ' ');
    ok = (set3 == set0) && (set0 == set3);
    CPPUNIT_ASSERT(ok);
}


void U32SetSuite::testCtor04()
{
    U32Set set;
    for (U32Set::key_t i = 1; i <= 99; set.add(i), i += 2);

    // Copy constructor.
    U32Set set0(set);
    bool ok = (set0 == set);
    CPPUNIT_ASSERT(ok);

    // Zero capacity.
    U32Set set1(U32Set::VALID_MIN, U32Set::VALID_MAX, 0);
    ok = set1.canGrow() &&
        (set1.capacity() == 1) &&
        (set1.growthFactor() == -1) &&
        (set1.numKeys() == 0) &&
        (set1.numRanges() == 0) &&
        (set1.validMin() == U32Set::VALID_MIN) &&
        (set1.validMax() == U32Set::VALID_MAX);
    CPPUNIT_ASSERT(ok);

    // Set of all valid keys.
    U32Set set2("0x00000000-0xffffffff");
    ok = ((set2.numKeys() == 0x100000000ULL) && (set2.numRanges() == 1));
    CPPUNIT_ASSERT(ok);
}


//
// Try iterating from low to high.
//
void U32SetSuite::testItor00()
{
    U32Set set("1,10-99,100002");
    U32Set::Itor it(set, false /*makeCopy*/);

    U32Set::key_t loKey;
    U32Set::key_t hiKey;
    bool ok = (!it.curRange(loKey, hiKey));
    CPPUNIT_ASSERT(ok);

    ok = it.nextRange(loKey, hiKey) && (loKey == 1) && (hiKey == 1);
    CPPUNIT_ASSERT(ok);

    ok = it.nextRange(loKey, hiKey) && (loKey == 10) && (hiKey == 99);
    CPPUNIT_ASSERT(ok);

    ok = it.nextRange(loKey, hiKey) && (loKey == 100002) && (hiKey == 100002);
    CPPUNIT_ASSERT(ok);

    ok = (!it.nextRange(loKey, hiKey));
    CPPUNIT_ASSERT(ok);

    ok = (!it.nextRange(loKey, hiKey));
    CPPUNIT_ASSERT(ok);

    loKey = 0;
    hiKey = 0;
    ok = it.curRange(loKey, hiKey) && (loKey == 100002) && (hiKey == 100002);
    CPPUNIT_ASSERT(ok);

    it.reset();
    ok = it.nextRange(loKey, hiKey) && (loKey == 1) && (hiKey == 1);
    CPPUNIT_ASSERT(ok);

    loKey = 0;
    hiKey = 0;
    ok = it.curRange(loKey, hiKey) && (loKey == 1) && (hiKey == 1);
    CPPUNIT_ASSERT(ok);
}


//
// Try iterating from low to high.
//
void U32SetSuite::testItor01()
{
    U32Set set("1,10-99,100002");
    U32Set::Itor it(set, false /*makeCopy*/);

    U32Set::key_t key;
    bool ok = (!it.curKey(key));
    CPPUNIT_ASSERT(ok);

    ok = it.nextKey(key) && (key == 1);
    CPPUNIT_ASSERT(ok);

    for (U32Set::key_t i = 10; i <= 99; ++i)
    {
        if ((!it.nextKey(key)) || (key != i))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = it.nextKey(key) && (key == 100002);
    CPPUNIT_ASSERT(ok);

    ok = (!it.nextKey(key));
    CPPUNIT_ASSERT(ok);

    ok = (!it.nextKey(key));
    CPPUNIT_ASSERT(ok);

    key = 0;
    ok = it.curKey(key) && (key == 100002);
    CPPUNIT_ASSERT(ok);

    it.reset();
    ok = it.nextKey(key) && (key == 1);
    CPPUNIT_ASSERT(ok);

    key = 0;
    ok = it.curKey(key) && (key == 1);
    CPPUNIT_ASSERT(ok);
}


//
// Try iterating from high to low.
//
void U32SetSuite::testItor02()
{
    U32Set set("1,10-99,100002");
    U32Set::Itor it(set, false /*makeCopy*/);

    U32Set::key_t loKey;
    U32Set::key_t hiKey;
    bool ok = it.prevRange(loKey, hiKey) && (loKey == 100002) && (hiKey == 100002);
    CPPUNIT_ASSERT(ok);

    ok = it.prevRange(loKey, hiKey) && (loKey == 10) && (hiKey == 99);
    CPPUNIT_ASSERT(ok);

    ok = it.prevRange(loKey, hiKey) && (loKey == 1) && (hiKey == 1);
    CPPUNIT_ASSERT(ok);

    ok = (!it.prevRange(loKey, hiKey));
    CPPUNIT_ASSERT(ok);

    ok = (!it.prevRange(loKey, hiKey));
    CPPUNIT_ASSERT(ok);

    loKey = 0;
    hiKey = 0;
    ok = it.curRange(loKey, hiKey) && (loKey == 1) && (hiKey == 1);
    CPPUNIT_ASSERT(ok);

    it.reset();
    ok = it.prevRange(loKey, hiKey) && (loKey == 100002) && (hiKey == 100002);
    CPPUNIT_ASSERT(ok);

    loKey = 0;
    hiKey = 0;
    ok = it.curRange(loKey, hiKey) && (loKey == 100002) && (hiKey == 100002);
    CPPUNIT_ASSERT(ok);
}


//
// Try iterating from high to low.
//
void U32SetSuite::testItor03()
{
    U32Set set("1,10-99,100002");
    U32Set::Itor it(set, false /*makeCopy*/);

    U32Set::key_t key;
    bool ok = it.prevKey(key) && (key == 100002);
    CPPUNIT_ASSERT(ok);

    for (U32Set::key_t i = 99; i >= 10; --i)
    {
        if ((!it.prevKey(key)) || (key != i))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = it.prevKey(key) && (key == 1);
    CPPUNIT_ASSERT(ok);

    ok = (!it.prevKey(key));
    CPPUNIT_ASSERT(ok);

    ok = (!it.prevKey(key));
    CPPUNIT_ASSERT(ok);

    key = 0;
    ok = it.curKey(key) && (key == 1);
    CPPUNIT_ASSERT(ok);

    it.reset();
    ok = it.prevKey(key) && (key == 100002);
    CPPUNIT_ASSERT(ok);

    key = 0;
    ok = it.curKey(key) && (key == 100002);
    CPPUNIT_ASSERT(ok);
}


void U32SetSuite::testNew00()
{
    U32Set* set0 = new U32Set;
    bool ok = (set0 != 0);
    CPPUNIT_ASSERT(ok);
    delete set0;

    unsigned char buf[sizeof(*set0)];
    set0 = new(buf)U32Set;
    ok = (reinterpret_cast<unsigned char*>(set0) == buf);
    CPPUNIT_ASSERT(ok);
    set0->U32Set::~U32Set();
}


//
// Try intersecting using an empty set. Resulting set remains empty.
//
void U32SetSuite::testOp00()
{
    U32Set set0;
    U32Set set1;
    for (U32Set::key_t i = 1; i <= 99; set1.add(i), i += 2);

    set0 &= set1;
    validateSet(set0);
    bool ok = (set0.numKeys() == 0) && (set0.numRanges() == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Try intersecting using an empty set. Resulting set becomes empty.
//
void U32SetSuite::testOp01()
{
    U32Set set0;
    U32Set set1;
    for (U32Set::key_t i = 1; i <= 99; set1.add(i), i += 2);

    set1 &= set0;
    validateSet(set1);
    bool ok = (set1 == set0) && (set0 == set1);
    CPPUNIT_ASSERT(ok);
}


//
// Try intersecting using non-empty sets. Resulting set becomes empty.
//
void U32SetSuite::testOp02()
{
    U32Set set0;
    U32Set set1;
    for (U32Set::key_t i = 1; i <= 99; set1.add(i), i += 2);
    U32Set set2;
    for (U32Set::key_t i = 0; i < 100; set2.add(i), i += 2);

    set1 &= set2;
    validateSet(set1);
    bool ok = (set1 == set0) && (set0 == set1);
    CPPUNIT_ASSERT(ok);
}


//
// Try intersecting using non-empty sets. Resulting set is non-empty.
//
void U32SetSuite::testOp03()
{
    U32Set set1;
    for (U32Set::key_t i = 1; i <= 99; set1.add(i), i += 2);

    U32Set tmp("0-99");
    tmp &= set1;
    validateSet(tmp);
    bool ok = (tmp == set1) && (set1 == tmp);
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator. No growth.
//
void U32SetSuite::testOp04()
{
    U32Set set1;
    for (U32Set::key_t i = 1; i <= 99; set1.add(i), i += 2);

    U32Set set(U32Set::VALID_MIN, U32Set::VALID_MAX, set1.capacity());
    set = set1;
    bool ok = (set == set1) && (set.capacity() == set1.capacity()) && (set.initialCap() == set1.capacity());
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator. Growth required.
//
void U32SetSuite::testOp05()
{
    U32Set set2;
    for (U32Set::key_t i = 0; i < 100; set2.add(i), i += 2);

    unsigned int capacity = 8;
    U32Set set(U32Set::VALID_MIN, U32Set::VALID_MAX, capacity);
    set = set2;
    bool ok = (set == set2) && (set.capacity() == 64) && (set.initialCap() == 8);
    CPPUNIT_ASSERT(ok);
}


//
// Remove random keys.
//
void U32SetSuite::testRm00()
{
    U32Set set0;
    for (const char* p = ITEM; *p; set0.add(*p++));
    set0.add("1,3,5,7,9,1000-1999,9990-9995,9999,11,13,15,17,19\n");

    U32Set::key_t loKey = 'A';
    U32Set::key_t hiKey = 'Z';
    set0.rm(loKey, hiKey);
    validateSet(set0);

    loKey = 'a';
    hiKey = 'z';
    set0.rm(loKey, hiKey);
    validateSet(set0);

    loKey = '0';
    hiKey = '9';
    set0.rm(loKey, hiKey);
    validateSet(set0);

    loKey = '!';
    hiKey = '!';
    set0.rm(loKey, hiKey);
    validateSet(set0);

    set0.rm("0-9,9990-9999");
    validateSet(set0);

    set0.rm("10-19;1234-1345;9980-9989\n", ';');
    validateSet(set0);

    set0.rm("20-1233 1346-9979", ' ');
    validateSet(set0);

    bool ok = (set0.numKeys() == 0) && (set0.numRanges() == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Start with one contiguous range. Remove even keys to maximize
// the number of ranges. Remove odd keys to collapse the set.
//
void U32SetSuite::testRm01()
{

    // Start with one contiguous range 0-99.
    U32Set::key_t lo = 0;
    U32Set::key_t hi = 99;
    U32Set set0;
    set0.add(lo, hi);

    // Remove even keys.
    bool ok = true;
    for (U32Set::key_t i = 0; i <= 99; i += 2)
    {
        if ((!set0.rm(i)) || set0.contains(i) || (set0.findIndex(i) != U32Set::INVALID_INDEX) || (set0.findIndex(i + 1) != (i / 2)))
        {
            ok = false;
            break;
        }
        validateSet(set0);
    }
    CPPUNIT_ASSERT(ok);

    ok = (set0.numKeys() == 50) && (set0.numRanges() == 50);
    CPPUNIT_ASSERT(ok);

    // Remove odd keys.
    for (U32Set::key_t i = 1; i <= 99; i += 2)
    {
        if ((set0.findIndex(i) != 0) || (!set0.rm(i)) || set0.contains(i))
        {
            ok = false;
            break;
        }
        validateSet(set0);
    }
    CPPUNIT_ASSERT(ok);

    ok = (set0.numKeys() == 0) && (set0.numRanges() == 0);
    CPPUNIT_ASSERT(ok);
}


void U32SetSuite::testRm02()
{

    // Remove all keys from a full set.
    U32Set tmp;
    U32Set::key_t lo = 0;
    U32Set::key_t hi = 0xffffffffU;
    tmp.add(lo, hi);
    bool ok = tmp.rm(lo, hi) && (tmp.numKeys() == 0) && (tmp.numRanges() == 0);
    CPPUNIT_ASSERT(ok);

    // Remove some keys from a full set.
    tmp.add(lo, hi);
    ok = tmp.rm(lo, lo) && (tmp.numKeys() == 0xffffffffULL) && (tmp.numRanges() == 1);
    CPPUNIT_ASSERT(ok);
    ok = tmp.rm(hi, hi) && (tmp.numKeys() == 0xfffffffeULL) && (tmp.numRanges() == 1);
    CPPUNIT_ASSERT(ok);
    lo = 0x12345678U;
    hi = 0x12345679U;
    ok = tmp.rm(lo, hi) && (tmp.numKeys() == 0xfffffffcULL) && (tmp.numRanges() == 2);
    CPPUNIT_ASSERT(ok);
}


void U32SetSuite::testSize00()
{
    size_t size = sizeof(U32Set);
    bool ok = (size == (sizeof(Set) + sizeof(void*) + 20)); //Win32:44 x64:56
    CPPUNIT_ASSERT(ok);
}


//
// Validate given set.
//
void U32SetSuite::validateSet(const U32Set& set)
{
    if (set.numKeys() == 0)
    {
        bool ok = (set.numRanges() == 0);
        CPPUNIT_ASSERT(ok);
        return;
    }

    U32Set::Itor it(set, false /*makeCopy*/);
    U32Set::key_t prevKey;
    bool ok = it.nextKey(prevKey);
    CPPUNIT_ASSERT(ok);

    U32Set::key_t key;
    for (; it.nextKey(key); prevKey = key)
    {
        if (prevKey >= key)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    it.reset();
    U32Set::key_t prevHiKey;
    U32Set::key_t prevLoKey;
    ok = it.nextRange(prevLoKey, prevHiKey);
    CPPUNIT_ASSERT(ok);

    U32Set::key_t hiKey;
    U32Set::key_t loKey;
    for (; it.nextRange(loKey, hiKey); prevLoKey = loKey, prevHiKey = hiKey)
    {
        if ((loKey > hiKey) || (loKey <= prevHiKey))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}
