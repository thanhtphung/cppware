/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_CRT_WIN_HPP
#define APPKIT_CRT_WIN_HPP
#ifndef APPKIT_CRT_HPP
#error "unsupported usage"
#endif

#ifdef strtoull
#undef strtoull
#endif
#define strtoull _strtoui64

#ifdef vsnprintf
#undef vsnprintf
#endif
#define vsnprintf vsprintf_s

#endif
