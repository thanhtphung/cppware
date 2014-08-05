/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include <io.h>

#include "syskit-pch.h"
#include "syskit/SilentInputMode.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//!
//! Enter silent terminal input mode.
//!
SilentInputMode::SilentInputMode()
{
    oldMode_ = 0;
    if (_isatty(_fileno(stdin)) != 0)
    {
        h_ = GetStdHandle(STD_INPUT_HANDLE);
        GetConsoleMode(h_, &oldMode_);
        unsigned long newMode = oldMode_ & ~ENABLE_ECHO_INPUT;
        SetConsoleMode(h_, newMode);
    }
    else
    {
        h_ = INVALID_HANDLE_VALUE;
    }
}


//!
//! Exit silent terminal input mode.
//!
SilentInputMode::~SilentInputMode()
{
    puts("");
    if (h_ != INVALID_HANDLE_VALUE)
    {
        SetConsoleMode(h_, oldMode_);
    }
}

END_NAMESPACE1
