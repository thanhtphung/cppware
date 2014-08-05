/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_ATOMIC64_HPP
#define SYSKIT_ATOMIC64_HPP

#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! atomic 64-bit number
class Atomic64
    //!
    //! A class representing an atomic unsigned long long. Implemented using intrinsics
    //! where applicable. This is usually the most efficient way to represent numbers
    //! which must be synchronized across threads. Reference counting is an example
    //! of typical usage. As a convenience, AtomicWord is an alias of Atomic64 in x64
    //! builds. Example:
    //!\code
    //! if (refCount_++ == 0ULL) //refCount_ is an Atomic64 instance
    //! {
    //!   // code to be run by the one thread which just incremented the count from zero
    //! }
    //! :
    //! if (--refCount_ == 0ULL) //refCount_ is an Atomic64 instance
    //! {
    //!   // code to be run by the one thread which just decremented the count to zero
    //! }
    //!\endcode
    //!
{

public:
    typedef unsigned long long item_t;

    Atomic64(item_t v = 0ULL);

    // Operators.
    operator item_t() const;
    item_t operator ++();
    item_t operator ++(int);
    item_t operator --();
    item_t operator --(int);
    void operator +=(item_t delta);
    void operator -=(item_t delta);
    void operator =(item_t v);

    // Getters.
    item_t asWord() const;

    // Setters.
    void decrement();
    void decrement(item_t& old);
    void decrementBy(item_t delta);
    void decrementBy(item_t delta, item_t& old);
    void increment();
    void increment(item_t& old);
    void incrementBy(item_t delta);
    void incrementBy(item_t delta, item_t& old);
    void set(item_t v);
    void set(item_t v, item_t& old);
    void setIfEqual(item_t v, item_t comperand);
    void setIfEqual(item_t v, item_t comperand, item_t& old);

private:
    volatile union
    {
        long long n_;
        item_t v_;
    };

};

END_NAMESPACE1

#if __linux || __CYGWIN__
#include "syskit/linux/Atomic64-linux.hpp"

#elif _WIN32
#include "syskit/win/Atomic64-win.hpp"

#if _M_X64
#include "syskit/x64/Atomic64-x64.hpp"
#else
#include "syskit/win32/Atomic64-win32.hpp"
#endif

#else
#error "unsupported architecture"

#endif
#endif
