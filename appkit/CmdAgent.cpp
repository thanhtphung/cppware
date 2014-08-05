/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <time.h>
#include "netkit/IpAddr.hpp"
#include "netkit/IpAddrSet.hpp"
#include "netkit/IpDevice.hpp"
#include "netkit/Paddr.hpp"
#include "netkit/Subnet.hpp"
#include "netkit/UdpClient.hpp"
#include "syskit/MappedTxtFile.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/Cmd.hpp"
#include "appkit/CmdAgent.hpp"
#include "appkit/DelimitedTxt.hpp"
#include "appkit/S32.hpp"
#include "appkit/String.hpp"
#include "appkit/StringDic.hpp"
#include "appkit/StringVec.hpp"
#include "appkit/SysIo.hpp"
#include "appkit/crt.hpp"

using namespace appkit;
using namespace netkit;
using namespace syskit;

const char COLON = ':';
const int IBUF_SIZE = 458752; //bytes (448k)
const int OBUF_SIZE = 196608; //bytes (192k)

BEGIN_NAMESPACE

// No-op if rspCopy is zero. Append given response to the copy otherwise.
static void saveRsp(String* rspCopy, const utf8_t* rsp, size_t rspSize)
{
    if (rspCopy != 0)
    {
        String s;
        s.reset8(rsp, rspSize);
        *rspCopy += s;
    }
}

// CmdAgent uses stdout by default.
// This QuietCmdAgent class suppresses the stdout usage.
class QuietCmdAgent: public CmdAgent
{
public:
    QuietCmdAgent(const CmdLine& config);
    virtual ~QuietCmdAgent();
    virtual void echoReq(const String& req);
    virtual void onRsp(const char* rsp, size_t rspLength, bool more);
    virtual void onTimeout();
private:
    QuietCmdAgent(const QuietCmdAgent&); //prohibit usage
    const QuietCmdAgent& operator =(const QuietCmdAgent&); //prohibit usage
};

QuietCmdAgent::QuietCmdAgent(const CmdLine& config):
CmdAgent(config)
{
}

QuietCmdAgent::~QuietCmdAgent()
{
}

void QuietCmdAgent::echoReq(const String& /*req*/)
{
}

void QuietCmdAgent::onRsp(const char* /*rsp*/, size_t /*rspLength*/, bool /*more*/)
{
}

void QuietCmdAgent::onTimeout()
{
}

END_NAMESPACE

BEGIN_NAMESPACE1(appkit)

const unsigned int CmdAgent::ETERNITY = UdpClient::ETERNITY;


//!
//! Construct agent relaying messages between a debugger and a debuggee.
//! The agent acts on behalf of a debugger, and debug commands are run in
//! the debuggee's context. Allowed options in config: dbuggee (required),
//! dbugger, ibufsiz, obufsize. When a command is run, some response must
//! be seen within timeout0InMsecs msecs. If the response is partial, each
//! subsequent message fragment must be seen within timeoutInMsecs msecs.
//! Otherwise, the command run is assumed to have failed.
//!
CmdAgent::CmdAgent(const CmdLine& config, unsigned int timeout0InMsecs, unsigned int timeoutInMsecs):
dbuggee_(0U /*addr*/, 0U /*port*/),
dbugger_(0U /*addr*/, 0U /*port*/)
{
    errDesc_ = 0;
    io_ = 0;
    rspTail_ = 0;
    timeout_ = timeoutInMsecs;
    timeout0_ = timeout0InMsecs;

    mkIo(config);
    connect(config);
}


//!
//! Construct agent relaying messages between a debugger and a debuggee.
//! The agent acts on behalf of a debugger, and debug commands are run in
//! the debuggee's context. When a command is run, some response must
//! be seen within timeout0InMsecs msecs. If the response is partial, each
//! subsequent message fragment must be seen within timeoutInMsecs msecs.
//! Otherwise, the command run is assumed to have failed.
//!
CmdAgent::CmdAgent(const Paddr& dbuggee, unsigned int timeout0InMsecs, unsigned int timeoutInMsecs):
dbuggee_(dbuggee),
dbugger_(0U /*addr*/, 0U /*port*/)
{
    errDesc_ = 0;
    io_ = 0;
    rspTail_ = 0;
    timeout_ = timeoutInMsecs;
    timeout0_ = timeout0InMsecs;

    CmdLine config;
    mkIo(config);
    connect(config);
}


