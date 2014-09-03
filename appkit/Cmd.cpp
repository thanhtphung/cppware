/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <stdarg.h>
#include "netkit/Paddr.hpp"
#include "netkit/UdpClient.hpp"
#include "syskit/Atomic32.hpp"
#include "syskit/Atomic64.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/Cmd.hpp"
#include "appkit/CmdLine.hpp"
#include "appkit/CmdMap.hpp"
#include "appkit/String.hpp"
#include "appkit/SysIo.hpp"
#include "appkit/crt.hpp"

using namespace appkit;
using namespace netkit;
using namespace syskit;

const size_t MAX_PKT_LENGTH = 1456; //bytes
const unsigned int SND_TIMEOUT = 1234; //msecs

BEGIN_NAMESPACE

class PrimitiveIo: public SysIo
{
public:
    using SysIo::ioPort;
    PrimitiveIo();
    const String& rsp() const;
    virtual ~PrimitiveIo();
    virtual bool onReq(const CmdLine& req, const Paddr& dbugger);
    virtual void onRsp(const CmdLine& req, const char* rsp, size_t rspLength, bool ok);
private:
    String rsp_;
    PrimitiveIo(const PrimitiveIo&); //prohibit usage
    const PrimitiveIo& operator =(const PrimitiveIo&); //prohibit usage
};

inline const String& PrimitiveIo::rsp() const
{
    return rsp_;
}

PrimitiveIo::PrimitiveIo():
SysIo(UdpClient::createNull(), UdpClient::createNull()),
rsp_()
{
}

PrimitiveIo::~PrimitiveIo()
{
}

bool PrimitiveIo::onReq(const CmdLine& /*req*/, const Paddr& /*dbugger*/)
{
    rsp_.reset();
    bool allowReq = true;
    return allowReq;
}

void PrimitiveIo::onRsp(const CmdLine& /*req*/, const char* rsp, size_t rspLength, bool /*ok*/)
{
    rsp_.append(rsp, rspLength);
}

END_NAMESPACE

BEGIN_NAMESPACE1(appkit)


//!
//! Construct command handler for given command set.
//!
Cmd::Cmd(const char* nameSet)
{
    map_ = CmdMap::instance();
    map_->addRef();
    nameSet_ = nameSet;
    unsigned int numMappedNames = map_->add(this, numErrs_);
    numNames_ = numMappedNames + numErrs_;
    unsigned int nameCount = (numNames_ < MaxNames)? numNames_: MaxNames;
    count_ = new Atomic32[nameCount * CountsPerName];
    count64_ = new Atomic64[nameCount * Count64sPerName];
}


Cmd::~Cmd()
{
    delete[] count64_;
    delete[] count_;
    map_->rm(this);
    map_->rmRef();
}


//!
//! This method is invoked when a command is being authenticated. The method returns
//! true/false to allow/disallow the command execution. The default implementation
//! returns true.
//!
bool Cmd::allowReq(const CmdLine& /*req*/, const Paddr& /*dbugger*/)
{
    bool allow = true;
    return allow;
}


//!
//! Form and send given null-terminated response. Assume response is
//! partial if not double-null-terminated and complete otherwise. For
//! examples, formRsp(req, "partial") indicates a partial response and
//! formRsp(req, "complete%c", 0) indicates a complete response. Return
//! true if successful.
//!
bool Cmd::formRsp(const CmdLine& req, const char* format, ...)
{
    bool ok;
    const tag_t& tag = *static_cast<const tag_t*>(req.tag());
    if (tag.rspPort == (tag.ioPort->asWord() & 0x0000ffffU))
    {
        va_list marker;
        va_start(marker, format);
        char rsp[MaxRspLength + 1];
        int rspLength = vsnprintf(rsp, sizeof(rsp), format, marker);
        va_end(marker);
        ok = ((rspLength > 0) && (rspLength < (int)(sizeof(rsp))))?
            tag.cmd->sndRsp(req, rsp, rspLength):
            false;
    }
    else
    {
        ok = false;
    }

    return ok;
}


//!
//! Respond with given null-terminated response. Assume response is complete.
//! Return true if successful. Use the respond(const CmdLine&, const char*, size_t)
//! or formRsp(const CmdLine&, const char*, ...) variance for a partial response.
//!
bool Cmd::respond(const CmdLine& req, const char* rsp)
{
    const tag_t& tag = *static_cast<const tag_t*>(req.tag());
    bool ok = (tag.rspPort == (tag.ioPort->asWord() & 0x0000ffffU))?
        tag.cmd->sndRsp(req, rsp, strlen(rsp) + 1):
        false;

    return ok;
}


//!
//! Respond with given response (rspLength bytes starting at rsp). Assume
//! response is partial if not null-terminated and complete otherwise. For
//! examples, respond(req, "partial", 7) indicates a partial response and
//! respond(req, "complete", 9) indicates a complete response. Return true
//! if successful.
//!
bool Cmd::respond(const CmdLine& req, const char* rsp, size_t rspLength)
{
    const tag_t& tag = *static_cast<const tag_t*>(req.tag());
    bool ok = (tag.rspPort == (tag.ioPort->asWord() & 0x0000ffffU))?
        tag.cmd->sndRsp(req, rsp, rspLength):
        false;

    return ok;
}


