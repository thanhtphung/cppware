/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/Semaphore.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)

const unsigned int Semaphore::ETERNITY = 0xffffffffU;
const unsigned int Semaphore::MAX_CAP = 32767;


//!
//! Construct a semaphore with the given system resource. The given system
//! resource belongs to the user and must be valid throughout this instance
//! life.
//!
Semaphore::Semaphore(sem_t sem)
{
    sem_ = sem;
    semIsMine_ = false;
}


//!
//! Construct a lock for given semaphore. Wait forever if necessary. A
//! token is acquired after the lock construction and will be released
//! when the lock is destructed. Use isOk() to verify proper instantiation
//! (i.e., to verify if a token was successfully acquired).
//!
Semaphore::Lock::Lock(Semaphore& semaphore)
{
    numTokens_ = 1;
    semaphore_ = semaphore.decrement()? &semaphore: 0;
}


//!
//! Construct a lock for given semaphore. Wait forever if necessary.
//! numTokens tokens are acquired after the lock construction and
//! will be released when the lock is destructed. Use isOk() to verify
//! proper instantiation (i.e., to verify if tokens were successfully
//! acquired). Locking zero tokens is considered a failure.
//!
Semaphore::Lock::Lock(Semaphore& semaphore, unsigned int numTokens)
{
    numTokens_ = numTokens;
    semaphore_ = 0;
    if (numTokens > 0)
    {
        bool ok = (numTokens == 1)? semaphore.decrement(): semaphore.decrementBy(numTokens);
        if (ok)
        {
            semaphore_ = &semaphore;
        }
    }
}


//!
//! Destruct the lock.
//!
Semaphore::Lock::~Lock()
{
    if (semaphore_ != 0)
    {
        if (numTokens_ == 1)
        {
            semaphore_->increment();
        }
        else
        {
            semaphore_->incrementBy(numTokens_);
        }
    }
}

END_NAMESPACE1
