/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <zlib/contrib/minizip/zip.h>

#include "syskit-pch.h"
#include "syskit/MappedFile.hpp"
#include "syskit/Zipped.hpp"
#include "syskit/macros.h"

#if _DEBUG
#pragma comment(lib,"zlibd")
#else
#pragma comment(lib,"zlib")
#endif

const size_t MAX_BASENAME_LENGTH = 255;
const size_t MAX_PATH_LENGTH = 4095;

BEGIN_NAMESPACE

typedef union
{
    FILETIME ft; //must be first
    unsigned long long ft64;
} filetime_t;

typedef union
{
    unsigned long long ft64; //must be first
    FILETIME ft;
} filetime64_t;

const zip_fileinfo* setFileInfo(zip_fileinfo* fi, unsigned long long itemFiletime)
{
    const zip_fileinfo* fip;
    if (itemFiletime == 0)
    {
        fip = 0;
    }
    else
    {
        filetime64_t filetime = {itemFiletime};
        FILETIME localFileTime;
        FileTimeToLocalFileTime(&filetime.ft, &localFileTime);
        unsigned short dosDate;
        unsigned short dosTime;
        FileTimeToDosDateTime(&localFileTime, &dosDate, &dosTime);
        memset(fi, 0, sizeof(*fi));
        fi->dosDate = (static_cast<unsigned long>(dosDate) << 16) | dosTime;
        fip = fi;
    }

    return fip;
}

END_NAMESPACE

BEGIN_NAMESPACE1(syskit)


//!
//! Add files in directory to the zip. Given file resides in the directory to be added.
//! A compression level can be specified to dictate how much the files will be compressed.
//! Use -1 for default compression, 0 for no compression, and 1 through 9 for minimal to
//! maximal compression. Behavior is unpredictable for other compression levels. Return
//! true if successful.
//!
bool Zipped::addDirectory(const MappedFile& sibling, int compressionLevel)
{
    wchar_t normalized[MAX_PATH_LENGTH + MAX_BASENAME_LENGTH + 1];
    wchar_t* basename = normalizePath(normalized, sibling.path());
    wchar_t siblingPath[MAX_PATH_LENGTH + MAX_BASENAME_LENGTH + 1];
    wcscpy_s(siblingPath, MAX_PATH_LENGTH + MAX_BASENAME_LENGTH + 1, normalized);
    wcscpy_s(basename, MAX_BASENAME_LENGTH + 1, L"*.*");

    WIN32_FIND_DATAW found;
    HANDLE h = FindFirstFileW(normalized, &found);
    bool ok = (h != INVALID_HANDLE_VALUE);
    if (!ok)
    {
        return ok;
    }

    do
    {
        if ((found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
        {
            filetime_t filetime = {found.ftLastWriteTime};
            wcscpy_s(basename, MAX_BASENAME_LENGTH + 1, found.cFileName);
            if (_wcsicmp(normalized, siblingPath) == 0)
            {
                ok = zipItem(sibling, basename, compressionLevel, filetime.ft64);
            }
            else
            {
                bool readOnly = true;
                MappedFile item(normalized, readOnly, mapSize_);
                ok = zipItem(item, basename, compressionLevel, filetime.ft64);
            }
        }
    } while (ok && (FindNextFileW(h, &found) != 0));

    FindClose(h);
    return ok;
}


//!
//! Add files in given directory to the zip. A compression level can be specified to
//! dictate how much the files will be compressed. Use -1 for default compression, 0
//! for no compression, and 1 through 9 for minimal to maximal compression. Behavior
//! is unpredictable for other compression levels. Return true if successful.
//!
bool Zipped::addDirectory(const wchar_t* inDir, int compressionLevel)
{
    wchar_t normalized[MAX_PATH_LENGTH + MAX_BASENAME_LENGTH + 1];
    size_t length = normalizeDir(normalized, inDir);
    wchar_t* basename = normalized + length;
    wcscpy_s(basename, MAX_BASENAME_LENGTH + 1, L"*.*");

    WIN32_FIND_DATAW found;
    HANDLE h = FindFirstFileW(normalized, &found);
    bool ok = (h != INVALID_HANDLE_VALUE);
    if (!ok)
    {
        return ok;
    }

    do
    {
        if ((found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
        {
            filetime_t filetime = {found.ftLastWriteTime};
            wcscpy_s(basename, MAX_BASENAME_LENGTH + 1, found.cFileName);
            bool readOnly = true;
            MappedFile item(normalized, readOnly, mapSize_);
            ok = zipItem(item, basename, compressionLevel, filetime.ft64);
        }
    } while (ok && (FindNextFileW(h, &found) != 0));

    FindClose(h);
    return ok;
}


bool Zipped::zipItem(const MappedFile& item, const wchar_t* basename, int compressionLevel, unsigned long long itemFiletime)
{
    char s[MAX_BASENAME_LENGTH * 3 + 1]; //assume worst-case non-ASCII 3 bytes per character
    WideCharToMultiByte(CP_UTF8, 0, basename, -1, s, sizeof(s), 0, 0);
    zip_fileinfo fi;
    const zip_fileinfo* fip = setFileInfo(&fi, itemFiletime);
    zipOpenNewFileInZip(zip_, s, fip, 0, 0, 0, 0, 0, Z_DEFLATED, compressionLevel);

    unsigned int numMaps = item.numMaps();
    unsigned int bytesToWrite = (numMaps > 1)? mapSize_: static_cast<unsigned int>(item.size());
    unsigned long long remainingBytes = item.size();
    for (unsigned int i = 0; i < numMaps;)
    {
        const unsigned char* src = item.map(i);
        if (++i == numMaps)
        {
            bytesToWrite = static_cast<unsigned int>(remainingBytes);
        }
        zipWriteInFileInZip(zip_, src, bytesToWrite);
        remainingBytes -= bytesToWrite;
    }

    zipCloseFileInZip(zip_);
    bool ok = true;
    return ok;
}


unsigned long long Zipped::getFiletime(const MappedFile& item)
{
    unsigned long long itemFiletime = 0;
    HANDLE h = item.handle();
    if (h != INVALID_HANDLE_VALUE)
    {
        filetime64_t write = {0ULL};
        GetFileTime(h, 0 /*create*/, 0 /*access*/, &write.ft);
        itemFiletime = write.ft64;
    }

    return itemFiletime;
}

END_NAMESPACE1
