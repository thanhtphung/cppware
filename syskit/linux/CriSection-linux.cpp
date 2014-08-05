/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <pthread.h>
#include "syskit/CriSection.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


CriSection::CriSection()
{
    cs_ = new pthread_mutex_t(PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP);
}


CriSection::~CriSection()
{
    pthread_mutex_destroy(cs_);
    delete cs_;
}


//!
//! Lock this critical section. Do not wait.
//! Return true if successful.
//!
bool CriSection::tryLock()
{
    return pthread_mutex_trylock(cs_) == 0;
}


//!
//! Lock this critical section. Wait forever if necessary.
//!
void CriSection::lock()
{
    pthread_mutex_lock(cs_);
}


//!
//! Unlock this critical section.
//!
void CriSection::unlock()
{
    pthread_mutex_unlock(cs_);
}

END_NAMESPACE1
