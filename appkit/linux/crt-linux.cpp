/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
//! @file crt-linux.cpp
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>
#include <wchar.h>
#include <cstdio>
#include "appkit/Path.hpp"
#include "appkit/String.hpp"
#include "appkit/crt.hpp"
#include "syskit/macros.h"

const size_t BUF_SIZE = 4095;

BEGIN_NAMESPACE1(appkit)


//!
//! access() wrapper.
//!
int access(const String& path, int mode)
{
    int rc = ::access(path.ascii(), mode);
    return rc; //0==success
}


//!
//! chdir() wrapper
//!
int chdir(const String& path)
{
    int rc = ::chdir(path.ascii());
    return rc; //0==success
}


//!
//! chmod() wrapper.
//!
int chmod(const String& path, int mode)
{
    int rc = ::chmod(path.ascii(), mode);
    return rc; //0==success
}


//!
//! link() wrapper
//!
int link(const String& oldPath, const String& newPath)
{
    int rc = ::link(oldPath.ascii(), newPath.ascii());
    return rc; //0==success
}


//!
//! mkdir() wrapper
//!
int mkdir(const String& path)
{
    mode_t mode = 0777;
    int rc = ::mkdir(path.ascii(), mode);
    return rc; //0==success
}


//!
//! mkdir() wrapper. However, do create intermediate directories as necessary.
//!
int mkdirRecursively(const String& path, bool skipNormalization)
{
    int rc = mkdir(path);
    if ((rc != 0) && (errno == ENOENT))
    {
        Path parentPath(path, skipNormalization);
        rc = mkdirRecursively(parentPath.dirname(), skipNormalization);
        if (rc == 0)
        {
            rc = mkdir(path);
        }
    }

    return rc; //0==success
}


//!
//! rename() wrapper
//!
int rename(const String& oldPath, const String& newPath)
{
    int rc = std::rename(oldPath.ascii(), newPath.ascii());
    return rc; //0==success
}


//!
//! rmdir() wrapper
//!
int rmdir(const String& path)
{
    int rc = ::rmdir(path.ascii());
    return rc; //0==success
}


//!
//! sprintf() wrapper.
//!
int sprintf(String& s, const char* format, ...)
{
    char buf[BUF_SIZE + 1];
    va_list marker;
    va_start(marker, format);
    int length = vsprintf(buf, format, marker);
    va_end(marker);

    if (length < 0)
    {
        length = 0;
    }
    s.reset(buf, length);
    return length;
}


//!
//! swprintf() wrapper.
//!
int swprintf(String& s, const wchar_t* format, ...)
{
    wchar_t buf[BUF_SIZE + 1];
    va_list marker;
    va_start(marker, format);
    int length = vswprintf(buf, sizeof(buf) / sizeof(buf[0]), format, marker);
    va_end(marker);

    if (length < 0)
    {
        length = 0;
    }
    s.reset(buf, length);
    return length;
}


//!
//! unlink() wrapper
//!
int unlink(const String& path)
{
    int rc = ::unlink(path.ascii());
    return rc; //0==success
}


//!
//! fopen() wrapper
//!
std::FILE* fopen(const String& path, const String& mode)
{
    std::FILE* file = std::fopen(path.ascii(), mode.ascii());
    return file;
}

END_NAMESPACE1
