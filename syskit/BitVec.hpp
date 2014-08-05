/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_BIT_VEC_HPP
#define SYSKIT_BIT_VEC_HPP

#if _M_X64 || __x86_64
#include "syskit/BitVec64.hpp"
#define BitVec BitVec64

#else
#include "syskit/BitVec32.hpp"
#define BitVec BitVec32

#endif
#endif
