/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_SYS_LINUX_HPP
#define SYSKIT_SYS_LINUX_HPP
#ifndef SYSKIT_SYS_HPP
#error "unsupported usage"
#endif

#include <limits.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <wchar.h>

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN __ORDER_LITTLE_ENDIAN
#endif

#ifndef BYTE_ORDER
#define BYTE_ORDER __BYTE_ORDER__
#endif

#ifndef MAX_PATH
#define MAX_PATH PATH_MAX
#endif

#define GCC_ATTR(x) __attribute__(x)
#define SIZE_T_SPEC "%zu"
#define SIZE_T_SPECW L"%zu"
#define sprintf_s snprintf
#define wmain main

BEGIN_NAMESPACE1(syskit)

typedef int fd_t;
typedef int shm_t;
typedef long(*winTopFilter_t)();
typedef pthread_t threadId_t;
typedef pthread_key_t threadKey_t;
typedef pthread_mutex_t criSection_t;
typedef pthread_mutex_t mutex_t;
typedef unsigned int ulong32_t;

typedef struct
{
    long id;
    unsigned short index;
    unsigned short ok;
} sem_t;

extern bool mkdir(const wchar_t*);

inline void cpuid(int code, unsigned int info[2])
{
    asm volatile("cpuid": "=a"(info[0]), "=d"(info[1]): "a"(code): "ecx", "ebx");
}

//! Return true if the popcnt intrinsic is supported.
inline bool popcntIsSupported()
{
#if GCC_VERSION >= 40800
    return (__builtin_cpu_supports("popcnt") != 0);
#else
    int code = 1;
    unsigned int info[2];
    cpuid(code, info);
    return ((info[0] & 0x00800000UL) != 0); //CPUID_FEAT_ECX_POPCNT: 1 <<23
#endif
}

//! Emulate the x86 _BitScanForward() msvc intrinsic.
inline unsigned char _BitScanForward(unsigned int* index, unsigned int mask)
{
    int leastSignificant1 = __builtin_ffs(mask);
    return leastSignificant1? ((*index = (leastSignificant1 - 1)), 1): (0);
}

//! Emulate the x64 _BitScanForward64() msvc intrinsic.
inline unsigned char _BitScanForward64(unsigned int* index, unsigned long long mask)
{
    int leastSignificant1 = __builtin_ffsl(mask);
    return leastSignificant1? ((*index = (leastSignificant1 - 1)), 1): (0);
}

//! Emulate the x86 _BitScanReverse() msvc intrinsic.
inline unsigned char _BitScanReverse(unsigned int* index, unsigned int mask)
{
    return mask? ((*index = (64 - __builtin_clz(mask))), 1): (0);
}

//! Emulate the x64 _BitScanReverse64() msvc intrinsic.
inline unsigned char _BitScanReverse64(unsigned int* index, unsigned long long mask)
{
    return mask? ((*index = (64 - __builtin_clzl(mask))), 1): (0);
}

//! Return the number of set bits in mask using the popcnt intrinsic.
inline unsigned int popcnt(unsigned int mask)
{
    return __builtin_popcount(mask);
}

//! Swap bytes and return result.
//! 0x12345678UL becomes 0x78563412UL.
inline unsigned int bswap32(unsigned int u32)
{
    unsigned int u23 = __builtin_bswap32(u32);
    return u23;
}

//! Swap bytes and return result.
//! 0x123456789abcdef0ULL becomes 0xf0debc9a78563412ULL.
inline unsigned long long bswap64(unsigned long long u64)
{
    unsigned long long u46 = __builtin_bswap64(u64);
    return u46;
}

//! Return the 64-bit time stamp counter provided by the RDTSC assembly instruction.
inline unsigned long long volatile rdtsc()
{
    register unsigned long long tsc;
    asm volatile("rdtsc": "=A"(tsc));
    return tsc;
}

//! Swap bytes and return result.
//! 0x1234U becomes 0x3412U.
inline unsigned short bswap16(unsigned short u16)
{
    unsigned short u61 = __builtin_bswap16(u16);
    return u61;
}

inline wchar_t* wcscpy_s(wchar_t* dst, size_t /*dstSize*/, const wchar_t* src)
{
    return wcscpy(dst, src);
}

END_NAMESPACE1

#endif
