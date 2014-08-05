/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_MESSENGER_HPP
#define APPKIT_MESSENGER_HPP

#include "appkit/CmdAgent.hpp"
#include "appkit/String.hpp"
#include "syskit/macros.h"

DECLARE_CLASS1(netkit, Paddr)
DECLARE_CLASS1(netkit, UdpClient)
DECLARE_CLASS1(syskit, ItemQ)
DECLARE_CLASS1(syskit, Thread)

BEGIN_NAMESPACE1(appkit)

class CmdLine;


//! debug command messenger
class Messenger: private CmdAgent
    //!
    //! Messenger delivering commands/events. With the CmdAgent class, messaging is
    //! done in the caller's context. With this Messenger class, messaging is done in
    //! a dedicated messenger's context. Use of a dedicated thread for each destination
    //! simplifies concurrent command executions. As an example, if a remote end is
    //! unresponsive, impact is minimal when communicating with other remote ends
    //! at the same time.
    //!
{

public:
    typedef void(*onRsp_t)(void* arg, const String& cmd, const String& rsp);

    // From CmdAgent.
    using CmdAgent::errDesc;
    using CmdAgent::setTimeouts;
    using CmdAgent::timeout0InMsecs;
    using CmdAgent::timeoutInMsecs;

    Messenger(const CmdLine& config, unsigned int timeout0InMsecs = Timeout0, unsigned int timeoutInMsecs = Timeout);
    Messenger(const netkit::Paddr& remoteAddr, unsigned int timeout0InMsecs = Timeout0, unsigned int timeoutInMsecs = Timeout);
    syskit::ItemQ& cmdQ();
    const netkit::Paddr& localAddr() const;
    const netkit::Paddr& remoteAddr() const;
    const syskit::ItemQ& cmdQ() const;

    virtual ~Messenger();
    virtual bool isOk() const;

    virtual bool deliver(const String& cmd);
    virtual bool deliver(const String& cmd, String& rsp);
    virtual bool deliver(const String& cmd, onRsp_t onRsp, void* arg = 0);
    virtual bool inform(const String& event);

    static Messenger* createNull();

protected:
    Messenger(netkit::UdpClient* io);

    virtual void echoReq(const String& req);
    virtual void onRsp(const char* rsp, size_t rspLength, bool more);
    virtual void onTimeout();

private:
    syskit::ItemQ* cmdQ_;
    String rsp_;
    syskit::Thread* messenger_;

    Messenger(const Messenger&); //prohibit usage
    const Messenger& operator =(const Messenger&); //prohibit usage

    void construct();
    void destruct();
    void loop();

    static void* messengerEntry(void*);

};

inline const netkit::Paddr& Messenger::localAddr() const
{
    return CmdAgent::dbugger();
}

inline const netkit::Paddr& Messenger::remoteAddr() const
{
    return CmdAgent::dbuggee();
}

END_NAMESPACE1

#endif
