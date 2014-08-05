/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_CMD_HPP
#define APPKIT_CMD_HPP

#include <cstdio>
#include <string>
#include "syskit/sys.hpp"

DECLARE_CLASS1(netkit, Paddr)
DECLARE_CLASS1(netkit, UdpClient)
DECLARE_CLASS1(syskit, Atomic32)
DECLARE_CLASS1(syskit, Atomic64)

BEGIN_NAMESPACE1(appkit)

class CmdLine;
class CmdMap;
class String;
class SysIo;


//! command handler
class Cmd
    //!
    //! A class representing a command handler. The onRun() method is invoked when
    //! a command is run. The method should return true/false to indicate command
    //! validation success/failure. A failure will result in a command usage hint.
    //! The method is invoked in a dedicated agent task. Each Cmd instance is
    //! associated w/ one or more commands (e.g., one Cmd class can be used to
    //! implement both "task-suspend" and "task-resume"), and each command is
    //! associated w/ at most one Cmd instance. The association is specified when
    //! a Cmd instance is constructed. Command outputs are required and must be
    //! generated using the formRsp() and/or respond() methods. Multiple onRun()
    //! invocations can occur concurrently.
    //!
{

public:
    enum
    {
        MaxCmdLength = 4095,
        MaxNames = 256,
        MaxRspLength = 4095
    };

    CmdMap* map();
    const CmdMap* map() const;
    const char* nameSet() const;
    unsigned int numErrs() const;
    unsigned int numNames() const;
    unsigned int numRuns(unsigned char cmdIndex, unsigned long long& bytes, unsigned int& pkts, unsigned int& fails) const;
    unsigned int numSprints(unsigned char cmdIndex) const;

    virtual ~Cmd();
    virtual bool onRun(const CmdLine& req) = 0;
    virtual bool sprint(const CmdLine& req, String& rsp);
    virtual const char* usage(unsigned char cmdIndex) const;
    virtual const char* xName(unsigned char cmdIndex) const;
    virtual const char* xUsage(unsigned char cmdIndex) const;

    static bool formRsp(const CmdLine& req, const char* format, ...) GCC_ATTR((format(printf, 2, 3)));
    static bool respond(const CmdLine& req, const char* rsp);
    static bool respond(const CmdLine& req, const char* rsp, size_t rspLength);
    static bool respond(const CmdLine& req, const String& rsp);

protected:
    Cmd(const char* nameSet);

    virtual bool allowReq(const CmdLine& req, const netkit::Paddr& dbugger);

    static std::FILE* log(const CmdLine& req);
    static unsigned char cmdIndex(const CmdLine& req);
    static unsigned int rspAddr(const CmdLine& req);
    static unsigned short reqPort(const CmdLine& req);
    static unsigned short rspPort(const CmdLine& req);
    static void overrideLogMode(const CmdLine& req, bool alwaysLogReq = true, bool alwaysLogRsp = false);

private:
    enum count_e
    {
        RspFails = 0,
        RspPkts,
        Runs,
        Sprints
    };

    enum count64_e
    {
        RspBytes = 0
    };

    enum
    {
        CountsPerName = Sprints + 1,
        Count64sPerName = RspBytes + 1
    };

    typedef struct tag_s
    {
        tag_s(SysIo* sysIo, netkit::UdpClient* io, std::FILE* log, syskit::Atomic32* ioPort);
        Cmd* cmd;
        SysIo* sysIo;
        netkit::UdpClient* io;
        std::FILE* log;
        syskit::Atomic32* ioPort;
        bool alwaysLogReq;
        bool alwaysLogRsp;
        unsigned int rspAddr;
        unsigned short rspPort;
        unsigned char cmdIndex;
        unsigned char rspTail;
    } tag_t;

    CmdMap* map_;
    syskit::Atomic32* count_;
    syskit::Atomic64* count64_;
    const char* nameSet_;
    unsigned int numNames_;
    unsigned int numErrs_;

    Cmd(const Cmd&); //prohibit usage
    const Cmd& operator =(const Cmd&); //prohibit usage

    bool run(const CmdLine&);
    bool sndRsp(const CmdLine&, const void*, size_t);

    friend class SysIo;

};

END_NAMESPACE1

#include "appkit/CmdLine.hpp"

BEGIN_NAMESPACE1(appkit)

inline CmdMap* Cmd::map()
{
    return map_;
}

//! Return the log file used for command logging. Functional only when the
//! given command is being run (i.e., while onRun(req) is being executed).
inline std::FILE* Cmd::log(const CmdLine& req)
{
    const tag_t* tag = static_cast<tag_t*>(req.tag());
    return tag->log;
}

inline const CmdMap* Cmd::map() const
{
    return map_;
}

//! Return the associated command set.
inline const char* Cmd::nameSet() const
{
    return nameSet_;
}

//! Return the command index. Command index is the index of the invoked
//! command in the original command set. For example, if the original
//! command set is "task-reinit task-restart task-shutdown", the command
//! index is 0 for "task-reinit", is 1 for "task-restart", and is 2 for
//! "task-shutdown". Functional only when the given command is being run
//! (i.e., while onRun(req) is being executed).
inline unsigned char Cmd::cmdIndex(const CmdLine& req)
{
    const tag_t* tag = static_cast<tag_t*>(req.tag());
    return tag->cmdIndex;
}

//! Return the response address (dbugger's address). Functional only when the
//! given command is being run (i.e., while onRun(req) is being executed).
inline unsigned int Cmd::rspAddr(const CmdLine& req)
{
    const tag_t* tag = static_cast<tag_t*>(req.tag());
    return tag->rspAddr;
}

//! Return the request port (dbuggee's port). Functional only when the
//! given command is being run (i.e., while onRun(req) is being executed).
inline unsigned short Cmd::reqPort(const CmdLine& req)
{
    const tag_t* tag = static_cast<tag_t*>(req.tag());
    unsigned short port = static_cast<unsigned short>(tag->ioPort->asWord() >> 16);
    return port;
}

//! Return the response port (dbugger's port). Functional only when the
//! given command is being run (i.e., while onRun(req) is being executed).
inline unsigned short Cmd::rspPort(const CmdLine& req)
{
    const tag_t* tag = static_cast<tag_t*>(req.tag());
    return tag->rspPort;
}

//! Return the number of erroneous (duplicate or overlimit) names in the
//! associated command set.
inline unsigned int Cmd::numErrs() const
{
    return numErrs_;
}

//! Return the number of names in the associated command set.
inline unsigned int Cmd::numNames() const
{
    return numNames_;
}

//! Override log mode for current run. Functional only when the given command is
//! being authenticated (i.e., while allowReq(req,...) is being executed). Log
//! mode is normally dictated by SysIo::setLogMode(). Use this for command runs
//! which must or must not be logged for any reason.
inline void Cmd::overrideLogMode(const CmdLine& req, bool alwaysLogReq, bool alwaysLogRsp)
{
    tag_t* tag = static_cast<tag_t*>(req.tag());
    tag->alwaysLogReq = alwaysLogReq;
    tag->alwaysLogRsp = alwaysLogRsp;
}

END_NAMESPACE1

#endif
