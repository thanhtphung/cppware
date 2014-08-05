/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <cstdio>
#include <string.h>

#include "syskit-pch.h"
#include "syskit/Module.hpp"
#include "syskit/sys.hpp"

const wchar_t SLASH = L'/';

BEGIN_NAMESPACE1(syskit)


//!
//! Construct an invalid instance.
//!
Module::Module()
{
    addr_ = 0;
    size_ = 0;

    path_ = 0;
    basename_ = 0;

    fileVer_ = 0;
    prodVer_ = 0;
    ok_ = false;
}


Module::~Module()
{
    delete[] path_;
}


//!
//! Reset instance. Return true if successful (given path identifies a module
//! with accessible versions).
//!
bool Module::reset(const wchar_t* path, const unsigned char* addr, unsigned int size)
{
    addr_ = addr;
    size_ = size;

    delete[] path_;
    path_ = strdup(path);
    const wchar_t* w = wcsrchr(path_, SLASH);
    basename_ = (w != 0)? (w + 1): (path_);

    ok_ = getVers(path_); //fileVer_, prodVer_
    return ok_;
}


const wchar_t* Module::formVerString(wchar_t verString[MaxVerStringLength + 1], unsigned long long ver)
{
    unsigned int a = static_cast<unsigned int>(ver >> 48);
    unsigned int b = static_cast<unsigned int>(ver >> 32) & 0xffffU;
    unsigned int c = static_cast<unsigned int>(ver >> 16) & 0xffffU;
    unsigned int d = static_cast<unsigned int>(ver)& 0xffffU;
    swprintf(verString, MaxVerStringLength + 1, L"%u.%u.%u.%u", a, b, c, d);

    return verString;
}

END_NAMESPACE1
