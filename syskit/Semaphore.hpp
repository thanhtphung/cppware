/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_SEMAPHORE_HPP
#define SYSKIT_SEMAPHORE_HPP

#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(syskit)


//! semaphore
class Semaphore
    //!
    //! A class representing a semaphore. This class views a semaphore as a pool
    //! of tokens. The semaphore's value represents the number of available tokens.
    //! When feasible, short-term-used tokens should be acquired/released by
    //! constructing/destructing a Semaphore::Lock instance. Example:
    //!\code
    //! static Semaphore s_semaphore(2 /*initialCount*/);
    //! s_semaphore.decrement();
    //! thisFuncCanWorkIfConcurrentlyUsedByTwoThreads();
    //! thisFuncDoesNotWorkIfConcurrentlyUsedByThreeOrMoreThreads();
    //! s_semaphore.increment();
    //!\endcode
    //!
{

public:
    static const unsigned int ETERNITY;
    static const unsigned int MAX_CAP;

    Semaphore(unsigned int capacity = 0U);
    ~Semaphore();

    bool decrement();
    bool decrement(unsigned int waitTimeInMsecs);
    bool decrementBy(unsigned int delta);
    bool decrementBy(unsigned int delta, unsigned int waitTimeInMsecs);
    bool increment();
    bool incrementBy(unsigned int delta);
    bool isOk() const;
    bool tryDecrement();
    bool tryDecrementBy(unsigned int delta);
    bool waitTilNonEmpty();
    bool waitTilFull(unsigned int numTokens);
    void detach();


    //! semaphore lock
    class Lock
        //!
        //! A class representing a semaphore lock. Construct an instance
        //! to lock a semaphore, and destruct it to unlock the semaphore.
        //! Locking a semaphore is equivalent to acquiring one or more tokens,
        //! and unlocking the semaphore is equivalent to releasing the token(s).
        //! Use isOk() to determine if the lock construction is successful.
        //! Example:
        //!\code
        //! static Semaphore s_semaphore(2 /*initialCount*/);
        //! {
        //!   Semaphore::Lock lock(s_semaphore);
        //!   thisFuncCanWorkIfConcurrentlyUsedByTwoThreads();
        //!   thisFuncDoesNotWorkIfConcurrentlyUsedByThreeOrMoreThreads();
        //! }
        //!\endcode
        //!
    {
    public:
        Lock(Semaphore& semaphore);
        Lock(Semaphore& semaphore, unsigned int numTokens);
        ~Lock();
        bool isOk() const;
    private:
        Semaphore* semaphore_;
        unsigned int numTokens_;
        Lock(const Lock&); //prohibit usage
        const Lock& operator=(const Lock&); //prohibit usage
    };

protected:
    Semaphore(sem_t sem);
    sem_t handle() const;

    void reset(unsigned int count);

private:
    bool semIsMine_;
    sem_t sem_;

    Semaphore(const Semaphore&); //prohibit usage
    const Semaphore& operator =(const Semaphore&); //prohibit usage

    bool acquireTokens(unsigned int);
    bool tryAcquireTokens(unsigned int);

};

inline sem_t Semaphore::handle() const
{
    return sem_;
}

//! Return true if instance was successfully constructed.
inline bool Semaphore::Lock::isOk() const
{
    return semaphore_ != 0;
}

END_NAMESPACE1

#if __linux || __CYGWIN__
#include "syskit/linux/Semaphore-linux.hpp"

#elif __FREERTOS__
#include "syskit/rtos/Semaphore-rtos.hpp"

#elif _WIN32
#include "syskit/win/Semaphore-win.hpp"

#else
#error "unsupported architecture"

#endif
#endif
