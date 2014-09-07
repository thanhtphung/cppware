#include "syskit/D64Heap.hpp"

#include "syskit-ut-pch.h"
#include "D64HeapSuite.hpp"

using namespace syskit;

const char ITEMS[] = "aRandomStringUsedForD64HeapPopulation!!!";


D64HeapSuite::D64HeapSuite()
{
}


D64HeapSuite::~D64HeapSuite()
{
}


void D64HeapSuite::testAdd00()
{
    D64Heap heap0(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(*p++));
    D64Heap heap1(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap1.add(*p++));
    D64Heap heap2(2 /*capacity*/, -1 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap2.add(*p++));
    D64Heap heap3(8 /*capacity*/, 5 /*growBy*/);
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


void D64HeapSuite::testCtor00()
{
    D64Heap heapA;
    D64Heap::item_t topItem = 0;
    bool ok = (!heapA.canGrow()) &&
        (heapA.growthFactor() == 0) &&
        (heapA.numItems() == 0) &&
        (heapA.capacity() == D64Heap::DefaultCap) &&
        (!heapA.peekAtTop(topItem));
    CPPUNIT_ASSERT(ok);
    heapA.add(123);
    ok = heapA.peekAtTop(topItem) && (topItem == 123);
    CPPUNIT_ASSERT(ok);

    // Zero capacity.
    D64Heap heap1(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap1.add(*p++));
    D64Heap heapB(0U /*capacity*/, 0 /*growBy*/);
    heapB = heap1;
    ok = (heapB.numItems() == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Copy constructor.
//
void D64HeapSuite::testCtor01()
{
    D64Heap heap0(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(*p++));
    D64Heap heapA(heap0);
    validate(heapA);

    D64Heap heap1(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap1.add(*p++));
    D64Heap heapB(heap1);
    validate(heapB);

    D64Heap heap2(2 /*capacity*/, -1 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap2.add(*p++));
    D64Heap heapC(heap2);
    validate(heapC);

    D64Heap heap3(8 /*capacity*/, 5 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap3.add(*p++));
    D64Heap heapD(heap3);
    validate(heapD);
}


//
// Assignment operator. No growing required.
//
void D64HeapSuite::testOp00()
{
    D64Heap heap0(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(*p++));

    D64Heap heap(heap0.numItems(), 0 /*growBy*/);
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
void D64HeapSuite::testOp01()
{
    D64Heap heap0(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(*p++));

    D64Heap heap(3 /*capacity*/, -1 /*growBy*/);
    heap = heap0;
    bool ok = (heap.capacity() == 48);
    CPPUNIT_ASSERT(ok);
    validate(heap);
}


//
// Assignment operator. Linear growth required.
//
void D64HeapSuite::testOp02()
{
    D64Heap heap0(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(*p++));

    D64Heap heap(7 /*capacity*/, 9 /*growBy*/);
    heap = heap0;
    bool ok = (heap.capacity() == 43);
    CPPUNIT_ASSERT(ok);
    validate(heap);
}


//
// Assignment operator. Truncation required.
//
void D64HeapSuite::testOp03()
{
    D64Heap heap0(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(*p++));

    D64Heap heap(7 /*capacity*/, 0 /*growBy*/);
    heap = heap0;
    bool ok = (heap.numItems() == 7);
    CPPUNIT_ASSERT(ok);
    validate(heap);
}


void D64HeapSuite::testResize00()
{

    // no-op
    D64Heap heap0(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(*p++));
    bool ok = heap0.resize(heap0.capacity());
    CPPUNIT_ASSERT(ok);

    // No truncation allowed.
    D64Heap heap1(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap1.add(*p++));
    ok = (!heap1.resize(heap1.numItems() - 1));
    CPPUNIT_ASSERT(ok);
}


void D64HeapSuite::testRm00()
{
    D64Heap heap0(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap0.add(*p++));

    D64Heap heap(heap0);
    bool ok = true;
    size_t numItems = heap.numItems();
    for (size_t i = 0; i < 5; ++i)
    {
        D64Heap::item_t item = heap.peek(i);
        D64Heap::item_t removedItem = 0;
        if (!heap.rmFromIndex(i, removedItem) || (removedItem != item) || (heap.numItems() != --numItems))
        {
            ok = false;
        }
        D64Heap tmp(heap);
        validate(tmp);
    }
    CPPUNIT_ASSERT(ok);

    // Invalid index.
    ok = (!heap.rmFromIndex(9999U));
    CPPUNIT_ASSERT(ok);
}


void D64HeapSuite::testRm01()
{
    D64Heap heap1(256 /*capacity*/, 0 /*growBy*/);
    for (const char* p = ITEMS; *p != 0; heap1.add(*p++));

    D64Heap heap(heap1);
    bool ok = true;
    size_t numItems = heap.numItems();
    for (size_t i = numItems - 5; i < numItems;)
    {
        D64Heap::item_t item = heap.peek(i);
        D64Heap::item_t removedItem = 0;
        if (!heap.rmFromIndex(i, removedItem) || (removedItem != item) || (heap.numItems() != --numItems))
        {
            ok = false;
        }
        D64Heap tmp(heap);
        validate(tmp);
    }

    CPPUNIT_ASSERT(ok);
}


void D64HeapSuite::testSort00()
{
    size_t count = sizeof(ITEMS) - 1;
    D64Heap::item_t* item = new D64Heap::item_t[count];
    size_t i = 0;
    for (const char* p = ITEMS; i < count; item[i++] = *p++);
    bool reverseOrder = false;
    D64Heap::sort(item, count, reverseOrder);

    bool ok = true;
    D64Heap::item_t prevItem = item[0];
    D64Heap::item_t curItem = 0;
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
    D64Heap::sort(item, count, reverseOrder);

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
void D64HeapSuite::validate(D64Heap& heap)
{
    D64Heap::item_t prevItem;
    if (heap.rm(prevItem))
    {
        D64Heap::item_t topItem;
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
