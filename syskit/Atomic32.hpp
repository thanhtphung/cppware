/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_ATOMIC32_HPP
#define SYSKIT_ATOMIC32_HPP

#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! atomic 32-bit number
class Atomic32
    //!
    //! A class representing an atomic unsigned int. Implemented using intrinsics
    //! where applicable. This is usually the most efficient way to represent numbers
    //! which must be synchronized across threads. Reference counting is an example
    //! of typical usage. As a convenience, AtomicWord is an alias of Atomic32 in x86
    //! builds. Example:
    //!\code
    //! if (refCount_++ == 0U) //refCount_ is an Atomic32 instance
    //! {
    //!   // code to be run by the one thread which just incremented the count from zero
    //! }
    //! :
    //! if (--refCount_ == 0U) //refCount_ is an Atomic32 instance
    //! {
    //!   // code to be run by the one thread which just decremented the count to zero
    //! }
    //!\endcode
    //!
{

public:
    typedef unsigned int item_t;

    Atomic32(item_t v = 0U);

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
        long n_;
        item_t v_;
    };

};

//! Construct an instance with given initial value.
inline Atomic32::Atomic32(item_t v)
{
    v_ = v;
}

//! Return the unsigned int value.
inline Atomic32::operator item_t() const
{
    return v_;
}

//! Return the unsigned int value.
inline Atomic32::item_t Atomic32::asWord() const
{
    return v_;
}

END_NAMESPACE1

#if __linux || __CYGWIN__ || __FREERTOS__
#include "syskit/linux/Atomic32-linux.hpp"

#elif _WIN32
#include "syskit/win/Atomic32-win.hpp"

#else
#error "unsupported architecture"

#endif
#endif
