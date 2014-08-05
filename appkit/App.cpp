/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include "syskit/CallStack.hpp"
#include "syskit/Foundation.hpp"
#include "syskit/Process.hpp"
#include "syskit/Thread.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/App.hpp"
#include "appkit/Bool.hpp"
#include "appkit/CmdLine.hpp"
#include "appkit/Directory.hpp"
#include "appkit/Path.hpp"
#include "appkit/Uword.hpp"
#include "appkit/crt.hpp"

using namespace syskit;

const wchar_t COPYRIGHT[] = L"\xa9 2012 Company. All rights reserved.";

BEGIN_NAMESPACE1(appkit)

App* App::instance_ = 0;
const char App::ID1[] = "appkit::App"; //singleton ID
const wchar_t App::DEFAULT_CONF_DIR[] = L"../Conf/"; //relative to executable path
const wchar_t App::DEFAULT_LOG_DIR[] = L"../Logs/"; //relative to executable path
unsigned int App::initialRefCount_ = 1U;


App::App(const CmdLine& cmdLine, bool startInExeDir, const wchar_t* confDir, const wchar_t* logDir):
RefCounted(initialRefCount_),
Singleton(ID1, initialRefCount_)
{
    cmdLine_ = new CmdLine(cmdLine);
    myBuildDate_ = buildDate_;
    myBuildTime_ = buildTime_;
    oldDir_ = 0;
    parseCmdLine(confDir, logDir); //confDir_, dumpPath_, logDir_, path_, usageOpt_
    setCurrentDir(startInExeDir, *path_);

    exitCode_ = Ok;
    instance_ = this;
}


App::~App()
{
    instance_ = 0;

    // Restore current directory if it was changed at construction.
    if (oldDir_ != 0)
    {
        Directory::setCurrent(*oldDir_);
        delete oldDir_;
    }

    delete[] dumpPath_;
    delete logDir_;
    delete confDir_;
    delete path_;

    delete cmdLine_;
}


//!
//! Return the singleton. Create it if necessary. The given parameters identifies
//! the create function. If zero, the default appkit::createApp() function is used
//! to create the singleton.
//!
App* App::instance(syskit::Singleton::create_t create, unsigned int initialRefCount, void* createArg)
{
    if (instance_ == 0)
    {
        Singleton::create_t createFunc = (create == 0)? createApp: create;
        instance_ = dynamic_cast<App*>(getSingleton(ID1, createFunc, initialRefCount, createArg));
    }

    return instance_;
}


//!
//! Return true if instance was successfully constructed.
//!
bool App::isOk() const
{
    const Foundation& r = Foundation::instance();
    bool ok = r.isOk();
    return ok;
}


//!
//! Show usage if asked.
//! Return true if usage shown.
//!
bool App::showUsage() const
{
    bool usageShown;
    if (usageOpt_ < 0)
    {
        const char* help = usage();
        const char* xHelp = xUsage();
        std::fprintf(stdout, "%s%s", help, xHelp);
        usageShown = true;
    }

    else if (usageOpt_ > 0)
    {
        const char* help = usage();
        std::fprintf(stdout, "%s", help);
        usageShown = true;
    }

    else
    {
        usageShown = false;
    }

    return usageShown;
}


//!
//! Return the application usage.
//! The default implementation returns an empty string.
//!
const char* App::usage() const
{
    return "";
}


//!
//! Return the application extended usage.
//! The default implementation returns an empty string.
//!
const char* App::xUsage() const
{
    return "";
}


const wchar_t* App::copyright() const
{
    return COPYRIGHT;
}


//!
//! This method is invoked when the application is to start running. The method
//! is expected to return an exit code. The default implementation is a no-op
//! returning Ok. The Fatal exit code is used by App to indicate a crash, and
//! it should not be used for other purposes.
//!
int App::onRun()
{
    return Ok;
}


