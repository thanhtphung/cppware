/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "syskit/sys.hpp"

#include "appkit-pch.h"
#include "appkit/Directory.hpp"

using namespace syskit;

BEGIN_NAMESPACE1(appkit)


//!
//! Return the current directory name.
//!
String Directory::getCurrent()
{
    String path;
    wchar_t pathW[MAX_PATH];
    size_t numWchars = GetCurrentDirectoryW(MAX_PATH, pathW);
    normalizeSlashes(pathW, numWchars);
    path.reset(pathW, numWchars);
    if (!nameIsDir(path))
    {
        path += MARKER;
    }

    return path;
}


String Directory::getTempPath()
{
    String path;
    wchar_t pathW[MAX_PATH];
    size_t numWchars = GetTempPathW(MAX_PATH, pathW);
    numWchars = GetLongPathNameW(pathW, pathW, MAX_PATH);
    normalizeSlashes(pathW, numWchars);
    path.reset(pathW, numWchars);
    if (!nameIsDir(path))
    {
        path += MARKER;
    }

    return path;
}


//!
//! Apply callback to directory entries. The callback should return true to
//! continue iterating and should return false to abort iterating. Return
//! false if the callback aborted the iterating. Return true otherwise.
//!
bool Directory::apply(cb0_t cb, void* arg) const
{

    // Assume normal iterating.
    bool ok = true;

    WIN32_FIND_DATAW found;
    String key(path_);
    key += "*.*";
    String::W keyW(key.widen());
    HANDLE h = FindFirstFileW(keyW, &found);
    if (h == INVALID_HANDLE_VALUE)
    {
        return ok;
    }

    String child;
    do
    {
        child = found.cFileName;
        if (found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if ((child == ".") || (child == "..")) continue;
            child += MARKER;
        }
        if (!cb(arg, *this, child))
        {
            ok = false;
            break;
        }
    } while (FindNextFileW(h, &found));

    // Return true if the iterating was not aborted.
    FindClose(h);
    return ok;
}


//!
//! Get attributes of given child.
//! Return true if successful.
//!
bool Directory::getChildAttr(Attr& attr, const String& childName) const
{
    bool ok;
    if (!childName.empty())
    {
        String path(path_);
        path += childName;
        String::W pathW(path.widen());
        WIN32_FILE_ATTRIBUTE_DATA data;
        ok = (GetFileAttributesExW(pathW, GetFileExInfoStandard, &data) != 0);
        if (ok)
        {
            union
            {
                FILETIME ft;
                unsigned long long time64;
            };
            unsigned long long time[3];
            ft = data.ftLastAccessTime;
            time[0] = time64;
            ft = data.ftCreationTime;
            time[1] = time64;
            ft = data.ftLastWriteTime;
            time[2] = time64;
            bool isDir = ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
            unsigned long long size = (static_cast<unsigned long long>(data.nFileSizeHigh) << 32) | data.nFileSizeLow;
            attr.reset(isDir, time, size);
        }
    }
    else
    {
        ok = false;
    }

    return ok;
}


//!
//! Return true if directory contains given child.
//!
bool Directory::hasChild(const String& childName) const
{
    if (!childName.empty())
    {
        String path(path_);
        path += childName;
        String::W pathW(path.widen());
        DWORD attr = GetFileAttributesW(pathW);
        if (attr != INVALID_FILE_ATTRIBUTES)
        {
            bool isDir = ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0);
            return (isDir == nameIsDir(childName));
        }
    }

    return false;
}


//!
//! Set current directory.
//! Return true if successful.
//!
bool Directory::setCurrent(const String& path)
{
    String::W str16(path.widen());
    return (SetCurrentDirectoryW(str16) != 0);
}


//!
//! Apply callback to directory entries.
//!
void Directory::apply(cb1_t cb, void* arg) const
{
    WIN32_FIND_DATAW found;
    String key(path_);
    key += "*.*";
    String::W keyW(key.widen());
    HANDLE h = FindFirstFileW(keyW, &found);
    if (h == INVALID_HANDLE_VALUE)
    {
        return;
    }

    String child;
    do
    {
        child = found.cFileName;
        if (found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if ((child == ".") || (child == "..")) continue;
            child += MARKER;
        }
        cb(arg, *this, child);
    } while (FindNextFileW(h, &found));

    FindClose(h);
}


//
// Normalize given path by replacing backslashes with slashes.
//
void Directory::normalizeSlashes(wchar_t* path, size_t length)
{
    const wchar_t BACKSLASH = L'\\';
    const wchar_t SLASH = L'/';
    const wchar_t* pEnd = path + length;
    for (wchar_t* p = path; p < pEnd; ++p)
    {
        if (*p == BACKSLASH)
        {
            *p = SLASH;
        }
    }
}


void Directory::validate()
{
    String::W pathW(path_.widen());
    unsigned int attr = GetFileAttributesW(pathW);
    ok_ = ((attr != INVALID_FILE_ATTRIBUTES) && ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0));

    // Normalize pathname.
    if (ok_)
    {
        normalizeSlashes(path_);
        if (!nameIsDir(path_))
        {
            path_ += MARKER;
        }
    }
}

END_NAMESPACE1
