/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <pthread.h>
#include "syskit/Mutex.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


Mutex::Mutex():
mu_(PTHREAD_MUTEX_INITIALIZER)
{
}


Mutex::~Mutex()
{
    pthread_mutex_destroy(&mu_);
}


//!
//! Return true if instance was successfully constructed.
//!
bool Mutex::isOk() const
{
    return true;
}


//!
//! Lock this mutex. Wait forever if necessary. Return true if successful.
//!
bool Mutex::lock()
{
    return pthread_mutex_lock(&mu_) == 0;
}


//!
//! Lock this mutex. Do not wait. Return true if successful.
//!
bool Mutex::tryLock()
{
    return pthread_mutex_trylock(&mu_) == 0;
}


//!
//! Unlock this mutex. Return true if successful. Unlocking an unlocked mutex
//! is allowed and is considered successful.
//!
bool Mutex::unlock()
{
    return pthread_mutex_unlock(&mu_) == 0;
}

END_NAMESPACE1
