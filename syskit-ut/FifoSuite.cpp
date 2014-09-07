#include "syskit/Fifo.hpp"

#include "syskit-ut-pch.h"
#include "FifoSuite.hpp"

using namespace syskit;

const char ITEM[] = "aRandomStringUsedForFifoPopulation!!!";
const size_t NUM_ITEMS = sizeof(ITEM) - 1;


FifoSuite::FifoSuite()
{
}


FifoSuite::~FifoSuite()
{
}


//
// Add random entries to tail.
//
void FifoSuite::testAdd00()
{
    Fifo q1(13 /*capacity*/, 11 /*growBy*/);

    // Add random entries to tail of queue.
    bool ok = true;
    for (const char* p = ITEM; *p != 0; ++p)
    {
        if (!q1.add(const_cast<char*>(p)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (q1.capacity() == 13 + 11 + 11 + 11);
    CPPUNIT_ASSERT(ok);

    // Remove entries from head of queue and validate contents.
    for (const char* p = ITEM; *p != 0; ++p)
    {
        void* item = 0;
        if ((!q1.rm(item)) || (*static_cast<const char*>(item) != *p))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (q1.numItems() == 0);
    CPPUNIT_ASSERT(ok);
    ok = q1.resize(q1.initialCap());
    CPPUNIT_ASSERT(ok);

    Fifo::Stat stat(q1);
    ok = (stat.numAdds() == NUM_ITEMS) &&
        (stat.numFails() == 0) &&
        (stat.numRms() == NUM_ITEMS) &&
        (stat.usagePeak() == NUM_ITEMS);
    CPPUNIT_ASSERT(ok);
}


//
// Add random entries to head.
//
void FifoSuite::testAdd01()
{
    Fifo q1(13 /*capacity*/, 11 /*growBy*/);

    // Add random entries to head of queue.
    bool ok = true;
    for (const char* p = ITEM; *p != 0; ++p)
    {
        if (!q1.addAtHead(const_cast<char*>(p)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (q1.capacity() == 13 + 11 + 11 + 11);
    CPPUNIT_ASSERT(ok);

    // Remove entries from tail of queue and validate contents.
    for (const char* p = ITEM; *p != 0; ++p)
    {
        void* item = 0;
        if ((!q1.rmFromTail(item)) || (*static_cast<const char*>(item) != *p))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (q1.numItems() == 0);
    CPPUNIT_ASSERT(ok);
    ok = q1.resize(q1.initialCap());
    CPPUNIT_ASSERT(ok);

    Fifo::Stat stat(q1);
    ok = (stat.numAdds() == NUM_ITEMS) &&
        (stat.numFails() == 0) &&
        (stat.numRms() == NUM_ITEMS) &&
        (stat.usagePeak() == NUM_ITEMS);
    CPPUNIT_ASSERT(ok);
}


//
// Add random entries to tail in a circular manner.
//
void FifoSuite::testAdd02()
{
    Fifo q0(16 /*capacity*/, 0 /*growBy*/);

    // Add random entries to tail of queue in a circular manner.
    size_t numDrops = 0;
    bool ok = true;
    for (const char* p = ITEM; *p != 0; ++p)
    {
        bool someWasDropped;
        void* droppedItem;
        if (!q0.addCircularly(const_cast<char*>(p), someWasDropped, droppedItem))
        {
            ok = false;
            break;
        }
        if (someWasDropped)
        {
            ++numDrops;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (q0.numItems() == q0.capacity());
    CPPUNIT_ASSERT(ok);

    // Remove entries from head of queue and validate contents.
    for (const char* p = ITEM + numDrops; *p != 0; ++p)
    {
        void* item = 0;
        if ((!q0.rm(item)) || (*static_cast<const char*>(item) != *p))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (q0.numItems() == 0);
    CPPUNIT_ASSERT(ok);
    ok = q0.resize(q0.capacity()); //no-op
    CPPUNIT_ASSERT(ok);

    Fifo::Stat stat(q0);
    ok = (stat.numAdds() == NUM_ITEMS) &&
        (stat.numFails() == 0) &&
        (stat.numRms() == NUM_ITEMS) &&
        (stat.usagePeak() == q0.capacity());
    CPPUNIT_ASSERT(ok);
}


//
// Add random entries to head in a circular manner.
//
void FifoSuite::testAdd03()
{
    Fifo q0(16 /*capacity*/, 0 /*growBy*/);

    // Add random entries to head of queue in a circular manner.
    size_t numDrops = 0;
    bool ok = true;
    for (const char* p = ITEM; *p != 0; ++p)
    {
        bool someWasDropped;
        void* droppedItem;
        if (!q0.addAtHeadCircularly(const_cast<char*>(p), someWasDropped, droppedItem))
        {
            ok = false;
            break;
        }
        if (someWasDropped)
        {
            ++numDrops;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (q0.numItems() == q0.capacity());
    CPPUNIT_ASSERT(ok);
    ok = (!q0.resize(1)); //no-op
    CPPUNIT_ASSERT(ok);

    // Remove entries from tail of queue and validate contents.
    for (const char* p = ITEM + numDrops; *p != 0; ++p)
    {
        void* item = 0;
        if ((!q0.rmFromTail(item)) || (*static_cast<const char*>(item) != *p))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (q0.numItems() == 0);
    CPPUNIT_ASSERT(ok);

    Fifo::Stat stat(q0);
    ok = (stat.numAdds() == NUM_ITEMS) &&
        (stat.numFails() == 0) &&
        (stat.numRms() == NUM_ITEMS) &&
        (stat.usagePeak() == q0.capacity());
    CPPUNIT_ASSERT(ok);
}


//
// Add to a full queue.
//
void FifoSuite::testAdd04()
{
    Fifo q0(16 /*capacity*/, 0 /*growBy*/);

    size_t capacity = q0.capacity();
    bool ok = true;
    for (size_t i = 0; i < capacity; ++i)
    {
        if (!q0.add(0))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = (!q0.add(0)) && (!q0.addAtHead(0));
    CPPUNIT_ASSERT(ok);

    bool someWasDropped = false;
    char* p = 0;
    void* droppedItem = p + 1;
    ok = q0.addCircularly(0, someWasDropped, droppedItem) && someWasDropped && (droppedItem == 0);
    CPPUNIT_ASSERT(ok);

    someWasDropped = false;
    droppedItem = p + 1;
    ok = q0.addAtHeadCircularly(0, someWasDropped, droppedItem) && someWasDropped && (droppedItem == 0);
    CPPUNIT_ASSERT(ok);

    Fifo::Stat stat(q0);
    ok = (stat.numAdds() == capacity + 2) &&
        (stat.numFails() == 2) &&
        (stat.numRms() == 2) &&
        (stat.usagePeak() == capacity);
    CPPUNIT_ASSERT(ok);
}


//
// Zero-capacity.
//
void FifoSuite::testCtor00()
{
    Fifo q(0);
    bool ok = (q.capacity() == 0) &&
        (q.numItems() == 0) &&
        (!q.add(0)) &&
        (!q.addAtHead(0)) &&
        (!q.addAtHeadCircularly(0)) &&
        (!q.addCircularly(0));
    CPPUNIT_ASSERT(ok);
}


//
// Default constructor.
//
void FifoSuite::testCtor01()
{
    Fifo q;
    size_t capacity = q.capacity();
    bool ok = (capacity == Fifo::DefaultCap) && (q.numItems() == 0) && (!q.canGrow());
    CPPUNIT_ASSERT(ok);

    // add() should fail as soon as capacity is reached.
    for (size_t i = capacity; i > 0; --i)
    {
        if (!q.add(0))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = (!q.add(0)) && (!q.addAtHead(0));
    CPPUNIT_ASSERT(ok);

    Fifo::Stat stat(q);
    ok = (stat.numAdds() == capacity) &&
        (stat.numFails() == 2) &&
        (stat.numRms() == 0) &&
        (stat.usagePeak() == capacity);
    CPPUNIT_ASSERT(ok);
}


//
// Copy constructor. Source queue did not wrap.
//
void FifoSuite::testCtor02()
{

    // Populate a queue with random entries.
    Fifo q0(NUM_ITEMS + 5);
    bool ok = true;
    for (const char* p = ITEM; *p != 0; ++p)
    {
        if (!q0.add(const_cast<char*>(p)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = q0.resize(NUM_ITEMS + 1);
    CPPUNIT_ASSERT(ok);

    // Copy constructor. Source queue did not wrap.
    Fifo q1(q0);

    // Validate duplicated queue.
    for (const char* p = ITEM; *p != 0; ++p)
    {
        void* item = 0;
        if ((!q1.rm(item)) || (*static_cast<const char*>(item) != *p))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    Fifo::Stat stat(q1);
    ok = (stat.numAdds() == NUM_ITEMS) &&
        (stat.numFails() == 0) &&
        (stat.numRms() == NUM_ITEMS) &&
        (stat.usagePeak() == NUM_ITEMS);
    CPPUNIT_ASSERT(ok);
}


//
// Copy constructor. Source queue wrapped.
//
void FifoSuite::testCtor03()
{

    // Populate a queue with random entries.
    Fifo q0(17 /*capacity*/, 0 /*growBy*/);
    size_t numDrops = 0;
    bool ok = true;
    for (const char* p = ITEM; *p != 0; ++p)
    {
        bool someWasDropped;
        void* droppedItem;
        if (!q0.addCircularly(const_cast<char*>(p), someWasDropped, droppedItem))
        {
            ok = false;
            break;
        }
        if (someWasDropped)
        {
            ++numDrops;
        }
    }
    CPPUNIT_ASSERT(ok);

    // Copy constructor. Source queue wrapped.
    Fifo q1(q0);

    // Validate duplicated queue.
    for (const char* p = ITEM + numDrops; *p != 0; ++p)
    {
        void* item = 0;
        if ((!q1.rm(item)) || (*static_cast<const char*>(item) != *p))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    Fifo::Stat stat(q1);
    ok = (stat.numAdds() == q0.capacity()) &&
        (stat.numFails() == 0) &&
        (stat.numRms() == q0.capacity()) &&
        (stat.usagePeak() == q0.capacity());
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator. Source queue did not wrap.
//
void FifoSuite::testOp00()
{

    // Populate a queue with random entries.
    Fifo q0(NUM_ITEMS);
    bool ok = true;
    for (const char* p = ITEM; *p != 0; ++p)
    {
        if (!q0.add(const_cast<char*>(p)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    Fifo q1(NUM_ITEMS);
    q1 = q0;
    Fifo q2(1 /*capacity*/, -1 /*growBy*/);
    q2 = q0;
    q2 = q2; //no-op

    // Validate duplicated queue.
    for (const char* p = ITEM; *p != 0; ++p)
    {
        void* item1 = 0;
        void* item2 = 0;
        if ((!q1.rm(item1)) || (*static_cast<const char*>(item1) != *p) ||
            (!q2.rm(item2)) || (*static_cast<const char*>(item2) != *p))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    Fifo::Stat stat(q2);
    ok = (stat.numAdds() == NUM_ITEMS) &&
        (stat.numFails() == 0) &&
        (stat.numRms() == NUM_ITEMS) &&
        (stat.usagePeak() == NUM_ITEMS);
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator. Source queue wrapped.
//
void FifoSuite::testOp01()
{

    // Populate a queue with random entries.
    Fifo q0(17 /*capacity*/, 0 /*growBy*/);
    size_t numDrops = 0;
    bool ok = true;
    for (const char* p = ITEM; *p != 0; ++p)
    {
        bool someWasDropped;
        void* droppedItem;
        if (!q0.addCircularly(const_cast<char*>(p), someWasDropped, droppedItem))
        {
            ok = false;
            break;
        }
        if (someWasDropped)
        {
            ++numDrops;
        }
    }
    CPPUNIT_ASSERT(ok);

    // Assignment operator. Source queue wrapped.
    Fifo q1(q0.numItems());
    q1 = q0;

    // Validate duplicated queue.
    for (const char* p = ITEM + numDrops; *p != 0; ++p)
    {
        void* item;
        if ((!q1.rm(item)) || (*static_cast<const char*>(item) != *p))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    Fifo::Stat stat(q1);
    ok = (stat.numAdds() == q0.capacity()) &&
        (stat.numFails() == 0) &&
        (stat.numRms() == q0.capacity()) &&
        (stat.usagePeak() == q0.capacity());
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator. Truncation required.
//
void FifoSuite::testOp02()
{

    // Fixed capacity.
    Fifo q0(16 /*capacity*/, 0 /*growBy*/);

    // Wrapped queue.
    Fifo q2(NUM_ITEMS, 0);
    for (unsigned int i = 0; i++ < 5; q2.add(0));
    for (const char* p = ITEM; *p != 0; q2.addCircularly(const_cast<char*>(p++)));

    // Assignment operator. Source queue has too many entries.
    q0 = q2;
    size_t numDrops = q2.numItems() - q0.numItems();
    bool ok = (q0.numItems() == q0.capacity());
    CPPUNIT_ASSERT(ok);

    // Validate truncated queue.
    for (const char* p = ITEM + numDrops; *p != 0; ++p)
    {
        void* item;
        if ((!q0.rm(item)) || (*static_cast<const char*>(item) != *p))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (q0.numItems() == 0);
    CPPUNIT_ASSERT(ok);

    Fifo::Stat stat(q0);
    ok = (stat.numAdds() == q0.capacity()) &&
        (stat.numFails() == 0) &&
        (stat.numRms() == q0.capacity()) &&
        (stat.usagePeak() == q0.capacity());
    CPPUNIT_ASSERT(ok);
}


//
// Try peek(). Queue did not wrap.
//
void FifoSuite::testPeek00()
{

    // Populate a queue with random entries.
    Fifo q(NUM_ITEMS);
    for (const char* p = ITEM; *p != 0; ++p)
    {
        q.add(const_cast<char*>(p));
    }

    size_t i = 0;
    bool ok = true;
    for (const char* p = ITEM; *p != 0; ++i, ++p)
    {
        if (q.peek(i) != p)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


//
// Try peek(). Queue wrapped.
//
void FifoSuite::testPeek01()
{

    // Populate a queue with random entries.
    Fifo q(17 /*capacity*/, 0 /*growBy*/);
    const char* p = ITEM;
    for (; *p != 0; ++p)
    {
        q.addCircularly(const_cast<char*>(p));
    }

    size_t i = q.capacity();
    bool ok = true;
    for (--p; i > 0; --p)
    {
        if (q.peek(--i) != p)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}
