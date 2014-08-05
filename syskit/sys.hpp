/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_SYS_HPP
#define SYSKIT_SYS_HPP

//! \file sys.hpp

#include <string.h>
#include "syskit/macros.h"

#if __linux || __CYGWIN__
#include <wchar.h>
#include "syskit/linux/sys-linux.hpp"

#elif __FREERTOS__
#include "syskit/rtos/sys-rtos.hpp"

#elif _WIN32
#include "syskit/win/sys-win.hpp"

#if _M_X64
#include "syskit/x64/sys-x64.hpp"
#else
#include "syskit/win32/sys-win32.hpp"
#endif

#else
#error "unsupported architecture"

#endif

#ifdef htonl
#undef htonl
#endif
#define htonl syskit::htonl__

#ifdef htonll
#undef htonll
#endif
#define htonll syskit::htonll__

#ifdef htons
#undef htons
#endif
#define htons syskit::htons__

#ifdef ntohl
#undef ntohl
#endif
#define ntohl syskit::ntohl__

#ifdef ntohll
#undef ntohll
#endif
#define ntohll syskit::ntohll__

#ifdef ntohs
#undef ntohs
#endif
#define ntohs syskit::ntohs__

BEGIN_NAMESPACE1(syskit)

typedef unsigned char utf8_t;
typedef unsigned int utf32_t;
typedef unsigned short utf16_t;

extern const char XDIGIT[];
extern const char xdigit[];
extern const unsigned char BIT_COUNT[];
extern const unsigned char NIBBLE[];

//! The crt strdup() uses malloc/free. This strdup variant uses new/delete which is
//! more const-correct. For example, deleting a constant string  is allowed while
//! freeing a constant string is not.
inline char* strdup(const char* src)
{
    size_t n = strlen(src) + 1;
    char* s = new char[n];
    memcpy(s, src, n);
    return s;
}

//! The crt strdup() uses malloc/free. This strdup variant uses new/delete which is
//! more const-correct. For example, deleting a constant string  is allowed while
//! freeing a constant string is not.
inline wchar_t* strdup(const wchar_t* src)
{
    size_t n = wcslen(src) + 1;
    wchar_t* w = new wchar_t[n];
    memcpy(w, src, n * sizeof(w[0]));
    return w;
}

#if BYTE_ORDER == LITTLE_ENDIAN
inline unsigned int htonl__(unsigned int u32)
{
    return bswap32(u32);
}

inline unsigned int ntohl__(unsigned int u32)
{
    return bswap32(u32);
}

inline unsigned long long htonll__(unsigned long long u64)
{
    return bswap64(u64);
}

inline unsigned long long ntohll__(unsigned long long u64)
{
    return bswap64(u64);
}

inline unsigned short htons__(unsigned short u16)
{
    return bswap16(u16);
}

inline unsigned short ntohs__(unsigned short u16)
{
    return bswap16(u16);
}

#else
inline unsigned int htonl__(unsigned int u32)
{
    return u32;
}

inline unsigned int ntohl__(unsigned int u32)
{
    return u32;
}

inline unsigned long long htonll__(unsigned long long u64)
{
    return u64;
}

inline unsigned long long ntohll__(unsigned long long u64)
{
    return u64;
}

inline unsigned short htons__(unsigned short u16)
{
    return u16;
}

inline unsigned short ntohs__(unsigned short u16)
{
    return u16;
}
#endif

END_NAMESPACE1

#endif
