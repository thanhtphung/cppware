/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_SYS_WIN32_HPP
#define SYSKIT_SYS_WIN32_HPP
#ifndef SYSKIT_SYS_HPP
#error "unsupported usage"
#endif

extern "C" unsigned int __popcnt(unsigned int);

#pragma intrinsic(__popcnt)

#define SIZE_T_SPEC "%lu"
#define SIZE_T_SPECW L"%lu"

BEGIN_NAMESPACE1(syskit)

//! Return the 64-bit time stamp counter provided by the RDTSC assembly instruction.
inline unsigned long long volatile rdtsc()
{
    __asm rdtsc
    // 64-bit time stamp counter is in edx:eax
}

//! Return the number of set bits in mask using the popcnt intrinsic.
inline unsigned int popcnt(unsigned long long mask)
{
    const unsigned int* p = reinterpret_cast<const unsigned int*>(&mask);
    return __popcnt(p[0]) + __popcnt(p[1]);
}

END_NAMESPACE1

#endif
