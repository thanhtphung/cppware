/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "syskit-pch.h"
#include "syskit/macros.h"
#include "syskit/win/CondVar-win.hpp"

BEGIN_NAMESPACE1(syskit)


//!
//! Construct a condition variable and its associated mutex.
//!
CondVar::CondVar():
numWaiters_(0)
{
    isBroadcasting_ = false;

    // An auto-reset event is used to alert the broadcaster
    // when the last waiter got the broadcast message.
    ev_ = CreateEventW(0, 0 /*bManualEvent*/, 0 /*bInitialState*/, 0);

    // A semaphore is used to queue the waiters.
    sem_ = CreateSemaphoreW(0, 0 /*initialCount*/, 0x7fffffffU /*maxCount*/, 0);

    // A binary semaphore is used to model
    // the associated non-recursive mutex.
    sem2_ = CreateSemaphoreW(0, 1 /*initialCount*/, 1 /*maxCount*/, 0);
}


//!
//! Destruct the condition variable and its associated mutex.
//!
CondVar::~CondVar()
{
    if (sem2_ != 0)
    {
        CloseHandle(sem2_);
    }

    if (sem_ != 0)
    {
        CloseHandle(sem_);
    }

    if (ev_ != 0)
    {
        CloseHandle(ev_);
    }
}


//!
//! Broadcast the condition. That is, no-op if there are no waiters, or
//! allow all waiters to proceed otherwise. Return true if successful.
//! The associated mutex must be locked before invoking this method.
//! Behavior is unpredictable otherwise.
//!
bool CondVar::broadcast()
{
    bool ok;
    if (numWaiters_)
    {

        // Wake up all waiters.
        isBroadcasting_ = true;
        ReleaseSemaphore(sem_, numWaiters_, 0 /*lpPreviousCount*/);

        // Wait until the last waiter got the broadcast message.
        ok = (WaitForSingleObjectEx(ev_, ETERNITY, 1 /*bAlertable*/) == WAIT_OBJECT_0);
        isBroadcasting_ = false;
    }

    else
    {
        ok = true;
    }

    return ok;
}


//!
//! Signal the condition. That is, no-op if there are no waiters, or
//! allow one waiter to proceed otherwise. Return true if successful.
//! The associated mutex must be locked before invoking this method.
//! Behavior is unpredictable otherwise.
//!
bool CondVar::signal()
{
    if (numWaiters_)
    {

        // Wake up one waiter.
        ReleaseSemaphore(sem_, 1 /*lReleaseCount*/, 0 /*lpPreviousCount*/);
    }

    return true;
}


//!
//! Lock associated mutex. Wait forever if necessary.
//! Return true if successful.
//!
bool CondVar::lockMutex()
{
    return WaitForSingleObjectEx(sem2_, ETERNITY, 1 /*bAlertable*/) == WAIT_OBJECT_0;
}


//!
//! Lock associated mutex. Do not wait.
//! Return true if successful.
//!
bool CondVar::tryLockMutex()
{
    return WaitForSingleObjectEx(sem2_, 0 /*dwMilliseconds*/, 0 /*bAlertable*/) == WAIT_OBJECT_0;
}


//!
//! Unlock associated mutex.
//! Return true if successful.
//! Unlocking an unlocked mutex is allowed and is considered successful.
//!
bool CondVar::unlockMutex()
{
    ReleaseSemaphore(sem2_, 1 /*lReleaseCount*/, 0 /*lpPreviousCount*/);
    return true;
}


//!
//! Atomically unlock associated mutex and wait until the condition
//! variable is signaled. Relock the associated mutex before return.
//! Wait at most <i>timeoutInMsecs</i> msecs. Return true if successful.
//! The associated mutex must be locked before invoking this method.
//! Behavior is unpredictable otherwise. Due to inconsitencies among
//! various implementations, do not assume this wait can be interrupted.
//!
bool CondVar::wait(unsigned int timeoutInMsecs)
{

    // Atomically unlock associated semaphore and wait until the
    // condition variable is signaled (i.e., wait until some other
    // thread invokes signal() or broadcast()).
    numWaiters_++;
    bool alertable = (timeoutInMsecs > 0);
    bool ok = (SignalObjectAndWait(sem2_, sem_, timeoutInMsecs, alertable) == WAIT_OBJECT_0);
    unsigned int numWaiters = numWaiters_--;

    // Alert the broadcaster if I'm the last waiter.
    // Relock the associated mutex before returning.
    if (ok)
    {
        unsigned int rc = ((numWaiters == 1) && isBroadcasting_)?
            SignalObjectAndWait(ev_, sem2_, ETERNITY, 1 /*bAlertable*/):
            WaitForSingleObjectEx(sem2_, ETERNITY, 1 /*bAlertable*/);
        ok = (rc == WAIT_OBJECT_0);
    }

    return ok;
}

END_NAMESPACE1
