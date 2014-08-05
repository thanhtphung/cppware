#include "syskit/ItemQ.hpp"

#include "syskit-ut-pch.h"
#include "ItemQSuite.hpp"

using namespace syskit;


BEGIN_NAMESPACE

class MyItem: public ItemQ::Item
{
public:
    MyItem(unsigned long id);
    unsigned long id() const;
    virtual ~MyItem();
private:
    unsigned long id_;
};

inline unsigned long
MyItem::id() const
{
    return id_;
}

MyItem::MyItem(unsigned long id):
ItemQ::Item()
{
    id_ = id;
}

MyItem::~MyItem()
{
}

static bool cb0a(void* arg, ItemQ::Item* item, size_t index)
{
    const MyItem* myItem = dynamic_cast<const MyItem*>(item);
    bool keepGoing = (myItem->id() == index);
    return keepGoing;
}

static bool cb0b(void* /*arg*/, ItemQ::Item* /*item*/, size_t /*index*/)
{
    bool keepGoing = false;
    return keepGoing;
}

END_NAMESPACE


ItemQSuite::ItemQSuite()
{
}


ItemQSuite::~ItemQSuite()
{
}


void ItemQSuite::testCtor00()
{
    ItemQ q0(12UL /*capacity*/, 0 /*growBy*/);
    bool ok = q0.isOk() &&
        (!q0.canGrow()) &&
        (q0.capacity() == 12UL) &&
        (q0.growthFactor() == 0) &&
        (q0.initialCap() == 12UL);
    (q0.numItems() == 0UL);
    CPPUNIT_ASSERT(ok);

    ItemQ::Stat stat(q0);
    ok = (stat.usagePeak() == 0) && (stat.numFails() == 0) && (stat.numGets() == 0) && (stat.numPuts() == 0);
    CPPUNIT_ASSERT(ok);

    ItemQ q1(ItemQ::MAX_CAP + 1, 0 /*growBy*/);
    ok = q1.isOk() &&
        (!q1.canGrow()) &&
        (q1.capacity() == ItemQ::MAX_CAP) &&
        (q1.growthFactor() == 0) &&
        (q1.initialCap() == ItemQ::MAX_CAP);
    (q1.numItems() == 0UL);
    CPPUNIT_ASSERT(ok);
}


void ItemQSuite::testCtor01()
{
    ItemQ q0(123UL /*capacity*/, -1 /*growBy*/);
    bool ok = q0.isOk() &&
        q0.canGrow() &&
        (q0.capacity() == 123UL) &&
        (q0.growthFactor() == -1) &&
        (q0.initialCap() == 123UL);
    (q0.numItems() == 0UL);
    CPPUNIT_ASSERT(ok);

    ItemQ::Stat stat(q0);
    ok = (stat.usagePeak() == 0) && (stat.numFails() == 0) && (stat.numGets() == 0) && (stat.numPuts() == 0);
    CPPUNIT_ASSERT(ok);

    ItemQ q1(23UL /*capacity*/, 45 /*growBy*/);
    ok = q1.isOk() &&
        q1.canGrow() &&
        (q1.capacity() == 23UL) &&
        (q1.growthFactor() == 45) &&
        (q1.initialCap() == 23UL);
    (q1.numItems() == 0UL);
    CPPUNIT_ASSERT(ok);
}


