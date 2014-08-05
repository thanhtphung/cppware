/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "netkit/Paddr.hpp"
#include "netkit/UdpClient.hpp"
#include "syskit/BufPool.hpp"
#include "syskit/ItemQ.hpp"
#include "syskit/Semaphore.hpp"
#include "syskit/Thread.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/Messenger.hpp"

using namespace appkit;
using namespace netkit;
using namespace syskit;

const unsigned int THREAD_STACK_SIZE = 0; //bytes, 0=use-default

BEGIN_NAMESPACE


//
// Package sent to the messenger thread.
//
class Package: public ItemQ::Item
{
public:
    Package(const String& cmd, String* rsp, Semaphore* sem = 0);
    Package(const String& cmd, Messenger::onRsp_t onRsp, void* arg = 0);
    Package(const String& event);
    static void operator delete(void* p, size_t size);
    static void* operator new(size_t size);
    Semaphore* sem() const;
    String* rsp() const;
    const String& cmd() const;
    const String& event() const;
    Messenger::onRsp_t onRsp(void*& arg) const;
    virtual ~Package();
private:
    Semaphore* sem_;
    String cmd_;
    String event_;
    String* rsp_;
    Messenger::onRsp_t onRsp_;
    void* onRspArg_;
    Package(const Package&); //prohibit usage
    const Package& operator =(const Package&); //prohibit usage
    static void noOp(void*, const String&, const String&);
};

inline void Package::operator delete(void* p, size_t size)
{
    BufPool::freeBuf(p, size);
}

inline void* Package::operator new(size_t size)
{
    void* buf = BufPool::allocateBuf(size);
    return buf;
}

inline Semaphore* Package::sem() const
{
    return sem_;
}

inline String* Package::rsp() const
{
    return rsp_;
}

inline const String& Package::cmd() const
{
    return cmd_;
}

inline const String& Package::event() const
{
    return event_;
}

inline Messenger::onRsp_t Package::onRsp(void*& arg) const
{
    arg = onRspArg_;
    return onRsp_;
}

// Construct package containing a synchronous command.
// The messenger thread will forward this command to its destination and wait
// for its execution. If a non-zero semaphore is provided, the command response
// will be saved in rsp and the semaphore will be incremented to indicate
// completion.
Package::Package(const String& cmd, String* rsp, Semaphore* sem):
ItemQ::Item(),
cmd_(cmd),
event_()
{
    onRsp_ = noOp;
    onRspArg_ = 0;
    rsp_ = rsp;
    sem_ = sem;
}

// Construct package containing an asynchronous command.
// The messenger thread will forward this command to its destination, wait
// for its execution, and invoke the onRsp callback to indicate completion.
Package::Package(const String& cmd, Messenger::onRsp_t onRsp, void* arg):
ItemQ::Item(),
cmd_(cmd),
event_()
{
    onRsp_ = onRsp;
    onRspArg_ = arg;
    rsp_ = 0;
    sem_ = 0;
}

// Construct package containing an asynchronous event.
// The messenger thread will forward this event to its destination ASAP.
Package::Package(const String& event):
ItemQ::Item(),
cmd_(),
event_(event)
{
    onRsp_ = 0;
    onRspArg_ = 0;
    rsp_ = 0;
    sem_ = 0;
}

Package::~Package()
{
}

void Package::noOp(void* /*arg*/, const String& /*cmd*/, const String& /*rsp*/)
{
}


class NullMessenger: public Messenger
{
public:
    NullMessenger();
    virtual ~NullMessenger();
    virtual bool isOk() const;
    virtual bool deliver(const String& cmd);
    virtual bool deliver(const String& cmd, String& rsp);
    virtual bool deliver(const String& cmd, onRsp_t onRsp, void* arg = 0);
    virtual bool inform(const String& event);
private:
    NullMessenger(const NullMessenger&); //prohibit usage
    const NullMessenger& operator =(const NullMessenger&); //prohibit usage
};

NullMessenger::NullMessenger():
Messenger(UdpClient::createNull())
{
}

NullMessenger::~NullMessenger()
{
}

bool NullMessenger::isOk() const
{
    bool ok = true;
    return ok;
}

bool NullMessenger::deliver(const String& /*cmd*/)
{
    bool ok = true;
    return ok;
}

bool NullMessenger::deliver(const String& /*cmd*/, String& rsp)
{
    rsp.reset();
    bool ok = true;
    return ok;
}

