/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "syskit-pch.h"
#include "syskit/CriSection.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


CriSection::CriSection()
{
    cs_ = new _RTL_CRITICAL_SECTION;
    InitializeCriticalSection(cs_);
}


CriSection::CriSection(unsigned int spinCount)
{
    cs_ = new _RTL_CRITICAL_SECTION;
    InitializeCriticalSectionAndSpinCount(cs_, spinCount);
}


CriSection::~CriSection()
{
    DeleteCriticalSection(cs_);
    delete cs_;
}


//!
//! Lock this critical section. Do not wait.
//! Return true if successful.
//!
bool CriSection::tryLock()
{
    return TryEnterCriticalSection(cs_) != 0;
}


//!
//! Lock this critical section. Wait forever if necessary.
//!
void CriSection::lock()
{
    EnterCriticalSection(cs_);
}


//!
//! Unlock this critical section.
//!
void CriSection::unlock()
{
    LeaveCriticalSection(cs_);
}

END_NAMESPACE1