void ItemQSuite::testDtor00()
{

    // Destructing a non-empty queue is okay and should result in self-destructing messages.
    ItemQ q(ItemQ::DefaultCap, 0 /*growBy*/);
    for (;;)
    {
        ItemQ::Item* item = new ItemQ::Item();
        if (!q.put(item, 0UL /*timeoutInMsecs*/))
        {
            break;
        }
    }

    ItemQ::Stat stat(q);
    size_t count = q.capacity();
    bool ok = (stat.usagePeak() == count) && (stat.numFails() == 1) && (stat.numGets() == 0) && (stat.numPuts() == count);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - bool ItemQ::expedite(Item* item, unsigned long timeoutInMsecs=ETERNITY);
//
void ItemQSuite::testExpedite00()
{
    ItemQ q(32 /*capacity*/, 0 /*growBy*/);

    // Try enqueuing when queue is not full. Wait forever.
    q.resetStat();
    ItemQ::Item* item;
    bool ok = true;
    for (unsigned long i = q.capacity(); i > 0; --i)
    {
        item = new ItemQ::Item();
        if (!q.expedite(item, ItemQ::ETERNITY))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    // Try enqueuing when queue is full.
    item = new ItemQ::Item();
    ok = (!q.expedite(item, 0UL /*timeoutInMsecs*/));
    CPPUNIT_ASSERT(ok);
    item = new ItemQ::Item();
    ok = (!q.expedite(item, 12UL /*timeoutInMsecs*/));
    CPPUNIT_ASSERT(ok);

    while (q.get(item, 0UL /*timeoutInMsecs*/))
    {
        ItemQ::Item::release(item);
    }

    ItemQ::Stat stat(q);
    size_t count = q.capacity();
    ok = (stat.usagePeak() == count) && (stat.numFails() == 2) && (stat.numGets() == count) && (stat.numPuts() == count);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - bool ItemQ::expedite(Item* item, unsigned long timeoutInMsecs=ETERNITY);
//
void ItemQSuite::testExpedite01()
{
    ItemQ q(32 /*capacity*/, 0 /*growBy*/);

    // Try enqueuing when queue is not full. Don't wait forever.
    q.resetStat();
    ItemQ::Item* item = new ItemQ::Item();
    bool ok = q.expedite(item, 0UL /*timeoutInMsecs*/);
    CPPUNIT_ASSERT(ok);
    item = new ItemQ::Item();
    ok = q.expedite(item, 12UL /*timeoutInMsecs*/);
    CPPUNIT_ASSERT(ok);

    while (q.get(item, 0UL /*timeoutInMsecs*/))
    {
        ItemQ::Item::release(item);
    }

    ItemQ::Stat stat(q);
    ok = (stat.usagePeak() == 2) && (stat.numFails() == 0) && (stat.numGets() == 2) && (stat.numPuts() == 2);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - bool ItemQ::get(Item*& item, unsigned long timeoutInMsecs=ETERNITY);
//
void ItemQSuite::testGet00()
{
    ItemQ q(32 /*capacity*/, 0 /*growBy*/);

    // Try dequeuing when queue is empty.
    ItemQ::Item* item;
    bool ok = (!q.get(item, 0UL /*timeoutInMsecs*/));
    CPPUNIT_ASSERT(ok);
    ok = (!q.get(item, 12UL /*timeoutInMsecs*/));
    CPPUNIT_ASSERT(ok);

    ItemQ::Stat stat(q);
    ok = (stat.usagePeak() == 0) && (stat.numFails() == 0) && (stat.numGets() == 0) && (stat.numPuts() == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - bool ItemQ::apply(cb0_t cb, void* arg=0) const;
// - bool ItemQ::get(Item*& item, unsigned long timeoutInMsecs=ETERNITY);
//
void ItemQSuite::testGet01()
{
    ItemQ q(32 /*capacity*/, 0 /*growBy*/);

    unsigned long id;
    for (id = 0;; ++id)
    {
        MyItem* item = new MyItem(id);
        if (!q.put(item, 0UL /*timeoutInMsecs*/))
        {
            break;
        }
    }

    void* arg = 0;
    bool ok = q.apply(cb0a, arg);
    CPPUNIT_ASSERT(ok);
    ok = (!q.apply(cb0b, arg));
    CPPUNIT_ASSERT(ok);

    // Make sure items are queued FIFO via put().
    ok = true;
    for (unsigned long i = 0; i < id; ++i)
    {
        ItemQ::Item* item;
        if ((!q.get(item, 0UL /*timeoutInMsecs*/)) || ((dynamic_cast<const MyItem*>(item))->id() != i))
        {
            ok = false;
            break;
        }
        ItemQ::Item::release(item);
    }
    CPPUNIT_ASSERT(ok);

    ItemQ::Stat stat(q);
    size_t count = q.capacity();
    ok = (stat.usagePeak() == count) && (stat.numFails() == 1) && (stat.numGets() == count) && (stat.numPuts() == count);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - bool ItemQ::expedite(Item* item, unsigned long timeoutInMsecs=ETERNITY);
// - bool ItemQ::get(Item*& item, unsigned long timeoutInMsecs=ETERNITY);
//
void ItemQSuite::testGet02()
{
    ItemQ q(32 /*capacity*/, 0 /*growBy*/);

    unsigned long id;
    for (id = 0;; ++id)
    {
        MyItem* item = new MyItem(id);
        if (!q.expedite(item, 0UL /*timeoutInMsecs*/))
        {
            break;
        }
    }

    // Make sure messages are queued LIFO via expedite().
    bool ok = true;
    for (long i = id - 1; i >= 0; --i)
    {
        ItemQ::Item* item;
        if ((!q.get(item, 0UL /*timeoutInMsecs*/)) ||
            ((dynamic_cast<const MyItem*>(item))->id() != static_cast<unsigned long>(i)))
        {
            ok = false;
            break;
        }
        ItemQ::Item::release(item);
    }
    CPPUNIT_ASSERT(ok);

    ItemQ::Stat stat(q);
    size_t count = q.capacity();
    ok = (stat.usagePeak() == count) && (stat.numFails() == 1) && (stat.numGets() == count) && (stat.numPuts() == count);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - bool ItemQ::put(Item* item, unsigned long timeoutInMsecs=ETERNITY);
//
void ItemQSuite::testGrow00()
{
    ItemQ q(16 /*capacity*/, -1 /*growBy*/);

    bool ok = true;
    for (unsigned long i = q.capacity(); i > 0; --i)
    {
        if (!q.put(0 /*item*/, ItemQ::ETERNITY))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    size_t oldCap = q.capacity();
    size_t newCap = oldCap * 2;
    ok = q.put(0 /*item*/, 0UL /*timeoutInMsecs*/) && (q.capacity() == newCap) && (q.initialCap() == oldCap);
    CPPUNIT_ASSERT(ok);

    ItemQ::Item* item;
    size_t count = q.numItems();
    while (q.get(item, 0UL /*timeoutInMsecs*/));

    ItemQ::Stat stat(q);
    ok = (stat.usagePeak() == count) && (stat.numFails() == 0) && (stat.numGets() == count) && (stat.numPuts() == count);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - bool ItemQ::expedite(Item* item, unsigned long timeoutInMsecs=ETERNITY);
//
void ItemQSuite::testGrow01()
{
    ItemQ q(24 /*capacity*/, 12 /*growBy*/);

    bool ok = true;
    for (unsigned long i = q.capacity(); i > 0; --i)
    {
        if (!q.expedite(0 /*item*/, ItemQ::ETERNITY))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    size_t oldCap = q.capacity();
    size_t newCap = oldCap + 12;
    ok = q.expedite(0 /*item*/, 0UL /*timeoutInMsecs*/) && (q.capacity() == newCap) && (q.initialCap() == oldCap);
    CPPUNIT_ASSERT(ok);

    ItemQ::Item* item;
    size_t count = q.numItems();
    while (q.get(item, 0UL /*timeoutInMsecs*/));

    ItemQ::Stat stat(q);
    ok = (stat.usagePeak() == count) && (stat.numFails() == 0) && (stat.numGets() == count) && (stat.numPuts() == count);
    CPPUNIT_ASSERT(ok);
}


//
// Make sure growable queue's capacity has a limit.
//
void ItemQSuite::testGrow02()
{
    ItemQ q(12345 /*capacity*/, 9000 /*growBy*/);
    while (q.put(0 /*item*/, 0UL /*timeoutInMsecs*/));

    ItemQ::Stat stat(q);
    bool ok = (q.capacity() == ItemQ::MAX_CAP) &&
        (q.numItems() == ItemQ::MAX_CAP) &&
        (stat.usagePeak() == ItemQ::MAX_CAP) &&
        (stat.numFails() == 1) &&
        (stat.numGets() == 0) &&
        (stat.numPuts() == ItemQ::MAX_CAP);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - bool ItemQ::put(Item* item, unsigned long timeoutInMsecs=ETERNITY);
//
void ItemQSuite::testPut00()
{
    ItemQ q(32 /*capacity*/, 0 /*growBy*/);

    // Try enqueuing when queue is not full. Wait forever.
    ItemQ::Item* item;
    bool ok = true;
    for (unsigned long i = q.capacity(); i > 0; --i)
    {
        item = new ItemQ::Item();
        if (!q.put(item, ItemQ::ETERNITY))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    // Try enqueuing when queue is full.
    item = new ItemQ::Item();
    ok = (!q.put(item, 0UL /*timeoutInMsecs*/));
    CPPUNIT_ASSERT(ok);
    item = new ItemQ::Item();
    ok = (!q.put(item, 12UL /*timeoutInMsecs*/));
    CPPUNIT_ASSERT(ok);

    while (q.get(item, 0UL /*timeoutInMsecs*/))
    {
        ItemQ::Item::release(item);
    }

    ItemQ::Stat stat(q);
    size_t count = q.capacity();
    ok = (stat.usagePeak() == count) && (stat.numFails() == 2) && (stat.numGets() == count) && (stat.numPuts() == count);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - bool ItemQ::put(Item* item, unsigned long timeoutInMsecs=ETERNITY);
//
void ItemQSuite::testPut01()
{
    ItemQ q(32 /*capacity*/, 0 /*growBy*/);

    // Try enqueuing when queue is not full. Don't wait forever.
    ItemQ::Item* item = new ItemQ::Item();
    bool ok = q.put(item, 0UL /*timeoutInMsecs*/);
    CPPUNIT_ASSERT(ok);
    item = new ItemQ::Item();
    ok = q.put(item, 12UL /*timeoutInMsecs*/);
    CPPUNIT_ASSERT(ok);

    while (q.get(item, 0UL /*timeoutInMsecs*/))
    {
        ItemQ::Item::release(item);
    }

    ItemQ::Stat stat(q);
    ok = (stat.usagePeak() == 2) && (stat.numFails() == 0) && (stat.numGets() == 2) && (stat.numPuts() == 2);
    CPPUNIT_ASSERT(ok);
}
