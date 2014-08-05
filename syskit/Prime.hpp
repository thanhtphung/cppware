/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_PRIME_HPP
#define SYSKIT_PRIME_HPP

#include <string.h>
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)

class BitVec;


//! prime number
class Prime
    //!
    //! A class representing a prime number.
    //! Construct an instance to find one. Example:
    //!\code
    //! Prime n(123 /*lo*/); //n==127
    //! Prime n(200 /*lo*/); //n==211
    //!\endcode
    //!
{

public:
    Prime(const Prime& prime);
    Prime(unsigned int lo);
    operator unsigned int() const;
    const Prime& operator =(const Prime& prime);

    bool isOk() const;
    unsigned int asU32() const;

    static bool isOne(unsigned int num);
    static unsigned int findHi(unsigned int hi);
    static unsigned int findLo(unsigned int lo);

private:
    unsigned int prime_;

    static bool isDivisible(unsigned int);
    static const BitVec& computePrimes();

};

inline Prime::Prime(const Prime& prime)
{
    prime_ = prime.prime_;
}

//! Find the nearest prime number greater than or equal to lo. The
//! instance construction fails and has value zero if there are no
//! such numbers. Use isOk() to verify proper instantiation.
inline Prime::Prime(unsigned int lo)
{
    prime_ = findLo(lo);
}

//! Return instance as an unsigned 32-bit number.
inline Prime::operator unsigned int() const
{
    return prime_;
}

inline const Prime& Prime::operator =(const Prime& prime)
{
    prime_ = prime.prime_;
    return *this;
}

//! Return true if instance was successfully constructed.
inline bool Prime::isOk() const
{
    return prime_ != 0;
}

//! Return instance as an unsigned 32-bit number.
inline unsigned int Prime::asU32() const
{
    return static_cast<unsigned int>(prime_);
}

END_NAMESPACE1

#endif
