/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_SEMAPHORE_WIN_HPP
#define SYSKIT_SEMAPHORE_WIN_HPP
#ifndef SYSKIT_SEMAPHORE_HPP
#error "unsupported usage"
#endif

BEGIN_NAMESPACE1(syskit)


//! Acquire delta tokens. delta must be non-zero and must not be larger
//! than MAX_CAP. Wait forever if necessary. Return true if successful.
inline bool Semaphore::decrementBy(unsigned int delta)
{
    bool ok = acquireTokens(delta);
    return ok;
}

//! Return true if instance was successfully constructed.
inline bool Semaphore::isOk() const
{
    bool ok = (sem_ != 0);
    return ok;
}

//! Acquire delta tokens. delta must be non-zero and must not be larger
//! than MAX_CAP. Do not wait. Return true if successful.
inline bool Semaphore::tryDecrementBy(unsigned int delta)
{
    bool ok = tryAcquireTokens(delta);
    return ok;
}

END_NAMESPACE1

#endif
