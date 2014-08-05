/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <math.h>

#include "syskit-pch.h"
#include "syskit/BitVec.hpp"
#include "syskit/Prime.hpp"
#include "syskit/macros.h"

const unsigned int MIN_PRIME = 2U;
const unsigned int MAX_PRIME32 = 4294967291U;
const unsigned int MAX_PRIME16 = 65521U;
const unsigned int MAX16 = 0xffffU;

BEGIN_NAMESPACE

class PrimeVec: public syskit::BitVec
{
public:
    PrimeVec();
    ~PrimeVec();
private:
    PrimeVec(const PrimeVec&); //prohibit usage
    const PrimeVec& operator =(const PrimeVec&); //prohibit usage
};

//
// Compute 16-bit odd prime numbers store them in the bit vector. The
// vector holds 32678 bits. Bit i is set if i*2+1 is a prime number.
// For example, bit 0 is clear since 1 is not a prime number and bit
// 1 is set since 3 is a prime number. This computation is done using
// a method similar to the sieve of Eratosthenes.
//
PrimeVec::PrimeVec():
syskit::BitVec(32768 /*maxBits*/, true /*initialVal*/)
{
    clear(0);
    for (unsigned int i = 3; i <= MAX16; i += 2)
    {
        if (isSet(i >> 1))
        {
            unsigned int nonPrime;
            for (unsigned int j = i; (nonPrime = i*j) <= MAX16; j += 2)
            {
                clear(nonPrime >> 1);
            }
        }
    }
}

PrimeVec::~PrimeVec()
{
}

END_NAMESPACE

BEGIN_NAMESPACE1(syskit)


//
// Return false if given number is a prime number using a brute-force
// method (i.e., by checking for divisibility). The given number is
// not divisible by the smaller primes (2, 3, 5, and 7), so don't bother
// checking those.
//
bool Prime::isDivisible(unsigned int num)
{
    bool answer = true;
    const BitVec& primeVec = computePrimes();
    unsigned int i = 4; //start here to get 11 as the next prime number
    for (unsigned int hi = static_cast<unsigned int>(sqrt(static_cast<double>(num)));;)
    {
        i = primeVec.nextSetBit(i);
        unsigned int primeNum = (i << 1) + 1;
        if (primeNum > hi)
        {
            answer = false;
            break;
        }
        if ((num % primeNum) == 0)
        {
            break;
        }
    }

    return answer;
}


//!
//! Return true if given number is prime.
//!
bool Prime::isOne(unsigned int num)
{

    // Perform a quick check based on number range.
    bool answer;
    if ((num < MIN_PRIME) || (num > MAX_PRIME32))
    {
        answer = false;
    }

    // The only even prime number is the smallest prime number.
    else if ((num & 1) == 0)
    {
        answer = (num == MIN_PRIME);
    }

    // Compute 16-bit prime numbers just once and search the result.
    else if (num <= MAX16)
    {
        const BitVec& primeVec = computePrimes();
        unsigned int i = (num - 1) >> 1;
        answer = primeVec.isSet(i);
    }

    // Use a brute-force method.
    else
    {
        answer = (((num % 3) != 0) && ((num % 5) != 0) && ((num % 7) != 0) && (!isDivisible(num)));
    }

    // Return true if given number is a prime number.
    return answer;
}


//
// On first use, 16-bit odd prime numbers are computed once and stored
// in a bit vector. The vector holds 32678 bits. Bit i is set if i*2+1
// is a prime number. For example, bit 0 is clear since 1 is not a prime
// number and bit 1 is set since 3 is a prime number. There is only one
// even prime number: 2, so effectively, this vector covers all 16-bit
// prime numbers. Return this vector.
//
const BitVec& Prime::computePrimes()
{
    static const PrimeVec s_primeVec;
    return s_primeVec;
}


//!
//! Find and return highest prime given ceiling. Return zero if none.
//!
unsigned int Prime::findHi(unsigned int hi)
{

    // No such numbers.
    unsigned int num;
    if (hi < MIN_PRIME)
    {
        num = 0;
    }

    // The largest prime will do.
    else if (hi >= MAX_PRIME32)
    {
        num = MAX_PRIME32;
    }

    // Compute 16-bit prime numbers just once and search the result.
    else if (hi <= MAX_PRIME16)
    {
        const BitVec& primeVec = computePrimes();
        unsigned int i = (hi - 1) >> 1;
        if (!primeVec.isSet(i))
        {
            i = primeVec.prevSetBit(i);
        }
        num = (i << 1) + 1;
    }

    // Find nearest prime number in a brute-force manner.
    else
    {
        for (num = (hi - 1) | 1;; num -= 2)
        {
            if (((num % 3) != 0) && ((num % 5) != 0) && ((num % 7) != 0) && (!isDivisible(num)))
            {
                break;
            }
        }
    }

    return num;
}


//!
//! Find and return lowest prime given floor. Return zero if none.
//!
unsigned int Prime::findLo(unsigned int lo)
{

    // No such numbers.
    unsigned int num;
    if (lo > MAX_PRIME32)
    {
        num = 0;
    }

    // The smallest prime will do.
    else if (lo <= MIN_PRIME)
    {
        num = MIN_PRIME;
    }

    // Compute 16-bit prime numbers just once and search the result.
    else if (lo <= MAX_PRIME16)
    {
        const BitVec& primeVec = computePrimes();
        unsigned int i = lo >> 1;
        if (!primeVec.isSet(i))
        {
            i = primeVec.nextSetBit(i);
        }
        num = (i << 1) + 1;
    }

    // Find nearest prime number in a brute-force manner.
    else
    {
        for (num = (lo - 1) | 1;; num += 2)
        {
            if (((num % 3) != 0) && ((num % 5) != 0) && ((num % 7) != 0) && (!isDivisible(num)))
            {
                break;
            }
        }
    }

    return num;
}

END_NAMESPACE1
