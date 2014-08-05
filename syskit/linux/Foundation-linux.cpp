/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <cstdio>
#include <cstdlib>
#include <new>

#include "syskit/Atomic32.hpp"
#include "syskit/AtomicWord.hpp"
#include "syskit/BufPool.hpp"
#include "syskit/CriSection.hpp"
#include "syskit/Foundation.hpp"
#include "syskit/Process.hpp"
#include "syskit/Thread.hpp"
#include "syskit/Vec.hpp"
#include "syskit/sys.hpp"

static syskit::Foundation::Count s_foundationLock(syskit::Foundation::instance());

BEGIN_NAMESPACE1(syskit)

typedef struct foundation_s
{

    // Header.
    Atomic32 refCount;
    AtomicWord ownerPid;
    Foundation* outerSkin;
    unsigned int signature;

    // Body. Private.
    unsigned char bufPool[sizeof(BufPool)];
    unsigned char singletonVec[sizeof(Vec)];
    unsigned char singletonVecCs[sizeof(CriSection)];
    winTopFilter_t winTopFilter;
} foundation_t;

const char BUF_POOL_CONFIG[] = "__SyskitBufPoolConfig";
const char FOUNDATION_KEY[] = "__SyskitFoundationKey";
const char START_TIME[] = "__SyskitStartTime";
const unsigned int FOUNDATION_SIZE = (sizeof(foundation_t) + 0x1000U) & 0xfffff000U; //bytes w/ room for forward compatibility
const unsigned int SIGNATURE = 0x74747030U; //ttp-3.0


Foundation::Foundation():
RefCounted(0U)
{

    // Use an environment variable to determine the first per-process reference.
    // Use another environment variable as per-process memory. This is a
    // relatively simple way to make Foundation a per-process singleton.
    char tag[20 + 1]; //big enough for "0xnnnnnnnn" or "0xnnnnnnnnnnnnnnnn"
    unsigned long long startTime = rdtsc();
    sprintf(tag, "%llu", startTime);
    setenv(START_TIME, tag, 0 /*overwrite*/);
    const char* s = getenv(START_TIME);
    bool isFirstReference = ((s != 0) && (strcmp(s + sizeof(START_TIME), tag) == 0));
    if (isFirstReference)
    {
        unsigned char* p = new unsigned char[FOUNDATION_SIZE];
        memset(p, 0, FOUNDATION_SIZE);
        foundation_ = reinterpret_cast<foundation_t*>(p);
    }
    else
    {
        for (; (s = getenv(FOUNDATION_KEY)) == 0; Thread::yield());
        foundation_ = reinterpret_cast<foundation_t*>(strtoull(tag, 0, 0));
        ++foundation_->refCount;
    }

    bufPool_ = reinterpret_cast<BufPool*>(foundation_->bufPool);
    singletonVec_ = reinterpret_cast<Vec*>(foundation_->singletonVec);
    singletonVecCs_ = reinterpret_cast<CriSection*>(foundation_->singletonVecCs);

    // Construct only if this is the first per-process reference.
    if (isFirstReference)
    {
        foundation_->outerSkin = this;
        size_t myPid = Process::myId();
        foundation_->ownerPid.set(myPid);
        foundation_->refCount.set(0U);
        foundation_->signature = SIGNATURE;
        construct();
        foundation_->refCount.set(1U);
        sprintf(tag, "0x%p", foundation_);
        setenv(FOUNDATION_KEY, tag, 0 /*overwrite*/);
    }
}


Foundation::~Foundation()
{

    // Destruct only if this is the last per-process reference.
    if (--foundation_->refCount == 0U)
    {
        destruct();
        unsetenv(FOUNDATION_KEY);
        const unsigned char* p = reinterpret_cast<const unsigned char*>(foundation_);
        delete p;
    }
}


//!
//! Return the syskit foundation. This is a per-process singleton.
//!
const Foundation& Foundation::instance()
{

    // Allow each binary to create its own instance.
    static Foundation* s_foundation = new Foundation;

    // But only the first instance survives. All subsequent instances are immediately
    // destroyed upon creation. This code seems unnecessary complicated and inefficient,
    // and it is. Historically, this object used shared memory and was synchronized with
    // other processes, and it should be simplified and cleaned up as that feature is
    // no longer supported.
    Foundation* foundation = s_foundation->foundation_->outerSkin;
    if (foundation != s_foundation)
    {
        const Foundation* tmp = s_foundation;
        s_foundation = foundation;
        delete tmp;
    }

    return *foundation;
}


void Foundation::construct()
{
    foundation_->winTopFilter = 0;

    // Allow default BufPool configuration via environment variable. If __SyskitBufPoolConfig
    // does not exist, no buffer arenas will be preallocated. That is, they all start out empty
    // and would grow when needs arise. If the variable exists, it will be used to construct
    // the default per-process pool of small-sized buffers. Refer to the BufPool constructor
    // for the configuration string format.
    char* config = std::getenv(BUF_POOL_CONFIG);
    new(bufPool_)BufPool(config);
    new(singletonVec_)Vec(8UL /*capacity*/, -1 /*growBy*/);
    new(singletonVecCs_)CriSection(CriSection::DefaultSpinCount);
}


void Foundation::destruct()
{
    singletonVecCs_->CriSection::~CriSection();
    singletonVec_->Vec::~Vec();
    bufPool_->BufPool::~BufPool();
}


winTopFilter_t Foundation::winTopFilter() const
{
    return foundation_->winTopFilter;
}

END_NAMESPACE1
