/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/CondVar.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)

const unsigned int CondVar::ETERNITY = 0xffffffffU;


//!
//! Construct a lock for the associated mutex. Wait forever if necessary.
//! Associated mutex is locked after the lock construction and remains
//! locked until the lock is destructed. Use isOk() to verify proper
//! instantiation (i.e., to verify if associated mutex was successfully
//! locked).
//!
CondVar::MutexLock::MutexLock(CondVar& condVar)
{
    cv_ = condVar.lockMutex()? &condVar: 0;
}


//!
//! Destruct the associated mutex lock.
//!
CondVar::MutexLock::~MutexLock()
{
    if (cv_ != 0)
    {
        cv_->unlockMutex();
    }
}

END_NAMESPACE1
