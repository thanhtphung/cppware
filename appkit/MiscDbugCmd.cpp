/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "netkit/IpAddr.hpp"
#include "netkit/IpAddrSet.hpp"
#include "netkit/Paddr.hpp"
#include "syskit/Module.hpp"
#include "syskit/Process.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/Bool.hpp"
#include "appkit/DelimitedTxt.hpp"
#include "appkit/MiscDbugCmd.hpp"
#include "appkit/Path.hpp"
#include "appkit/Str.hpp"
#include "appkit/StringVec.hpp"
#include "appkit/SysIo.hpp"
#include "appkit/crt.hpp"

using namespace netkit;
using namespace syskit;

const char EQUALS_SIGN = '=';
const char NEW_LINE[] = "\n";
const unsigned int ACL_CAP0 = 1;
const size_t ADDR_STRING_LENGTH = (sizeof(void*) == 8)? 16: 8; //"%016x" or "%08x"?

// Supported command set.
const char CMD_SET[] =
" acl-config"
" env"
" version-show"
;

// Usage texts. One per command. Must match supported command set.
const char USAGE_0[] =
"Usage:\n"
"  acl-config [--acl=xxx   ]\n"
"             [--add       ] addrrange...\n"
"             [--ioaddr=xxx]\n"
"             [--localonly ]\n"
"             [--remove    ] addrrange...\n\n"
"Examples:\n"
"  acl-config -acl=10.0.4.209\n"
"  acl-config -add 10.0.4.10-10.0.4.19\n"
"  acl-config -remove 10.0.4.10,10.0.4.19-10.0.4.99\n"
;

const char USAGE_1[] =
"Usage:\n"
"  env [--expandEnv]\n\n"
"Examples:\n"
"  env\n"
"  env --expandEnv\n"
;

const char USAGE_2[] =
"Usage:\n"
"  version-show [--v]\n\n"
"Examples:\n"
"  version-show\n"
"  version-show --v\n"
;

const char* const USAGE[] =
{
    USAGE_0, //acl-config
    USAGE_1, //env
    USAGE_2  //version-show
};

// Extended names. One per command. Must match supported command set.
const char* const X_NAME[] =
{
    "configure access control list", //acl-config
    "show application environment",  //env
    "show versions"                  //version-show
};

// Extended usage texts. One per command. Must match supported command set.
const char X_USAGE_0[] =
"\n"
"Options:\n"
"--acl=xxx\n"
"  Update access control list with given address ranges.\n"
"--add\n"
"--remove\n"
"  Add to or remove from access control list.\n"
"  Use command-line arguments as address ranges.\n"
;

const char X_USAGE_2[] =
"\n"
"Options:\n"
"--v\n"
"  Be verbvose.\n"
;

const char* const X_USAGE[] =
{
    X_USAGE_0, //acl-config
    "",        //env
    X_USAGE_2  //version-show
};

BEGIN_NAMESPACE1(appkit)


// Command doers. One per command. Must match supported command set.
MiscDbugCmd::doer_t MiscDbugCmd::doer_[] =
{
    &MiscDbugCmd::doAclConfig,  //acl-config
    &MiscDbugCmd::doEnv,        //env
    &MiscDbugCmd::doVersionShow //version-show
};


MiscDbugCmd::MiscDbugCmd(SysIo* sysIo):
Cmd(CMD_SET)
{
    sysIo_ = sysIo;
}


MiscDbugCmd::~MiscDbugCmd()
{
}


//!
//! This method is invoked when a command is being authenticated. The method returns
//! true/false to allow/disallow the command execution.
//!
bool MiscDbugCmd::allowReq(const CmdLine& req, const Paddr& /*dbugger*/)
{

    // Always log this command.
    if (cmdIndex(req) == 0) //acl-config
    {
        bool alwaysLogRsp = sysIo_->rspLoggingIsEnabled();
        overrideLogMode(req, true /*alwaysLogReq*/, alwaysLogRsp);
    }

    bool allow = true;
    return allow;
}


