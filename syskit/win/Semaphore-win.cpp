/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "syskit-pch.h"
#include "syskit/Semaphore.hpp"
#include "syskit/Thread.hpp"
#include "syskit/macros.h"
#include "syskit/win/Janitor-win.hpp"

BEGIN_NAMESPACE1(syskit)


//!
//! Construct a semaphore with given capacity. Given capacity must not be larger
//! than MAX_CAP. If it is, it is considered equivalent to MAX_CAP. The capacity
//! specifies the number of tokens available for acquisition. The construction
//! can fail due to lack of system resources. Use isOk() to determine if the
//! semaphore construction is successful.
//!
Semaphore::Semaphore(unsigned int capacity)
{
    sem_ = CreateSemaphoreW(0, (capacity > MAX_CAP)? MAX_CAP: capacity, MAX_CAP, 0);
    semIsMine_ = true;
}


Semaphore::~Semaphore()
{
    if (semIsMine_ && (sem_ != 0))
    {
        CloseHandle(sem_);
    }
}


//
// Acquire delta tokens. Wait forever if necessary.
//
bool Semaphore::acquireTokens(unsigned int delta)
{

    // Sanity check.
    if ((delta == 0) || (delta > MAX_CAP))
    {
        return false;
    }

    // Acquire all tokens. Undo and return immediately if failure occurs.
    unsigned int waitTimeInMsecs = ETERNITY;
    bool alertable = true;
    bool ok = true;
    for (unsigned int tokensAcquired = 0; tokensAcquired < delta; ++tokensAcquired)
    {
        unsigned int rc = WaitForSingleObjectEx(sem_, waitTimeInMsecs, alertable);
        if (rc != WAIT_OBJECT_0)
        {
            if ((rc == WAIT_IO_COMPLETION) && alertable && (!Thread::isTerminating()))
            {
                --tokensAcquired; //the loop increment will neutralize this
            }
            else
            {
                if (tokensAcquired > 0)
                {
                    ReleaseSemaphore(sem_, tokensAcquired, 0 /*lpPreviousCount*/);
                }
                ok = false;
                break;
            }
        }
    }

    // Return true if successful.
    return ok;
}


//!
//! Acquire a token. Wait forever if necessary.
//! Return true if successful.
//!
bool Semaphore::decrement()
{
    unsigned int rc;
    do
    {
        rc = WaitForSingleObjectEx(sem_, ETERNITY, 1 /*bAlertable*/);
        if (rc == WAIT_OBJECT_0)
        {
            bool ok = true;
            return ok;
        }
    } while ((rc == WAIT_IO_COMPLETION) && (!Thread::isTerminating()));

    bool ok = false;
    return ok;
}


//!
//! Acquire a token. Wait at most waitTimeInMsecs msecs.
//! Return true if successful.
//!
bool Semaphore::decrement(unsigned int waitTimeInMsecs)
{

    // Simple case. Non-blocking.
    if (waitTimeInMsecs == 0)
    {
        return tryDecrement();
    }

    // Simple case. Blocking.
    if (waitTimeInMsecs == ETERNITY)
    {
        return decrement();
    }

    bool ok;
    bool alertable = true;
    for (unsigned int startTimeInMsecs = GetTickCount(), waitTime = waitTimeInMsecs;;)
    {
        unsigned int rc = WaitForSingleObjectEx(sem_, waitTime, alertable);
        if (rc == WAIT_OBJECT_0)
        {
            ok = true;
            break;
        }
        if ((rc != WAIT_IO_COMPLETION) || (!alertable) || Thread::isTerminating())
        {
            ok = false;
            break;
        }
        unsigned int delta = GetTickCount() - startTimeInMsecs;
        waitTime = (delta >= waitTimeInMsecs)? ((alertable = false), 0): (waitTimeInMsecs - delta);
    }

    return ok;
}


//!
//! Acquire delta tokens. delta must be non-zero and must not be larger than
//! MAX_CAP. Wait at most waitTimeInMsecs msecs. Return true if successful.
//!
bool Semaphore::decrementBy(unsigned int delta, unsigned int waitTimeInMsecs)
{

    // Simple case. Non-blocking.
    if (waitTimeInMsecs == 0)
    {
        return tryAcquireTokens(delta);
    }

    // Simple case. Blocking.
    if (waitTimeInMsecs == ETERNITY)
    {
        return acquireTokens(delta);
    }

    // Sanity check.
    if ((delta == 0) || (delta > MAX_CAP))
    {
        return false;
    }

    // Acquire all tokens, one at a time. Adjust timeout appropriately.
    // Undo if failure occurs.
    bool alertable = true;
    unsigned int tokensAcquired = 0;
    for (unsigned int startTimeInMsecs = GetTickCount(), waitTime = waitTimeInMsecs;;)
    {
        unsigned int rc = WaitForSingleObjectEx(sem_, waitTime, alertable);
        if (rc != WAIT_OBJECT_0)
        {
            if ((rc != WAIT_IO_COMPLETION) || (!alertable) || Thread::isTerminating())
            {
                if (tokensAcquired > 0)
                {
                    ReleaseSemaphore(sem_, tokensAcquired, 0 /*lpPreviousCount*/);
                }
                return false;
            }
        }
        else if (++tokensAcquired == delta)
        {
            return true;
        }
        else if (waitTime > 0)
        {
            unsigned int delta = GetTickCount() - startTimeInMsecs;
            waitTime = (delta >= waitTimeInMsecs)? ((alertable = false), 0): (waitTimeInMsecs - delta);
        }
        else
        {
            alertable = false;
        }
    }
}


