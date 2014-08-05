#include "syskit/Lifo.hpp"

#include "syskit-ut-pch.h"
#include "LifoSuite.hpp"

using namespace syskit;

const char ITEM[] = "aRandomStringUsedForLifoPopulation!!!";
const size_t NUM_ITEMS = sizeof(ITEM) - 1;


LifoSuite::LifoSuite()
{

    // Fixed capacity.
    q0_ = new Lifo(16 /*capacity*/, 0 /*growBy*/);

    // Growable.
    q1_ = new Lifo(13 /*capacity*/, 11 /*growBy*/);
}


LifoSuite::~LifoSuite()
{
    delete q1_;
    delete q0_;
}


void LifoSuite::testPeek00()
{
    Lifo q(1 /*capacity*/, 0 /*growBy*/);
    bool ok = (q.pushHard(0) && (q.peek(0) == 0));
    CPPUNIT_ASSERT(ok);
    ok = (q.pushHard(this) && (q.peek(0) == this));
    CPPUNIT_ASSERT(ok);
}


//
// Push random entries down.
//
void LifoSuite::testPush00()
{

    // Push random entries down.
    bool ok = true;
    const char* p;
    for (p = ITEM; *p; ++p)
    {
        if (!q1_->push(const_cast<char*>(p)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (q1_->capacity() == 46);
    CPPUNIT_ASSERT(ok);

    // Pop entries up and validate contents.
    Lifo q0(*q1_);
    q0 = q0; //no-op
    for (--p; p >= ITEM; --p)
    {
        void* item0 = 0;
        void* item1 = 0;
        if ((!q0.pop(item0)) || (*static_cast<const char*>(item0) != *p) ||
            (!q1_->pop(item1)) || (*static_cast<const char*>(item1) != *p))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (q0.numItems() == 0) && (q1_->numItems() == 0);
    CPPUNIT_ASSERT(ok);
    ok = q1_->resize(q1_->initialCap());
    CPPUNIT_ASSERT(ok);
}


//
// Push random entries down hard.
//
void LifoSuite::testPush01()
{

    // Push random entries down hard.
    size_t numDrops = 0;
    bool ok = true;
    const char* p;
    for (p = ITEM; *p; ++p)
    {
        bool someWasDropped;
        void* droppedItem;
        if (!q0_->pushHard(const_cast<char*>(p), someWasDropped, droppedItem))
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
    ok = (q0_->numItems() == q0_->capacity());
    CPPUNIT_ASSERT(ok);

    // Pop entries up and validate contents.
    for (const char* p0 = ITEM + numDrops; --p >= p0;)
    {
        void* item = 0;
        if ((!q0_->pop(item)) || (*static_cast<const char*>(item) != *p))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (q0_->numItems() == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator. Truncation required.
//
void LifoSuite::testOp00()
{

    // Populate a queue with random entries.
    bool ok = true;
    const char* p;
    for (p = ITEM; *p; ++p)
    {
        if (!q1_->push(const_cast<char*>(p)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    // Assignment operator. Source queue has too many entries.
    *q0_ = *q1_;
    size_t numDrops = q1_->numItems() - q0_->numItems();
    q1_->reset();
    Lifo q1(1 /*capacity*/, -1 /*growBy*/);
    q1 = *q0_;

    // Validate truncated queue.
    ok = (q0_->numItems() == q0_->capacity());
    CPPUNIT_ASSERT(ok);

    for (const char* p0 = ITEM + numDrops; --p >= p0;)
    {
        void* item0 = 0;
        void* item1 = 0;
        if ((!q0_->pop(item0)) || (*static_cast<const char*>(item0) != *p) ||
            (!q1.pop(item1)) || (*static_cast<const char*>(item1) != *p))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (q0_->numItems() == 0) && (q1.numItems() == 0);
    CPPUNIT_ASSERT(ok);
}