bool MiscDbugCmd::addToAcl(const CmdLine& req)
{
    bool updated = false;

    String optK("add");
    Bool adding(req.opt(optK), false /*defaultV*/);
    if (adding)
    {
        IpAddrSet addrRange;
        bool ok = getAddrRange(req, addrRange);
        if (ok)
        {
            IpAddrSet acl(sysIo_->acl());
            updated = acl.add(addrRange);
            if (updated)
            {
                sysIo_->setAcl(acl);
            }
        }
    }

    return updated;
}


//
// acl-config [--acl=xxx   ]
//            [--add       ] addrrange...
//            [--ioaddr=xxx]
//            [--localonly ]
//            [--remove    ] addrrange...
//
bool MiscDbugCmd::doAclConfig(const CmdLine& req)
{

    // Move to a new home?
    if (moveHome(req))
    {
        bool cmdIsValid = true;
        return cmdIsValid;
    }

    // Update, add, or remove?
    updateAcl(req) || addToAcl(req) || rmFromAcl(req);

    // Header.
    formRsp(req, "Allow access from:\n");

    // Any?
    IpAddrSet acl(sysIo_->acl());
    if (acl.numAddrs() == 0)
    {
        respond(req, "  any", 5);
    }

    // Specific IPv4 ranges.
    else
    {
        const char* delim = ",\n  ";
        String s(acl.toString(delim));
        respond(req, "  ", 2);
        respond(req, s.ascii(), s.byteSize() - 1);
    }

    // Show access ports.
    formRsp(req, "\n\nAllow access to:\n");
    char buf[Paddr::StrLength + 1];
    char delim = ':';
    Paddr paddr(sysIo_->addr(0));
    formRsp(req, "  %s\n", paddr.toString(buf, delim));
    paddr = sysIo_->addr(1);
    formRsp(req, "  %s%c", paddr.toString(buf, delim), 0);

    bool cmdIsValid = true;
    return cmdIsValid;
}


//
// env [--expandEnv]
//
bool MiscDbugCmd::doEnv(const CmdLine& req)
{
    wchar_t* envW = GetEnvironmentStringsW();
    StringVec env;

    String optK("expandEnv");
    bool expandEnv = Bool(req.opt(optK), false /*defaultV*/);
    DelimitedTxt txt(EQUALS_SIGN);
    String k;
    String kv;
    String v;
    for (const wchar_t* w = envW; *w;)
    {
        kv = w;
        txt.setTxt(kv, false /*makeCopy*/);
        (txt.next(k, true /*doTrimLine*/) && getenv(k, v, expandEnv))?
            env.add(k + EQUALS_SIGN + v):
            env.add(kv);
        size_t n = wcslen(w);
        w += n + 1;
    }

    FreeEnvironmentStringsW(envW);
    env.sort();
    String rsp(env.stringify(NEW_LINE));
    respond(req, rsp);

    bool cmdIsValid = true;
    return cmdIsValid;
}


//
// version-show [--v]
//
bool MiscDbugCmd::doVersionShow(const CmdLine& req)
{

    // Start with hosting OS version.
    String rsp;
    const wchar_t* osVer = Process::osVer();
    swprintf(rsp, L"%9s: %s\n", L"osVer", osVer);
    delete[] osVer;

    // Then module versions.
    String optK("v");
    Bool showAll(req.opt(optK), false /*defaultV*/);
    Process myProc(Process::myId());
    myProc.apply(showAll? showModule: showExe, &rsp);

    // Terminating response.
    respond(req, rsp);

    bool cmdIsValid = true;
    return cmdIsValid;
}


//
// Populate given address range with given command-line arguments.
// Assume command-line arguments, if any, are address ranges. Return
// true if successful.
//
bool MiscDbugCmd::getAddrRange(const CmdLine& req, IpAddrSet& addrRange)
{
    addrRange.reset();
    size_t numArgs = req.numArgs();
    for (size_t i = 1; i < numArgs; ++i)
    {
        const String* arg = req.arg(i);
        char delim = ',';
        if (!IpAddrSet::isValid(arg->ascii(), delim))
        {
            addrRange.reset();
            break;
        }
        addrRange.add(arg->ascii(), delim);
    }

    bool ok = (addrRange.numAddrs() > 0);
    return ok;
}


