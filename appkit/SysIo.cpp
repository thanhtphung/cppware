/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <time.h>
#include "netkit/IpAddr.hpp"
#include "netkit/Paddr.hpp"
#include "netkit/UdpClient.hpp"
#include "syskit/Thread.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/Bool.hpp"
#include "appkit/Cmd.hpp"
#include "appkit/CmdLine.hpp"
#include "appkit/CmdMap.hpp"
#include "appkit/LogPath.hpp"
#include "appkit/S32.hpp"
#include "appkit/SysIo.hpp"
#include "appkit/U32.hpp"
#include "appkit/crt.hpp"

using namespace netkit;
using namespace syskit;

const char COLON = ':';
const char COMMENT_INDICATOR = '#';
const char NEW_LINE[] = "\n";
const char UNKNOWN_RSP[] = ": Unknown command. Try \"help\".\n";
const int IBUF_SIZE = 196608; //bytes (192k)
const int OBUF_SIZE = 458752; //bytes (448k)
const unsigned int ACL_CAP0 = 1; //IPv4 address ranges
const unsigned int LOG_FILE_CAP = 24576; //commands
const unsigned int THREAD_STACK_SIZE = 0; //bytes, 0=use-default

BEGIN_NAMESPACE1(appkit)

const char SysIo::LOG_PATH_HI = 'z';
const char SysIo::LOG_PATH_LO = 'a';


//!
//! Construct minimal I/O subsystem. The subsystem does not have dedicated agent
//! threads to run commands. Take over ownership of the given UdpClient objects
//! which will be destroyed by SysIo when appropriate.
//!
SysIo::SysIo(UdpClient* io0, UdpClient* io1):
ioPort_(),
ioAddr_(),
ss_()
{
    logMask_ = 0;
    acl_ = new IpAddrSet(ACL_CAP0);
    curLog_[0] = stdout;
    curLog_[1] = stdout;

    ibufSize_ = 0;
    obufSize_ = 0;

    Paddr paddr;
    io0->getMyAddr(paddr);
    addr_[0] = paddr.addr();
    ioPort_[0].set(static_cast<unsigned int>(paddr.port()) << 16);
    io1->getMyAddr(paddr);
    addr_[1] = paddr.addr();
    ioPort_[1].set(static_cast<unsigned int>(paddr.port()) << 16);
    map_ = CmdMap::instance();
    map_->addRef();
    io_[0] = io0;
    io_[1] = io1;

    oldCrashCb_ = 0;
    oldCrashArg_ = 0;
    agent_[0] = 0;
    agent_[1] = 0;
}


//!
//! Construct I/O subsystem. The subsystem includes two dedicated agent threads
//! to run commands. Commands which can be run must implement the Cmd interface.
//! Allowed options in config: acl, ibufsize, ioaddr, localonly, obufsize, threadstacksize.
//!
SysIo::SysIo(const CmdLine& config, unsigned int defaultAddr, unsigned short defaultPort):
ioPort_(),
ioAddr_(defaultAddr, defaultPort),
ss_()
{
    curLog_[0] = stdout;
    curLog_[1] = stdout;
    logMask_ = LogReq;
    setLogMode(); //logMask_
    formAddr(config); //ioAddr_
    formAcl(config); //acl_

    String optK("ibufsize");
    ibufSize_ = S32(config.opt(optK), IBUF_SIZE /*defaultV*/);
    optK = "obufsize";
    obufSize_ = S32(config.opt(optK), OBUF_SIZE /*defaultV*/);

    addr_[0] = 0;
    addr_[1] = 0;
    map_ = CmdMap::instance();
    map_->addRef();
    mkIo(0); //io_[0]
    mkIo(1); //io_[1]

    const wchar_t* dumpPath = Thread::dumpPath();
    Thread::monitorCrash(onCrash, this, dumpPath, &oldCrashCb_, &oldCrashArg_);

    optK = "threadstacksize";
    unsigned int stackSizeInBytes = U32(config.opt(optK), THREAD_STACK_SIZE /*defaultV*/);
    size_t index = 0;
    runArg_t* arg = new runArg_t(this, index);
    agent_[0] = new Thread(agentEntry, arg, stackSizeInBytes);

    index = 1;
    arg = new runArg_t(this, index);
    agent_[1] = new Thread(agentEntry, arg, stackSizeInBytes);
}


