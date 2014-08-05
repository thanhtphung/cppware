#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "appkit/String.hpp"
#include "appkit/TempDir.hpp"
#include "appkit/TempFile.hpp"
#include "syskit/Atomic64.hpp"
#include "syskit/BitVec.hpp"
#include "syskit/Process.hpp"
#include "syskit/Semaphore.hpp"
#include "syskit/Thread.hpp"

#include "syskit-ut-pch.h"
#include "ThreadSuite.hpp"

using namespace appkit;
using namespace syskit;


ThreadSuite::ThreadSuite()
{
}


ThreadSuite::~ThreadSuite()
{
}


void ThreadSuite::crashCb0(void* arg)
{
    bool* crashCbReached = static_cast<bool*>(arg);
    *crashCbReached = true;
}


//
// Interfaces under test:
// - bool Thread::setAffinityMask(size_t affinityMask);
// - bool Thread::resume();
// - bool Thread::suspend();
// - size_t Thread::affinityMask() const;
//
void ThreadSuite::testAffinity00()
{
    size_t processAffinity = Process::affinityMask();
    unsigned long numThreads = BitVec::countSetBits(processAffinity);

    Atomic64 counter;
    Thread** thread = new Thread*[numThreads];
    bool startSuspended = true;
    unsigned long stackSizeInBytes = 4096;
    for (unsigned long i = 0; i < numThreads; thread[i++] = new Thread(entrance05, &counter, stackSizeInBytes, startSuspended));

    bool ok = true;
    for (unsigned long i = 0; i < numThreads; ++i)
    {
        Thread* t = thread[i];
        if ((!t->resume()) || (!t->setAffinityMask(processAffinity)) || (t->affinityMask() != processAffinity))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (Thread::takeANap(100), (counter > 0ULL));
    CPPUNIT_ASSERT(ok);

    for (unsigned long i = 0; i < numThreads; ++i)
    {
        if (!thread[i]->suspend())
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    unsigned long long frozenAt = counter;
    ok = (Thread::takeANap(100), (counter == frozenAt));
    CPPUNIT_ASSERT(ok);

    for (unsigned long i = 0; i < numThreads; thread[i++]->resume());
    BitVec affinityMask(numThreads, reinterpret_cast<const BitVec::word_t*>(&processAffinity), sizeof(processAffinity));
    for (size_t bit = affinityMask.firstSetBit(); bit != BitVec::INVALID_BIT; bit = affinityMask.nextSetBit(bit))
    {
        Thread* t = thread[0];
        size_t threadAffinity = (1 << bit);
        if ((!t->setAffinityMask(threadAffinity)) || (t->affinityMask() != threadAffinity))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    Thread::takeANap(100);
    ok = (counter > frozenAt);
    CPPUNIT_ASSERT(ok);

    for (unsigned long i = 0; i < numThreads; thread[i++]->kill());
    for (unsigned long i = numThreads; i > 0; delete thread[--i]);
    delete[] thread;
}


//
// Interfaces under test:
// - Thread::Thread(entrance_t entrance, void* arg=0, unsigned long stackSizeInBytes=0, bool startSuspended=false);
// - bool Thread::isOk() const;
// - bool Thread::waitTilDone(void** exitCode=0);
// - state_e Thread::state() const;
//
void ThreadSuite::testCtor00()
{
    Semaphore sem(0U);
    Thread** thread = new Thread*[10];
    bool ok = true;
    long i;
    for (i = 0; i < 10; ++i)
    {
        Thread* t = new Thread(entrance00, &sem, 0);
        if ((!t->isOk()) || (t->id() == Thread::INVALID_ID))
        {
            ok = false;
            delete t;
            break;
        }
        else
        {
            thread[i] = t;
        }
    }
    CPPUNIT_ASSERT(ok);

    sem.incrementBy(i);
    for (--i; i >= 0; --i)
    {
        Thread* t = thread[i];
        if ((!t->waitTilDone()) || (t->state() != Thread::Done))
        {
            ok = false;
            break;
        }
        delete t;
    }
    CPPUNIT_ASSERT(ok);

    delete[] thread;
}


//
// Interfaces under test:
// - Thread::Thread(entrance_t entrance, void* arg=0, unsigned long stackSizeInBytes=0, bool startSuspended=false);
// - bool Thread::isOk() const;
//
// Invalid stack size (32-bit os).
//
void ThreadSuite::testCtor01()
{
    int inWow64Mode = 0;
    IsWow64Process(GetCurrentProcess(), &inWow64Mode);

    Semaphore sem(1UL);
    unsigned long stackSizeInBytes = 0xffffffffUL;
    Thread thread(entrance00, &sem, stackSizeInBytes);
    bool ok = (!thread.isOk()) || (sizeof(size_t) == 8) || inWow64Mode;
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - Thread::Thread(entrance_t entrance, void* arg=0, unsigned long stackSizeInBytes=0, bool startSuspended=false);
// - bool Thread::isOk() const;
// - bool Thread::resume();
// - bool Thread::waitTilDone(void** exitCode=0);
// - state_e Thread::state() const;
//
// Start thread in suspended state.
//
void ThreadSuite::testCtor02()
{
    Semaphore sem(1UL);
    unsigned long stackSizeInBytes = 0;
    bool startSuspended = true;
    Thread thread(entrance00, &sem, stackSizeInBytes, startSuspended);

    // At this time, the thread must be in the Idle state.
    bool ok = true;
    for (unsigned long i = 0; i < 5; ++i)
    {
        Thread::yield();
        if ((!thread.isOk()) || (thread.state() != Thread::Idle))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = thread.resume();
    CPPUNIT_ASSERT(ok);

    ok = thread.waitTilDone();
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - state_e Thread::state() const;
// - void Thread::detach();
//
void ThreadSuite::testDetach00()
{
    Semaphore* sem = new Semaphore(0U);
    Thread* thread = new Thread(entrance01, sem);
    bool ok = (thread->state() != Thread::Done);
    CPPUNIT_ASSERT(ok);

    thread->detach();
    delete thread;

    // Resume detached child thread.
    sem->increment();
}


//
// Interfaces under test:
// - bool Thread::waitTilDone(void** exitCode=0);
// - state_e Thread::state() const;
// - void Thread::kill();
//
void ThreadSuite::testKill00()
{
    Semaphore sem(0U);
    Thread** thread = new Thread*[5];
    bool ok = true;
    long i;
    for (i = 0; i < 5; ++i)
    {
        Thread* t = new Thread(entrance02, &sem);
        if (t->isOk())
        {
            thread[i] = t;
        }
        else
        {
            ok = false;
            delete t;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    sem.incrementBy(i); //threads can proceed now
    Thread::takeANap(321); //give them time to reach the napping state
    sem.decrementBy(i, Semaphore::ETERNITY); //wait for all threads to have proceeded
    --i;

    // At this time, the threads must be in the Active state.
    // Kill them, one by one.
    for (long j = i; j >= 0; --j)
    {
        Thread* t = thread[j];
        if (t->state() != Thread::Active)
        {
            ok = false;
            break;
        }
        t->kill();
    }
    CPPUNIT_ASSERT(ok);

    // Validate kill results.
    for (long j = i; j >= 0; --j)
    {
        Thread* t = thread[j];
        void* exitCode = 0;
        if ((!t->waitTilDone(&exitCode)) || (exitCode != &sem) || (t->state() != Thread::Done))
        {
            ok = false;
            break;
        }
        delete t;
    }
    CPPUNIT_ASSERT(ok);
    delete[] thread;
}


//
// Interfaces under test:
// - state_e Thread::state() const;
// - bool Thread::killAndWait(void** exitCode=0);
//
void ThreadSuite::testKill01()
{
    Semaphore sem(0U);
    Thread t(entrance02, &sem);
    sem.increment(); //thread can proceed now
    Thread::takeANap(123); //give it time to reach the napping state
    void* exitCode = 0;
    bool ok = t.killAndWait(&exitCode) && (exitCode == &sem) && (t.state() == Thread::Done);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - static void Thread::monitorCrash(crashCb_t cb, void* arg, const wchar_t* dumpPath);
//
void ThreadSuite::testMonitorCrash00()
{
    String basename("stack-dump.txt");
    TempDir temp;
    TempFile dump(temp, basename);

    Thread::crashCb_t cb = crashCb0;
    bool crashCbReached = false;
    void* arg = &crashCbReached;
    Thread::monitorCrash(cb, arg, dump.path().widen());

    arg = 0;
    unsigned long stackSizeInBytes = 0;
    bool startSuspended = true;
    bool ok;
    {
        Thread thread(entrance03, arg, stackSizeInBytes, startSuspended);
        thread.resume();
        ok = thread.waitTilDone() && crashCbReached;
    }
    CPPUNIT_ASSERT(ok);

    crashCbReached = false;
    cb = 0;
    arg = 0;
    const wchar_t* dumpPath = 0;
    Thread::monitorCrash(cb, arg, dumpPath);

    {
        Thread thread(entrance03, arg, stackSizeInBytes, startSuspended);
        thread.resume();
        ok = thread.waitTilDone() && (!crashCbReached);
    }
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - unsigned long Thread::id() const;
// - static unsigned long Thread::myId();
//
void ThreadSuite::testMyId00()
{
    size_t tid = 0;
    Thread* thread = new Thread(entrance04, &tid, 1234UL /*stackSizeInBytes*/);
    thread->waitTilDone();
    bool ok = (thread->id() == tid);
    CPPUNIT_ASSERT(ok);

    delete thread;
}


//
// Wait for parent before proceeding with doing nothing. Given
// argument is a semaphore constructed by the parent and to be
// destructed also by the parent.
//
void* ThreadSuite::entrance00(void* arg)
{
    Semaphore* sem = static_cast<Semaphore*>(arg);
    sem->decrement(Semaphore::ETERNITY);

    return 0;
}


//
// Wait for parent before proceeding with doing nothing. Given
// argument is a semaphore constructed by the parent and to be
// destructed by the child.
//
void* ThreadSuite::entrance01(void* arg)
{
    Semaphore* sem = static_cast<Semaphore*>(arg);
    sem->decrement(Semaphore::ETERNITY);
    delete sem;

    return 0;
}


//
// Take a nap until interrupted by Thread::kill().
// Return arg if interrupted as expected.
// Return zero otherwise.
//
void* ThreadSuite::entrance02(void* arg)
{
    Semaphore* sem = static_cast<Semaphore*>(arg);
    sem->decrement(Semaphore::ETERNITY);

    sem->increment();
    bool ok = ((!Thread::takeANap(12345678UL /*napTimeInMsecs*/)) && Thread::isTerminating());
    return ok? arg: 0;
}


//
// Return *arg even if arg if zero.
//
void* ThreadSuite::entrance03(void* arg)
{
    void** zero = static_cast<void**>(arg);
    return *zero;
}


//
// Look at given argument as a pointer to an unsigned long.
// Save Thread::myId() in given argument. Yield, then return.
//
void* ThreadSuite::entrance04(void* arg)
{
    size_t& tid = *static_cast<size_t*>(arg);
    tid = Thread::myId();
    Thread::yield();

    return 0;
}


//
// Look at given argument as an atomic 64-bit number.
// Keep incrementing it until killed.
//
void* ThreadSuite::entrance05(void* arg)
{
    Atomic64& counter = *static_cast<Atomic64*>(arg);
    while (!Thread::isTerminating())
    {
        ++counter;
        Thread::takeANap(1);
    }

    return 0;
}
