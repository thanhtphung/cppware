/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <time.h>
#include "syskit/linux/CondVar-linux.hpp"
#include "syskit/macros.h"

const int NSECS_PER_SEC = 1000000000;
const unsigned int MSECS_PER_SEC = 1000U;
const unsigned int NSECS_PER_MSEC = 1000000U;

BEGIN_NAMESPACE1(syskit)


//!
//! Construct a condition variable and its associated mutex.
//!
CondVar::CondVar():
cv_(PTHREAD_COND_INITIALIZER),
mu_(PTHREAD_MUTEX_INITIALIZER)
{
}


//!
//! Destruct the condition variable and its associated mutex.
//!
CondVar::~CondVar()
{
    pthread_mutex_destroy(&mu_);
    pthread_cond_destroy(&cv_);
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
    int rc;
    if (timeoutInMsecs == ETERNITY)
    {
        rc = pthread_cond_wait(&cv_, &mu_);
    }

    else
    {
        struct timespec now;
        clock_gettime(CLOCK_REALTIME, &now);
        struct timespec then;
        then.tv_sec = now.tv_sec + (timeoutInMsecs / MSECS_PER_SEC);
        then.tv_nsec = now.tv_nsec + ((timeoutInMsecs%MSECS_PER_SEC) * NSECS_PER_MSEC);
        if (then.tv_nsec >= NSECS_PER_SEC)
        {
            ++then.tv_sec;
            then.tv_nsec -= NSECS_PER_SEC;
        }
        rc = pthread_cond_timedwait(&cv_, &mu_, &then);
    }

    return (rc == 0);
}

END_NAMESPACE1