SysIo::~SysIo()
{
    if (agent_[1] != 0)
    {
        agent_[1]->killAndWait();
        delete agent_[1];
        agent_[0]->killAndWait();
        delete agent_[0];
        const wchar_t* dumpPath = Thread::dumpPath();
        Thread::monitorCrash(oldCrashCb_, oldCrashArg_, dumpPath);
    }

    delete io_[1];
    delete io_[0];
    map_->rmRef();
    delete acl_;
}


//!
//! Return the current access control list. This is used to restrict remote
//! access when remote access is enabled. An empty value allows remote access
//! from any client, and a non-empty value allows remote access from only
//! addresses in the list.
//!
IpAddrSet SysIo::acl() const
{
    SpinSection::Lock lock(ss_);
    return *acl_;
}


//!
//! Return one of the effective addresses of the I/O subsystem. Index 0
//! identifies a loopback port. Index 1 identifies another loopback port
//! if remote access is disabled and an external port if remote access
//! is enabled.
//!
Paddr SysIo::addr(size_t index) const
{
    SpinSection::Lock lock(ss_);
    unsigned short port = static_cast<unsigned short>(ioPort_[index] >> 16);
    Paddr paddr(addr_[index], port);
    return paddr;
}


//!
//! Return the desired home address of the I/O subsystem. Use addr(1) to
//! get the actual home address of the I/O subsystem.
//!
Paddr SysIo::ioAddr() const
{
    SpinSection::Lock lock(ss_);
    return ioAddr_;
}


//!
//! Update home address using the localonly and ioaddr options. The localonly option
//! can be used to disable remote access. The ioaddr option specifies how this process
//! can be accessed remotely. Return the updated desired home address of the I/O
//! subsystem.
//!
Paddr SysIo::setIoAddr(const CmdLine& config)
{
    Paddr cur;
    Paddr paddr1;
    bool moving;
    {
        SpinSection::Lock lock(ss_);
        Paddr old(ioAddr_);
        formAddr(config);
        cur = ioAddr_;
        paddr1.setPort(ioPort_[1] >> 16);
        paddr1.setAddr((addr_[1] == 0)? INADDR_LOOPBACK: addr_[1]);
        moving = (old != cur);
    }

    // Sending an empty message from agent zero to agent one to start the move.
    // Upon receipt, a new server socket using the desired new home address will
    // replace the current one.
    if (moving)
    {
        const unsigned char* msg = 0;
        size_t length = 0;
        unsigned int timeoutInMsecs = 0;
        io_[0]->snd(msg, length, paddr1, timeoutInMsecs);
    }

    return cur;
}


//!
//! Return matching client configuration as a string. Index 0 identifies a loopback
//! port. Index 1 identifies another loopback port if remote access is disabled and
//! an external port if remote access is enabled. The returned string can help
//! configure a client like CmdAgent or Messenger.
//!
String SysIo::clientConfig(size_t index) const
{
    String opts;
    Paddr serverAddr(addr(index));
    sprintf(opts, "cmd --dbuggee=%s --ibufsize=%ld --obufsize=%ld",
        serverAddr.toString().ascii(),
        obufSize_, //client incoming buffer should match server outgoing buffer
        ibufSize_); //client outgoing buffer should match server incoming buffer

    return opts;
}


//!
//! Return current state as a string.
//!
String SysIo::state() const
{
    String acl;
    char ioAddr[Paddr::StrLength + 1];
    {
        SpinSection::Lock lock(ss_);
        const char* delim = ",";
        acl = acl_->toString(delim);
        ioAddr_.toString(ioAddr, COLON);
    }

    String s("acl=\"");
    s += acl;
    s += "\"\nioaddr=";
    s += ioAddr;
    s += NEW_LINE[0];

    return s;
}


UdpClient* SysIo::move()
{
    UdpClient* old = io_[1];
    {
        SpinSection::Lock lock(ss_);
        mkIo(1);
    }
    delete old;

    return io_[1];
}


//
// Return true if access control list is empty or if it contains given address.
//
bool SysIo::allowAccessFrom(unsigned int srcAddr) const
{
    SpinSection::Lock lock(ss_);
    bool yes = (acl_->numAddrs() == 0) || acl_->contains(srcAddr);
    return yes;
}


