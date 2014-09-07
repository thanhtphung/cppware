#include "syskit/U16Heap.hpp"

#include "syskit-ut-pch.h"
#include "U16HeapSuite.hpp"

using namespace syskit;

const char ITEMS[] = "aRandomStringUsedForU16HeapPopulation!!!";


U16HeapSuite::U16HeapSuite()
{
}


U16HeapSuite::~U16HeapSuite()
{
}


void U16HeapSuite::testAdd00()
{
    U16Heap heap0(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(*p++));
    U16Heap heap1(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap1.add(*p++));
    U16Heap heap2(2 /*capacity*/, -1 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap2.add(*p++));
    U16Heap heap3(8 /*capacity*/, 5 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap3.add(*p++));

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


void U16HeapSuite::testCtor00()
{
    U16Heap heapA;
    U16Heap::item_t topItem = 0;
    bool ok = (!heapA.canGrow()) &&
        (heapA.growthFactor() == 0) &&
        (heapA.numItems() == 0) &&
        (heapA.capacity() == U16Heap::DefaultCap) &&
        (!heapA.peekAtTop(topItem));
    CPPUNIT_ASSERT(ok);
    heapA.add(123);
    ok = heapA.peekAtTop(topItem) && (topItem == 123);
    CPPUNIT_ASSERT(ok);

    // Zero capacity.
    U16Heap heap1(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap1.add(*p++));
    U16Heap heapB(0U /*capacity*/, 0 /*growBy*/);
    heapB = heap1;
    ok = (heapB.numItems() == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Copy constructor.
//
void U16HeapSuite::testCtor01()
{
    U16Heap heap0(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(*p++));
    U16Heap heapA(heap0);
    validate(heapA);

    U16Heap heap1(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap1.add(*p++));
    U16Heap heapB(heap1);
    validate(heapB);

    U16Heap heap2(2 /*capacity*/, -1 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap2.add(*p++));
    U16Heap heapC(heap2);
    validate(heapC);

    U16Heap heap3(8 /*capacity*/, 5 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap3.add(*p++));
    U16Heap heapD(heap3);
    validate(heapD);
}


//
// Assignment operator. No growing required.
//
void U16HeapSuite::testOp00()
{
    U16Heap heap0(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(*p++));

    U16Heap heap(heap0.numItems(), 0 /*growBy*/);
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
void U16HeapSuite::testOp01()
{
    U16Heap heap0(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(*p++));

    U16Heap heap(3 /*capacity*/, -1 /*growBy*/);
    heap = heap0;
    bool ok = (heap.capacity() == 48);
    CPPUNIT_ASSERT(ok);
    validate(heap);
}


//
// Assignment operator. Linear growth required.
//
void U16HeapSuite::testOp02()
{
    U16Heap heap0(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(*p++));

    U16Heap heap(7 /*capacity*/, 9 /*growBy*/);
    heap = heap0;
    bool ok = (heap.capacity() == 43);
    CPPUNIT_ASSERT(ok);
    validate(heap);
}


//
// Assignment operator. Truncation required.
//
void U16HeapSuite::testOp03()
{
    U16Heap heap0(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(*p++));

    U16Heap heap(7 /*capacity*/, 0 /*growBy*/);
    heap = heap0;
    bool ok = (heap.numItems() == 7);
    CPPUNIT_ASSERT(ok);
    validate(heap);
}


void U16HeapSuite::testResize00()
{

    // no-op
    U16Heap heap0(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(*p++));
    bool ok = heap0.resize(heap0.capacity());
    CPPUNIT_ASSERT(ok);

    // No truncation allowed.
    U16Heap heap1(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap1.add(*p++));
    ok = (!heap1.resize(heap1.numItems() - 1));
    CPPUNIT_ASSERT(ok);
}


void U16HeapSuite::testRm00()
{
    U16Heap heap0(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(*p++));

    U16Heap heap(heap0);
    bool ok = true;
    size_t numItems = heap.numItems();
    for (size_t i = 0; i < 5; ++i)
    {
        U16Heap::item_t item = heap.peek(i);
        U16Heap::item_t removedItem = 0;
        if (!heap.rmFromIndex(i, removedItem) || (removedItem != item) || (heap.numItems() != --numItems))
        {
            ok = false;
        }
        U16Heap tmp(heap);
        validate(tmp);
    }
    CPPUNIT_ASSERT(ok);

    // Invalid index.
    ok = (!heap.rmFromIndex(9999U));
    CPPUNIT_ASSERT(ok);
}


void U16HeapSuite::testRm01()
{
    U16Heap heap1(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap1.add(*p++));

    U16Heap heap(heap1);
    bool ok = true;
    size_t numItems = heap.numItems();
    for (size_t i = numItems - 5; i < numItems;)
    {
        U16Heap::item_t item = heap.peek(i);
        U16Heap::item_t removedItem = 0;
        if (!heap.rmFromIndex(i, removedItem) || (removedItem != item) || (heap.numItems() != --numItems))
        {
            ok = false;
        }
        U16Heap tmp(heap);
        validate(tmp);
    }

    CPPUNIT_ASSERT(ok);
}


void U16HeapSuite::testSort00()
{
    size_t count = sizeof(ITEMS) - 1;
    U16Heap::item_t* item = new U16Heap::item_t[count];
    size_t i = 0;
    for (const char* p = ITEMS; i < count; item[i++] = *p++);
    bool reverseOrder = false;
    U16Heap::sort(item, count, reverseOrder);

    bool ok = true;
    U16Heap::item_t prevItem = item[0];
    U16Heap::item_t curItem = 0;
    for (i = 1; i < count; ++i, prevItem = curItem)
    {
        curItem = item[i];
        if (prevItem > curItem)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    reverseOrder = true;
    U16Heap::sort(item, count, reverseOrder);

    prevItem = item[0];
    curItem = 0;
    for (i = 1; i < count; ++i, prevItem = curItem)
    {
        curItem = item[i];
        if (prevItem < curItem)
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
void U16HeapSuite::validate(U16Heap& heap)
{
    U16Heap::item_t prevItem;
    if (heap.rm(prevItem))
    {
        U16Heap::item_t topItem;
        bool ok = true;
        for (; heap.rm(topItem); prevItem = topItem)
        {
            if (prevItem < topItem)
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
