/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/BitVec64.hpp"
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(syskit)


//!
//! Emulate the x64 _BitScanForward64() intrinsic which is not available for the x86 builds.
//!
unsigned char _BitScanForward64(unsigned long* index, unsigned long long mask)
{
    unsigned int curBit = 0;
    for (BitVec64::word_t m = 0x0000000000000001ULL; m; m <<= 1, ++curBit)
    {
        if (mask & m)
        {
            *index = curBit;
            return 1;
        }
    }

    return 0;
}


//!
//! Emulate the x64 _BitScanReverse64() intrinsic which is not available for the x86 builds.
//!
unsigned char _BitScanReverse64(unsigned long* index, unsigned long long mask)
{
    unsigned int curBit = 63;
    for (BitVec64::word_t m = 0x8000000000000000ULL; m; m >>= 1, --curBit)
    {
        if (mask & m)
        {
            *index = curBit;
            return 1;
        }
    }

    return 0;
}

END_NAMESPACE1