bool MiscDbugCmd::moveHome(const CmdLine& req)
{
    Paddr old(sysIo_->ioAddr());
    Paddr cur(sysIo_->setIoAddr(req));

    bool moving;
    if (old == cur)
    {
        moving = false;
    }
    else
    {
        char oldBuf[Paddr::StrLength + 1];
        char curBuf[Paddr::StrLength + 1];
        formRsp(req, "Moving from %s to %s...\nDone.%c", old.toString(oldBuf, ':'), cur.toString(curBuf, ':'), 0);
        moving = true;
    }

    return moving;
}


//!
//! Run command using dedicated agent threads provided by the Cmd framework. Return
//! true if command is valid. A false return results in some command usage hints as
//! response to the request.
//!
bool MiscDbugCmd::onRun(const CmdLine& req)
{
    doer_t doer = doer_[cmdIndex(req)];
    bool cmdIsValid = (this->*doer)(req);
    return cmdIsValid;
}


bool MiscDbugCmd::rmFromAcl(const CmdLine& req)
{
    bool updated = false;

    String optK("remove");
    Bool removing(req.opt(optK), false /*defaultV*/);
    if (removing)
    {
        IpAddrSet addrRange;
        bool ok = getAddrRange(req, addrRange);
        if (ok)
        {
            IpAddrSet acl(sysIo_->acl());
            updated = acl.rm(addrRange);
            if (updated)
            {
                sysIo_->setAcl(acl);
            }
        }
    }

    return updated;
}


bool MiscDbugCmd::showExe(void* arg, const Module& module)
{
    bool keepGoing;
    bool skipNormalization = true;
    Path path(module.path(), skipNormalization);
    if (Str::compareKI(path.extension().ascii(), "exe") == 0)
    {
        showModule(arg, module);
        keepGoing = false;
    }
    else
    {
        keepGoing = true;
    }

    return keepGoing;
}


bool MiscDbugCmd::showModule(void* arg, const Module& module)
{
    String modulePath(module.path());
    Path path(modulePath);
    modulePath = path.fullName(true /*beautify*/);

    String s;
    wchar_t fileVerString[Module::MaxVerStringLength + 1];
    wchar_t prodVerString[Module::MaxVerStringLength + 1];
    swprintf(s, L"%9s:\n" L"%9s: %s\n" L"%9s: %s\n" L"%9s: %s\n" L"%9s: %0*X\n" L"%9s: %lu\n",
        L"-------",
        L"module", modulePath.widen(),
        L"fileVer", module.fileVerString(fileVerString),
        L"prodVer", module.prodVerString(prodVerString),
        L"addr", ADDR_STRING_LENGTH, reinterpret_cast<size_t>(module.addr()),
        L"size", module.size());

    String& rsp = *static_cast<String*>(arg);
    rsp += s;

    bool keepGoing = true;
    return keepGoing;
}


bool MiscDbugCmd::updateAcl(const CmdLine& req)
{
    bool updated;

    String optK("acl");
    const String* optV = req.opt(optK, 0 /*defaultV*/);
    if (optV == 0)
    {
        updated = false;
    }
    else
    {
        unsigned int cap = ACL_CAP0;
        char delim = ',';
        IpAddrSet addrRange(optV->ascii(), delim, cap);
        sysIo_->setAcl(addrRange);
        updated = true;
    }

    return updated;
}


const char* MiscDbugCmd::usage(unsigned char cmdIndex) const
{
    return USAGE[cmdIndex];
}


const char* MiscDbugCmd::xName(unsigned char cmdIndex) const
{
    return X_NAME[cmdIndex];
}


const char* MiscDbugCmd::xUsage(unsigned char cmdIndex) const
{
    return X_USAGE[cmdIndex];
}

END_NAMESPACE1