//!
//! Construct minimal agent. Take over ownership of the given UdpClient object
//! which will be destroyed by CmdAgent when appropriate.
//!
CmdAgent::CmdAgent(UdpClient* io):
dbuggee_(0U /*addr*/, 0U /*port*/),
dbugger_(0U /*addr*/, 0U /*port*/)
{
    errDesc_ = 0;
    io_ = io;
    rspTail_ = 0;
    timeout_ = Timeout;
    timeout0_ = Timeout0;
}


CmdAgent::~CmdAgent()
{
    delete io_;
}


//!
//! Create a loopback agent. Returned agent must be deleted using the delete operator
//! when done. This agent communicates quietly with the given I/O subsystem at its
//! loopback port.
//!
CmdAgent* CmdAgent::createLoopback(const SysIo* sysIo)
{
    CmdLine agentConfig(sysIo->clientConfig(0));
    CmdAgent* loopbackAgent = new QuietCmdAgent(agentConfig);
    return loopbackAgent;
}


//!
//! Form command echo used in logging. This echo contains current timestamp and
//! the debuggee's address. For example, "|10/25/11 15:46:11|10.0.4.82:1911| ".
//!
String CmdAgent::formReqEcho(const String& req) const
{

    // Form timestamp (example: "|10/25/11 15:46:11|10.0.4.82:1911| ").
    struct tm now;
    time_t now0 = time(0);
    localtime_s(&now, &now0);
    char stamp[32 + Paddr::StrLength + 2];
    size_t size = strftime(stamp, 32, "\n|%c|", &now);
    size += dbuggee_.toDigits(stamp + size, COLON);
    stamp[size++] = '|';
    stamp[size++] = ' ';

    String echo(stamp, size);
    echo += req;
    echo += '\n';
    return echo;
}


//!
//! Ping debuggees at given subnet using given port. Return responsive
//! dbuggees. The returned value is a dictionary with responsive debuggee
//! addresses (e.g., "10.0.4.82:1911") as keys and corresponding responses
//! (e.g., "pong") as values.
//!
StringDic CmdAgent::broadcast(const String& ping, const Subnet& subnet, unsigned short port)
{
    StringDic pong;
    UdpClient io(IBUF_SIZE, OBUF_SIZE);
    Paddr dst(subnet.broadcastAddr(), port);
    bool ok = io.bind(subnet.addr(), 0) && io.snd(ping.raw(), ping.byteSize(), dst);
    if (ok)
    {
        Paddr src;
        String k;
        String v;
        utf8_t rawRsp[Cmd::MaxRspLength + 1];
        for (unsigned int timeoutInMsecs = 765;; timeoutInMsecs = 234)
        {
            int rspSize = io.rcv(rawRsp, sizeof(rawRsp), src, timeoutInMsecs);
            if (rspSize > 1)
            {
                if (rawRsp[rspSize - 1] == 0)
                {
                    k = src;
                    v.reset8(rawRsp, rspSize);
                    pong.associate(k, v);
                }
                continue;
            }
            break;
        }
    }

    return pong;
}


//!
//! Ping debuggees at connected subnets using given port. Return responsive debuggees.
//! The returned value is a dictionary with responsive debuggee addresses (e.g.,
//! "10.0.4.82:1911") as keys and corresponding responses (e.g., "pong") as
//! values. Ignore zero (0.0.0.0) interfaces.
//!
StringDic CmdAgent::broadcast(const String& ping, unsigned short port)
{
    IpAddrSet pinged;
    StringDic pong;
    Subnet subnet;
    for (unsigned char i = 0, numDevices = IpDevice::numDevices(); i < numDevices; ++i)
    {
        const IpDevice* d = IpDevice::instance(i);
        unsigned int numAddrs = d->numAddrs();
        for (unsigned int j = 0; j < numAddrs; ++j)
        {
            unsigned int addr = d->addr(j);
            if (addr != 0)
            {
                subnet.reset(addr, d->netMask(j));
                if (!pinged.contains(subnet.broadcastAddr()))
                {
                    broadcast(ping, subnet, port).mergeInto(pong);
                    pinged.add(subnet.broadcastAddr());
                }
            }
        }
    }

    return pong;
}


