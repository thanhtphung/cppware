/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include "syskit/Thread.hpp"
#include "syskit/macros.h"

#if __CYGWIN__
#define PTHREAD_ATTR0 {0}
#else
#define PTHREAD_ATTR0 {{0}}
#endif

BEGIN_NAMESPACE1(syskit)

//
// All threads created by Thread use this entry point.
//
static void* start(void* arg)
{

    // Utilize entry point and argument specified by creating user.
    bool crashed;
    void* rc;
    Thread::Entry* entry = static_cast<Thread::Entry*>(arg);
    try
    {
        rc = entry->enter();
        crashed = false;
    }
    catch (...)
    {
        rc = static_cast<unsigned char*>(0) - 1;
        crashed = true;
    }

    // Notify monitor of crash.
    if (crashed)
    {
        void* arg;
        Thread::crashCb_t crashCb = Thread::crashCb(arg);
        crashCb(arg);
    }

    // Return the thread exit code.
    delete entry;
    return rc;
}


//!
//! Create and start a thread with given characteristics. That is, the
//! new thread has stack size of stackSizeInBytes bytes. Its entry point
//! is entrance. When the entry point is entered, the argument arg is
//! used as an opaque parameter. Start thread in suspended (vs. running)
//! mode if startSuspended is true. Suspended threads remain idle until
//! resumed via resume(). The construction can fail due to lack of system
//! resources. Use isOk() to determine if the thread construction is
//! successful.
//!
Thread::Thread(entrance_t entrance, void* arg, unsigned int stackSizeInBytes, bool /*startSuspended*/)
{
    bool ok;
    pthread_attr_t attr = PTHREAD_ATTR0;
    pthread_attr_t* attrP = 0;
    if (stackSizeInBytes)
    {
        attrP = &attr;
        pthread_attr_init(attrP);
        ok = (pthread_attr_setstacksize(attrP, stackSizeInBytes) == 0);
    }
    else
    {
        ok = true;
    }

    state_ = new State();
    if (ok)
    {
        Entry* entry = new Entry(entrance, arg, state_);
        if (pthread_create(&id_, attrP, start, entry) == 0)
        {
            thread_ = this;
            return;
        }
        delete entry;
    }

    id_ = INVALID_ID;
    thread_ = 0;
}


//!
//! Suspend calling thread's execution for at least napTimeInMsecs msecs
//! unless interrupted. Return true if the nap completes successfully.
//! Return false otherwise (the nap was interrupted).
//!
bool Thread::takeANap(unsigned int napTimeInMsecs, bool /*beAlert*/)
{
    struct timespec napTime;
    const unsigned int MSECS_PER_SEC = 1000U;
    const unsigned int NANOSECS_PER_MSEC = 1000000U;
    napTime.tv_sec = napTimeInMsecs / MSECS_PER_SEC;
    napTime.tv_nsec = (napTimeInMsecs % MSECS_PER_SEC) * NANOSECS_PER_MSEC;
    bool ok = (nanosleep(&napTime, 0) == 0);
    return ok;
}


//!
//! Wait until this thread is done. If given exitCode is non-zero, save
//! the thread's exit code there upon return. Return true if successful.
//! Return true and use exit code zero if this thread is caller's thread.
//!
bool Thread::waitTilDone(void** exitCode)
{
    bool ok = false;
    if (id_ != INVALID_ID)
    {
        void* rc = 0;
        if (pthread_join(id_, &rc) == 0)
        {
            ok = true;
            if (exitCode != 0)
            {
                *exitCode = rc;
            }
        }
        id_ = INVALID_ID;
        thread_ = 0;
    }

    return ok;
}


//!
//! Return the thread identifier of the caller.
//!
size_t Thread::myId()
{
    return (size_t)(pthread_self());
}


//!
//! Detach from created thread. Allow the thread to persist even if this Thread
//! instance is destructed.
//!
void Thread::detach()
{
    if (id_ != INVALID_ID)
    {
        pthread_detach(id_);
        id_ = INVALID_ID;
        thread_ = 0;
    }
}


//!
//! Kill this thread using an interrupt. If this thread is running in an alertable
//! mode, it will be interrupted (i.e., system call is interrupted and the call
//! returns with the WAIT_IO_COMPLETION error).
//!
void Thread::kill()
{
    for (; state_->state() == Thread::Idle; Thread::yield());
    state_->setState(Terminating);
    pthread_kill(id_, SIGUSR1);
}


//!
//! Give up calling thread's time slice.
//!
void Thread::yield()
{
    sched_yield();
}

END_NAMESPACE1
