/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_SEMAPHORE_LINUX_HPP
#define SYSKIT_SEMAPHORE_LINUX_HPP
#ifndef SYSKIT_SEMAPHORE_HPP
#error "unsupported usage"
#endif

BEGIN_NAMESPACE1(syskit)

//! Acquire a token. Wait forever if necessary.
//! Return true if successful.
inline bool Semaphore::decrement()
{
    unsigned int delta = 1;
    bool ok = decrementBy(delta);
    return ok;
}

//! Acquire a token. Wait at most waitTimeInMsecs msecs.
//! Return true if successful.
inline bool Semaphore::decrement(unsigned int waitTimeInMsecs)
{
    unsigned int delta = 1;
    bool ok = decrementBy(delta, waitTimeInMsecs);
    return ok;
}

//! Release a token. Return true if successful.
inline bool Semaphore::increment()
{
    unsigned int delta = 1;
    bool ok = incrementBy(delta);
    return ok;
}

//! Return true if instance was successfully constructed.
inline bool Semaphore::isOk() const
{
    return (sem_.ok != 0);
}

//! Acquire a token. Do not wait.
//! Return true if successful.
inline bool Semaphore::tryDecrement()
{
    unsigned int delta = 1;
    bool ok = tryDecrementBy(delta);
    return ok;
}

//! Wait until this pool is full (i.e., at least numTokens tokens are
//! available). Wait forever if necessary. Return true if successful.
inline bool Semaphore::waitTilFull(unsigned int numTokens)
{
    bool ok = decrementBy(numTokens)? (incrementBy(numTokens), true): (false);
    return ok;
}

//! Wait until this pool is non-empty (i.e., at least one token is
//! available). Wait forever if necessary. Return true if successful.
inline bool Semaphore::waitTilNonEmpty()
{
    unsigned int numTokens = 1;
    bool ok = decrementBy(numTokens)? (incrementBy(numTokens), true): (false);
    return ok;
}

END_NAMESPACE1

#endif