//!
//! Run application.
//!
int App::run()
{
    bool crashed;
    __try
    {
        exitCode_ = showUsage()? Ok: onRun();
        crashed = false;
    }
    __except (CallStack::filter(GetExceptionInformation(), GetExceptionCode(), dumpPath_))
    {
        exitCode_ = Fatal;
        crashed = true;
    }

    if (crashed)
    {
        void* arg;
        Thread::crashCb_t crashCb = Thread::crashCb(arg);
        crashCb(arg);
    }

    return exitCode_;
}


//
// Form default dump path (i.e., "<logDir><exeBase>-dump-<pid>.txt").
//
void App::formDumpPath(const Path& path)
{
    String dumpPath(*logDir_);
    dumpPath += path.base();
    dumpPath += "-dump-";
    dumpPath += Uword(Process::myId());
    dumpPath += ".txt";

    dumpPath_ = dumpPath.widen().detachRaw();
}


void App::mkDirs(const Path& exePath, const wchar_t* confDir, const wchar_t* logDir)
{
    String path(confDir);
    bool skipNormalization = false;
    Path confPath(path, skipNormalization);
    if (confPath.isRelative())
    {
        path = exePath.dirname();
        path += confPath.asString();
        confPath.reset(path, skipNormalization);
    }
    bool beautify = true;
    confDir_ = new String(confPath.fullName(beautify));
    mkdir(*confDir_);

    path = logDir;
    confPath.reset(path, skipNormalization);
    if (confPath.isRelative())
    {
        path = exePath.dirname();
        path += confPath.asString();
        confPath.reset(path, skipNormalization);
    }
    logDir_ = new String(confPath.fullName(beautify));
    mkdir(*logDir_);
}


//
// Parse command line for help options and form various paths.
//
void App::parseCmdLine(const wchar_t* confDir, const wchar_t* logDir)
{

    // Save my path.
    size_t argI = 0;
    const String* arg0 = cmdLine_->arg(argI);
    Path path(*arg0);
    bool beautify = true;
    path_ = new String(path.fullName(beautify));

    // Form various paths based on the executable location.
    bool skipNormalization = true;
    path.reset(*path_, skipNormalization);
    mkDirs(path, confDir, logDir); //confDir_, logDir_
    formDumpPath(path); //dumpPath_

    // Show extended usage?
    // Show usage?
    String optK("??");
    Bool giveHelp(cmdLine_->opt(optK), false /*defaultV*/);
    if (giveHelp)
    {
        usageOpt_ = -1;
    }
    else
    {
        optK = "?";
        giveHelp = Bool(cmdLine_->opt(optK), false /*defaultV*/);
        usageOpt_ = giveHelp? 1: 0;
    }
}


void App::reset(const CmdLine& cmdLine, const wchar_t* confDir, const wchar_t* logDir)
{
    delete[] dumpPath_;
    delete logDir_;
    delete confDir_;
    delete path_;

    delete cmdLine_;
    cmdLine_ = new CmdLine(cmdLine);
    parseCmdLine(confDir, logDir); //confDir_, dumpPath_, logDir_, path_, usageOpt_
}


//
// Change current directory to the executable location at start.
//
void App::setCurrentDir(bool startInExeDir, const String& normalizedExePath)
{
    String optK("startinexedir");
    bool defaultV = startInExeDir;
    Bool goToExeDir(cmdLine_->opt(optK), defaultV);
    if (goToExeDir)
    {
        bool skipNormalization = true;
        Path exePath(normalizedExePath, skipNormalization);
        String exeDir(exePath.dirname());
        String currentDir(Directory::getCurrent());
        oldDir_ = ((exeDir != currentDir) && Directory::setCurrent(exeDir))? new String(currentDir): 0;
    }
    else
    {
        oldDir_ = 0;
    }
}


//!
//! Override the default dump path. The dump path specifies the full path
//! of the file holding the call stack dump if a crash is encountered.
//!
void App::setDumpPath(const String& dumpPath)
{
    delete[] dumpPath_;
    String clone(dumpPath);
    dumpPath_ = clone.widen().detachRaw();
}

END_NAMESPACE1
