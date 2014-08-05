/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "syskit/MappedFile.hpp"
#include "syskit/Utf8Seq.hpp"
#include "syskit/sys.hpp"

using namespace syskit;

const unsigned long long TBD = 0xffffffffffffffffULL;

BEGIN_NAMESPACE

fd_t openReadOnlyFile(const wchar_t* path, unsigned long long& size)
{
    Utf8Seq seq;
    size_t numWchars = wcslen(path) + 1;
    seq.shrink(reinterpret_cast<const utf32_t*>(path), numWchars);
    fd_t fd = open(seq.ascii(), O_RDONLY);
    if (fd >= 0)
    {
        size = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);
        int flags = fcntl(fd, F_GETFD);
        if ((flags >= 0) && ((flags | FD_CLOEXEC) != 0))
        {
            flags |= FD_CLOEXEC;
            fcntl(fd, F_SETFD, flags);
        }
    }

    return fd;
}

END_NAMESPACE

BEGIN_NAMESPACE1(syskit)


#if 0
bool MappedFile::createFile(const wchar_t* path, bool failIfExists, unsigned long long size)
{
    unsigned long desiredAccess = GENERIC_READ | GENERIC_WRITE;
    unsigned long shareMode = FILE_SHARE_READ; //allow subsequent reads only
    unsigned long creationDisposition = failIfExists? CREATE_NEW: CREATE_ALWAYS;
    HANDLE h = CreateFileW(path, desiredAccess, shareMode, 0, creationDisposition, FILE_ATTRIBUTE_NORMAL, 0);
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
#endif


//!
//! Destroy existing contents. Load new contents from given path.
//! Return true if successful.
//!
bool MappedFile::loadFrom(const wchar_t* path)
{
    unsigned long long size;
    fd_t src = openReadOnlyFile(path, size);
    bool ok = false;
    if (src <= 0)
    {
        return ok;
    }

    if (!resize(size))
    {
        close(src);
        return ok;
    }

    ok = true;
    unsigned long long remainingBytes = size;
    for (int i = 0, lastMap = numMaps_ - 1; ok && (i <= lastMap); ++i)
    {
        unsigned char* dst = map_[i];
        size_t bytesToRead = (i < lastMap)? mapSize_: static_cast<size_t>(remainingBytes);
        ok = (read(src, dst, bytesToRead) == static_cast<ssize_t>(bytesToRead));
        remainingBytes -= bytesToRead;
    }

    close(src);
    return ok;
}


#if 0
bool MappedFile::openFile(const wchar_t* path, unsigned long long& size)
{

    // If read-only, allow subsequent writes. Otherwise, allow subsequent reads only.
    unsigned long desiredAccess = readOnly_? GENERIC_READ: GENERIC_READ | GENERIC_WRITE;
    unsigned long shareMode = readOnly_? FILE_SHARE_READ | FILE_SHARE_WRITE: FILE_SHARE_READ;
    HANDLE h = CreateFileW(path, desiredAccess, shareMode, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    bool ok = (h != INVALID_HANDLE_VALUE);
    if (ok)
    {
        fileHandle_ = h;
        SetHandleInformation(h, HANDLE_FLAG_INHERIT, 0);
        unsigned long hiSize;
        unsigned long loSize = GetFileSize(h, &hiSize);
        size = (static_cast<unsigned long long>(hiSize) << 32) | loSize;
    }

    return ok;
}
#endif


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

    off_t off = lseek(fileHandle_, size, SEEK_SET);
    bool ok = (off >= 0) && (ftruncate(fileHandle_, off) == 0);
    if (ok)
    {
        size_ = size;
    }

#if 0
    if (size_ > 0)
    {
        unsigned long pageProtection = PAGE_READWRITE;
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
#endif

    return ok;
}


#if 0
//
// The size of a memory map must be a multiple of some granularity.
// If given map size needs to be adjusted, return the next higher
// proper size. Allow use of zero to indicate no limit.
//
unsigned long MappedFile::adjustMapSize(unsigned long mapSize)
{
    if (mapSize > 0)
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        unsigned long oddSize = mapSize % si.dwAllocationGranularity;
        if (oddSize > 0)
        {
            mapSize = mapSize - oddSize + si.dwAllocationGranularity;
        }
    }

    return mapSize;
}
#endif


void MappedFile::closeFile()
{
    if (fileHandle_ >= 0)
    {
        close(fileHandle_);
    }
}


void MappedFile::construct(const wchar_t* path, bool failIfExists, unsigned long long size, unsigned int mapSize)
{
    fileHandle_ = -1;
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
    }

    closeFile();
    delete[] path_;
}


#if 0
//
// No map size limit or file fits in one map.
// Map non-empty file in one piece.
//
void MappedFile::map1View()
{
    numMaps_ = 1;
    map_ = new unsigned char*[1];
    unsigned long desiredAccess = readOnly_? FILE_MAP_READ: FILE_MAP_WRITE;
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
    unsigned long desiredAccess = readOnly_? FILE_MAP_READ: FILE_MAP_WRITE;
    unsigned long long unmappedSize = size_;
    numMaps_ = static_cast<unsigned long>((size_ - 1) / mapSize_ + 1);
    map_ = new unsigned char*[numMaps_];
    offset_ = new unsigned long long[numMaps_];
    unsigned long long offset = 0;
    for (unsigned long i = 0; i < numMaps_; ++i, offset += mapSize_, unmappedSize -= mapSize_)
    {
        unsigned long hiOffset = static_cast<unsigned long>(offset >> 32);
        unsigned long loOffset = static_cast<unsigned long>(offset);
        unsigned long mapSize = (unmappedSize >= mapSize_)? mapSize_: 0;
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
#endif

END_NAMESPACE1
