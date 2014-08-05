/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>

#include "syskit-pch.h"
#include "syskit/CallStack.hpp"
#include "syskit/Process.hpp"
#include "syskit/Thread.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE


//
// This is invoked by the terminating thread when it's being killed via kill().
//
void __stdcall apc(ULONG_PTR /*arg*/)
{
}


//
// All threads created by Thread use this entry point.
//
void* __stdcall start(void* arg)
{

    // Utilize entry point and argument specified by creating user.
    bool crashed;
    void* rc;
    syskit::Thread::Entry* entry = static_cast<syskit::Thread::Entry*>(arg);
    __try
    {
        rc = entry->enter();
        crashed = false;
    }
    __except (syskit::CallStack::filter(GetExceptionInformation(), GetExceptionCode(), syskit::Thread::dumpPath()))
    {
        rc = static_cast<unsigned char*>(0) - 1;
        crashed = true;
    }

    // Notify monitor of crash.
    if (crashed)
    {
        void* arg;
        syskit::Thread::crashCb_t crashCb = syskit::Thread::crashCb(arg);
        crashCb(arg);
    }

    // Return the thread exit code.
    delete entry;
    return rc;
}

END_NAMESPACE

BEGIN_NAMESPACE1(syskit)


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
Thread::Thread(entrance_t entrance, void* arg, unsigned int stackSizeInBytes, bool startSuspended)
{
    state_ = new State();
    Entry* entry = new Entry(entrance, arg, state_);
    unsigned int initflag = startSuspended? CREATE_SUSPENDED: 0;
    unsigned int id;
    uintptr_t rc = _beginthreadex(0,
        stackSizeInBytes,
        reinterpret_cast<unsigned int(__stdcall*)(void*)>(start),
        entry,
        initflag,
        &id);

    if (rc == 0)
    {
        delete entry;
        id_ = INVALID_ID;
        thread_ = 0;
    }

    else
    {
        id_ = id;
        thread_ = reinterpret_cast<HANDLE>(rc);
    }
}


//!
//! Return true if successful.
//!
bool Thread::resume()
{
    bool ok = (ResumeThread(thread_) != 0xffffffffUL);
    return ok;
}


//!
//! Set the affinity mask for the thread.
//! Return true if successful.
//!
bool Thread::setAffinityMask(size_t affinityMask)
{
    return SetThreadAffinityMask(thread_, affinityMask) != 0;
}


//!
//! Return true if successful.
//!
bool Thread::suspend()
{
    bool ok = (SuspendThread(thread_) != 0xffffffffUL);
    return ok;
}


//!
//! Suspend calling thread's execution for at least napTimeInMsecs msecs
//! unless interrupted. Return true if the nap completes successfully.
//! Return false otherwise (the nap was interrupted).
//!
bool Thread::takeANap(unsigned int napTimeInMsecs, bool beAlert)
{
    BOOL alertable = beAlert? 1: 0;
    return SleepEx(napTimeInMsecs, alertable) == 0;
}


//!
//! Wait until this thread is done. If given exitCode is non-zero, save
//! the thread's exit code there upon return. Return true if successful.
//! Return true and use exit code zero if this thread is caller's thread.
//!
bool Thread::waitTilDone(void** exitCode)
{
    bool ok = false;
    if (thread_)
    {
        if (id_ == Thread::myId())
        {
            ok = true;
            if (exitCode != 0)
            {
                *exitCode = 0;
            }
        }
        else
        {
            unsigned int rc;
            do
            {
                rc = WaitForSingleObjectEx(thread_, ETERNITY, 1 /*bAlertable*/);
                if (rc == WAIT_OBJECT_0)
                {
                    ok = true;
                    if (exitCode != 0)
                    {
                        DWORD rc = 0;
                        GetExitCodeThread(thread_, &rc);
                        *exitCode = reinterpret_cast<void*>(rc);
                    }
                    break;
                }
            } while ((rc == WAIT_IO_COMPLETION) && (!isTerminating()));
        }
        CloseHandle(thread_);
        thread_ = 0;
    }

    return ok;
}


//!
//! Return the affinity mask for the thread.
//!
size_t Thread::affinityMask() const
{

    // There's no official API to retrieve this mask. As a work-around, this code
    // retrieves the mask by first modifying it to be the same as the process affinity
    // mask, and if necessary, restoring the current mask. The side effect of the
    // work-around is minimal, if any. This seems like a better approach than using
    // the kernel-internal NtQueryInformationThread() function from the DDK.
    size_t processAffinityMask = Process::affinityMask();
    size_t affinityMask = SetThreadAffinityMask(thread_, processAffinityMask);
    if (affinityMask != processAffinityMask)
    {
        SetThreadAffinityMask(thread_, affinityMask);
    }

    return affinityMask;
}


//!
//! Return the thread identifier of the caller.
//!
size_t Thread::myId()
{
    return GetCurrentThreadId();
}


//!
//! Detach from created thread. Allow the thread to persist even if this Thread
//! instance is destructed.
//!
void Thread::detach()
{
    if (thread_ != 0)
    {
        CloseHandle(thread_);
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
    QueueUserAPC(apc, thread_, 0);
}


//!
//! Give up calling thread's time slice.
//!
void Thread::yield()
{
    SleepEx(0 /*dwMilliseconds*/, 0 /*bAlertable*/);
}

END_NAMESPACE1
