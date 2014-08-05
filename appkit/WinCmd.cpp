/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/Semaphore.hpp"
#include "syskit/SpinSection.hpp"
#include "syskit/Thread.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/WinApp.hpp"
#include "appkit/WinCmd.hpp"

using namespace syskit;

BEGIN_NAMESPACE1(appkit)


WinCmd::WinCmd(const WinApp* winApp, const char* nameSet):
Cmd(nameSet),
RefCounted(1U)
{
    cmdIsValid_ = true;
    messagingId_ = winApp->messagingId();
    messagingWin_ = winApp->messagingWin();
    winApp_ = winApp;

    // Serialize individual commands at onRun().
    // Serialize all commands at onFinalRun().
    unsigned int n = numNames();
    sem_ = new Semaphore[n];
    ss_ = new SpinSection[n + 1];
}


WinCmd::~WinCmd()
{
    delete[] ss_;
    delete[] sem_;
}


//!
//! A critical section is used to serialize commands at onFinalRun(). A derived
//! class can use the critical section for the same purpose. This method returns
//! that critical section.
//!
SpinSection& WinCmd::ss() const
{
    size_t i = numNames();
    return ss_[i];
}


//!
//! Run command using dedicated agent threads provided by the Cmd framework. Return
//! true if command is valid. A false return results in some command usage hints as
//! response to the request.
//!
bool WinCmd::onRun(const CmdLine& req)
{

    // Prevent the instance from going out of scope during a run.
    Count countyLock(*this);

    // This command needs to be run by the WinApp messaging thread.
    if (Thread::myId() != winApp_->messagingThreadId())
    {

        // Serialize individual commands at onRun().
        size_t i = cmdIndex(req);
        SpinSection::Lock lock(ss_[i]);
        WPARAM wparam = reinterpret_cast<WPARAM>(this);
        LPARAM lparam = reinterpret_cast<LPARAM>(&req);
        int ok = PostMessageW(messagingWin_, messagingId_, wparam, lparam);
        if (ok)
        {
            sem_[i].decrement(); //wait until command is run by the WinApp messaging thread, forever if necessary
            return cmdIsValid_;
        }
    }

    // Serialize all commands at onFinalRun(). Commands are typically already serialized
    // via the PostMessageW() usage. However, sprint() does allow bypassing the windows
    // messaging in some scenarios.
    size_t i = numNames();
    SpinSection::Lock lock(ss_[i]);
    bool cmdIsValid = onFinalRun(req);
    return cmdIsValid;
}


//!
//! A command is typically run using a few components in the framework including
//! CmdAgent (command client) and SysIo (command server). That is sometimes not
//! feasible. This method offers a way to run (i.e., sprint) commands w/o those
//! components. With this method, the onRun() method is invoked by the caller thread
//! (vs. a dedicated thread), and there's no client/server communications. Return
//! true if successful.
//!
bool WinCmd::sprint(const CmdLine& req, String& rsp)
{

    // Prevent the instance from going out of scope during a sprint.
    Count lock(*this);
    bool ok = Cmd::sprint(req, rsp);
    return ok;
}

END_NAMESPACE1