//!
//! Respond with given null-terminated response. Assume response is complete.
//! Return true if successful. Use the respond(const CmdLine&, const char*, size_t)
//! or formRsp(const CmdLine&, const char*, ...) variance for a partial response.
//!
bool Cmd::respond(const CmdLine& req, const String& rsp)
{
    const tag_t& tag = *static_cast<const tag_t*>(req.tag());
    bool ok = (tag.rspPort == (tag.ioPort->asWord() & 0x0000ffffU))?
        tag.cmd->sndRsp(req, rsp.raw(), rsp.byteSize()):
        false;

    return ok;
}


bool Cmd::sndRsp(const CmdLine& req, const void* rsp, size_t rspLength)
{
    const tag_t& tag = *static_cast<const tag_t*>(req.tag());
    UdpClient* io = tag.io;
    Paddr src(tag.rspAddr, tag.rspPort);
    size_t countIndex = tag.cmdIndex * CountsPerName;

    // Allow empty responses.
    // Packetize long responses.
    bool ok;
    const unsigned char* pkt = static_cast<const unsigned char*>(rsp);
    size_t more = rspLength;
    unsigned int pktCount = 0;
    unsigned int timeoutInMsecs = SND_TIMEOUT;
    do
    {
        size_t pktLength = (more > MAX_PKT_LENGTH)? MAX_PKT_LENGTH: more;
        ok = io->snd(pkt, pktLength, src, timeoutInMsecs);
        ++pktCount;
        pkt += pktLength;
        more -= pktLength;
    } while ((more > 0) && ok);

    // Collect some stats.
    if (ok)
    {
        count_[countIndex + RspPkts] += pktCount;
        size_t count64Index = tag.cmdIndex * Count64sPerName;
        count64_[count64Index + RspBytes] += rspLength;
    }
    else
    {
        ++count_[countIndex + RspFails];
    }

    // Give application an opportunity to look at the response.
    SysIo* sysIo = tag.sysIo;
    sysIo->onRsp(req, static_cast<const char*>(rsp), rspLength, ok);
    return ok;
}


bool Cmd::run(const CmdLine& req)
{
    const tag_t& tag = *static_cast<const tag_t*>(req.tag());
    size_t countIndex = tag.cmdIndex * CountsPerName;
    ++count_[countIndex + Runs];
    bool ok = onRun(req); //allow commands to be self-destructive at onRun() -- avoid further Cmd usage
    return ok;
}


//!
//! A command is typically run using a few components in the framework including
//! CmdAgent (command client) and SysIo (command server). That is sometimes not
//! feasible. This method offers a way to run (i.e., sprint) commands w/o those
//! components. With this method, the onRun() method is invoked by the caller thread
//! (vs. a dedicated thread), and there's no client/server communications. Return
//! true if successful.
//!
bool Cmd::sprint(const CmdLine& req, String& rsp)
{
    bool ok = false;

    PrimitiveIo sysIo;
    const String* cmdName = req.arg(0);
    if (cmdName != 0)
    {
        UdpClient* io = UdpClient::createNull();
        tag_t cmdTag(&sysIo, io, stdout, sysIo.ioPort(0));
        cmdTag.cmd = map_->find(cmdName->ascii(), cmdTag.cmdIndex);
        if (cmdTag.cmd == this)
        {
            void* oldTag = req.tag();
            req.setTag(&cmdTag);
            size_t countIndex = cmdTag.cmdIndex * CountsPerName;
            ++count_[countIndex + Sprints];
            ok = onRun(req); //allow commands to be self-destructive at onRun() -- avoid further Cmd usage
            req.setTag(oldTag);
        }

        delete io;
    }

    rsp = sysIo.rsp();
    return ok;
}


//!
//! Default implementation.
//! No usage available.
//!
const char* Cmd::usage(unsigned char /*cmdIndex*/) const
{
    return "";
}


//!
//! Default implementation.
//! No extended name available.
//!
const char* Cmd::xName(unsigned char /*cmdIndex*/) const
{
    return "";
}


//!
//! Default implementation.
//! No extended usage available.
//!
const char* Cmd::xUsage(unsigned char /*cmdIndex*/) const
{
    return "";
}


//!
//! Return the number of times given command has run. Also return a few more related stats.
//!
unsigned int Cmd::numRuns(unsigned char cmdIndex, unsigned long long& bytes, unsigned int& pkts, unsigned int& fails) const
{
    size_t countIndex = cmdIndex * CountsPerName;
    pkts = count_[countIndex + RspPkts];
    fails = count_[countIndex + RspFails];
    unsigned int runs = count_[countIndex + Runs];

    size_t count64Index = cmdIndex * Count64sPerName;
    bytes = count64_[count64Index + RspBytes];

    return runs;
}


//!
//! Return the number of times given command has been sprinted (i.e., internally run).
//!
unsigned int Cmd::numSprints(unsigned char cmdIndex) const
{
    size_t countIndex = cmdIndex * CountsPerName;
    unsigned int sprints = count_[countIndex + Sprints];
    return sprints;
}


Cmd::tag_s::tag_s(SysIo* sysIo, UdpClient* io, std::FILE* log, Atomic32* ioPort)
{
    this->alwaysLogReq = false;
    this->alwaysLogRsp = false;
    this->cmd = 0;
    this->cmdIndex = 0;
    this->io = io;
    this->ioPort = ioPort;
    this->log = log;
    this->rspAddr = 0;
    this->rspPort = 0;
    this->rspTail = 0;
    this->sysIo = sysIo;
}

END_NAMESPACE1
