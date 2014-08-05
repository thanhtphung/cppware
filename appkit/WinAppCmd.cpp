/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include "netkit/Paddr.hpp"
#include "syskit/Date.hpp"
#include "syskit/Module.hpp"
#include "syskit/Process.hpp"
#include "syskit/Thread.hpp"
#include "syskit/Utc.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/CmdCmd.hpp"
#include "appkit/Bool.hpp"
#include "appkit/BufPoolCmd.hpp"
#include "appkit/CmdMap.hpp"
#include "appkit/Directory.hpp"
#include "appkit/MiscDbugCmd.hpp"
#include "appkit/SysIo.hpp"
#include "appkit/WinApp.hpp"
#include "appkit/WinAppCmd.hpp"
#include "appkit/crt.hpp"

using namespace appkit;
using namespace netkit;
using namespace syskit;

const char COLON = ':';

// Supported command set.
const char CMD_SET[] =
" app-config"
" app-show"
" exit"
;

// Usage texts. One per command. Must match supported command set.
const char USAGE_0[] =
"Usage:\n"
"  app-config [--alias=xxx  ]\n"
"             [--ioaddr=xxx ]\n"
"             [--localonly  ]\n"
"             [--logmask=xxx]\n\n"
"Examples:\n"
"  app-config\n"
"  app-config --alias=abc --logmask=1\n"
;

const char USAGE_2[] =
"Usage:\n"
"  exit [--crash]\n\n"
"Examples:\n"
"  exit\n"
"  exit --crash\n"
;

const char* const USAGE[] =
{
    USAGE_0, //app-config
    "",      //app-show
    USAGE_2  //exit
};

// Extended names. One per command. Must match supported command set.
const char* const X_NAME[] =
{
    "configure app",                 //app-config
    "show app status & environment", //app-show
    "crash and/or exit"              //exit
};

BEGIN_NAMESPACE1(appkit)


// Command doers. One per command. Must match supported command set.
WinAppCmd::doer_t WinAppCmd::doer_[] =
{
    &WinAppCmd::doConfig, //app-config
    &WinAppCmd::doShow,   //app-show
    &WinAppCmd::doExit    //exit
};


WinAppCmd::WinAppCmd(WinApp* winApp, SysIo* sysIo):
WinCmd(winApp, CMD_SET)
{
    exitInProgress_ = false;
    winApp_ = winApp;
    sysIo_ = sysIo;

    new BufPoolCmd;
    new CmdCmd(sysIo_);
    new MiscDbugCmd(sysIo_);
}


WinAppCmd::~WinAppCmd()
{
    const CmdMap* map = Cmd::map();
    delete map->find("version-show"); //MiscDbugCmd
    delete map->find("help");         //CmdCmd
    delete map->find("bufpool-show"); //BufPoolCmd
}


//!
//! This method is invoked when a command is being authenticated. The method returns
//! true/false to allow/disallow the command execution.
//!
bool WinAppCmd::allowReq(const CmdLine& req, const Paddr& /*dbugger*/)
{

    // Always log these commands.
    unsigned int i = cmdIndex(req);
    if ((i == 0) || (i == 2)) //app-config, exit
    {
        bool alwaysLogRsp = sysIo_->rspLoggingIsEnabled();
        overrideLogMode(req, true /*alwaysLogReq*/, alwaysLogRsp);
    }

    bool allow = true;
    return allow;
}


//
// app-config [--alias=xxx  ]
//            [--ioaddr=xxx ]
//            [--localonly  ]
//            [--logmask=xxx]
//
bool WinAppCmd::doConfig(const CmdLine& req)
{
    winApp_->onConfig(req);
    Paddr ioAddr(sysIo_->setIoAddr(req));

    char delim = COLON;
    formRsp(req, "%s=\"%ws\"\n" "%s=%s\n" "%s=0x%08x\n%c",
        "alias", winApp_->alias().widen(),
        "ioAddr", ioAddr.toString(delim).ascii(),
        "logMask", winApp_->logMask(),
        0);

    bool cmdIsValid = true;
    return cmdIsValid;
}


