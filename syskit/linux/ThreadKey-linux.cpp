/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/ThreadKey.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)

const threadKey_t ThreadKey::INVALID_KEY = 0;


ThreadKey::ThreadKey()
{
    if (pthread_key_create(&key_, 0) != 0)
    {
        key_ = INVALID_KEY;
    }
}


ThreadKey::~ThreadKey()
{
    if (key_ != INVALID_KEY)
    {
        pthread_key_delete(key_);
    }
}


//!
//! Associate given value to this key for the calling thread.
//!
void ThreadKey::setValue(void* v)
{
    pthread_setspecific(key_, v);
}


//!
//! Return the associated value for the calling thread.
//! Return zero if key is unassociated.
//!
void* ThreadKey::value() const
{
    void* v = pthread_getspecific(key_);
    return v;
}

END_NAMESPACE1
