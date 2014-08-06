/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
//! @file crt-win.cpp
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include <errno.h>
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/DelimitedTxt.hpp"
#include "appkit/Path.hpp"
#include "appkit/String.hpp"
#include "appkit/crt.hpp"

using namespace appkit;

const size_t BUF_SIZE = 4095;
const size_t MAX_LENGTH = 4095;
const unsigned int EBUF_SIZ = 32766 + 1;

inline int resetErr()
{
    _set_errno(0);
    return 0;
}

BEGIN_NAMESPACE

int setErr()
{
    int err;
    unsigned int lastError = GetLastError();
    switch (lastError)
    {
    case ERROR_ACCESS_DENIED:   err = EPERM;  break;
    case ERROR_ALREADY_EXISTS:
    case ERROR_FILE_EXISTS:     err = EEXIST; break;
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:  err = ENOENT; break;
    case ERROR_NOT_SAME_DEVICE: err = EXDEV;  break;
    case ERROR_INVALID_NAME:
    default: err = EINVAL; break;
    }

    _set_errno(err);
    return -1;
}

int setAttr(const String::W& pathW, unsigned int attr)
{
    int rc = (SetFileAttributesW(pathW, attr) != 0)? resetErr(): setErr();
    return rc;
}

END_NAMESPACE

BEGIN_NAMESPACE1(appkit)


//!
//! getenv() wrapper. Return true if found. 
//!
bool getenv(const String& k, String& v, bool expandEnv)
{
    String::W kW(k.widen());
    wchar_t* vW = new wchar_t[EBUF_SIZ];
    size_t numWchars = GetEnvironmentVariableW(kW, vW, EBUF_SIZ - 1);
    bool ok = ((numWchars > 0) || (GetLastError() != ERROR_ENVVAR_NOT_FOUND));

    if (ok && expandEnv)
    {
        vW[numWchars] = 0;
        wchar_t* xW = new wchar_t[EBUF_SIZ];
        size_t n = ExpandEnvironmentStringsW(vW, xW, EBUF_SIZ - 1);
        n? v.reset(xW, n): v.reset(vW, numWchars);
        delete[] xW;
    }
    else
    {
        v.reset(vW, numWchars);
    }

    delete[] vW;
    return ok;
}


//!
//! Get a line from the standard input, but don't get more than maxLength characters.
//! Trim trailing delimiter (if any) from the returned line. Return true if successful.
//! Return false otherwise (error or end-of-file).
//!
bool getline(String& line, size_t maxLength, bool doTrimLine, bool doEchoInput)
{
    char buf0[MAX_LENGTH + 1];
    char* buf = (maxLength <= MAX_LENGTH)? buf0: new char[maxLength + 1];
    buf[0] = 0;
    bool ok = (fgets(buf, static_cast<int>(maxLength), stdin) == buf); //TODO: make fgetws() work
    line = buf;

    if (doEchoInput && ok)
    {
        size_t count = 1;
        std::fwrite(line.ascii(), line.byteSize() - 1, count, stdout);
    }

    if (doTrimLine)
    {
        DelimitedTxt txt;
        txt.trimLine(line);
    }

    if (buf != buf0)
    {
        delete[] buf;
    }

    return ok;
}


//!
//! putenv() wrapper. If v is zero, remove the environment variable named k.
//! Otherwise, create or update the environment key-value pair.
//!
bool putenv(const String& k, const String* v)
{
    String::W kW(k.widen());
    bool ok;
    if (v == 0)
    {
        // SetEnvironmentVariableW() in win7 (but not other platforms?) is ok for a nonexistent variable.
        wchar_t* vW = 0;
        ok = (GetEnvironmentVariableW(kW, vW, 0) > 0)? (SetEnvironmentVariableW(kW, 0) != 0): false;
    }
    else
    {
        String::W vW(v->widen());
        ok = (SetEnvironmentVariableW(kW, vW) != 0);
    }

    return ok;
}


