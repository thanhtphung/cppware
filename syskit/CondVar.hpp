/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_COND_VAR_HPP
#define SYSKIT_COND_VAR_HPP

#if __linux || __CYGWIN__
#include "syskit/linux/CondVar-linux.hpp"

#elif _WIN32
#include "syskit/win/CondVar-win.hpp"

#else
#error "unsupported platform"

#endif
#endif
