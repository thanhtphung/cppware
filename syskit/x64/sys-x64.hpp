/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
//! @file sys-x64.hpp
#ifndef SYSKIT_SYS_X64_HPP
#define SYSKIT_SYS_X64_HPP
#ifndef SYSKIT_SYS_HPP
#error "unsupported usage"
#endif

extern "C" unsigned long long __popcnt64(unsigned long long);
extern "C" unsigned long long __rdtsc();

#pragma intrinsic(__popcnt64)
#pragma intrinsic(__rdtsc)

#define SIZE_T_SPEC "%llu"
#define SIZE_T_SPECW L"%llu"

BEGIN_NAMESPACE1(syskit)

//! Return the 64-bit time stamp counter provided by the RDTSC assembly instruction.
inline unsigned long long volatile rdtsc()
{
    return __rdtsc();
}

//! Return the number of set bits in mask using the popcnt intrinsic.
inline unsigned int popcnt(unsigned long long mask)
{
    return static_cast<unsigned int>(__popcnt64(mask));
}

END_NAMESPACE1

#endif
