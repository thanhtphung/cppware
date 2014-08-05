#include "syskit/HeapX.hpp"

#include "syskit-ut-pch.h"
#include "HeapXSuite.hpp"

using namespace syskit;

const char ITEMS[] = "aRandomStringUsedForHeapXPopulation!!!";


HeapXSuite::HeapXSuite()
{
}


HeapXSuite::~HeapXSuite()
{
}


//
// Comparison function (ascending order).
//
int HeapXSuite::compare(const void* item0, const void* item1)
{
    long c0 = *static_cast<const char*>(item0);
    long c1 = *static_cast<const char*>(item1);
    return c0 - c1;
}


//
// Comparison function (descending order).
//
int HeapXSuite::compareR(const void* item0, const void* item1)
{
    long c0 = *static_cast<const char*>(item0);
    long c1 = *static_cast<const char*>(item1);
    return c1 - c0;
}


void HeapXSuite::testAdd00()
{
    HeapX heap0(compare, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(const_cast<char*>(p++)));
    HeapX heap1(compareR, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap1.add(const_cast<char*>(p++)));
    HeapX heap2(compare, 2 /*capacity*/, -1 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap2.add(const_cast<char*>(p++)));
    HeapX heap3(compareR, 8 /*capacity*/, 5 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap3.add(const_cast<char*>(p++)));

    size_t count = sizeof(ITEMS) - 1;
    bool ok = (heap0.numItems() == count) &&
        (heap1.numItems() == count) &&
        (heap2.numItems() == count) &&
        (heap3.numItems() == count);
    CPPUNIT_ASSERT(ok);

    ok = (heap2.capacity() != heap2.initialCap());
    CPPUNIT_ASSERT(ok);

    ok = (heap3.capacity() != heap3.initialCap());
    CPPUNIT_ASSERT(ok);
}


