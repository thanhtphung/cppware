/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_SYS_WIN_HPP
#define SYSKIT_SYS_WIN_HPP
#ifndef SYSKIT_SYS_HPP
#error "unsupported usage"
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <cstdlib>

extern "C" unsigned int __popcnt(unsigned int);
extern "C" void __cpuid(int[4], int);

#pragma intrinsic(__cpuid)
#pragma intrinsic(__popcnt)
#pragma intrinsic(_byteswap_ushort)
#pragma intrinsic(_byteswap_ulong)
#pragma intrinsic(_byteswap_uint64)

#define GCC_ATTR(x)

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#endif

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

#ifndef wcstoull
#undef wcstoull
#endif
#define wcstoull _wcstoui64

BEGIN_NAMESPACE1(syskit)

//! Swap bytes and return result.
//! 0x1234U becomes 0x3412U.
#define bswap16 _byteswap_ushort

//! Swap bytes and return result.
//! 0x12345678UL becomes 0x78563412UL.
#define bswap32 _byteswap_ulong

//! Swap bytes and return result.
//! 0x123456789abcdef0ULL becomes 0xf0debc9a78563412ULL.
#define bswap64 _byteswap_uint64

typedef _RTL_CRITICAL_SECTION criSection_t;
typedef HANDLE fd_t;
typedef HANDLE mutex_t;
typedef HANDLE sem_t;
typedef HANDLE shm_t;
typedef int socklen_t;
typedef long(__stdcall *winTopFilter_t)(_EXCEPTION_POINTERS* p);
typedef unsigned int threadId_t;
typedef unsigned long threadKey_t;
typedef unsigned long ulong32_t;

inline bool mkdir(const wchar_t* path)
{
    bool ok = (CreateDirectoryW(path, 0) != 0) || (GetLastError() == ERROR_ALREADY_EXISTS);
    return ok;
}

//! Return true if the popcnt intrinsic is supported.
inline bool popcntIsSupported()
{
    int info[4];
    __cpuid(info, 1 /*infoType*/);
    return ((info[2] & 0x00800000UL) != 0); //CPUID_FEAT_ECX_POPCNT: 1 <<23
}

//! Return the number of set bits in mask using the popcnt intrinsic.
inline unsigned int popcnt(unsigned int mask)
{
    return __popcnt(mask);
}

END_NAMESPACE1

#endif
