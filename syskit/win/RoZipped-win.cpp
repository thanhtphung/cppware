/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <zlib/contrib/minizip/ioapi.h>
#include <zlib/contrib/minizip/unzip.h>

#include "syskit-pch.h"
#include "syskit/MappedFile.hpp"
#include "syskit/RoZipped.hpp"
#include "syskit/sys.hpp"

#if _DEBUG
#pragma comment(lib,"zlibd")
#else
#pragma comment(lib,"zlib")
#endif

const char* const COMMENT = "";
const size_t MAX_BASEPATH_LENGTH = 4095;
const size_t MAX_ZIP_DIR_LENGTH = 4095;
const wchar_t SLASH_W = L'/';


BEGIN_NAMESPACE1(syskit)


//
// Create directory for item extraction. Create intermediate directories if necessary.
// Return true if successful.
//
bool RoZipped::createItemDir(wchar_t* itemDir)
{
    bool ok = (CreateDirectoryW(itemDir, 0) != 0);
    if (ok)
    {
        return ok;
    }

    // If parent is non-existent, recursively create parent and try again.
    unsigned int error = GetLastError();
    if (error == ERROR_PATH_NOT_FOUND)
    {
        wchar_t* w = wcsrchr(itemDir, SLASH_W);
        if (w != 0)
        {
            *w = 0;
            ok = createItemDir(itemDir);
            *w = SLASH_W;
            if (ok)
            {
                ok = (CreateDirectoryW(itemDir, 0) != 0) || (GetLastError() == ERROR_ALREADY_EXISTS);
            }
        }
    }
    else if (error == ERROR_ALREADY_EXISTS)
    {
        ok = true;
    }

    return ok;
}


//
// Return true if successful.
// Return false if failed or if current extraction has been canceled.
//
bool RoZipped::extractCurItemAs(const wchar_t* path, unsigned long long bytesBeingExtracted) const
{
    unz_file_info64 hdr = {0};
    unzGetCurrentFileInfo64(zip_, &hdr, 0, 0, 0, 0, 0, 0);
    unsigned long long size = hdr.uncompressed_size;

    unzOpenCurrentFile(zip_);
    bytesBeingExtracted_ = bytesBeingExtracted;
    bool failIfExists = true;
    MappedFile file(path, size, failIfExists, mapSize_);
    unsigned int numMaps = file.numMaps();
    unsigned int bytesToRead = (numMaps > 1)? file.mapSize(): static_cast<unsigned int>(size);
    unsigned long long bytesRead = 0;
    for (unsigned int i = 0; i < numMaps; ++i)
    {
        unsigned char* dst = file.map(i);
        int rc = unzReadCurrentFile(zip_, dst, bytesToRead);
        bytesRead += rc;
        if (rc == 0) //extraction being canceled
        {
            break;
        }
    }

    bytesBeingExtracted_ = 0;
    unzCloseCurrentFile(zip_);
    bool ok = (bytesRead == size);
    if (ok)
    {
        unsigned short dosDate = (hdr.dosDate >> 16);
        unsigned short dosTime = (hdr.dosDate & 0xffffU);
        FILETIME localFileTime;
        DosDateTimeToFileTime(dosDate, dosTime, &localFileTime);
        FILETIME fileTime;
        LocalFileTimeToFileTime(&localFileTime, &fileTime);
        HANDLE h = file.handle();
        SetFileTime(h, &fileTime, 0, &fileTime);
    }

    return ok;
}


bool RoZipped::flattenOutPath(wchar_t* basename, const wchar_t* item)
{
    const wchar_t* w = wcsrchr(item, SLASH_W);
    w = (w != 0)? (w + 1): (item);
    wcscpy_s(basename, MAX_BASEPATH_LENGTH + 1, w);
    return true;
}


//
// Return true if items match. If directory hierarchy is not being flattened, then
// the key item has to be an exact match. Otherwise, the key item can be just the
// basename.
//
bool RoZipped::itemsMatch(const wchar_t* normalizedItem, const wchar_t* keyItem, bool flattenDirHierarchy)
{
    bool matched = (wcscmp(normalizedItem, keyItem) == 0);
    if ((!matched) && flattenDirHierarchy)
    {
        const wchar_t* w = wcsrchr(normalizedItem, SLASH_W);
        if ((w != 0) && (wcscmp(w + 1, keyItem) == 0))
        {
            matched = true;
        }
    }

    return matched;
}


void RoZipped::cacheItems()
{
    item_ = new item_t[numItems_];
    unzGoToFirstFile(zip_);
    for (unsigned int i = 0; i < numItems_; unzGoToNextFile(zip_), ++i)
    {
        unz_file_info64 hdr;
        unzGetCurrentFileInfo64(zip_, &hdr, 0, 0, 0, 0, 0, 0);
        char* name = new char[hdr.size_filename + 1];
        char* comment = new char[hdr.size_file_comment + 1];
        unzGetCurrentFileInfo64(zip_, &hdr, name, hdr.size_filename + 1, 0, 0, comment, hdr.size_file_comment + 1);
        normalizeItem(name);

        wchar_t* nameW = 0;
        int bufSizeInChars = MultiByteToWideChar(CP_UTF8, 0, name, hdr.size_filename + 1, nameW, 0);
        nameW = new wchar_t[bufSizeInChars];
        MultiByteToWideChar(CP_UTF8, 0, name, hdr.size_filename + 1, nameW, bufSizeInChars);

        delete[] name;
        item_t& item = item_[i];
        item.nameW = nameW;
        item.comment = comment;
        item.compressedSize = hdr.compressed_size;
        item.uncompressedSize = hdr.uncompressed_size;
    }
}

END_NAMESPACE1