void HeapXSuite::testAdd01()
{
    HeapX heap0(compare, 64 /*capacity*/, 0 /*growBy*/);
    HeapX::handle_t handle0[64];
    HeapX heap1(compareR, 64 /*capacity*/, 0 /*growBy*/);
    HeapX::handle_t handle1[64];

    bool ok = true;
    size_t numAdds = 0;
    for (const char* p = ITEMS; *p != 0; ++numAdds, ++p)
    {
        if ((!heap0.add(const_cast<char*>(p), handle0[numAdds])) ||
            (!heap1.add(const_cast<char*>(p), handle1[numAdds])))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    for (size_t i = 0; i < numAdds; ++i)
    {
        HeapX::item_t item0;
        HeapX::item_t item1;
        if ((!heap0.getItem(handle0[i], item0)) || (item0 != ITEMS + i) ||
            ((!heap1.getItem(handle1[i], item1)) || (item1 != ITEMS + i)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = ((heap0.numItems() == numAdds) && (heap1.numItems() == numAdds));
    CPPUNIT_ASSERT(ok);
    validate(heap0);
    validate(heap1);
}


void HeapXSuite::testCtor00()
{
    HeapX heapA(compare);
    HeapX::item_t topItem = 0;
    bool ok = (!heapA.canGrow()) &&
        (heapA.growthFactor() == 0) &&
        (heapA.numItems() == 0) &&
        (heapA.capacity() == HeapX::DefaultCap) &&
        (!heapA.peekAtTop(topItem));
    CPPUNIT_ASSERT(ok);
    heapA.add(&heapA);
    ok = heapA.peekAtTop(topItem) && (topItem == &heapA);
    CPPUNIT_ASSERT(ok);
    HeapX::handle_t handle = HeapX::INVALID_HANDLE;
    HeapX::item_t item = 0;
    ok = heapA.peekAtTop(topItem, handle) && heapA.getItem(handle, item) && (item == topItem);
    CPPUNIT_ASSERT(ok);

    // Zero capacity.
    HeapX heap1(compareR, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap1.add(const_cast<char*>(p++)));
    HeapX heapB(0, 0 /*capacity*/, 0 /*growBy*/);
    heapB = heap1;
    ok = (heapB.numItems() == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Copy constructor.
//
void HeapXSuite::testCtor01()
{
    HeapX heap0(compare, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(const_cast<char*>(p++)));
    HeapX heapA(heap0);
    validate(heapA);

    HeapX heap1(compareR, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap1.add(const_cast<char*>(p++)));
    HeapX heapB(heap1);
    validate(heapB);

    HeapX heap2(compare, 2 /*capacity*/, -1 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap2.add(const_cast<char*>(p++)));
    HeapX heapC(heap2);
    validate(heapC);

    HeapX heap3(compareR, 8 /*capacity*/, 5 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap3.add(const_cast<char*>(p++)));
    HeapX heapD(heap3);
    validate(heapD);
}


void HeapXSuite::testCtor02()
{
    HeapX::compare_t compare = 0;
    HeapX heap(compare);
    compare = heap.cmpFunc();
    bool ok = (compare != 0);
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
// Assignment operator. No growing required.
//
void HeapXSuite::testOp00()
{
    HeapX heap0(compare, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(const_cast<char*>(p++)));

    HeapX heap(heap0.cmpFunc(), heap0.numItems(), 0 /*growBy*/);
    heap = heap0;
    heap = heap; //no-op
    bool ok = (heap.initialCap() == heap.capacity());
    CPPUNIT_ASSERT(ok);
    validate(heap);

    // Full heap.
    heap = heap0;
    ok = (!heap.add(0));
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator. Exponential growth required.
//
void HeapXSuite::testOp01()
{
    HeapX heap0(compare, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(const_cast<char*>(p++)));

    HeapX heap(compare, 3 /*capacity*/, -1 /*growBy*/);
    heap = heap0;
    bool ok = (heap.capacity() == 48);
    CPPUNIT_ASSERT(ok);
    validate(heap);
}


//
// Assignment operator. Linear growth required.
//
void HeapXSuite::testOp02()
{
    HeapX heap0(compare, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(const_cast<char*>(p++)));

    HeapX heap(compare, 7 /*capacity*/, 9 /*growBy*/);
    heap = heap0;
    bool ok = (heap.capacity() == 43);
    CPPUNIT_ASSERT(ok);
    validate(heap);
}


//
// Assignment operator. Failure case.
//
void HeapXSuite::testOp03()
{
    HeapX heap0(compare, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(const_cast<char*>(p++)));

    HeapX heap(compare, 7 /*capacity*/, 0 /*growBy*/);
    heap.add(0);
    heap = heap0;
    bool ok = (heap.numItems() == 0);
    CPPUNIT_ASSERT(ok);
    validate(heap);
}


void HeapXSuite::testReplace00()
{
    HeapX heap(compareR, 64 /*capacity*/, 0 /*growBy*/);
    HeapX::handle_t handle[64];
    size_t i = 0;
    for (const char* p = ITEMS; *p != 0; heap.add(const_cast<char*>(p++), handle[i++]));

    char c0;
    char c1;
    bool ok = (heap.replace(handle[0], &c0) && heap.replace(handle[1], &c1));
    CPPUNIT_ASSERT(ok);
    validate(heap);
}


void HeapXSuite::testResize00()
{

    // no-op
    HeapX heap0(compare, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(const_cast<char*>(p++)));
    bool ok = heap0.resize(heap0.capacity());
    CPPUNIT_ASSERT(ok);

    // No truncation allowed.
    HeapX heap1(compareR, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap1.add(const_cast<char*>(p++)));
    ok = (!heap1.resize(heap1.numItems() - 1));
    CPPUNIT_ASSERT(ok);
}


void HeapXSuite::testRm00()
{
    HeapX heap0(compare, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(const_cast<char*>(p++)));

    HeapX heap(heap0);
    bool ok = true;
    size_t numItems = heap.numItems();
    for (size_t i = 0; i < 5; ++i)
    {
        HeapX::item_t item = heap.peek(i);
        HeapX::item_t removedItem = 0;
        if (!heap.rmFromIndex(i, removedItem) || (removedItem != item) || (heap.numItems() != --numItems))
        {
            ok = false;
        }
        HeapX tmp(heap);
        validate(tmp);
    }
    CPPUNIT_ASSERT(ok);

    // Invalid index.
    ok = (!heap.rmFromIndex(9999UL));
    CPPUNIT_ASSERT(ok);
}


void HeapXSuite::testRm01()
{
    HeapX heap1(compareR, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap1.add(const_cast<char*>(p++)));

    HeapX heap(heap1);
    bool ok = true;
    size_t numItems = heap.numItems();
    for (size_t i = numItems - 5; i < numItems;)
    {
        HeapX::item_t item = heap.peek(i);
        HeapX::item_t removedItem = 0;
        if (!heap.rmFromIndex(i, removedItem) || (removedItem != item) || (heap.numItems() != --numItems))
        {
            ok = false;
        }
        HeapX tmp(heap);
        validate(tmp);
    }

    CPPUNIT_ASSERT(ok);
}


void HeapXSuite::testRmItem00()
{
    HeapX heap(compare, 64 /*capacity*/, 0 /*growBy*/);
    HeapX::handle_t handle[64];
    size_t i = 0;
    for (const char* p = ITEMS; *p != 0; heap.add(const_cast<char*>(p++), handle[i++]));

    bool ok = true;
    while (i > 0)
    {
        if (!heap.rmItem(handle[--i]))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (heap.numItems() == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Validate given heap by removing items from the heap and verify the ordering.
//
void HeapXSuite::validate(HeapX& heap)
{
    HeapX::item_t prevItem;
    if (heap.rm(prevItem))
    {
        HeapX::compare_t cmpFunc = heap.cmpFunc();
        HeapX::item_t topItem;
        bool ok = true;
        for (; heap.rm(topItem); prevItem = topItem)
        {
            if (cmpFunc(prevItem, topItem) < 0)
            {
                ok = false;
                break;
            }
        }
        CPPUNIT_ASSERT(ok);
    }

    bool ok = (heap.numItems() == 0);
    CPPUNIT_ASSERT(ok);
}