//!
//! Return true if instance was successfully constructed.
//!
bool SysIo::isOk() const
{
    bool ok = io_[0]->isOk() && io_[1]->isOk();
    return ok;
}


//!
//! This method is invoked when a command is about to be run. The method returns
//! true/false to allow/disallow the command execution. The default implementation
//! provides some logging before returning true.
//!
bool SysIo::onReq(const CmdLine& req, const Paddr& dbugger)
{
    Cmd::tag_t& tag = *static_cast<Cmd::tag_t*>(req.tag());
    bool allowReq = tag.cmd->allowReq(req, dbugger);
    if (((logMask_ & LogReq) == 0) && (!tag.alwaysLogReq))
    {
        return allowReq;
    }

    tag.rspTail = 0;
    std::FILE* log = tag.log;
    bool flushLogEntries = ((logMask_ & FlushLogEntries) != 0);
    logReq(req, dbugger, log, flushLogEntries);
    return allowReq;
}


bool SysIo::showUsage(const CmdLine& cmdLine, const Cmd& cmd)
{

    // Show extended usage if asked.
    String optK("??");
    Bool giveHelp(cmdLine.opt(optK), false /*defaultV*/);
    if (giveHelp)
    {
        unsigned char cmdIndex = cmd.cmdIndex(cmdLine);
        const char* usage = cmd.usage(cmdIndex);
        const char* xUsage = cmd.xUsage(cmdIndex);
        cmd.respond(cmdLine, usage, strlen(usage));
        cmd.respond(cmdLine, xUsage);
        return true;
    }

    // Show usage if asked.
    optK = "?";
    giveHelp = Bool(cmdLine.opt(optK), false /*defaultV*/);
    if (giveHelp)
    {
        unsigned char cmdIndex = cmd.cmdIndex(cmdLine);
        const char* usage = cmd.usage(cmdIndex);
        cmd.respond(cmdLine, usage);
        return true;
    }

    // Not asked.
    return false;
}


void SysIo::bind(UdpClient& io, unsigned int addr, unsigned short port)
{
    bool ok = io.bind(addr, port);
    if ((!ok) && (port != 0))
    {
        port = 0;
        io.bind(addr, port);
    }
}


//
// Form access control list.
//
void SysIo::formAcl(const CmdLine& config)
{
    String optK("acl");
    const String* acl = config.opt(optK, 0 /*defaultV*/);
    unsigned int cap = ACL_CAP0;
    if (acl == 0)
    {
        acl_ = new IpAddrSet(cap);
    }
    else
    {
        char delim = ',';
        acl_ = new IpAddrSet(acl->ascii(), delim, cap);
    }
}


void SysIo::formAddr(const CmdLine& config)
{

    // The localonly option is equivalent to "--ioaddr=127.0.0.1:0".
    String optK("localonly");
    bool localOnly = Bool(config.opt(optK), false /*defaultV*/);
    if (localOnly)
    {
        ioAddr_.reset(INADDR_LOOPBACK, 0U);
    }

    // If ioaddr is specified, the "a.b.c.d:n" format is more likely than the "a.b.c.d" format.
    else
    {
        optK = "ioaddr";
        const String* ioAddr = config.opt(optK, 0 /*defaultV*/);
        if (ioAddr != 0)
        {
            bool portIsOptional = true;
            char delim = COLON;
            ioAddr_.reset(ioAddr->ascii(), delim, portIsOptional);
        }
    }
}


void SysIo::logReq(const CmdLine& req, const Paddr& dbugger, std::FILE* log, bool flushLogEntries)
{

    // Form timestamp (example: "|10/25/11 15:46:11|10.0.4.82:32510| ").
    struct tm now;
    time_t t = time(0);
    localtime_s(&now, &t);
    char stamp[32 + Paddr::StrLength + 2];
    size_t size = strftime(stamp, 32, "|%c|", &now);
    size += dbugger.toDigits(stamp + size, COLON);
    stamp[size++] = '|';
    stamp[size++] = ' ';

    // Log timestamp.
    size_t count = 1;
    std::fwrite(stamp, size, count, log);

    // Log request.
    const String& cmd = req.asString();
    size = cmd.byteSize() - 1;
    std::fwrite(cmd.ascii(), size, count, log);
    size = 1;
    std::fwrite(NEW_LINE, size, count, log);
    if (flushLogEntries)
    {
        std::fflush(log);
    }
}


