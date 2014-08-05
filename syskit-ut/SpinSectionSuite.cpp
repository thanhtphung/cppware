#include "syskit/Semaphore.hpp"
#include "syskit/SpinSection.hpp"
#include "syskit/Thread.hpp"

#include "syskit-ut-pch.h"
#include "SpinSectionSuite.hpp"

using namespace syskit;

const unsigned long long MAGIC_NUMBER = 0x123456789abcdef0ULL;

SpinSection SpinSectionSuite::ss_;
unsigned long long SpinSectionSuite::u64_ = MAGIC_NUMBER;


SpinSectionSuite::SpinSectionSuite()
{
}


SpinSectionSuite::~SpinSectionSuite()
{
}


void SpinSectionSuite::testCtor00()
{
    SpinSection ss;
    bool ok = ss.isOk();
    CPPUNIT_ASSERT(ok);

    // Try lock/unlock using SpinSection::Lock.
    {
        SpinSection::Lock lock(ss);
    }

    // Non-blocking recursive locks should fail.
    ok = (ss.tryLock() && (!ss.tryLock()));
    CPPUNIT_ASSERT(ok);
    ss.unlock();

    // Make sure spin count can be set if allowed.
    bool spinAllowed = (ss.spinCount() == SpinSection::DefaultSpinCount);
    ok = (ss.setSpinCount(12345UL) == spinAllowed);
    CPPUNIT_ASSERT(ok);
}


void SpinSectionSuite::testCtor01()
{

    // Create threads which will exercise some SpinSection code.
    Semaphore sem(0U);
    Thread** threadVec = new Thread*[16];
    long i;
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
    sem.incrementBy(i);

    // Wait for their deaths.
    for (--i; i >= 0; --i)
    {
        Thread* t = threadVec[i];
        t->waitTilDone();
        delete t;
    }
    delete[] threadVec;

    // Sanity check.
    bool ok = (u64_ == MAGIC_NUMBER);
    CPPUNIT_ASSERT(ok);
}


void* SpinSectionSuite::entry00(void* arg)
{
    Semaphore* sem = static_cast<Semaphore*>(arg);
    sem->decrement(Semaphore::ETERNITY);

    bool ok = true;
    for (unsigned long i = 0; i < 128; ++i)
    {

        if (ss_.tryLock())
        {
            ss_.unlock();
        }

        // Validate shared resource.
        SpinSection::Lock lock(ss_);
        if (u64_ != MAGIC_NUMBER)
        {
            ok = false;
            break;
        }

        // Update shared resource.
        for (unsigned long j = 0; j < 64; ++j)
        {
            u64_ >>= 1;
            Thread::yield();
        }

        // Restore shared resource.
        u64_ = MAGIC_NUMBER;
    }

    CPPUNIT_ASSERT(ok);
    return 0;
}
