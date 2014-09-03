#include "syskit/Semaphore.hpp"
#include "syskit/Thread.hpp"

#include "syskit-ut-pch.h"
#include "SemaphoreSuite.hpp"

using namespace syskit;

BEGIN_NAMESPACE

class Xema4: public Semaphore
{
public:
    Xema4(sem_t sem);
    Xema4(unsigned int capacity);
    ~Xema4();
    using Semaphore::handle;
    using Semaphore::reset;
};

Xema4::Xema4(unsigned int capacity):
Semaphore(capacity)
{
}

Xema4::Xema4(sem_t sem):
Semaphore(sem)
{
}

Xema4::~Xema4()
{
}

END_NAMESPACE


SemaphoreSuite::SemaphoreSuite()
{
}


SemaphoreSuite::~SemaphoreSuite()
{
}


void SemaphoreSuite::testCtor00()
{
    Semaphore sem(Semaphore::MAX_CAP - 1 /*capacity*/);
    bool ok = sem.isOk();
    CPPUNIT_ASSERT(ok);

    // Release token using increment().
    // Should fail if releasing unallocated tokens.
    ok = sem.increment() && (!sem.increment());
    CPPUNIT_ASSERT(ok);
    sem.decrement(Semaphore::ETERNITY);

    // Release token using incrementBy().
    // Should fail if releasing unallocated tokens.
    ok = sem.incrementBy(1) && (!sem.incrementBy(1));
    CPPUNIT_ASSERT(ok);
    sem.decrement(Semaphore::ETERNITY);

    // Release tokens using incrementBy().
    // Should fail if releasing unallocated tokens.
    sem.decrementBy(4);
    ok = sem.incrementBy(5) && (!sem.incrementBy(1));
    CPPUNIT_ASSERT(ok);
    sem.decrement(Semaphore::ETERNITY);

    // Releasing zero tokens is not allowed.
    // Releasing unallocated tokens is not allowed.
    ok = (!sem.incrementBy(0)) && (!sem.incrementBy(2)) && (!sem.incrementBy(Semaphore::MAX_CAP + 1));
    CPPUNIT_ASSERT(ok);
}


void SemaphoreSuite::testCtor01()
{

    // Create threads which will exercise some Semaphore code.
    Semaphore sem(0U /*capacity*/);
    Thread** threadVec = new Thread*[16];
    int i;
    for (i = 0; i < 16; ++i)
    {
        Thread* t = new Thread(entry00, &sem);
        if (t->isOk())
        {
            threadVec[i] = t;
        }
        else
        {
            delete t;
            break;
        }
    }

    // Allow threads to proceed.
    sem.incrementBy(i * 3);

    // Wait for their deaths.
    bool ok = true;
    for (--i; i >= 0; --i)
    {
        Thread* t = threadVec[i];
        void* exitCode = 0;
        t->waitTilDone(&exitCode);
        if (exitCode == 0)
        {
            ok = false;
            break;
        }
        delete t;
    }
    CPPUNIT_ASSERT(ok);

    delete[] threadVec;
}


//
// Blocking and Non-blocking operations.
//
void SemaphoreSuite::testDecrement00()
{
    Semaphore sem(Semaphore::MAX_CAP + 1);
    bool ok = (sem.decrementBy(Semaphore::MAX_CAP, 0) && sem.incrementBy(Semaphore::MAX_CAP));
    CPPUNIT_ASSERT(ok);

    ok = ((!sem.decrementBy(0, Semaphore::ETERNITY)) && (!sem.decrementBy(Semaphore::MAX_CAP + 1, 0)));
    CPPUNIT_ASSERT(ok);

    sem.decrement();
    ok = (!sem.decrementBy(Semaphore::MAX_CAP, 0));
    CPPUNIT_ASSERT(ok);
}


//
// Timed operations.
//
void SemaphoreSuite::testDecrement01()
{
    Semaphore sem(5U /*capacity*/);
    bool ok = (sem.decrementBy(5, 12) && sem.incrementBy(5));
    CPPUNIT_ASSERT(ok);

    ok = ((!sem.decrementBy(0, 34)) && (!sem.decrementBy(Semaphore::MAX_CAP + 1, 56)));
    CPPUNIT_ASSERT(ok);

    sem.decrement(78);
    ok = (!sem.decrementBy(5, 1));
    CPPUNIT_ASSERT(ok);

    sem.incrementBy(Semaphore::MAX_CAP - 4);
    ok = sem.decrementBy(Semaphore::MAX_CAP, 1);
    CPPUNIT_ASSERT(ok);
}


void SemaphoreSuite::testDetach00()
{
    Xema4 xem0(123U);
    Xema4 xem1(xem0.handle());
    bool ok = xem1.isOk();
    CPPUNIT_ASSERT(ok);

    xem0.detach();
    xem1.detach();
}


