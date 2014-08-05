#include "syskit/Heap.hpp"

#include "syskit-ut-pch.h"
#include "HeapSuite.hpp"

using namespace syskit;

const char ITEMS[] = "aRandomStringUsedForHeapPopulation!!!";


HeapSuite::HeapSuite()
{
}


HeapSuite::~HeapSuite()
{
}


//
// Comparison function (ascending order).
//
int HeapSuite::compare(const void* item0, const void* item1)
{
    int c0 = *static_cast<const char*>(item0);
    int c1 = *static_cast<const char*>(item1);
    return c0 - c1;
}


//
// Comparison function (descending order).
//
int HeapSuite::compareR(const void* item0, const void* item1)
{
    int c0 = *static_cast<const char*>(item0);
    int c1 = *static_cast<const char*>(item1);
    return c1 - c0;
}


void HeapSuite::testAdd00()
{
    Heap heap0(compare, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(const_cast<char*>(p++)));
    Heap heap1(compareR, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap1.add(const_cast<char*>(p++)));
    Heap heap2(compare, 2 /*capacity*/, -1 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap2.add(const_cast<char*>(p++)));
    Heap heap3(compareR, 8 /*capacity*/, 5 /*growBy*/);
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


void HeapSuite::testCtor00()
{
    Heap heapA(compare);
    Heap::item_t topItem = 0;
    bool ok = (!heapA.canGrow()) &&
        (heapA.growthFactor() == 0) &&
        (heapA.numItems() == 0) &&
        (heapA.capacity() == Heap::DefaultCap) &&
        (!heapA.peekAtTop(topItem));
    CPPUNIT_ASSERT(ok);
    heapA.add(&heapA);
    ok = heapA.peekAtTop(topItem) && (topItem == &heapA);
    CPPUNIT_ASSERT(ok);

    // Zero capacity.
    Heap heap1(compareR, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap1.add(const_cast<char*>(p++)));
    Heap heapB(0, 0UL /*capacity*/, 0 /*growBy*/);
    heapB = heap1;
    ok = (heapB.numItems() == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Copy constructor.
//
void HeapSuite::testCtor01()
{
    Heap heap0(compare, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(const_cast<char*>(p++)));
    Heap heapA(heap0);
    validate(heapA);

    Heap heap1(compareR, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap1.add(const_cast<char*>(p++)));
    Heap heapB(heap1);
    validate(heapB);

    Heap heap2(compare, 2 /*capacity*/, -1 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap2.add(const_cast<char*>(p++)));
    Heap heapC(heap2);
    validate(heapC);

    Heap heap3(compareR, 8 /*capacity*/, 5 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap3.add(const_cast<char*>(p++)));
    Heap heapD(heap3);
    validate(heapD);
}


void HeapSuite::testCtor02()
{
    Heap::compare_t compare = 0;
    Heap heap(compare);
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
void HeapSuite::testOp00()
{
    Heap heap0(compare, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(const_cast<char*>(p++)));

    Heap heap(heap0.cmpFunc(), heap0.numItems(), 0 /*growBy*/);
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
void HeapSuite::testOp01()
{
    Heap heap0(compare, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(const_cast<char*>(p++)));

    Heap heap(compare, 3 /*capacity*/, -1 /*growBy*/);
    heap = heap0;
    bool ok = (heap.capacity() == 48);
    CPPUNIT_ASSERT(ok);
    validate(heap);
}


//
// Assignment operator. Linear growth required.
//
void HeapSuite::testOp02()
{
    Heap heap0(compare, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(const_cast<char*>(p++)));

    Heap heap(compare, 7 /*capacity*/, 9 /*growBy*/);
    heap = heap0;
    bool ok = (heap.capacity() == 43);
    CPPUNIT_ASSERT(ok);
    validate(heap);
}


//
// Assignment operator. Truncation required.
//
void HeapSuite::testOp03()
{
    Heap heap0(compare, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(const_cast<char*>(p++)));

    Heap heap(compare, 7 /*capacity*/, 0 /*growBy*/);
    heap = heap0;
    bool ok = (heap.numItems() == 7);
    CPPUNIT_ASSERT(ok);
    validate(heap);
}


void HeapSuite::testResize00()
{

    // no-op
    Heap heap0(compare, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(const_cast<char*>(p++)));
    bool ok = heap0.resize(heap0.capacity());
    CPPUNIT_ASSERT(ok);

    // No truncation allowed.
    Heap heap1(compareR, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap1.add(const_cast<char*>(p++)));
    ok = (!heap1.resize(heap1.numItems() - 1));
    CPPUNIT_ASSERT(ok);
}


void HeapSuite::testRm00()
{
    Heap heap0(compare, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(const_cast<char*>(p++)));

    Heap heap(heap0);
    bool ok = true;
    size_t numItems = heap.numItems();
    for (size_t i = 0; i < 5; ++i)
    {
        Heap::item_t item = heap.peek(i);
        Heap::item_t removedItem = 0;
        if (!heap.rmFromIndex(i, removedItem) || (removedItem != item) || (heap.numItems() != --numItems))
        {
            ok = false;
        }
        Heap tmp(heap);
        validate(tmp);
    }
    CPPUNIT_ASSERT(ok);

    // Invalid index.
    ok = (!heap.rmFromIndex(9999UL));
    CPPUNIT_ASSERT(ok);
}


void HeapSuite::testRm01()
{
    Heap heap1(compareR, 256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap1.add(const_cast<char*>(p++)));

    Heap heap(heap1);
    bool ok = true;
    size_t numItems = heap.numItems();
    for (size_t i = numItems - 5; i < numItems;)
    {
        Heap::item_t item = heap.peek(i);
        Heap::item_t removedItem = 0;
        if (!heap.rmFromIndex(i, removedItem) || (removedItem != item) || (heap.numItems() != --numItems))
        {
            ok = false;
        }
        Heap tmp(heap);
        validate(tmp);
    }

    CPPUNIT_ASSERT(ok);
}


void HeapSuite::testSort00()
{
    size_t count = sizeof(ITEMS) - 1;
    Heap::item_t* item = new Heap::item_t[count];
    size_t i = 0;
    for (const char* p = ITEMS; i < count; item[i++] = const_cast<char*>(p++));
    Heap::sort(item, count, compare);

    bool ok = true;
    Heap::item_t prevItem = item[0];
    Heap::item_t curItem = 0;
    for (i = 1; i < count; ++i, prevItem = curItem)
    {
        curItem = item[i];
        if (compare(prevItem, curItem) > 0)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    delete[] item;
}


//
// Validate given heap by removing items from the heap and verify the ordering.
//
void HeapSuite::validate(Heap& heap)
{
    Heap::item_t prevItem;
    if (heap.rm(prevItem))
    {
        Heap::compare_t cmpFunc = heap.cmpFunc();
        Heap::item_t topItem;
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