bool NullMessenger::deliver(const String& /*cmd*/, onRsp_t /*onRsp*/, void* /*arg*/)
{
    bool ok = true;
    return ok;
}

bool NullMessenger::inform(const String& /*event*/)
{
    bool ok = true;
    return ok;
}

END_NAMESPACE

BEGIN_NAMESPACE1(appkit)


//!
//! Construct messenger. This is practically a debug command agent running
//! in a dedicated thread. Allowed options in config: dbuggee (required),
//! dbugger, ibufsiz, obufsize. When a command is run, some response must
//! be seen within timeout0InMsecs msecs. If the response is partial, each
//! subsequent message fragment must be seen within timeoutInMsecs msecs.
//! Otherwise, the command run is assumed to have failed.
//!
Messenger::Messenger(const CmdLine& config, unsigned int timeout0InMsecs, unsigned int timeoutInMsecs):
CmdAgent(config, timeout0InMsecs, timeoutInMsecs),
rsp_()
{
    construct();
}


//!
//! Construct messenger. This is practically a debug command agent running
//! in a dedicated thread. When a command is run, some response must be seen
//! within timeout0InMsecs msecs. If the response is partial, each subsequent
//! message fragment must be seen within timeoutInMsecs msecs. Otherwise, the
//! command run is assumed to have failed.
//!
Messenger::Messenger(const Paddr& remoteAddr, unsigned int timeout0InMsecs, unsigned int timeoutInMsecs):
CmdAgent(remoteAddr, timeout0InMsecs, timeoutInMsecs),
rsp_()
{
    construct();
}


//!
//! Construct minimal messenger. This messenger does not have a dedicated thread,
//! and its use results in mostly no-ops. Take over ownership of the given UdpClient
//! object which will be destroyed by Messenger when appropriate.
//!
Messenger::Messenger(UdpClient* io):
CmdAgent(io),
rsp_()
{
    cmdQ_ = 0;
    messenger_ = 0;
}


Messenger::~Messenger()
{
    destruct();
}


ItemQ& Messenger::cmdQ()
{
    return *cmdQ_;
}


//!
//! Create a minimal messenger. Its use results in mostly no-ops. Returned messenger must
//! be deleted using the delete operator when done.
//!
Messenger* Messenger::createNull()
{
    Messenger* messenger = new NullMessenger;
    return messenger;
}


//!
//! Return true if instance was successfully constructed.
//!
bool Messenger::isOk() const
{
    bool ok = CmdAgent::isOk();
    return ok;
}


//!
//! Deliver given command. Return true if successful. Messaging is done in a
//! dedicated messenger's context. Commands are delivered sequentially on a
//! FIFO basis. For this delivery method, responses are assumed insignificant
//! and are not made available to callers. With this delivery method, an empty
//! command requests the messenger thread to end its loop.
//!
bool Messenger::deliver(const String& cmd)
{

    // Allow messenger loop termination via an empty command.
    ItemQ::Item* item = cmd.empty()? 0: new Package(cmd);
    unsigned int timeoutInMsecs = 0;
    bool ok = cmdQ_->put(item, timeoutInMsecs);

    // For this delivery scheme, message is allocated here and is to be freed by the
    // messenger thread after the remote command execution. However, the message must
    // be freed here if communication with the messenger thread fails.
    if (!ok)
    {
        ItemQ::Item::release(item);
    }

    return ok;
}


//!
//! Deliver given command. Wait and bring back a response. Return true if successful.
//! Messaging is done in a dedicated messenger's context. Commands are delivered
//! sequentially on a FIFO basis. The wait is done in the caller's context. That is,
//! this is a synchronous method.
//!
bool Messenger::deliver(const String& cmd, String& rsp)
{
    Semaphore sem(0U /*capacity*/);
    ItemQ::Item* item = new Package(cmd, &rsp, &sem);
    unsigned int timeoutInMsecs = 0;
    bool ok = cmdQ_->put(item, timeoutInMsecs);
    if (ok)
    {
        sem.decrement(); //wait for remote command execution, forever if necessary
    }

    // For this delivery scheme, message is allocated here and also freed here after the
    // remote command execution.
    ItemQ::Item::release(item);
    return ok;
}


