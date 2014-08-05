/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "syskit-pch.h"
#include "syskit/Mutex.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


Mutex::Mutex()
{

    // A binary semaphore is used to model a non-recursive mutex.
    mu_ = CreateSemaphoreW(0, 1 /*initialCount*/, 1 /*maxCount*/, 0);
}


Mutex::~Mutex()
{
    if (mu_ != 0)
    {
        CloseHandle(mu_);
    }
}


//!
//! Return true if instance was successfully constructed.
//!
bool Mutex::isOk() const
{
    return mu_ != 0;
}


//!
//! Lock this mutex. Wait forever if necessary. Return true if successful.
//!
bool Mutex::lock()
{
    return WaitForSingleObjectEx(mu_, ETERNITY, 1 /*bAlertable*/) == WAIT_OBJECT_0;
}


//!
//! Lock this mutex. Do not wait. Return true if successful.
//!
bool Mutex::tryLock()
{
    return WaitForSingleObjectEx(mu_, 0 /*dwMilliseconds*/, 0 /*bAlertable*/) == WAIT_OBJECT_0;
}


//!
//! Unlock this mutex. Return true if successful. Unlocking an unlocked mutex
//! is allowed and is considered successful.
//!
bool Mutex::unlock()
{
    ReleaseSemaphore(mu_, 1 /*lReleaseCount*/, 0 /*lpPreviousCount*/);
    return true;
}

END_NAMESPACE1
