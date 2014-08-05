/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_MUTEX_HPP
#define SYSKIT_MUTEX_HPP

#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(syskit)


//! mutual exclusion device
class Mutex
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
{

public:
    static const unsigned int ETERNITY;

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
    private:
        Mutex& mutex_;
        Lock(const Lock&); //prohibit usage
        const Lock& operator=(const Lock&); //prohibit usage
    };

private:
    mutex_t mu_;

    Mutex(const Mutex&); //prohibit usage
    const Mutex& operator=(const Mutex&); //prohibit usage

};

END_NAMESPACE1

#endif