//!
//! Deliver given command. Wait and bring back a response. When response is available,
//! invoke given callback. Return true if successful. Messaging is done in a dedicated
//! messenger's context. Commands are delivered sequentially on a FIFO basis. The wait
//! is done in the dedicated messenger's context. That is, this is an asynchronous
//! method.
//!
bool Messenger::deliver(const String& cmd, onRsp_t onRsp, void* arg)
{
    ItemQ::Item* item = new Package(cmd, onRsp, arg);
    unsigned int timeoutInMsecs = 0;
    bool ok = cmdQ_->put(item, timeoutInMsecs);

    // For this delivery scheme, message is allocated here and is to be freed by the
    // messenger thread after the remote command execution. However, the message must
    // be freed here if communication with the messenger thread fails.
    if (!ok)
    {
        ItemQ::Item::release(item);
    }

    return ok;
}


//!
//! Deliver given event. Return true if successful. Messaging is done in a dedicated
//! messenger's context. Events and commands are delivered sequentially on a FIFO basis.
//!
bool Messenger::inform(const String& event)
{
    ItemQ::Item* item = new Package(event);
    unsigned int timeoutInMsecs = 0;
    bool ok = cmdQ_->put(item, timeoutInMsecs);

    // For this delivery scheme, message is allocated here and is to be freed by the
    // messenger thread after the remote command execution. However, the message must
    // be freed here if communication with the messenger thread fails.
    if (!ok)
    {
        ItemQ::Item::release(item);
    }

    return ok;
}


const ItemQ& Messenger::cmdQ() const
{
    return *cmdQ_;
}


void Messenger::construct()
{
    int growBy = -1;
    cmdQ_ = new ItemQ(ItemQ::DefaultCap, growBy);
    messenger_ = new Thread(messengerEntry, this, THREAD_STACK_SIZE);
}


void Messenger::destruct()
{
    if (messenger_ != 0)
    {
        messenger_->killAndWait();
        delete messenger_;
    }

    delete cmdQ_;
}


//!
//! This method is invoked when a command has just been sent successfully.
//! The default implementation is a no-op.
//!
void Messenger::echoReq(const String& /*req*/)
{
}


//
// Main loop for the messenger thread. Each thread has its own queue allowing
// commands/events to be delivered sequentially on a FIFO basis. The loop can
// terminate due to an interrupt or a null message.
//
void Messenger::loop()
{
    ItemQ::Item* item;
    unsigned int timeoutInMsecs = ItemQ::ETERNITY;
    while ((!Thread::isTerminating()) && cmdQ_->get(item, timeoutInMsecs))
    {

        // Allow messenger loop termination via a null message.
        const Package* package = dynamic_cast<const Package*>(item);
        if (package == 0)
        {
            break;
        }

        // Send events sequentially.
        const String& cmd = package->cmd();
        if (cmd.empty())
        {
            const Paddr& thirdParty = dbuggee();
            sndEvent(package->event(), thirdParty);
            delete package;
            continue;
        }

        // Run commands sequentially.
        // In synchronous mode, notify waiter by incrementing a semaphore.
        // Package is to be deleted by waiter.
        rsp_.reset();
        runCmd(cmd);
        Semaphore* sem = package->sem();
        if (sem != 0)
        {
            *package->rsp() = rsp_;
            sem->increment();
            continue;
        }

        // In asynchronous mode, invoke onRsp callback upon each command completion.
        void* arg;
        onRsp_t onRsp = package->onRsp(arg);
        onRsp(arg, cmd, rsp_);
        delete package;
    }
}


//!
//! This method is invoked when some response (rspLength bytes starting at
//! rsp) has been received. This response is not null-terminated if more is
//! expected, and is null-terminated otherwise. However, rspLength does not
//! include the terminating null. This implementation accumulates the response
//! for the current command.
//!
void Messenger::onRsp(const char* rsp, size_t rspLength, bool /*more*/)
{
    String s;
    s.reset8(reinterpret_cast<const utf8_t*>(rsp), rspLength);
    rsp_ += s;
}


//!
//! This method is invoked when a failure (typically timeout) occurred waiting
//! for response. This implementation is a no-op. This class sees failures via
//! the runCmd() returned code.
//!
void Messenger::onTimeout()
{
}


//
// Entry point for the messenger thread.
//
void* Messenger::messengerEntry(void* arg)
{
    Messenger* messenger = static_cast<Messenger*>(arg);
    messenger->loop();
    return 0;
}

END_NAMESPACE1
