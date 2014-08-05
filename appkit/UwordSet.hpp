/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_UWORD_SET_HPP
#define APPKIT_UWORD_SET_HPP

#if _M_X64 || __x86_64
#include "appkit/U64Set.hpp"
#define UwordSet U64Set

#else
#include "appkit/U32Set.hpp"
#define UwordSet U32Set

#endif
#endif
