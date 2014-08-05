/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_ATOMIC_WORD_HPP
#define SYSKIT_ATOMIC_WORD_HPP

#if _M_X64 || __x86_64
#include "syskit/Atomic64.hpp"
#define AtomicWord Atomic64

#else
#include "syskit/Atomic32.hpp"
#define AtomicWord Atomic32

#endif
#endif
