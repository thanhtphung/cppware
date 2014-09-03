/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <signal.h>
#include "syskit/CallStack.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/Bool.hpp"
#include "appkit/CmdLine.hpp"
#include "appkit/Demon.hpp"
#include "appkit/String.hpp"

using namespace syskit;

BEGIN_NAMESPACE1(appkit)


Demon::Demon(const CmdLine& cmdLine,
const String& name,
bool startInExeDir,
bool usesNet,
const wchar_t* confDir,
const wchar_t* logDir):
App(cmdLine, startInExeDir, confDir, logDir)
{
    name_ = new String(name);
    nameW_ = name_->widen().detachRaw();
    restartOnExit_ = false;
    runningAsProcess_ = false;
    toBeRunAsProcess_ = false;
    usesNet_ = usesNet;
}


Demon::~Demon()
{
    if (restartOnExit_)
    {
        startSvc();
    }

    delete[] nameW_;
    delete name_;
}


//!
//! Return the singleton.
//!
Demon* Demon::instance()
{
    return dynamic_cast<Demon*>(App::instance());
}


//!
//! Stop demon. Also run demon again if restart is set. Return true if successful.
//! Return false otherwise (restarting a process is unsupported).
//!
bool Demon::stop(bool restart)
{
    bool ok;
    if (runningAsProcess_)
    {
        ok = restart? false: (raise(SIGTERM) == 0);
    }
    else
    {
        ok = stopSvc();
        restartOnExit_ = restart;
    }

    return ok;
}


//!
//! Return the demon usage.
//!
const char* Demon::usage() const
{
    const char* s = App::usage();
    return s;
}


//!
//! Return the demon extended usage.
//!
const char* Demon::xUsage() const
{
    const char* s = App::xUsage();
    return s;
}


//!
//! This method is invoked when the demon is to start running as a process.
//! The method is expected to return an exit code. The default implementation
//! is a no-op returning Ok. The Fatal exit code is used by App to indicate a
//! crash, and it should not be used for other purposes. Similary, the following
//! exit codes are used by Demon: AddSvcFailed, RmSvcFailed, StartFailed,
//! StartSvcFailed, and StopSvcFailed.
//!
int Demon::onRun()
{
    int exitCode = App::onRun();
    return exitCode;
}


//!
//! Run demon. Command-line options can be used to control how the demon
//! is run. By default, a demon runs as a service on windows. To run
//! as a process, specify the "process" option. The demon service can
//! also be installed or removed using the "install" or "remove" option.
//!
int Demon::run()
{
    int rc;
    __try
    {
        rc = showUsage()? Ok: walk();
    }
    __except (CallStack::filter(GetExceptionInformation(), GetExceptionCode(), dumpPath()))
    {
        rc = Fatal;
    }

    setExitCode(rc);
    return rc;
}


//
// Might need to walk a little before running.
//
int Demon::walk()
{

    // Install service?
    const CmdLine& cmdLine = App::cmdLine();
    String optK("install");
    Bool v(cmdLine.opt(optK), false /*defaultV*/);
    if (v)
    {
        const String* defaultOptV = 0;
        optK = "runarg";
        const String* runArg = cmdLine.opt(optK, defaultOptV);
        int rc = addSvc(runArg)? Ok: AddSvcFailed;
        setExitCode(rc);
        return rc;
    }

    // Remove service?
    // Start service?
    // Stop service?
    const char* const k[3] = {"remove", "start", "stop"};
    controlSvc_t const control[3] = {&Demon::rmSvc, &Demon::startSvc, &Demon::stopSvc};
    int const failureRc[3] = {RmSvcFailed, StartSvcFailed, StopSvcFailed};
    for (int i = 0; i < 3; ++i)
    {
        optK = k[i];
        v = Bool(cmdLine.opt(optK), false /*defaultV*/);
        if (v)
        {
            int rc = (this->*control[i])()? Ok: failureRc[i];
            setExitCode(rc);
            return rc;
        }
    }

    // Run as process?
    optK = "process";
    bool defaultV = toBeRunAsProcess_;
    v = Bool(cmdLine.opt(optK), defaultV);
    runningAsProcess_ = v;
    int rc = runningAsProcess_? onRun(): start();
    setExitCode(rc);
    return rc;
}


//!
//! This method is invoked when the demon is to start running as a service.
//! The default implementation is a no-op.
//!
void Demon::onStart()
{
}


//!
//! This method is invoked when the demon service is asked to stop.
//! The default implementation is a no-op.
//!
void Demon::onStop()
{
}


//!
//! This method is invoked when system shutdown is observed.
//! The default implementation is a no-op.
//!
void Demon::onSysShutdown()
{
}

END_NAMESPACE1
