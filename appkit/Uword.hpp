/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_UWORD_HPP
#define APPKIT_UWORD_HPP

#if _M_X64 || __x86_64
#include "appkit/U64.hpp"
#define Uword U64

#else
#include "appkit/U32.hpp"
#define Uword U32

#endif
#endif
