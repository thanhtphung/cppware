/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_THREAD_HPP
#define SYSKIT_THREAD_HPP

#include <sys/types.h>
#include "syskit/RefCounted.hpp"
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(syskit)


//! thread (aka task)
class Thread
    //!
    //! A class representing a thread. A thread is Idle when first created. It becomes
    //! Active when the user-provided starting point is entered. It is considered Done
    //! when the user-provided starting point returns.
    //!
{

public:
    enum state_e
    {
        Idle = 0,
        Active,
        Done,
        Terminating
    };

    typedef void(*crashCb_t)(void* arg);
    typedef void* (*entrance_t)(void* arg);

    static const threadId_t INVALID_ID;
    static const unsigned int ETERNITY;

    Thread(entrance_t entrance, void* arg = 0, unsigned int stackSizeInBytes = 0, bool startSuspended = false);
    ~Thread();

    bool isOk() const;
    bool killAndWait(void** exitCode = 0);
    bool resume();
    bool setAffinityMask(size_t affinityMask);
    bool suspend();
    bool waitTilDone(void** exitCode = 0);
    size_t affinityMask() const;
    size_t id() const;
    state_e state() const;
    void detach();
    void kill();

    static bool isTerminating();
    static bool takeANap(unsigned int napTimeInMsecs, bool beAlert = true);
    static const wchar_t* dumpPath();
    static crashCb_t crashCb(void*& arg);
    static size_t myId();
    static void monitorCrash(crashCb_t cb, void* arg, const wchar_t* dumpPath, crashCb_t* oldCb = 0, void** oldArg = 0);
    static void yield();

    class State: public RefCounted
    {
    public:
        State();
        state_e state() const;
        void setState(state_e state);
    protected:
        virtual ~State();
    private:
        state_e state_;
        State(const State&); //prohibit usage
        const State& operator=(const State&); //prohibit usage
    };

    class Entry
    {
    public:
        Entry(entrance_t entrance, void* arg, State* state);
        ~Entry();
        void* enter();
    private:
        State* state_;
        entrance_t entrance_;
        void* arg_;
        Entry(const Entry&); //prohibit usage
        const Entry& operator=(const Entry&); //prohibit usage
    };

private:
    State* state_;
    threadId_t id_;
    void* thread_;

    static const wchar_t* dumpPath_;
    static crashCb_t crashCb_;
    static void* crashCbArg_;

    Thread(const Thread&); //prohibit usage
    const Thread& operator=(const Thread&); //prohibit usage

    static void noOp(void*);

};

inline Thread::crashCb_t Thread::crashCb(void*& arg)
{
    arg = crashCbArg_;
    return crashCb_;
}

inline Thread::state_e Thread::State::state() const
{
    return state_;
}

inline const wchar_t* Thread::dumpPath()
{
    return dumpPath_;
}

inline void Thread::State::setState(state_e state)
{
    state_ = state;
}

//! Return the state of the created thread. A thread is Idle when first
//! created. It becomes Active when the user-provided starting point is
//! entered. It is considered Done when the user-provided starting point
//! returns.
inline Thread::state_e Thread::state() const
{
    return state_->state();
}

//! Return true if instance was successfully constructed.
inline bool Thread::isOk() const
{
    return (thread_ != 0);
}

//! Return the identifier of the created thread.
inline size_t Thread::id() const
{
    return (size_t)(id_);
}

END_NAMESPACE1

#endif