//
// Main loop for each of the dedicated command agent threads. Agent zero is accessible
// locally only via loopback addressing. Agent one can be accessible remotely if remote
// access has not been disabled.
//
void SysIo::loop(size_t index)
{
    const char* name = (index == 0)? "-sysio-a": "-sysio-b";
    LogPath logPath(name, LOG_PATH_LO, LOG_PATH_HI);
    curLog_[index] = logPath.curLog();

    Atomic32* ioPort = &ioPort_[index];
    UdpClient* io = io_[index];
    bool loopback = (index == 0) || (ioAddr_.addr() == INADDR_LOOPBACK);
    Cmd::tag_t cmdTag(this, io, curLog_[index], ioPort);
    CmdLine req;
    req.setTag(&cmdTag);

    // Wait forever for input commands. For each arriving command, make
    // sure it comes from a legitimate source, and let its handler take
    // over the command execution via the onRun() method.
    Paddr src;
    String cmd;
    String defaultCmdName;
    unsigned int cmdCount = 0;
    unsigned int timeoutInMsecs = 0;
    while (!Thread::isTerminating())
    {

        utf8_t rawCmd[Cmd::MaxCmdLength + 1];
        int cmdSize = io->rcv(rawCmd, sizeof(rawCmd), src, UdpClient::ETERNITY);

        // Move to a different home. The move request is an empty message sent from
        // agent zero to agent one. The move is done by replacing the current server
        // socket w/ a new one using the desired new home address.
        if (cmdSize == 0)
        {
            if ((index == 1) && (src == addr(0)))
            {
                io = move();
                cmdTag.io = io;
                loopback = (ioAddr_.addr() == INADDR_LOOPBACK);
            }
            continue;
        }

        // Interrupted?
        // Ignore illegitimate commands.
        if ((cmdSize < 0) || (rawCmd[cmdSize - 1] != 0) ||
            (loopback && (src.addr() != INADDR_LOOPBACK)) ||
            ((!loopback) && (!allowAccessFrom(src.addr()))))
        {
            continue;
        }

        // Give standard response to unknown commands.
        // Allow comments to be logged like a no-op command if command logging is enabled.
        ioPort->set(ioPort->asWord() & 0xffff0000U | src.port()); //MSB=dbuggee-port, LSB=dbugger-port
        cmdTag.rspAddr = loopback? INADDR_LOOPBACK: src.addr();
        cmdTag.rspPort = src.port();
        cmd.reset8(rawCmd, cmdSize);
        req = cmd;
        const String* cmdName = req.arg(0, &defaultCmdName);
        cmdTag.cmd = map_->find(cmdName->ascii(), cmdTag.cmdIndex);
        if (cmdTag.cmd == 0)
        {
            if ((!cmdName->empty()) && (*cmdName->ascii() == COMMENT_INDICATOR))
            {
                if ((logMask_ & LogReq) != 0)
                {
                    bool flushLogEntries = ((logMask_ & FlushLogEntries) != 0);
                    logReq(req, src, cmdTag.log, flushLogEntries);
                }
                unsigned char emptyRsp = 0;
                io->snd(&emptyRsp, 1, src, timeoutInMsecs);
            }
            else
            {
                io->snd((const unsigned char*)(cmdName->ascii()), cmdName->length(), src, timeoutInMsecs);
                io->snd((const unsigned char*)(UNKNOWN_RSP), sizeof(UNKNOWN_RSP), src, timeoutInMsecs);
            }
            continue;
        }

        // Give usage if asked.
        // Give application an opportunity to disallow running the command.
        cmdTag.alwaysLogReq = false;
        cmdTag.alwaysLogRsp = false;
        if (showUsage(req, *cmdTag.cmd) || (!onReq(req, src)))
        {
            continue;
        }

        // Run command.
        // Give usage if some hints seem helpful.
        if (!cmdTag.cmd->run(req))
        {
            const char* usage = cmdTag.cmd->usage(cmdTag.cmdIndex);
            cmdTag.cmd->respond(req, usage);
        }

        if ((++cmdCount % LOG_FILE_CAP) == 0)
        {
            curLog_[index] = logPath.reopen();
            cmdTag.log = curLog_[index];
        }
    }
}


