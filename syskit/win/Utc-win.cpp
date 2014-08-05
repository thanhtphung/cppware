/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "syskit-pch.h"
#include "syskit/Utc.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//!
//! Return current time as windows filetime (100-nsecs since 1/1/1601).
//!
unsigned long long Utc::nowAsFiletime()
{
    union
    {
        FILETIME now;
        unsigned long long now64;
    };
    GetSystemTimeAsFileTime(&now);
    return now64;
}

END_NAMESPACE1
