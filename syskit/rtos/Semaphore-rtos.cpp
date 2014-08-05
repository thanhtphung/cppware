/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include "syskit/Semaphore.hpp"
#include "syskit/sys.hpp"

const int INVALID_ID = -1;
const unsigned int MSECS_PER_SEC = 1000U;
const unsigned int NSECS_PER_MSEC = 1000000U;
const portTickType TICKS_PER_MSEC = configTICK_RATE_HZ / MSECS_PER_SEC;

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
    unsigned int initialCap = (capacity > MAX_CAP)? MAX_CAP: capacity;
    sem_ = xSemaphoreCreateCounting(capacity, initialCap);
    semIsMine_ = true;
}


Semaphore::~Semaphore()
{
    if (semIsMine_ && (sem_ != 0))
    {
        vSemaphoreDelete(sem_);
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
    portTickType ticksToWait = portMAX_DELAY;
    bool ok = true;
    for (unsigned int tokensAcquired = 0; tokensAcquired < delta; ++tokensAcquired)
    {
        if (xSemaphoreTake(sem_, ticksToWait) == 0)
        {
            if (tokensAcquired > 0)
            {
                incrementBy(tokensAcquired);
            }
            ok = false;
            break;
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
    portTickType ticksToWait = portMAX_DELAY;
    bool ok = (xSemaphoreTake(sem_, ticksToWait) != 0);
    return ok;
}


//!
//! Acquire a token. Wait at most waitTimeInMsecs msecs.
//! Return true if successful.
//!
bool Semaphore::decrement(unsigned int waitTimeInMsecs)
{
    portTickType ticksToWait = waitTimeInMsecs * TICKS_PER_MSEC;
    bool ok = (xSemaphoreTake(sem_, ticksToWait) != 0);
    return ok;
}


//!
//! Release a token. Return true if successful.
//!
bool Semaphore::increment()
{
    bool ok = (xSemaphoreGiveFromISR(sem_, 0 /*pxHigherPriorityTaskWoken*/) != 0);
    return ok;
}


//!
//! Release delta tokens. delta must be non-zero and must not be more
//! than the number of acquired tokens. Return true if successful.
//!
bool Semaphore::incrementBy(unsigned int delta)
{
    bool ok = true;
    for (; delta > 0; --delta)
    {
        if (xSemaphoreGiveFromISR(sem_, 0 /*pxHigherPriorityTaskWoken*/) == 0)
        {
            ok = false;
            break;
        }
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
    portTickType ticksToWait = waitTimeInMsecs * TICKS_PER_MSEC;
    unsigned int tokensAcquired = 0;
    for (unsigned int startTimeInTicks = xTaskGetTickCount(), waitTimeInTicks = ticksToWait;;)
    {
        if (xSemaphoreTake(sem_, ticksToWait) == 0)
        {
            if (tokensAcquired > 0)
            {
                incrementBy(tokensAcquired);
            }
            return false;
        }
        else if (++tokensAcquired == delta)
        {
            return true;
        }
        else if (ticksToWait > 0)
        {
            unsigned int delta = xTaskGetTickCount() - startTimeInTicks;
            ticksToWait = (delta >= waitTimeInTicks)? (0): (waitTimeInTicks - delta);
        }
    }
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
    bool ok = true;
    for (unsigned int tokensAcquired = 0; tokensAcquired < delta; ++tokensAcquired)
    {
        if (xSemaphoreTakeFromISR(sem_, 0 /*pxHigherPriorityTaskWoken*/) == 0)
        {
            if (tokensAcquired > 0)
            {
                incrementBy(tokensAcquired);
            }
            ok = false;
            break;
        }
    }

    // Return true if successful.
    return ok;
}


//!
//! Acquire a token. Do not wait.
//! Return true if successful.
//!
bool Semaphore::tryDecrement()
{
    bool ok = (xSemaphoreTakeFromISR(sem_, 0 /*pxHigherPriorityTaskWoken*/) != 0);
    return ok;
}


//!
//! Detach from the system resource and allow it to persist. That is, after
//! detaching, the Semaphore instance is no longer valid, but the system
//! resource persits even after the Semaphore instance is destructed.
//!
void Semaphore::detach()
{
    sem_ = 0;
    semIsMine_ = true;
}


//!
//! Reset semaphore value to given count.
//!
void Semaphore::reset(unsigned int count)
{
    // There's no official RTOS API to reset a semaphore value. This implementation
    // uses non-blocking incrementing and decrementing to get to the desired value. --ttp

    // Special case of resetting to zero. Just decrement to
    // zero in a non-blocking manner and return immediately.
    while (xSemaphoreTakeFromISR(sem_, 0 /*pxHigherPriorityTaskWoken*/));
    if (count == 0)
    {
        return;
    }

    // Restrict the semaphore value.
    if (count > MAX_CAP)
    {
        count = MAX_CAP;
    }

    for (; count > 0; --count)
    {
        xSemaphoreGiveFromISR(sem_, 0 /*pxHigherPriorityTaskWoken*/);
    }
}

END_NAMESPACE1
