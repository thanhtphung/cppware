/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "syskit-pch.h"
#include "syskit/MappedFile.hpp"
#include "syskit/sys.hpp"

using namespace syskit;

const unsigned long long TBD = 0xffffffffffffffffULL;

BEGIN_NAMESPACE

fd_t openReadOnlyFile(const wchar_t* path, unsigned long long& size)
{
    DWORD desiredAccess = GENERIC_READ;
    DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE; //allow subsequent writes
    fd_t h = CreateFileW(path, desiredAccess, shareMode, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (h != INVALID_HANDLE_VALUE)
    {
        DWORD hiSize;
        DWORD loSize = GetFileSize(h, &hiSize);
        size = (static_cast<unsigned long long>(hiSize) << 32) | loSize;
    }

    return h;
}

END_NAMESPACE

BEGIN_NAMESPACE1(syskit)


bool MappedFile::createFile(const wchar_t* path, bool failIfExists, unsigned long long size)
{
    DWORD desiredAccess = GENERIC_READ | GENERIC_WRITE;
    DWORD shareMode = FILE_SHARE_READ; //allow subsequent reads only
    DWORD creationDisposition = failIfExists? CREATE_NEW: CREATE_ALWAYS;
    fd_t h = CreateFileW(path, desiredAccess, shareMode, 0, creationDisposition, FILE_ATTRIBUTE_NORMAL, 0);
    bool ok = (h != INVALID_HANDLE_VALUE);
    if (ok)
    {
        fileHandle_ = h;
        SetHandleInformation(h, HANDLE_FLAG_INHERIT, 0);
        long hiSize = static_cast<long>(size >> 32);
        long loSize = static_cast<long>(size & 0xffffffffULL);
        bool failed = (SetFilePointer(h, loSize, &hiSize, FILE_BEGIN) == INVALID_SET_FILE_POINTER) && (GetLastError() != NO_ERROR);
        ok = (!failed) && (SetEndOfFile(h) != 0);
    }

    return ok;
}


//!
//! Destroy existing contents. Load new contents from given path.
//! Return true if successful.
//!
bool MappedFile::loadFrom(const wchar_t* path)
{
    unsigned long long size;
    fd_t src = openReadOnlyFile(path, size);
    bool ok = false;
    if (src == INVALID_HANDLE_VALUE)
    {
        return ok;
    }

    if (!resize(size))
    {
        CloseHandle(src);
        return ok;
    }

    ok = true;
    unsigned long long remainingBytes = size;
    for (int i = 0, lastMap = numMaps_ - 1; ok && (i <= lastMap); ++i)
    {
        unsigned char* dst = map_[i];
        unsigned int bytesToRead = (i < lastMap)? mapSize_: static_cast<unsigned int>(remainingBytes);
        DWORD bytesRead;
        ok = ReadFile(src, dst, bytesToRead, &bytesRead, 0) && (bytesRead == bytesToRead);
        remainingBytes -= bytesRead;
    }

    CloseHandle(src);
    return ok;
}


bool MappedFile::openFile(const wchar_t* path, unsigned long long& size)
{

    // If read-only, allow subsequent writes. Otherwise, allow subsequent reads only.
    DWORD desiredAccess = readOnly_? GENERIC_READ: GENERIC_READ | GENERIC_WRITE;
    DWORD shareMode = readOnly_? FILE_SHARE_READ | FILE_SHARE_WRITE: FILE_SHARE_READ;
    fd_t h = CreateFileW(path, desiredAccess, shareMode, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    bool ok = (h != INVALID_HANDLE_VALUE);
    if (ok)
    {
        fileHandle_ = h;
        SetHandleInformation(h, HANDLE_FLAG_INHERIT, 0);
        DWORD hiSize;
        DWORD loSize = GetFileSize(h, &hiSize);
        size = (static_cast<unsigned long long>(hiSize) << 32) | loSize;
    }

    return ok;
}


//!
//! Resize file. No-op if same size or if mapped file is read-only.
//! Return true if successful.
//!
bool MappedFile::resize(unsigned long long size)
{
    if (readOnly_)
    {
        bool ok = false;
        return ok;
    }

    if (size_ == size)
    {
        bool ok = true;
        return ok;
    }

    unmap();
    if (mapHandle_ != 0)
    {
        CloseHandle(mapHandle_);
    }

    long hiSize = static_cast<long>(size >> 32);
    long loSize = static_cast<long>(size & 0xffffffffULL);
    bool failed = (SetFilePointer(fileHandle_, loSize, &hiSize, FILE_BEGIN) == INVALID_SET_FILE_POINTER) &&
        (GetLastError() != NO_ERROR);
    bool ok = (!failed) && (SetEndOfFile(fileHandle_) != 0);
    if (ok)
    {
        size_ = size;
    }

    if (size_ > 0)
    {
        DWORD pageProtection = PAGE_READWRITE;
        mapHandle_ = CreateFileMappingW(fileHandle_, 0, pageProtection, 0, 0, 0);
        ((mapSize_ == 0) || (mapSize_ >= size_))? map1View(): mapNViews();
    }
    else
    {
        map_ = 0;
        mapHandle_ = 0;
        numMaps_ = 0;
        offset_ = 0;
    }

    return ok;
}


//
// The size of a memory map must be a multiple of some granularity.
// If given map size needs to be adjusted, return the next higher
// proper size. Allow use of zero to indicate no limit.
//
unsigned int MappedFile::adjustMapSize(unsigned int mapSize)
{
    if (mapSize > 0)
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        unsigned int oddSize = mapSize % si.dwAllocationGranularity;
        if (oddSize > 0)
        {
            mapSize = mapSize - oddSize + si.dwAllocationGranularity;
        }
    }

    return mapSize;
}


void MappedFile::closeFile()
{
    if (fileHandle_ != INVALID_HANDLE_VALUE)
    {
        CloseHandle(fileHandle_);
    }
}


void MappedFile::construct(const wchar_t* path, bool failIfExists, unsigned long long size, unsigned int mapSize)
{
    fileHandle_ = INVALID_HANDLE_VALUE;
    map_ = 0;
    mapHandle_ = 0;
    mapSize_ = adjustMapSize(mapSize);
    mapSize2_ = 0;
    if ((mapSize_ > 0) && ((mapSize_ & (mapSize_ - 1)) == 0)) //map size is power of two?
    {
        for (unsigned int mask = mapSize_ >> 1; mask; ++mapSize2_, mask >>= 1);
    }
    numMaps_ = 0;
    offset_ = 0;

    size_t n = wcslen(path) + 1;
    path_ = new wchar_t[n];
    memcpy(path_, path, n * sizeof(*path_));

    ok_ = (size == TBD)? openFile(path, size): createFile(path, failIfExists, size);
    if (ok_)
    {
        size_ = size;
        if (size_ > 0)
        {
            DWORD pageProtection = readOnly_? PAGE_READONLY: PAGE_READWRITE;
            mapHandle_ = CreateFileMappingW(fileHandle_, 0, pageProtection, 0, 0, 0);
            ((mapSize_ == 0) || (mapSize_ >= size_))? map1View(): mapNViews();
        }
    }
    else
    {
        size_ = 0;
    }
}


void MappedFile::destruct()
{
    if (size_ > 0)
    {
        unmap();
        if (mapHandle_ != 0)
        {
            CloseHandle(mapHandle_);
        }
    }

    closeFile();
    delete[] path_;
}


//
// No map size limit or file fits in one map.
// Map non-empty file in one piece.
//
void MappedFile::map1View()
{
    numMaps_ = 1;
    map_ = new unsigned char*[1];
    DWORD desiredAccess = readOnly_? FILE_MAP_READ: FILE_MAP_WRITE;
    map_[0] = static_cast<unsigned char*>(MapViewOfFile(mapHandle_, desiredAccess, 0, 0, 0));
    if (map_[0] == 0)
    {
        ok_ = false;
    }

    offset_ = new unsigned long long[1];
    offset_[0] = 0;
}


//
// Map non-empty file piecemeal, mapSize_ bytes at a time.
//
void MappedFile::mapNViews()
{
    DWORD desiredAccess = readOnly_? FILE_MAP_READ: FILE_MAP_WRITE;
    unsigned long long unmappedSize = size_;
    numMaps_ = static_cast<unsigned int>((size_ - 1) / mapSize_ + 1);
    map_ = new unsigned char*[numMaps_];
    offset_ = new unsigned long long[numMaps_];
    unsigned long long offset = 0;
    for (unsigned int i = 0; i < numMaps_; ++i, offset += mapSize_, unmappedSize -= mapSize_)
    {
        DWORD hiOffset = static_cast<DWORD>(offset >> 32);
        DWORD loOffset = static_cast<DWORD>(offset);
        unsigned int mapSize = (unmappedSize >= mapSize_)? mapSize_: 0;
        map_[i] = static_cast<unsigned char*>(MapViewOfFile(mapHandle_, desiredAccess, hiOffset, loOffset, mapSize));
        if (map_[i] == 0)
        {
            ok_ = false;
        }
        offset_[i] = offset;
    }
}


void MappedFile::unmap()
{
    delete[] offset_;
    for (size_t i = numMaps_; i > 0;)
    {
        const void* p = map_[--i];
        if (p != 0)
        {
            UnmapViewOfFile(p);
        }
    }

    delete[] map_;
}

END_NAMESPACE1