//!
//! Release a token. Return true if successful.
//!
bool Semaphore::increment()
{
    bool ok = (ReleaseSemaphore(sem_, 1 /*lReleaseCount*/, 0 /*lpPreviousCount*/) != 0);
    return ok;
}


//!
//! Release delta tokens. delta must be non-zero and must not be more
//! than the number of acquired tokens. Return true if successful.
//!
bool Semaphore::incrementBy(unsigned int delta)
{
    bool ok = (ReleaseSemaphore(sem_, delta, 0 /*lpPreviousCount*/) != 0);
    return ok;
}


//!
//! Acquire a token. Do not wait.
//! Return true if successful.
//!
bool Semaphore::tryDecrement()
{
    bool ok = (WaitForSingleObjectEx(sem_, 0 /*dwMilliseconds*/, 0 /*bAlertable*/) == WAIT_OBJECT_0);
    return ok;
}


//
// Acquire delta tokens. Do not wait.
//
bool Semaphore::tryAcquireTokens(unsigned int delta)
{

    // Sanity check.
    if ((delta == 0) || (delta > MAX_CAP))
    {
        return false;
    }

    // Acquire all tokens. Undo and return immediately if failure occurs.
    unsigned int waitTimeInMsecs = 0;
    bool alertable = false;
    bool ok = true;
    for (unsigned int tokensAcquired = 0; tokensAcquired < delta; ++tokensAcquired)
    {
        unsigned int rc = WaitForSingleObjectEx(sem_, waitTimeInMsecs, alertable);
        if (rc != WAIT_OBJECT_0)
        {
            if ((rc == WAIT_IO_COMPLETION) && alertable && (!Thread::isTerminating()))
            {
                --tokensAcquired; //the loop increment will neutralize this
            }
            else
            {
                if (tokensAcquired > 0)
                {
                    ReleaseSemaphore(sem_, tokensAcquired, 0 /*lpPreviousCount*/);
                }
                ok = false;
                break;
            }
        }
    }

    // Return true if successful.
    return ok;
}


//!
//! Wait until this pool is full (i.e., at least numTokens tokens are
//! available). Wait forever if necessary. Return true if successful.
//!
bool Semaphore::waitTilFull(unsigned int numTokens)
{
    bool ok = acquireTokens(numTokens)?
        (ReleaseSemaphore(sem_, numTokens, 0 /*lpPreviousCount*/), true):
        (false);
    return ok;
}


//!
//! Wait until this pool is non-empty (i.e., at least one token is
//! available). Wait forever if necessary. Return true if successful.
//!
bool Semaphore::waitTilNonEmpty()
{
    unsigned int rc;
    do
    {
        rc = WaitForSingleObjectEx(sem_, ETERNITY, 1 /*bAlertable*/);
        if (rc == WAIT_OBJECT_0)
        {
            ReleaseSemaphore(sem_, 1 /*lReleaseCount*/, 0 /*lpPreviousCount*/);
            bool ok = true;
            return ok;
        }
    } while ((rc == WAIT_IO_COMPLETION) && (!Thread::isTerminating()));

    bool ok = false;
    return ok;
}


//!
//! Detach from the system resource and allow it to persist. That is, after
//! detaching, the Semaphore instance is no longer valid, but the system
//! resource persits even after the Semaphore instance is destructed.
//!
void Semaphore::detach()
{
    if (sem_ != 0)
    {
        if (semIsMine_)
        {
            Janitor& janitor = Janitor::instance();
            janitor.closeAtExit(sem_);
        }
        sem_ = 0;
        semIsMine_ = true;
    }
}


//!
//! Reset semaphore value to given count.
//!
void Semaphore::reset(unsigned int count)
{

    // There's no official Win32 API to reset a semaphore value. This implementation
    // uses non-blocking incrementing and decrementing to get to the desired value. --ttp

    // Special case of resetting to zero. Just decrement to
    // zero in a non-blocking manner and return immediately.
    if (count == 0)
    {
        while (WaitForSingleObjectEx(sem_, 0 /*dwMilliseconds*/, 0 /*bAlertable*/) == WAIT_OBJECT_0);
        return;
    }

    // Restrict the semaphore value.
    if (count > MAX_CAP)
    {
        count = MAX_CAP;
    }

    // Assume current count is zero. Reset the value to given count by
    // incrementing by that amount. If the assumption turns out to be
    // incorrect, make the necessary adjustment.
    long old;
    if (ReleaseSemaphore(sem_, count, &old))
    {
        for (; old > 0; --old)
        {
            WaitForSingleObjectEx(sem_, 0 /*dwMilliseconds*/, 0 /*bAlertable*/);
        }
    }

    // If the increment fail for any reason, try decrementing to zero and
    // try one more time. This can happen if the current value is non-zero
    // and we're setting the new value to MAX_CAP.
    else
    {
        while (WaitForSingleObjectEx(sem_, 0 /*dwMilliseconds*/, 0 /*bAlertable*/) == WAIT_OBJECT_0);
        ReleaseSemaphore(sem_, count, 0 /*lpPreviousCount*/);
    }
}

END_NAMESPACE1
