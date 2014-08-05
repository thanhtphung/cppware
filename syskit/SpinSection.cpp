/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/BitVec.hpp"
#include "syskit/Process.hpp"
#include "syskit/SpinSection.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//!
//! Construct a spinnable critical section. A spinnable critical section
//! behaves like a standard critical section if the calling process is
//! initially bound to one and only one processor. If the calling process
//! is initially bound to more than one processor, a thread waits for a
//! critical section by first spinning in a loop and if the critical section
//! becomes free during the spin, the thread would avoid a real wait.
//!
SpinSection::SpinSection(unsigned int spinCount):
state_(Unlocked),
sem_(0U)
{
    setSpinCount(spinCount);
}


SpinSection::~SpinSection()
{
}


//!
//! Set the spin count. Spinning is allowed only if the calling process
//! is initially bound to more than one processor. The effective spin
//! count stays at zero otherwise. Return true if spinning is allowed.
//!
bool SpinSection::setSpinCount(unsigned int spinCount)
{
    static bool s_allowSpin = (BitVec::countSetBits(Process::affinityMask()) > 1);
    spinCount_ = s_allowSpin? spinCount: 0;
    return s_allowSpin;
}


//!
//! Lock this critical section. Do not wait. Return true if successful.
//!
bool SpinSection::tryLock()
{
    unsigned int oldState;
    state_.setIfEqual(LockedWithNoWaiters, Unlocked, oldState);
    return (oldState == Unlocked);
}


//!
//! Lock this critical section. Wait forever if necessary.
//!
void SpinSection::lock()
{
    for (;;)
    {

        // Successfully locked this critical section. Move on.
        unsigned int oldState;
        state_.setIfEqual(LockedWithNoWaiters, Unlocked, oldState);
        if (oldState == Unlocked)
        {
            return;
        }

        // Spin and monitor the lock status. As soon as it changes, stop spinning
        // and try again. If the spin limit has been reached, wait forever on the
        // semaphore. The semaphore will be incremented when the critical section
        // becomes unlocked. The spin limit is zero if the calling process is
        // initially bound to one and only one processor. If there are waiters,
        // don't bother spinning and just join the wait. Otherwise, there's a good
        // chance a late-arriving spinner would get the lock before the waiters.
        for (int i = (oldState > LockedWithNoWaiters)? 0: spinCount_;; --i)
        {
            if (i <= 0)
            {

                // Didn't really mean to, but successfully locked this critical section.
                // Move on.
                oldState = state_++;
                if (oldState == Unlocked)
                {
                    return;
                }

                // Wait forever until unlocked to try again.
                sem_.decrement();
                break;
            }
            if (state_.asWord() == Unlocked)
            {
                break;
            }
        }
    }
}


//!
//! Unlock this critical section.
//!
void SpinSection::unlock()
{

    // Unlock by updating the critical section's state.
    unsigned int oldState;
    state_.set(Unlocked, oldState);

    // Wake up all current waiters, so they can rejoin the fight for the lock.
    if (oldState > LockedWithNoWaiters)
    {
        unsigned int numWaiters = oldState - LockedWithNoWaiters;
        sem_.incrementBy(numWaiters);
    }
}


//!
//! Construct a lock for given critical section. Wait forever if necessary.
//! Given critical section is locked after the lock construction and remains
//! locked until the lock is destructed.
//!
SpinSection::Lock::Lock(SpinSection& spinSect):
ss_(spinSect)
{
    ss_.lock();
}


SpinSection::Lock::~Lock()
{
    ss_.unlock();
}

END_NAMESPACE1
