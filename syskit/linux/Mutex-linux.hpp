/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_MUTEX_LINUX_HPP
#define SYSKIT_MUTEX_LINUX_HPP

#include <pthread.h>

BEGIN_NAMESPACE1(syskit)


//!
//! A class representing a mutex (mutual exclusion device). In general,
//! a mutex is similar to a critical section, but it can work accross
//! process boundaries (a critical section cannot). Also, a critical
//! section can be locked recursively, but this mutex cannot. The mutex
//! should be locked/unlocked by contructing/destructing a lock using
//! Mutex::Lock. Use Mutex::lock() and Mutex::unlock() only if Mutex::Lock
//! is not feasible. Use tryLock() to avoid blocking calls. Recursive locks
//! are not allowed. That is, recursive locks will result in a deadlock. Example:
//!\code
//! static Mutex s_mutex;
//! s_mutex.lock();
//! thisFuncDoesNotWorkIfConcurrentlyUsed();
//! thisFuncDoesNotWorkEitherIfConcurrentlyUsed();
//! s_mutex.unlock();
//!\endcode
//!
class Mutex
{

public:
    Mutex();
    ~Mutex();

    bool isOk() const;
    bool lock();
    bool tryLock();
    bool unlock();


    //!
    //! A class representing a mutex lock. Construct an instance to
    //! lock a mutex, and destruct it to unlock the mutex. Use isOk()
    //! to determine if the lock construction is successful. Example:
    //!\code
    //! static Mutex s_mutex;
    //! {
    //!   Mutex::Lock lock(s_mutex);
    //!   thisFuncDoesNotWorkIfConcurrentlyUsed();
    //!   thisFuncDoesNotWorkEitherIfConcurrentlyUsed();
    //! }
    //!\endcode
    //!
    class Lock
    {
    public:
        Lock(Mutex& mutex);
        ~Lock();
        bool isOk() const;
    private:
        Mutex* m_mutex;
        Lock(const Lock&); //prohibit usage
        const Lock& operator=(const Lock&); //prohibit usage
    };

private:
    pthread_mutex_t m_mu;

    static const pthread_mutex_t MU_0;

    Mutex(const Mutex&); //prohibit usage
    const Mutex& operator =(const Mutex&); //prohibit usage

};

//! Return true if instance was successfully constructed.
inline bool Mutex::isOk() const
{
    return true;
}

//! Lock this mutex. Wait forever if necessary.
//! Return true if successful.
inline bool Mutex::lock()
{
    return pthread_mutex_lock(&m_mu) == 0;
}

//! Lock this mutex. Do not wait.
//! Return true if successful.
inline bool Mutex::tryLock()
{
    return pthread_mutex_trylock(&m_mu) == 0;
}

//! Unlock this mutex.
//! Return true if successful.
//! Unlocking an unlocked mutex is allowed and is considered successful.
inline bool Mutex::unlock()
{
    return pthread_mutex_unlock(&m_mu) == 0;
}

//! Return true if instance was successfully constructed.
inline bool Mutex::Lock::isOk() const
{
    return m_mutex != 0;
}

END_NAMESPACE1

#endif