//!
//! Minimal access() emulation. Check given path for accessibility specified
//! by mode (00=existence, 02=writable). Return 0 if given path is accessible.
//! Return non-zero otherwise.
//!
int access(const String& path, int mode)
{
    int rc;
    String::W pathW(path.widen());
    unsigned int attr = GetFileAttributesW(pathW);
    if (attr != INVALID_FILE_ATTRIBUTES)
    {
        switch (mode)
        {
        case 00:
            rc = resetErr();
            break;
        case 02:
            rc = ((attr & FILE_ATTRIBUTE_READONLY) == 0)? (resetErr()): (_set_errno(EACCES), -1);
            break;
        default:
            rc = (_set_errno(EINVAL), -1);
            break;
        }
    }
    else
    {
        rc = setErr();
    }

    return rc; //0==success
}


//!
//! chdir() wrapper
//!
int chdir(const String& path)
{
    String::W pathW(path.widen());
    int rc = (SetCurrentDirectoryW(pathW) != 0)? resetErr(): setErr();
    return rc; //0==success
}


//!
//! Minimal chmod() emulation. Change given path accessibility specified by
//! mode (00=readonly, 02=writable). Return 0 if successful. Return non-zero
//! otherwise.
//!
int chmod(const String& path, int mode)
{
    int rc;
    String::W pathW(path.widen());
    unsigned int attr = GetFileAttributesW(pathW);
    if (attr != INVALID_FILE_ATTRIBUTES)
    {
        switch (mode)
        {
        case 00:
            rc = ((attr & FILE_ATTRIBUTE_READONLY) == 0)?
                setAttr(pathW, attr | FILE_ATTRIBUTE_READONLY):
                resetErr();
            break;
        case 02:
            rc = ((attr & FILE_ATTRIBUTE_READONLY) != 0)?
                setAttr(pathW, attr & ~FILE_ATTRIBUTE_READONLY):
                resetErr();
            break;
        default:
            rc = (_set_errno(EINVAL), -1);
            break;
        }
    }
    else
    {
        rc = setErr();
    }

    return rc; //0==success
}


//!
//! CopyFile() wrapper. This Win32 API is technically not part of the
//! c-runtime library, but it fits here best.
//!
int copy(const String& srcPath, const String& dstPath, bool failIfExists)
{
    String::W srcPathW(srcPath.widen());
    String::W dstPathW(dstPath.widen());
    int rc = (CopyFileW(srcPathW, dstPathW, failIfExists) != 0)? resetErr(): setErr();
    return rc; //0==success
}


//!
//! link() wrapper
//!
int link(const String& oldPath, const String& newPath)
{
    String::W oldPathW(oldPath.widen());
    String::W newPathW(newPath.widen());
    int rc = (CreateHardLinkW(newPathW, oldPathW, 0) != 0)? resetErr(): setErr();
    return rc; //0==success
}


//!
//! mkdir() wrapper
//!
int mkdir(const String& path)
{
    String::W pathW(path.widen());
    int rc = (CreateDirectoryW(pathW, 0) != 0)? resetErr(): setErr();
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
    String::W oldPathW(oldPath.widen());
    String::W newPathW(newPath.widen());
    int rc = (MoveFileW(oldPathW, newPathW) != 0)? resetErr(): setErr();
    return rc; //0==success
}


//!
//! rmdir() wrapper
//!
int rmdir(const String& path)
{
    String::W pathW(path.widen());
    int rc = (RemoveDirectoryW(pathW) != 0)? resetErr(): setErr();
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
    int length = vsprintf_s(buf, sizeof(buf), format, marker);
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
    int length = vswprintf_s(buf, sizeof(buf) / sizeof(buf[0]), format, marker);
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
    String::W pathW(path.widen());
    int rc = (DeleteFileW(pathW) != 0)? resetErr(): setErr();
    return rc; //0==success
}


//!
//! fopen() wrapper
//!
std::FILE* fopen(const String& path, const String& mode)
{
    String::W pathW(path.widen());
    String::W modeW(mode.widen());
    std::FILE* file = 0;
    _wfopen_s(&file, pathW, modeW);
    return file;
}

END_NAMESPACE1