//
// exit [--crash  ]
//
bool WinAppCmd::doExit(const CmdLine& req)
{

    // Determine exiting scheme.
    String optK("crash");
    Bool crash(req.opt(optK), false /*defaultV*/);
    const char* rsp;
    rsp = crash? "Crashing...": "Exiting...";

    // Perform operation.
    exitInProgress_ = true;
    respond(req, rsp);
    Thread::yield();
    if (crash)
    {
        const char* const TOMBSTONE = "WinAppCmd::doExit: crashing...";
        const char** grave = 0;
        *grave = TOMBSTONE;
    }
    winApp_->stop();

    bool cmdIsValid = true;
    return cmdIsValid;
}


//
// app-show
//
bool WinAppCmd::doShow(const CmdLine& req)
{
    char dateString[Date::MaxDateStringLength + 1];
    Date buildDate(winApp_->buildDate());
    String buf;
    String rsp;
    rsp += (sprintf(buf, "%11s: %ws\n" "%11s: %s\n" "%11s: %s\n",
        "exePath", winApp_->path().widen(),
        "bldDate", buildDate.asString(dateString),
        "bldTime", winApp_->buildTime()), buf);

    Module module(winApp_->path().widen(), 0 /*addr*/, 0 /*size*/);
    wchar_t verString[Module::MaxVerStringLength + 1];
    Process myProc(Process::myId());
    Utc now;
    Utc startTime(Process::startTime());
    unsigned int upTime = now.secs() - startTime.secs();
    char upTimeString[Date::MaxTimeStringLength + 1];
    rsp += (swprintf(buf, L"%11s: %lu\n" L"%11s: %s\n" L"%11s: %s\n" L"%11s: %s\n" L"%11s:\n",
        L"pid", myProc.id(),
        L"prodVer", module.prodVerString(verString),
        L"upTime", String(Date::formatTime(upTimeString, upTime)).widen(),
        L"copyright", winApp_->copyright(),
        L"---------"), buf);

    Paddr ioAddr(sysIo_->ioAddr());
    rsp += (sprintf(buf, "%11s: %ws\n" "%11s: %s\n" "%11s: %ws\n" "%11s: 0x%08x\n" "%11s:\n",
        "alias", winApp_->alias().widen(),
        "ioAddr", ioAddr.toString().ascii(),
        "logDir", winApp_->logDir().widen(),
        "logMask", winApp_->logMask(),
        "---------"), buf);

    char host[255 + 1];
    host[0] = 0;
    gethostname(host, sizeof(host));
    String s(host);
    const wchar_t* osVer = Process::osVer();
    const wchar_t* user = myProc.user();
    rsp += (swprintf(buf, L"%11s: %s\n" L"%11s: %s\n" L"%11s: %s\n" L"%11s: %s\n" L"%11s: %s\n",
        L"cmdLine", winApp_->cmdLine().asString().widen(),
        L"curDir", Directory::getCurrent().widen(),
        L"host", s.widen(),
        L"osVer", osVer,
        L"user", user), buf);
    delete[] user;
    delete[] osVer;
    respond(req, rsp);

    bool cmdIsValid = true;
    return cmdIsValid;
}


//!
//! Run command using the WinApp messaging thread (i.e., the UI thread). Return
//! true if command is valid. A false return results in some command usage hints as
//! response to the request.
//!
bool WinAppCmd::onFinalRun(const CmdLine& req)
{
    doer_t doer = doer_[cmdIndex(req)];
    bool cmdIsValid = (this->*doer)(req);
    return cmdIsValid;
}


//!
//! Run command using dedicated agent threads provided by the Cmd framework. Return
//! true if command is valid. A false return results in some command usage hints as
//! response to the request.
//!
bool WinAppCmd::onRun(const CmdLine& req)
{
    bool cmdIsValid;
    doer_t doer;
    switch (cmdIndex(req))
    {

    case 2: //exit
        cmdIsValid = WinCmd::onRun(req);
        break;

    default:
        doer = doer_[cmdIndex(req)];
        cmdIsValid = (this->*doer)(req);
        break;

    }

    return cmdIsValid;
}


const char* WinAppCmd::usage(unsigned char cmdIndex) const
{
    return USAGE[cmdIndex];
}


const char* WinAppCmd::xName(unsigned char cmdIndex) const
{
    return X_NAME[cmdIndex];
}

END_NAMESPACE1