//!
//! Return true if instance was successfully constructed.
//!
bool CmdAgent::isOk() const
{
    bool ok = ((dbugger_.port() != 0) && (dbuggee_.addr() != 0));
    return ok;
}


//!
//! Queue all commands in given script. Commands are then run asynchronously in
//! the debuggee's context.
//!
bool CmdAgent::queueScript(const MappedTxtFile& script, unsigned int timeoutInMsecs)
{
    bool ok = true;
    size_t fileSize = static_cast<size_t>(script.size());
    bool makeCopy = false;
    DelimitedTxt txt(reinterpret_cast<const char*>(script.map(0U /*index*/)), fileSize, makeCopy);
    String event;
    for (bool doTrimLine = true; txt.next(event, doTrimLine);)
    {
        if (!event.empty())
        {
            if (!sndEvent(event, dbuggee_, timeoutInMsecs))
            {
                ok = false;
            }
        }
    }

    return ok;
}


//!
//! Run given command synchronously in the debuggee's context. Invoke echoReq()
//! when command has just been sent successfully. Invoke onRsp() when some
//! response is received. Invoke onTimeout() when timeout occurs waiting for
//! a response. If rspCopy is non-zero, save response there upon return. Return
//! true if successful. Return false if timed out.
//!
bool CmdAgent::runCmd(const String& cmd, String* rspCopy)
{
    flush();
    unsigned int timeoutInMsecs = UdpClient::ETERNITY;
    bool ok = io_->snd(cmd.raw(), cmd.byteSize(), dbuggee_, timeoutInMsecs);
    if (ok)
    {
        rspTail_ = 0;
        echoReq(cmd);
        ok = showRsp(rspCopy);
    }

    return ok;
}


//!
//! Run given commands synchronously. Treat each command-line argument as
//! one command to be run in the debuggee's context. Invoke echoReq() when
//! a command has just been sent successfully. Invoke onRsp() when some
//! response is received. Invoke onTimeout() when timeout occurs waiting
//! for a response. If rspCopies is non-zero, save responses there upon return.
//! If Return true if successful. Return false if timed out.
//!
bool CmdAgent::runCmds(const CmdLine& cmdLine, StringVec* rspCopies)
{
    if (rspCopies != 0)
    {
        rspCopies->reset();
    }

    bool ok = true;
    unsigned int numArgs = cmdLine.numArgs();
    if (numArgs > 1)
    {
        String s;
        String* rspCopy = (rspCopies == 0)? (0): (&s);
        for (unsigned int i = 1; (i < numArgs) && ok; ++i)
        {
            flush();
            const String* req = cmdLine.arg(i);
            unsigned int timeoutInMsecs = UdpClient::ETERNITY;
            ok = io_->snd(req->raw(), req->byteSize(), dbuggee_, timeoutInMsecs);
            if (ok)
            {
                rspTail_ = 0;
                echoReq(*req);
                ok = showRsp(rspCopy);
                if (rspCopy != 0)
                {
                    rspCopies->add(*rspCopy);
                }
            }
        }
    }

    return ok;
}


//
// Show response. Return true if successful. Return false if timed out.
//
bool CmdAgent::showRsp(String* rspCopy)
{
    if (rspCopy != 0)
    {
        rspCopy->reset();
    }

    bool ok = true;
    utf8_t rsp[Cmd::MaxRspLength + 1];
    for (unsigned int timeoutInMsecs = timeout0_;; timeoutInMsecs = timeout_)
    {
        int rspSize = io_->rcv(rsp, sizeof(rsp), timeoutInMsecs);
        if (rspSize > 0)
        {

            // Show partial response, then wait for more.
            unsigned char eor = rsp[rspSize - 1];
            if (eor != 0)
            {
                bool more = true;
                onRsp(reinterpret_cast<const char*>(rsp), rspSize, more);
                saveRsp(rspCopy, rsp, rspSize);
                continue;
            }

            // Show complete response, then return ASAP.
            else if (rspSize > 1)
            {
                bool more = false;
                onRsp(reinterpret_cast<const char*>(rsp), rspSize - 1, more);
                saveRsp(rspCopy, rsp, rspSize - 1);
            }
        }
        else
        {
            onTimeout();
            ok = false;
        }
        break;
    }

    return ok;
}