//
// Create socket to serve as channel for command inputs/outputs.
// Save bound port in MSB of ioPort.
//
void SysIo::mkIo(size_t index)
{

    // Determine address to bind to.
    unsigned int addr;
    unsigned short port;
    if (index == 0)
    {
        addr = INADDR_LOOPBACK;
        port = 0U;
    }
    else //1
    {
        addr = ioAddr_.addr();
        port = ioAddr_.port();
    }

    // Bind and save effective I/O address.
    // Consider port binding optional.
    UdpClient* io = new UdpClient(ibufSize_, obufSize_);
    bind(*io, addr, port);
    Paddr myPaddr;
    io->getMyAddr(myPaddr);
    addr_[index] = myPaddr.addr();
    io_[index] = io;
    ioPort_[index].set(static_cast<unsigned int>(myPaddr.port()) << 16);
}


//
// Flush buffered I/O at crash.
//
void SysIo::onCrash(void* arg)
{
    const SysIo* sysIo = static_cast<const SysIo*>(arg);
    std::fflush(sysIo->curLog_[0]);
    std::fflush(sysIo->curLog_[1]);
    sysIo->oldCrashCb_(sysIo->oldCrashArg_);
}


//!
//! This method is invoked when some response has been generated (successful or not).
//! The default implementation provides logging of the response if logging is enabled.
//!
void SysIo::onRsp(const CmdLine& req, const char* rsp, size_t rspLength, bool ok)
{
    Cmd::tag_t& tag = *static_cast<Cmd::tag_t*>(req.tag());
    if (((logMask_ & LogRsp) == 0) && (!tag.alwaysLogRsp))
    {
        return;
    }

    // Form timestamp (example: "\n|08/28/09 15:46:13|1|\n").
    char stamp[1 + 32 + 3];
    const char* tail = 0;
    size_t tailSize = 0;
    if (rsp[rspLength - 1] == 0)
    {
        struct tm now;
        time_t t = time(0);
        localtime_s(&now, &t);
        stamp[0] = NEW_LINE[0];
        tail = stamp;
        tailSize = 1;
        tailSize += strftime(stamp + 1, 32, "|%c|", &now);
        stamp[tailSize++] = ok? '1': '0';
        stamp[tailSize++] = '|';
        stamp[tailSize++] = NEW_LINE[0];
        if (--rspLength > 0)
        {
            if (rsp[rspLength - 1] == NEW_LINE[0])
            {
                ++tail;
                --tailSize;
            }
        }
        else if (tag.rspTail == NEW_LINE[0])
        {
            ++tail;
            --tailSize;
        }
    }
    else
    {
        tag.rspTail = rsp[rspLength - 1];
    }

    // Log response without terminating null.
    // Log timestamp at end-of-response.
    std::FILE* log = tag.log;
    size_t count = 1;
    std::fwrite(rsp, rspLength, count, log);
    if (tail != 0)
    {
        std::fwrite(tail, tailSize, count, log);
    }
}


//!
//! Update the access control list. An empty set allows remote access to all if
//! remote access is enabled. A non-empty set allows remote access from only those
//! IPv4 addresses in the set.
//!
void SysIo::setAcl(const IpAddrSet& acl)
{
    SpinSection::Lock lock(ss_);
    *acl_ = acl;
}


//!
//! Enable/Disable logging. By default, requests are logged and responses are not
//! logged. Response logging can be enabled only if request logging is also enabled.
//! Log entries are buffered and flushed as needed. To flush entries as soon as
//! they are logged, set flushLogEntries to true.
//!
void SysIo::setLogMode(bool logReq, bool logRsp, bool flushLogEntries)
{
    unsigned int flushMask = flushLogEntries? FlushLogEntries: 0;
    unsigned int logMask = logReq? (logRsp? (LogReq | LogRsp | flushMask): (LogReq | flushMask)): (flushMask);
    if (logMask != logMask_)
    {
        logMask_ = logMask;
        std::fflush(curLog_[0]);
        std::fflush(curLog_[1]);
    }
}


//
// Entry point for each of the dedicated command agent threads.
//
void* SysIo::agentEntry(void* arg)
{
    const runArg_t* runArg = static_cast<const runArg_t*>(arg);
    SysIo* sysIo = runArg->sysIo;
    size_t index = runArg->index;
    delete runArg;

    sysIo->loop(index);
    return 0;
}

END_NAMESPACE1
