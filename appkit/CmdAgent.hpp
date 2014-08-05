/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_CMD_AGENT_HPP
#define APPKIT_CMD_AGENT_HPP

#include "netkit/Paddr.hpp"
#include "syskit/macros.h"

DECLARE_CLASS1(netkit, Subnet)
DECLARE_CLASS1(netkit, UdpClient)
DECLARE_CLASS1(syskit, MappedTxtFile)

BEGIN_NAMESPACE1(appkit)

class CmdLine;
class String;
class StringDic;
class StringVec;
class SysIo;


//! debug command agent
class CmdAgent
    //!
    //! Agent relaying debug messages between a debugger and a debuggee.
    //! Also capable of sending arbitrary events to third-parties. A debug
    //! command agent acts on behalf of a debugger, and debug commands are
    //! run in the debuggee's context.
    //!
{

public:
    enum
    {

        //! msecs, timeout waiting for subsequent message fragments, if any
        Timeout = 1789,

        //! msecs, timeout waiting for first message fragment
        Timeout0 = 2789
    };

    static const unsigned int ETERNITY;

    CmdAgent(const CmdLine& config, unsigned int timeout0InMsecs = Timeout0, unsigned int timeoutInMsecs = Timeout);
    CmdAgent(const netkit::Paddr& dbuggee, unsigned int timeout0InMsecs = Timeout0, unsigned int timeoutInMsecs = Timeout);

    bool queueScript(const syskit::MappedTxtFile& script, unsigned int timeoutInMsecs = ETERNITY);
    bool runCmd(const String& cmd, String* rspCopy = 0);
    bool runCmds(const CmdLine& cmdLine, StringVec* rspCopies = 0);
    bool sndEvent(const String& event, const netkit::Paddr& thirdParty, unsigned int timeoutInMsecs = ETERNITY);
    const netkit::Paddr& dbuggee() const;
    const netkit::Paddr& dbugger() const;
    const char* errDesc() const;
    unsigned int timeout0InMsecs() const;
    unsigned int timeoutInMsecs() const;
    void setTimeouts(unsigned int timeout0InMsecs, unsigned int timeoutInMsecs);

    virtual ~CmdAgent();
    virtual bool isOk() const;
    virtual void echoReq(const String& req);
    virtual void onRsp(const char* rsp, size_t rspLength, bool more);
    virtual void onTimeout();

    static CmdAgent* createLoopback(const SysIo* sysIo);
    static StringDic broadcast(const String& ping, const netkit::Subnet& subnet, unsigned short port);
    static StringDic broadcast(const String& ping, unsigned short port);

protected:
    CmdAgent(netkit::UdpClient* io);
    String formReqEcho(const String& req) const;

private:
    netkit::Paddr dbuggee_;
    netkit::Paddr dbugger_;
    netkit::UdpClient* io_;
    char rspTail_;
    const char* errDesc_;
    unsigned int timeout_;
    unsigned int timeout0_;

    CmdAgent(const CmdAgent&); //prohibit usage
    const CmdAgent& operator =(const CmdAgent&); //prohibit usage

    bool showRsp(String*);
    void bind(const CmdLine&, unsigned int);
    void connect(const CmdLine&);
    void flush();
    void mkIo(const CmdLine&);

};

//! Return the effective debuggee address.
inline const netkit::Paddr& CmdAgent::dbuggee() const
{
    return dbuggee_;
}

//! Return the effective debugger address.
inline const netkit::Paddr& CmdAgent::dbugger() const
{
    return dbugger_;
}

//! Return zero if instance was successfully constructed.
//! Return an error description otherwise.
inline const char* CmdAgent::errDesc() const
{
    return errDesc_;
}

//! Return the timeout waiting for first message fragment. When a command is run,
//! some response must be seen within this timeout. Otherwise, the command run is
//! assumed to have failed.
inline unsigned int CmdAgent::timeout0InMsecs() const
{
    return timeout0_;
}

//! Return the timeout waiting for subsequent message fragments. When a command is run,
//! some response must be seen within timeout0InMsecs(). If the response is partial,
//! each subsequent message fragment must be seen within this timeout. Otherwise, the
//! command run is assumed to have failed.
inline unsigned int CmdAgent::timeoutInMsecs() const
{
    return timeout_;
}

//! Update the timeout settings.
inline void CmdAgent::setTimeouts(unsigned int timeout0InMsecs, unsigned int timeoutInMsecs)
{
    timeout_ = timeoutInMsecs;
    timeout0_ = timeout0InMsecs;
}

END_NAMESPACE1

#endif
