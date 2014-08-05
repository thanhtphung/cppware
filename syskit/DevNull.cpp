/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <fcntl.h>
#include <io.h>

#include "syskit-pch.h"
#include "syskit/DevNull.hpp"
#include "syskit/macros.h"

const int INVALID_FD = -1;
const wchar_t DEV_NULL[] = L"NUL";

BEGIN_NAMESPACE1(syskit)


//!
//! Construct a null device.
//!
DevNull::DevNull()
{
    orig_ = INVALID_FD;
    clone_ = INVALID_FD;

#pragma warning(push)
#pragma warning(disable: 4996)
    null_ = _wopen(DEV_NULL, _O_RDWR);
#pragma warning(pop)
}


//!
//! Nullify given open file descriptor.
//!
DevNull::DevNull(int fd)
{
    orig_ = fd;
    clone_ = _dup(orig_);

#pragma warning(push)
#pragma warning(disable: 4996)
    null_ = _wopen(DEV_NULL, _O_RDWR);
#pragma warning(pop)

    _dup2(null_, orig_);
}


DevNull::~DevNull()
{
    _close(null_);
    if (orig_ != INVALID_FD)
    {
        _dup2(clone_, orig_);
        _close(clone_);
    }
}

END_NAMESPACE1