//!
//! Send arbitrary event to a third-party destination which can also be the debuggee.
//! Return true if successful.
//!
bool CmdAgent::sndEvent(const String& event, const Paddr& thirdParty, unsigned int timeoutInMsecs)
{
    flush();
    bool ok = io_->snd(event.raw(), event.byteSize(), thirdParty, timeoutInMsecs);
    return ok;
}


//
// Bind debugger socket. Use port address specified by the debugger option if
// available. Use given default address otherwise.
//
void CmdAgent::bind(const CmdLine& config, unsigned int defaultAddr)
{

    // Use default dbugger address.
    unsigned short port = 0U;
    dbugger_.reset(defaultAddr, port);

    // Use specified dbugger address.
    String optK("dbugger");
    const String* dbugger = config.opt(optK, 0 /*defaultV*/);
    if (dbugger != 0)
    {
        bool portIsOptional = true;
        char delim = COLON;
        dbugger_.reset(dbugger->ascii(), delim, portIsOptional);
    }

    // Bind dbugger socket.
    // Consider port binding optional.
    bool ok = io_->bind(dbugger_.addr(), dbugger_.port());
    if ((!ok) && (dbugger_.port() != 0))
    {
        io_->bind(dbugger_.addr(), port);
    }

    // Save effective dbugger address.
    io_->getMyAddr(dbugger_);
}


void CmdAgent::connect(const CmdLine& config)
{

    // dbuggee option is required.
    String optK("dbuggee");
    const String* dbuggee = config.opt(optK, 0 /*defaultV*/);
    if (dbuggee == 0)
    {
        errDesc_ = "dbuggee option is required";
        return;
    }

    // Prepare to communicate with debuggee.
    char delim = COLON;
    bool portIsOptional = false;
    if (dbuggee_.reset(dbuggee->ascii(), delim, portIsOptional))
    {
        bind(config, INADDR_ANY);
        io_->connect(dbuggee_);
        return;
    }

    errDesc_ = "debuggee not found";
}


//!
//! This method is invoked when a command has just been sent successfully.
//! The default implementation provides a command echo.
//!
void CmdAgent::echoReq(const String& req)
{
    String echo(formReqEcho(req));
    size_t count = 1;
    size_t size = echo.byteSize() - 1;
    std::fwrite(echo.ascii(), size, count, stdout);
}


//
// Flush stale response, if any.
//
void CmdAgent::flush()
{
    utf8_t rsp[Cmd::MaxRspLength + 1];
    unsigned int timeoutInMsecs = 0;
    while (io_->rcv(rsp, sizeof(rsp), timeoutInMsecs) > 0);
}


//
// Create socket to serve as channel for command inputs/outputs.
//
void CmdAgent::mkIo(const CmdLine& config)
{
    String optK("ibufsize");
    int ibufSize = S32(config.opt(optK), IBUF_SIZE /*defaultV*/);
    optK = "obufsize";
    int obufSize = S32(config.opt(optK), OBUF_SIZE /*defaultV*/);

    io_ = new UdpClient(ibufSize, obufSize);
}


//!
//! This method is invoked when some response (rspLength bytes starting at
//! rsp) has been received. This response is not null-terminated if more is
//! expected, and is null-terminated otherwise. However, rspLength does not
//! include the terminating null. The default implementation writes the
//! response to standard output.
//!
void CmdAgent::onRsp(const char* rsp, size_t rspLength, bool more)
{
    rspTail_ = rsp[rspLength - 1];
    size_t count = 1;
    std::fwrite(rsp, rspLength, count, stdout);
    if ((!more) && (rspTail_ != '\n'))
    {
        size_t size = 1;
        rspTail_ = '\n';
        std::fwrite(&rspTail_, size, count, stdout);
    }
}


//!
//! This method is invoked when a failure (typically timeout) occurred waiting
//! for response.
//!
void CmdAgent::onTimeout()
{
    if ((rspTail_ != 0) && (rspTail_ != '\n'))
    {
        size_t size = 1;
        size_t count = 1;
        rspTail_ = '\n';
        std::fwrite(&rspTail_, size, count, stdout);
    }
}

END_NAMESPACE1
