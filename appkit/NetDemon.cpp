/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "netkit/Paddr.hpp"
#include "netkit/Winsock.hpp"
#include "syskit/MappedFile.hpp"
#include "syskit/SigTrap.hpp"
#include "syskit/Thread.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/CmdAgent.hpp"
#include "appkit/CmdMap.hpp"
#include "appkit/DelimitedTxt.hpp"
#include "appkit/Directory.hpp"
#include "appkit/NetDemon.hpp"
#include "appkit/NetDemonCmd.hpp"
#include "appkit/Path.hpp"
#include "appkit/String.hpp"
#include "appkit/SysIo.hpp"
#include "appkit/U32.hpp"

using namespace netkit;
using namespace syskit;

const char COMMENT_INDICATOR = '#';
const unsigned int LOG_CMDS_M = 0x01U;
const unsigned int MAX_DUMPS = 9;

BEGIN_NAMESPACE1(appkit)


NetDemon::NetDemon(const CmdLine& cmdLine,
const String& name,
bool startInExeDir,
const char* defaultIoAddr,
const wchar_t* confDir,
const wchar_t* logDir):
Demon(cmdLine, name, startInExeDir, true /*usesNet*/, confDir, logDir)
{
    CmdMap::instance()->addRef();
    restartOnCrash_ = (countDumps() < MAX_DUMPS);
    winsock_ = new Winsock;

    alias_ = new String(Demon::name());
    cmd_ = 0;
    defaultIoAddr_ = new Paddr(defaultIoAddr);
    logMask_ = 0;
    loopbackAgent_ = 0;
    sigTrap_ = 0;
    sysIo_ = 0;
}


NetDemon::~NetDemon()
{
    delete loopbackAgent_;
    delete sysIo_;
    delete sigTrap_;
    delete defaultIoAddr_;
    delete cmd_;
    delete alias_;

    delete winsock_;
    CmdMap::instance()->rmRef();
}


MappedFile* NetDemon::mapRcFile() const
{

    // No problem if no accessible rc3 file. Allow use of same rc3 file for both debug
    // and release builds (e.g., x.rc3 can be used by both x.exe and xd.exe).
    String rcPath(confDir());
    rcPath += Path(path()).base();
    rcPath += ".rc3";
    bool readOnly = true;
    MappedFile* rcFile = new MappedFile(rcPath.widen(), readOnly);
    if (!rcFile->isOk())
    {
        bool ignoreCase = true;
        if (rcPath.endsWith("d.rc3", ignoreCase))
        {
            rcPath.truncate(rcPath.length() - 5);
            rcPath += ".rc3";
            rcFile->remap(rcPath.widen(), readOnly);
        }
    }

    return rcFile;
}


//!
//! Return the singleton.
//!
NetDemon* NetDemon::instance()
{
    return dynamic_cast<NetDemon*>(Demon::instance());
}


Paddr NetDemon::defaultIoAddr() const
{
    return *defaultIoAddr_;
}


//!
//! Stop demon. Also run demon again if restart is set. Return true if successful.
//! Return false otherwise (restarting a process is unsupported).
//!
bool NetDemon::stop(bool restart)
{
    bool ok = Demon::stop(restart);
    return ok;
}


//!
//! Return the demon usage.
//!
const char* NetDemon::usage() const
{
    const char* s = Demon::usage();
    return s;
}


//!
//! Return the demon extended usage.
//!
const char* NetDemon::xUsage() const
{
    const char* s = Demon::xUsage();
    return s;
}


//!
//! Run demon. Command-line options can be used to control how the demon
//! is run. By default, a demon runs as a service on windows. To run
//! as a process, specify the "process" option. The demon service can
//! also be installed or removed using the "install" or "remove" option.
//!
int NetDemon::run()
{
    int exitCode = Demon::run();
    return exitCode;
}


//!
//! This method is invoked when the demon is to start running as a process.
//!
int NetDemon::onRun()
{
    onStart();
    Thread::takeANap(Thread::ETERNITY); //stop when interrupted

    int exitCode = Ok;
    return exitCode;
}


unsigned int NetDemon::countDumps() const
{
    unsigned int numDumps = 0;
    bool skipNormalization = true;
    Path myPath(path(), skipNormalization);
    String prefix(myPath.base());
    prefix += "-dump-";

    bool ignoreCase = true;
    Directory dir(logDir());
    StringVec children;
    dir.list(children);
    for (unsigned int i = 0, numChildren = children.numItems(); i < numChildren; ++i)
    {
        if (children.peek(i).startsWith(prefix, ignoreCase))
        {
            ++numDumps;
        }
    }

    return numDumps;
}


//!
//! This method is invoked when the demon is being configured via the demon-config command.
//! The config parameter holds the full demon-config command being processed.
//!
void NetDemon::onConfig(const CmdLine& config)
{
    String optK("alias");
    const String* alias = config.opt(optK);
    if (alias != 0)
    {
        setAlias(*alias);
    }

    optK = "logmask";
    U32 logMask(config.opt(optK), logMask_ /*defaultV*/);
    if (logMask_ != logMask)
    {
        setLogMask(logMask);
    }
}


//!
//! This method is invoked when the demon is to start running as a service.
//!
void NetDemon::onStart()
{
    sigTrap_ = new SigTrap;
    const CmdLine& config = Demon::cmdLine();
    sysIo_ = new SysIo(config, defaultIoAddr_->addr(), defaultIoAddr_->port());
    cmd_ = new NetDemonCmd(this, sysIo_);
    loopbackAgent_ = CmdAgent::createLoopback(sysIo_);

    const MappedFile* rcFile = mapRcFile();
    if (rcFile->isOk())
    {
        runStartUpCmds(*rcFile);
    }
    delete rcFile;
}


//!
//! This method is invoked when the demon service is asked to stop.
//!
void NetDemon::onStop()
{
    Demon::onStop();
}


//!
//! This method is invoked when system shutdown is observed.
//!
void NetDemon::onSysShutdown()
{
    Demon::onSysShutdown();
}


//
// Run start-up commands from rc3 file, if any.
//
void NetDemon::runStartUpCmds(const MappedFile& rcFile) const
{

    // Run start-up commands from rc3 file using loopback agent.
    bool makeCopy = false;
    DelimitedTxt rcTxt(reinterpret_cast<const char*>(rcFile.map(0UL /*index*/)), static_cast<size_t>(rcFile.size()), makeCopy);
    String cmd;
    bool doTrimLine = true;
    while (rcTxt.next(cmd, doTrimLine))
    {
        if ((!cmd.empty()) && (*cmd.ascii() != COMMENT_INDICATOR))
        {
            loopbackAgent_->runCmd(cmd);
        }
    }
}


//!
//! Set the default home address of the I/O subsystem. This is used during second-phase
//! initialization occurring right before onRun(). Setting this address after that point
//! has no practical meaning.
//!
void NetDemon::setDefaultIoAddr(const char* defaultIoAddr)
{
    delete defaultIoAddr_;
    defaultIoAddr_ = new Paddr(defaultIoAddr);
}


//!
//! Override the default alias. By default, each NetDemon instance has an alias
//! of its service name.
//!
void NetDemon::setAlias(const String& alias)
{
    delete alias_;
    alias_ = new String(alias);
}


//!
//! Update the current log mask. Logging is minimal if most or all bits are off
//! and maximal if most or all bits are on.
//!
void NetDemon::setLogMask(unsigned int logMask)
{
    logMask_ = logMask;
    bool logReq = true;
    bool logRsp = (logMask_ & LOG_CMDS_M);
    sysIo_->setLogMode(logReq, logRsp);
}

END_NAMESPACE1
