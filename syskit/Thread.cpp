/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/Thread.hpp"
#include "syskit/ThreadKey.hpp"
#include "syskit/sys.hpp"


BEGIN_NAMESPACE

class StateKey: public syskit::ThreadKey, public syskit::RefCounted
{
public:
    StateKey();
    static StateKey& instance();
protected:
    virtual ~StateKey();
private:
    StateKey(const StateKey&); //prohibit usage
    const StateKey& operator=(const StateKey&); //prohibit usage
};

StateKey::StateKey():
syskit::ThreadKey(),
syskit::RefCounted(0U /*initialRefCount*/)
{
}

StateKey::~StateKey()
{
}

StateKey& StateKey::instance()
{
    static StateKey* s_stateKey = new StateKey;
    return *s_stateKey;
}

static StateKey::Count s_stateKeyLock(StateKey::instance());

END_NAMESPACE

BEGIN_NAMESPACE1(syskit)

const threadId_t Thread::INVALID_ID = (threadId_t)(0xffffffffU);
const unsigned int Thread::ETERNITY = 0xffffffffU;
const wchar_t* Thread::dumpPath_ = 0;

Thread::crashCb_t Thread::crashCb_ = noOp;
void* Thread::crashCbArg_ = 0;


//!
//! Destruct thread. If the created thread is still attached to this object,
//! the destructor will wait (forever if necessary) until the created thread
//! is dead.
//!
Thread::~Thread()
{
    waitTilDone();
    state_->rmRef();
}


bool Thread::isTerminating()
{
    const State* myState = static_cast<const State*>(StateKey::instance().value());
    bool terminating = (myState->state() == Terminating);
    return terminating;
}


//!
//! Kill this thread using an interrupt. If this thread is running in an alertable
//! mode, it will be interrupted (i.e., system call is interrupted and the call
//! returns with the WAIT_IO_COMPLETION error). Also wait for its exit. Return true
//! if successful.
//!
bool Thread::killAndWait(void** exitCode)
{
    kill();
    bool ok = waitTilDone(exitCode);
    return ok;
}


//!
//! Use given path for call stack dump. Call stack is dumped when a thread
//! encounters an unhandled exception. Also invoke given callback (if non-zero)
//! after dumping the call stack. The previous callback info can be returned in
//! oldCb, oldArg. Use non-zero to get the info.
//!
void Thread::monitorCrash(crashCb_t cb, void* arg, const wchar_t* dumpPath, crashCb_t* oldCb, void** oldArg)
{
    if (oldCb != 0)
    {
        *oldCb = crashCb_;
    }

    if (oldArg != 0)
    {
        *oldArg = crashCbArg_;
    }

    if (dumpPath != dumpPath_)
    {
        delete[] dumpPath_;
        dumpPath_ = (dumpPath == 0)? 0: strdup(dumpPath);
    }
    crashCbArg_ = arg;
    crashCb_ = (cb == 0)? noOp: cb;
}


void Thread::noOp(void* /*arg*/)
{
}


//
// Construct thread entry. Normally, this is constructed by the parent thread
// and destructed by the child thread. However, when the child thread creation
// fails, the destruction is done by the parent thread.
//
Thread::Entry::Entry(entrance_t entrance, void* arg, State* state)
{
    StateKey::instance().addRef();
    state->addRef();
    arg_ = arg;
    entrance_ = entrance;
    state_ = state;
}


//
// Destruct thread entry.
//
Thread::Entry::~Entry()
{
    state_->setState(Done);
    state_->rmRef();
    StateKey::instance().rmRef();
}


//
// Enter user-provided thread starting point. Return user-provided thread
// exit code.
//
void* Thread::Entry::enter()
{
    StateKey::instance().setValue(state_);
    state_->setState(Active);
    return entrance_(arg_);
}


//
// Construct reference counted state object. Normally, this is constructed
// by the parent thread and also destructed by the parent thread. When the
// child thread is detached from the Thread object, the destruction is done
// by the detached child thread.
//
Thread::State::State():
RefCounted(1U)
{
    state_ = Idle;
}


Thread::State::~State()
{
}

END_NAMESPACE1
