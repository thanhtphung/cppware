/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_COND_VAR_LINUX_HPP
#define SYSKIT_COND_VAR_LINUX_HPP

#include <pthread.h>
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! condition variable and its associated mutex
class CondVar
    //!
    //! A class representing a condition variable and its associated mutex.
    //! A mutex is usually used to prevent other threads from executing some
    //! code concurrently. A condition variable and its associated mutex is
    //! a synchronization device that is usually used to wait and let other
    //! threads run until some condition is met. There are basically two
    //! categories of CondVar users: an observer and a notifier. An observer
    //! uses the wait() methods to wait until some condition is met. A notifier
    //! uses the signal() method to wake up one observer or the broadcast()
    //! method to wake up all observers. Example:
    //!\code
    //! static CondVar s_condVar;
    //! :
    //! // observer code --wait until some condition is met
    //! s_condVar.lockMutex();
    //! while (/*conditionNotMet*/)
    //! {
    //!   s_condVar.wait();
    //! }
    //! s_condVar.unlockMutex();
    //! :
    //! // notifier code --wake up all observers
    //! s_condVar.lockMutex();
    //! doSomethingSoConditionIsMet();
    //! s_condVar.broadcast();
    //! s_condVar.unlockMutex();
    //! :
    //! // notifier code --wake up one observer
    //! s_condVar.lockMutex();
    //! doSomethingSoConditionIsMet();
    //! s_condVar.signal();
    //! s_condVar.unlockMutex();
    //!\endcode
    //!
{

public:
    static const unsigned int ETERNITY;

    CondVar();
    ~CondVar();

    bool broadcast();
    bool isOk() const;
    bool lockMutex();
    bool signal();
    bool tryLockMutex();
    bool unlockMutex();
    bool wait();
    bool wait(unsigned int timeoutInMsecs);


    //! associated mutex lock
    class MutexLock
        //!
        //! A class representing an associated mutex lock. Construct an
        //! instance to lock the associated mutex, and destruct it to
        //! unlock the mutex. Use isOk() to determine if the lock
        //! construction is successful. Example:
        //!\code
        //! static CondVar s_condVar;
        //! :
        //! // observer code --wait until some condition is met
        //! {
        //!   CondVar::MutexLock lock(s_condVar);
        //!   while (/*conditionNotMet*/)
        //!   {
        //!     s_condVar.wait();
        //!   }
        //! }
        //! :
        //! // notifier code --wake up all observers
        //! {
        //!   CondVar::MutexLock lock(s_condVar);
        //!   doSomethingSoConditionIsMet();
        //!   s_condVar.broadcast();
        //! }
        //! :
        //! // notifier code --wake up one observer
        //! {
        //!   CondVar::MutexLock lock(s_condVar);
        //!   doSomethingSoConditionIsMet();
        //!   s_condVar.signal();
        //! }
        //!\endcode
        //!
    {
    public:
        MutexLock(CondVar& condVar);
        ~MutexLock();
        bool isOk() const;
    private:
        CondVar* cv_;
        MutexLock(const MutexLock&); //prohibit usage
        const MutexLock& operator=(const MutexLock&); //prohibit usage
    };

private:
    pthread_cond_t cv_;
    pthread_mutex_t mu_;

    CondVar(const CondVar&); //prohibit usage
    const CondVar& operator=(const CondVar&); //prohibit usage

};

//! Broadcast the condition. That is, no-op if there are no waiters, or
//! allow all waiters to proceed otherwise. Return true if successful.
//! The associated mutex must be locked before invoking this method.
//! Behavior is unpredictable otherwise.
inline bool CondVar::broadcast()
{
    return pthread_cond_broadcast(&cv_) == 0;
}

//! Return true if instance was successfully constructed.
inline bool CondVar::isOk() const
{
    return true;
}

//! Lock associated mutex. Wait forever if necessary.
//! Return true if successful.
inline bool CondVar::lockMutex()
{
    return pthread_mutex_lock(&mu_) == 0;
}

//! Signal the condition. That is, no-op if there are no waiters, or
//! allow one waiter to proceed otherwise. Return true if successful.
//! The associated mutex must be locked before invoking this method.
//! Behavior is unpredictable otherwise.
inline bool CondVar::signal()
{
    return pthread_cond_signal(&cv_) == 0;
}

//! Lock associated mutex. Do not wait.
//! Return true if successful.
inline bool CondVar::tryLockMutex()
{
    return pthread_mutex_trylock(&mu_) == 0;
}

//! Unlock associated mutex.
//! Return true if successful.
//! Unlocking an unlocked mutex is allowed and is considered successful.
inline bool CondVar::unlockMutex()
{
    return pthread_mutex_unlock(&mu_) == 0;
}

//! Atomically unlock associated mutex and wait until the condition
//! variable is signaled. Relock the associated mutex before return.
//! Wait forever if necessary. Return true if successful. The associated
//! mutex must be locked before invoking this method. Behavior is
//! unpredictable otherwise. Due to inconsitencies among various
//! implementations, do not assume this wait can be interrupted.
inline bool CondVar::wait()
{
    return pthread_cond_wait(&cv_, &mu_) == 0;
}

//! Return true if instance was successfully constructed.
inline bool CondVar::MutexLock::isOk() const
{
    return cv_ != 0;
}

END_NAMESPACE1

#endif
