/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/Mutex.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)

const unsigned int Mutex::ETERNITY = 0xffffffffU;


//!
//! Construct a lock for given mutex. Wait forever if necessary.
//! Given mutex is locked after the lock construction and remains
//! locked until the lock is destructed.
//!
Mutex::Lock::Lock(Mutex& mutex):
mutex_(mutex)
{
    mutex_.lock();
}


Mutex::Lock::~Lock()
{
    mutex_.unlock();
}

END_NAMESPACE1
