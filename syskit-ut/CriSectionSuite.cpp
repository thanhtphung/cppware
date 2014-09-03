#include "syskit/CriSection.hpp"
#include "syskit/Semaphore.hpp"
#include "syskit/Thread.hpp"

#include "syskit-ut-pch.h"
#include "CriSectionSuite.hpp"

using namespace syskit;

const unsigned long long MAGIC_NUMBER = 0x123456789abcdef0ULL;

CriSection CriSectionSuite::cs_;
unsigned long long CriSectionSuite::u64_ = MAGIC_NUMBER;


CriSectionSuite::CriSectionSuite()
{
}


CriSectionSuite::~CriSectionSuite()
{
}


void CriSectionSuite::testCtor00()
{
    CriSection cs;

    // Try lock/unlock using CriSection::Lock.
    {
        CriSection::Lock lock(cs);
    }

    // Recursive locks should not cause deadlock.
    CPPUNIT_ASSERT(cs.tryLock());
    CPPUNIT_ASSERT(cs.tryLock());
    cs.lock();
    cs.unlock();
    cs.unlock();
    cs.unlock();
}


void CriSectionSuite::testCtor01()
{

    // Create threads which will exercise some CriSection code.
    Semaphore sem(0U);
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


void* CriSectionSuite::entry00(void* arg)
{
    Semaphore* sem = static_cast<Semaphore*>(arg);
    sem->decrement(Semaphore::ETERNITY);

    bool ok = true;
    for (unsigned int i = 0; i < 128; ++i)
    {

        // Validate shared resource.
        CriSection::Lock lock(cs_);
        if (u64_ != MAGIC_NUMBER)
        {
            ok = false;
            break;
        }

        // Update shared resource.
        for (unsigned int j = 0; j < 64; ++j)
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
