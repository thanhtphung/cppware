/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_SPIN_SECTION_HPP
#define SYSKIT_SPIN_SECTION_HPP

#include "syskit/Atomic32.hpp"
#include "syskit/Semaphore.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! section of critical code synchronized by a spin lock
class SpinSection
    //!
    //! A class representing a spinnable critical section. A spinnable critical
    //! section behaves like a standard critical section if the calling process
    //! is initially bound to one and only one processor. If the calling process
    //! is initially bound to more than one processor, a thread waits for a
    //! critical section by first spinning in a loop and if the critical section
    //! becomes free during the spin, the thread would avoid a real wait. The
    //! critical section should be locked/unlocked by contructing/destructing a
    //! lock using SpinSection::Lock. Use lock() and unlock() only if SpinSection::Lock
    //! cannot be used. Use tryLock() to avoid blocking calls. Unlike a standard
    //! critical section, recursive locks are not allowed. That is, recursive locks
    //! will result in a deadlock. Example:
    //!\code
    //! static SpinSection s_ss;
    //! s_ss.lock();
    //! thisFuncDoesNotWorkIfConcurrentlyUsed();
    //! thisFuncDoesNotWorkEitherIfConcurrentlyUsed();
    //! s_ss.unlock();
    //!\endcode
    //!
{

public:
    enum
    {
        DefaultSpinCount = 9876
    };

    SpinSection(unsigned int spinCount = DefaultSpinCount);
    ~SpinSection();

    bool isOk() const;
    bool setSpinCount(unsigned int spinCount);
    bool tryLock();
    unsigned int spinCount() const;
    void lock();
    void unlock();


    //! critical section lock
    class Lock
        //!
        //! A class representing a critical section lock. Construct an instance
        //! to lock a critical section, and destruct it to unlock the critical
        //! section. Example:
        //!\code
        //! static SpinSection s_ss;
        //! {
        //!   SpinSection::Lock lock(s_ss);
        //!   thisFuncDoesNotWorkIfConcurrentlyUsed();
        //!   thisFuncDoesNotWorkEitherIfConcurrentlyUsed();
        //! }
        //!\endcode
        //!
    {
    public:
        Lock(SpinSection& spiSection);
        ~Lock();
    private:
        SpinSection& ss_;
        Lock(const Lock&); //prohibit usage
        const Lock& operator=(const Lock&); //prohibit usage
    };

private:
    enum
    {
        Unlocked = 0,
        LockedWithNoWaiters = 1,
        LockedWith1Waiter = 2,
        LockedWith2Waiters = 3 //etc.
    };

    Atomic32 state_;
    Semaphore sem_;
    unsigned int spinCount_;

    SpinSection(const SpinSection&); //prohibit usage
    const SpinSection& operator=(const SpinSection&); //prohibit usage

};

//! Return true if instance was successfully constructed.
inline bool SpinSection::isOk() const
{
    return sem_.isOk();
}

//! Return the effective spin count. The effective spin count is zero if
//! the calling process is initially bound to one and only one processor.
inline unsigned int SpinSection::spinCount() const
{
    return spinCount_;
}

END_NAMESPACE1

#endif
