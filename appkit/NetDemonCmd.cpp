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
#include "appkit/Bool.hpp"
#include "appkit/BufPoolCmd.hpp"
#include "appkit/CmdCmd.hpp"
#include "appkit/CmdMap.hpp"
#include "appkit/Directory.hpp"
#include "appkit/MiscDbugCmd.hpp"
#include "appkit/NetDemon.hpp"
#include "appkit/NetDemonCmd.hpp"
#include "appkit/SysIo.hpp"
#include "appkit/crt.hpp"

using namespace netkit;
using namespace syskit;

const char COLON = ':';

// Supported command set.
const char CMD_SET[] =
" demon-config"
" demon-show"
" exit"
;

// Usage texts. One per command. Must match supported command set.
const char USAGE_0[] =
"Usage:\n"
"  demon-config [--alias=xxx  ]\n"
"               [--ioaddr=xxx ]\n"
"               [--localonly  ]\n"
"               [--logmask=xxx]\n\n"
"Examples:\n"
"  demon-config\n"
"  demon-config --alias=abc --logmask=1\n"
;

const char USAGE_2[] =
"Usage:\n"
"  exit [--crash  ]\n"
"       [--restart] #allowed if run as a service\n\n"
"Examples:\n"
"  exit\n"
"  exit --crash --restart\n"
;

const char* const USAGE[] =
{
    USAGE_0, //demon-config
    "",      //demon-show
    USAGE_2  //exit
};

// Extended names. One per command. Must match supported command set.
const char* const X_NAME[] =
{
    "configure demon",                 //demon-config
    "show demon status & environment", //demon-show
    "crash, exit, and/or restart"      //exit
};

BEGIN_NAMESPACE1(appkit)


// Command doers. One per command. Must match supported command set.
NetDemonCmd::doer_t NetDemonCmd::doer_[] =
{
    &NetDemonCmd::doConfig, //demon-config
    &NetDemonCmd::doShow,   //demon-show
    &NetDemonCmd::doExit    //exit
};


NetDemonCmd::NetDemonCmd(NetDemon* netDemon, SysIo* sysIo):
Cmd(CMD_SET)
{
    netDemon_ = netDemon;
    sysIo_ = sysIo;

    new BufPoolCmd;
    new CmdCmd(sysIo_);
    new MiscDbugCmd(sysIo_);
}


NetDemonCmd::~NetDemonCmd()
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
bool NetDemonCmd::allowReq(const CmdLine& req, const Paddr& /*dbugger*/)
{

    // Always log these commands.
    unsigned int i = cmdIndex(req);
    if ((i == 0) || (i == 2)) //demon-config, exit
    {
        overrideLogMode(req, true /*alwaysLogReq*/, false /*alwaysLogRsp*/);
    }

    bool allow = true;
    return allow;
}


//
// demon-config [--alias=xxx  ]
//              [--ioaddr=xxx ]
//              [--localonly  ]
//              [--logmask=xxx]
//
bool NetDemonCmd::doConfig(const CmdLine& req)
{
    netDemon_->onConfig(req);
    Paddr ioAddr(sysIo_->setIoAddr(req));

    char delim = COLON;
    formRsp(req, "%s=\"%ws\"\n" "%s=%s\n" "%s=0x%08x\n%c",
        "alias", netDemon_->alias().widen(),
        "ioAddr", ioAddr.toString(delim).ascii(),
        "logMask", netDemon_->logMask(),
        0);

    bool cmdIsValid = true;
    return cmdIsValid;
}


//
// exit [--crash  ]
//      [--restart] #allowed if run as a service
//
bool NetDemonCmd::doExit(const CmdLine& req)
{

    // Determine exiting scheme.
    String optK("crash");
    Bool crash(req.opt(optK), false /*defaultV*/);
    optK = "restart";
    const String* restartOpt = req.opt(optK);
    Bool restart(restartOpt, false /*defaultV*/);

    // Sanity check.
    bool ok = true;
    const char* rsp;
    if (restart)
    {
        if (netDemon_->runningAsProcess())
        {
            ok = false;
            rsp = "exit: restarting a process is unsupported.";
        }
        else
        {
            rsp = crash? "Crashing and restarting...": "Restarting...";
        }
    }
    else
    {
        rsp = crash? "Crashing...": "Exiting...";
    }

    // Perform operation if allowed.
    respond(req, rsp);
    Thread::yield();
    if (ok)
    {
        if (crash)
        {
            if (restartOpt != 0)
            {
                netDemon_->setRestartMode(restart);
            }
            const char* const TOMBSTONE = "NetDemonCmd::doExit: crashing...";
            const char** grave = 0;
            *grave = TOMBSTONE;
        }
        netDemon_->stop(restart);
    }

    bool cmdIsValid = true;
    return cmdIsValid;
}


//
// demon-show
//
bool NetDemonCmd::doShow(const CmdLine& req)
{
    char dateString[Date::MaxDateStringLength + 1];
    Date buildDate(netDemon_->buildDate());
    String buf;
    String rsp;
    rsp += (sprintf(buf, "%11s: %ws\n" "%11s: %s\n" "%11s: %s\n",
        "exePath", netDemon_->path().widen(),
        "bldDate", buildDate.asString(dateString),
        "bldTime", netDemon_->buildTime()), buf);

    Module module(netDemon_->path().widen(), 0 /*addr*/, 0 /*size*/);
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
        L"copyright", netDemon_->copyright(),
        L"---------"), buf);

    Paddr ioAddr(sysIo_->ioAddr());
    rsp += (sprintf(buf, "%11s: %ws\n" "%11s: %s\n" "%11s: %ws\n" "%11s: 0x%08x\n" "%11s:\n",
        "alias", netDemon_->alias().widen(),
        "ioAddr", ioAddr.toString().ascii(),
        "logDir", netDemon_->logDir().widen(),
        "logMask", netDemon_->logMask(),
        "---------"), buf);

    char host[255 + 1];
    host[0] = 0;
    gethostname(host, sizeof(host));
    String s(host);
    const wchar_t* osVer = Process::osVer();
    const wchar_t* user = myProc.user();
    rsp += (swprintf(buf, L"%11s: %s\n" L"%11s: %s\n" L"%11s: %s\n" L"%11s: %s\n" L"%11s: %s\n",
        L"cmdLine", netDemon_->cmdLine().asString().widen(),
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
//! Run command using dedicated agent threads provided by the Cmd framework. Return
//! true if command is valid. A false return results in some command usage hints as
//! response to the request.
//!
bool NetDemonCmd::onRun(const CmdLine& req)
{
    doer_t doer = doer_[cmdIndex(req)];
    bool cmdIsValid = (this->*doer)(req);
    return cmdIsValid;
}


const char* NetDemonCmd::usage(unsigned char cmdIndex) const
{
    return USAGE[cmdIndex];
}


const char* NetDemonCmd::xName(unsigned char cmdIndex) const
{
    return X_NAME[cmdIndex];
}

END_NAMESPACE1