//
// Interfaces under test:
// - Semaphore::Lock::Lock(Semaphore&);
//
void SemaphoreSuite::testLock00()
{
    Semaphore sem(1U /*capacity*/);

    // Acquire/Release token using Lock.
    {
        Semaphore::Lock lock(sem);
        bool ok = lock.isOk();
        CPPUNIT_ASSERT(ok);
    }

    bool ok = (!sem.incrementBy(Semaphore::MAX_CAP));
    CPPUNIT_ASSERT(ok);

    ok = sem.incrementBy(Semaphore::MAX_CAP - 1);
    CPPUNIT_ASSERT(ok);

    ok = sem.decrementBy(Semaphore::MAX_CAP);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - Semaphore::Lock::Lock(Semaphore&, unsigned int);
//
void SemaphoreSuite::testLock01()
{
    Semaphore sem(3U /*capacity*/);

    // Locking zero or too many tokens.
    {
        unsigned int numTokens = 0U;
        Semaphore::Lock lock0(sem, numTokens);
        bool ok = (!lock0.isOk());
        CPPUNIT_ASSERT(ok);
        numTokens = Semaphore::MAX_CAP + 1U;
        Semaphore::Lock lock1(sem, numTokens);
        ok = (!lock1.isOk());
        CPPUNIT_ASSERT(ok);
    }

    // Acquire/Release token using Lock.
    {
        unsigned int numTokens = 1U;
        Semaphore::Lock lock(sem, numTokens);
        bool ok = lock.isOk();
        CPPUNIT_ASSERT(ok);
    }

    // Acquire/Release tokens using Lock.
    {
        unsigned int numTokens = 3U;
        Semaphore::Lock lock(sem, numTokens);
        bool ok = lock.isOk();
        CPPUNIT_ASSERT(ok);
    }

    bool ok = (!sem.incrementBy(Semaphore::MAX_CAP));
    CPPUNIT_ASSERT(ok);

    ok = sem.incrementBy(Semaphore::MAX_CAP - 3U);
    CPPUNIT_ASSERT(ok);

    ok = sem.decrementBy(Semaphore::MAX_CAP);
    CPPUNIT_ASSERT(ok);
}


void SemaphoreSuite::testReset00()
{

    // From zero.
    Xema4 sem(0U);
    sem.reset(5);
    bool ok = (sem.tryDecrementBy(5) && sem.incrementBy(5));
    CPPUNIT_ASSERT(ok);

    // Non-zero to zero.
    sem.reset(0);
    ok = (!sem.tryDecrement());
    CPPUNIT_ASSERT(ok);

    // Exceed limit.
    sem.incrementBy(5);
    sem.reset(Semaphore::MAX_CAP + 1);
    ok = sem.tryDecrementBy(Semaphore::MAX_CAP);
    CPPUNIT_ASSERT(ok);

    // Non-zero to non-zero.
    sem.reset(5);
    sem.reset(Semaphore::MAX_CAP - 5);
    ok = sem.tryDecrementBy(Semaphore::MAX_CAP - 5);
    CPPUNIT_ASSERT(ok);
}


void SemaphoreSuite::testWait00()
{
    Xema4* xem0 = new Xema4(1U /*capacity*/);
    bool ok = xem0->waitTilNonEmpty();
    CPPUNIT_ASSERT(ok);

    xem0->decrement();
    Thread thread(entry01, xem0);
    Thread::takeANap(21U);
    xem0->increment();
    void* exitCode = 0;
    thread.waitTilDone(&exitCode);
    ok = (exitCode != 0);
    CPPUNIT_ASSERT(ok);

    // Invalid semaphore handle.
    Xema4 xem1(xem0->handle());
    delete xem0;
    ok = (!xem1.waitTilNonEmpty());
    CPPUNIT_ASSERT(ok);
}


void SemaphoreSuite::testWait01()
{
    Semaphore sem(123U /*capacity*/);
    bool ok = sem.waitTilFull(123);
    CPPUNIT_ASSERT(ok);

    ok = (!sem.waitTilFull(Semaphore::MAX_CAP + 1));
    CPPUNIT_ASSERT(ok);
}


void* SemaphoreSuite::entry00(void* arg)
{
    Semaphore* sem = static_cast<Semaphore*>(arg);
    bool ok = sem->decrementBy(2, 1234 /*waitTimeInMsecs*/) && sem->decrement();
    return ok? sem: 0;
}


void* SemaphoreSuite::entry01(void* arg)
{
    Semaphore* sem = static_cast<Semaphore*>(arg);
    bool ok = sem->waitTilNonEmpty();
    return ok? sem: 0;
}
