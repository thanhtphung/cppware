#include "appkit/U32.hpp"
#include "syskit/HashTable.hpp"

#include "syskit-ut-pch.h"
#include "HashTableSuite.hpp"

using namespace appkit;
using namespace syskit;

const char
ITEMS[] = "aRandomStringUsedForHashTablePopulation!!!";


HashTableSuite::HashTableSuite()
{
}


HashTableSuite::~HashTableSuite()
{
}


bool HashTableSuite::cb0a(void* arg, void* item)
{
    const HashTable* t = static_cast<const HashTable*>(arg);
    bool keepGoing = t->find(item);
    return keepGoing;
}


bool HashTableSuite::cb0b(void* /*arg*/, void* /*item*/)
{
    bool keepGoing = false;
    return keepGoing;
}


void HashTableSuite::deleteItem(void* /*arg*/, void* item)
{
    delete static_cast<unsigned int*>(item);
}


void HashTableSuite::testAdd00()
{
    HashTable t(U32::compareP, U32::hashP);

    bool ok = true;
    for (const char* p = ITEMS; *p != 0; ++p)
    {
        void* foundItem = 0;
        unsigned int* item = new unsigned int(*p);
        if (t.find(item))
        {
            void* foundByAdd = this;
            if (t.addIfNotFound(item, foundByAdd) ||
                (foundByAdd == item) ||
                (*static_cast<const unsigned int*>(foundByAdd) != *item))
            {
                ok = false;
                break;
            }
            delete item;
        }
        else if ((!t.addIfNotFound(item)) || (!t.find(item, foundItem)) || (item != foundItem))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    unsigned int* item = new unsigned int(ITEMS[5]);
    void* removedItem = 0;
    ok = t.rm(item, removedItem) && (removedItem != 0);
    CPPUNIT_ASSERT(ok);
    delete static_cast<unsigned int*>(removedItem);
    delete item;

    item = new unsigned int(0x12345678U);
    void* replacedItem = item;
    ok = t.add(item, replacedItem) && (replacedItem == 0);
    CPPUNIT_ASSERT(ok);
    item = new unsigned int(ITEMS[9]);
    ok = t.add(item, replacedItem) && (replacedItem != 0);
    CPPUNIT_ASSERT(ok);
    delete static_cast<unsigned int*>(replacedItem);

    t.apply(deleteItem, 0);
}


void HashTableSuite::testAdd01()
{
    HashTable t(U32::compareP, U32::hashP, 0 /*capacity*/, 5.0 /*bucketCap*/);

    // Add same item repeatedly.
    unsigned int item = 0x12345678U;
    for (unsigned int i = 0; i < 15; ++i)
    {
        t.add(&item);
    }

    // Only one bucket should be in use.
    bool ok = (t.numItems() == 15) && (t.usagePeak() == 15) && (t.numInUseBuckets() == 1);
    CPPUNIT_ASSERT(ok);

    unsigned int nonExistent = 0x12345U;
    ok = (!t.find(&nonExistent));
    CPPUNIT_ASSERT(ok);

    for (unsigned int i = 0; i < 15; ++i)
    {
        void* removedItem = 0;
        if ((!t.rm(&item, removedItem)) || (removedItem != &item))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (t.numItems() == 0) && (t.usagePeak() == 15) && (t.numInUseBuckets() == 0);
    CPPUNIT_ASSERT(ok);
}


void HashTableSuite::testAdd02()
{
    unsigned int capacity = HashTable::DefaultCap;
    double bucketCap = 1.0;
    HashTable t(U32::compareP, U32::hashP, capacity, bucketCap);

    // Add unique items. Each should reside in its own bucket.
    bool ok = true;
    for (unsigned int i = 0; i < t.numBuckets();)
    {
        unsigned int* item = new unsigned int(i);
        t.add(item);
        if (t.numInUseBuckets() != ++i)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (t.usagePeak() == capacity) && (t.peakBucketSize() == 1);
    CPPUNIT_ASSERT(ok);

    capacity = 7;
    HashTable* t0 = new HashTable(U32::compareP, U32::hashP, capacity, bucketCap);
    *t0 = t;
    ok = (t0->numItems() == t.numItems()) && (t0->usagePeak() == t.numItems());
    CPPUNIT_ASSERT(ok);
    delete t0;

    t0 = new HashTable(t);
    ok = (t0->numItems() == t.numItems());
    CPPUNIT_ASSERT(ok);
    ok = (t0->apply(cb0a, &t) && t.apply(cb0a, t0)); //items in t0 must be found in t, and vice versa
    CPPUNIT_ASSERT(ok);
    ok = (!t0->apply(cb0b, 0));
    CPPUNIT_ASSERT(ok);
    t0->reset();
    ok = (t0->numItems() == 0);
    CPPUNIT_ASSERT(ok);
    delete t0;

    t.apply(deleteItem, 0);
}


void HashTableSuite::testAdd03()
{

    // Add enough items to cause a few growths and rehashes.
    HashTable t(U32::compareP, U32::hashP, 8 /*capacity*/, 1.0 /*bucketCap*/);
    bool ok = true;
    for (unsigned int i = 'a'; i <= 'z'; i += 2)
    {
        unsigned int* item = new unsigned int(i);
        void* foundItem;
        if ((!t.add(item)) || (!t.find(item, foundItem)) || (item != foundItem))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    for (unsigned int i = 'a' + 1; i <= 'z'; i += 2)
    {
        unsigned int* item = new unsigned int(i);
        void* foundItem;
        if ((!t.addIfNotFound(item)) || (!t.find(item, foundItem)) || (item != foundItem))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (t.capacity() == 47) && (t.initialCap() == 11) && (t.numItems() == 'z' - 'a' + 1);
    CPPUNIT_ASSERT(ok);

    // Make sure things can still be found after rehashes.
    for (unsigned int i = 'a'; i <= 'z'; ++i)
    {
        if (!t.find(&i))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    // Try removes after rehashes also.
    for (unsigned int i = 'i';;)
    {
        void* removedItem;
        if (!t.rm(&i, removedItem))
        {
            ok = false;
            break;
        }
        delete (unsigned int*)(removedItem);
        if (i == 'h')
        {
            break;
        }
        else if (i == 'z')
        {
            i = 'a';
        }
        else
        {
            ++i;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (t.numInUseBuckets() == 0) && (t.numItems() == 0);
    CPPUNIT_ASSERT(ok);
}


void HashTableSuite::testCtor00()
{
    HashTable t(U32::compareP, U32::hashP, HashTable::DefaultCap, 1.0 /*bucketCap*/);
    bool ok = t.canGrow() && (t.growthFactor() < 0) && (t.capacity() == HashTable::DefaultCap);
    CPPUNIT_ASSERT(ok);

    ok = (t.cmpFunc() == U32::compareP) &&
        (t.hashFunc() == U32::hashP) &&
        (t.bucketCap() == 1.0) &&
        (t.numEmptyBuckets() == HashTable::DefaultCap) &&
        (t.numInUseBuckets() == 0) &&
        (t.numItems() == 0) &&
        (t.usagePeak() == 0);
    CPPUNIT_ASSERT(ok);

    ok = (!t.setGrowth(0));
    CPPUNIT_ASSERT(ok);
    ok = (!t.setGrowth(10));
    CPPUNIT_ASSERT(ok);

    unsigned int newCap = 4294967291U + 1; //MAX_PRIME32 + 1
    ok = (!t.resize(newCap));
    CPPUNIT_ASSERT(ok);
}


void HashTableSuite::testSize00()
{
    bool ok = (sizeof(HashTable::node_t) == sizeof(void*) * 2);
    CPPUNIT_ASSERT(ok);
}
