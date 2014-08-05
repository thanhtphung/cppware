/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include <new>

#include "syskit-pch.h"
#include "syskit/Atomic32.hpp"
#include "syskit/AtomicWord.hpp"
#include "syskit/BufPool.hpp"
#include "syskit/CallStack.hpp"
#include "syskit/CriSection.hpp"
#include "syskit/Foundation.hpp"
#include "syskit/Process.hpp"
#include "syskit/Thread.hpp"
#include "syskit/Vec.hpp"
#include "syskit/sys.hpp"

#if _DEBUG && _WIN32 && !_M_X64
#include "rational/pure.h"

#else
inline bool PurifyIsRunning()
{
    return false;
}

#endif

static syskit::Foundation::Count s_foundationLock(syskit::Foundation::instance());

BEGIN_NAMESPACE1(syskit)

typedef struct foundation_s
{

    // Header.
    Atomic32 refCount;
    AtomicWord ownerPid;
    Foundation* outerSkin;
    HANDLE mutex1;
    unsigned int signature;

    // Body. Private.
    unsigned char bufPool[sizeof(BufPool)];
    unsigned char singletonVec[sizeof(Vec)];
    unsigned char singletonVecCs[sizeof(CriSection)];
    winTopFilter_t winTopFilter;
} foundation_t;

const unsigned int FOUNDATION_SIZE = (sizeof(foundation_t) + 0x1000U) & 0xfffff000U; //bytes w/ room for forward compatibility
const unsigned int SIGNATURE = 0x74747030U; //ttp-3.0
const wchar_t BUF_POOL_CONFIG[] = L"__SyskitBufPoolConfig";
const wchar_t FOUNDATION_KEY[] = L"__SyskitFoundationKey";


Foundation::Foundation():
RefCounted(0U)
{

    // Use a named mutex to determine the first per-process reference.
    // Use an environment variable as per-process memory. This is a
    // relatively simple way to make Foundation a per-process singleton.
    wchar_t tag[13 + 20 + 1]; //big enough for "Local\\syskit-xxx" or "0xnnnnnnnn" or "0xnnnnnnnnnnnnnnnn"
    unsigned int tagSizeInChars = sizeof(tag) / sizeof(tag[0]);
    size_t myPid = Process::myId();
    swprintf(tag, tagSizeInChars, L"Local\\syskit-" SIZE_T_SPECW, myPid);
    HANDLE key = CreateMutexW(0, 0, tag);
    bool isFirstReference = ((key != 0) && (GetLastError() == ERROR_SUCCESS));
    if (isFirstReference)
    {
        unsigned char* p = new unsigned char[FOUNDATION_SIZE];
        memset(p, 0, FOUNDATION_SIZE);
        foundation_ = reinterpret_cast<foundation_t*>(p);
    }
    else
    {
        tag[0] = L'0';
        tag[1] = 0;
        while ((GetEnvironmentVariableW(FOUNDATION_KEY, tag, tagSizeInChars) == 0) && (GetLastError() == ERROR_ENVVAR_NOT_FOUND))
        {
            Thread::yield();
        }
        foundation_ = reinterpret_cast<foundation_t*>(wcstoull(tag, 0, 0));
        ++foundation_->refCount;
    }

    bufPool_ = reinterpret_cast<BufPool*>(foundation_->bufPool);
    singletonVec_ = reinterpret_cast<Vec*>(foundation_->singletonVec);
    singletonVecCs_ = reinterpret_cast<CriSection*>(foundation_->singletonVecCs);

    // Construct only if this is the first per-process reference.
    if (isFirstReference)
    {
        foundation_->mutex1 = key;
        foundation_->outerSkin = this;
        foundation_->ownerPid.set(myPid);
        foundation_->refCount.set(0U);
        foundation_->signature = SIGNATURE;
        construct();
        foundation_->refCount.set(1U);
        swprintf(tag, tagSizeInChars, L"0x%p", foundation_);
        SetEnvironmentVariableW(FOUNDATION_KEY, tag);
    }
    else
    {
        CloseHandle(key);
    }
}


Foundation::~Foundation()
{

    // Destruct only if this is the last per-process reference.
    if (--foundation_->refCount == 0U)
    {
        destruct();
        CloseHandle(foundation_->mutex1);
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

    // Intercept top exception handling filter.
    foundation_->winTopFilter = SetUnhandledExceptionFilter(CallStack::topFilter);

    // Allow default BufPool configuration via environment variable. If __SyskitBufPoolConfig
    // does not exist, no buffer arenas will be preallocated. That is, they all start out empty
    // and would grow when needs arise. If the variable exists, it will be used to construct
    // the default per-process pool of small-sized buffers. Refer to the BufPool constructor
    // for the configuration string format.
    char* config = 0;
    wchar_t* configW = 0;
    unsigned int configWSizeInChars = 0;
    configWSizeInChars = GetEnvironmentVariableW(BUF_POOL_CONFIG, configW, configWSizeInChars);
    if (configWSizeInChars > 0)
    {
        configW = new wchar_t[configWSizeInChars];
        GetEnvironmentVariableW(BUF_POOL_CONFIG, configW, configWSizeInChars);
        unsigned int configSizeInBytes = configWSizeInChars * 3; //assume worst-case non-ASCII 3 bytes per character
        config = new char[configSizeInBytes];
        WideCharToMultiByte(CP_UTF8, 0, configW, -1, config, configSizeInBytes, 0, 0);
    }
    else if (PurifyIsRunning())
    {
        config = new char[6 + 1];
        memcpy(config, "0:0:0;", 6 + 1);
    }

    new(bufPool_)BufPool(config);
    new(singletonVec_)Vec(8UL /*capacity*/, -1 /*growBy*/);
    new(singletonVecCs_)CriSection(CriSection::DefaultSpinCount);

    delete[] config;
    delete[] configW;
}


void Foundation::destruct()
{
    singletonVecCs_->CriSection::~CriSection();
    singletonVec_->Vec::~Vec();
    bufPool_->BufPool::~BufPool();

    // Restore top exception handling filter.
    winTopFilter_t topFilter = foundation_->winTopFilter;
    foundation_->winTopFilter = 0;
    SetUnhandledExceptionFilter(topFilter);
}


winTopFilter_t Foundation::winTopFilter() const
{
    return foundation_->winTopFilter;
}

END_NAMESPACE1
