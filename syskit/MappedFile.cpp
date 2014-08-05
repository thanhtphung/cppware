/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <string.h>

#include "syskit-pch.h"
#include "syskit/MappedFile.hpp"
#include "syskit/macros.h"

const unsigned long long TBD = 0xffffffffffffffffULL;

BEGIN_NAMESPACE1(syskit)

unsigned char MappedFile::emptyMark_ = 0x55U;


//!
//! Map given existing file. A zero mapSize in bytes indicates no limit (i.e., use one
//! and only one view regardless of file size). With a non-zero limit, a file can be
//! mapped into multiple views. Each view except the last covers a contiguous area
//! of this size. The last view covers a contiguous area up to this size. The file is
//! mapped in read-only mode if readOnly is true. Otherwise, the mapped file is writable.
//!
MappedFile::MappedFile(const wchar_t* path, bool readOnly, unsigned int mapSize)
{
    readOnly_ = readOnly;
    bool failIfExists = false;
    unsigned long long size = TBD;
    construct(path, failIfExists, size, mapSize);
}


//!
//! Create and map given file. A zero mapSize in bytes indicates no limit (i.e., use one
//! and only one view regardless of file size). With a non-zero limit, a file can be
//! mapped into multiple views. Each view except the last covers a contiguous area
//! of this size. The last view covers a contiguous area up to this size. The file is
//! created to be size bytes long initially.
//!
MappedFile::MappedFile(const wchar_t* path, unsigned long long size, bool failIfExists, unsigned int mapSize)
{
    readOnly_ = false;
    construct(path, failIfExists, size, mapSize);
}


MappedFile::~MappedFile()
{
    destruct();
}


//!
//! Return true if instance was constructed successfully.
//!
bool MappedFile::isOk() const
{
    return ok_;
}


//!
//! Reset and map given file. A zero mapSize in bytes indicates no limit (i.e., use one and
//! only one view regardless of file size). With a non-zero limit, a file can be
//! mapped into multiple views. Each view except the last covers a contiguous area
//! of this size. The last view covers a contiguous area up to this size. Given file
//! is mapped in read-only mode if readOnly is true. Otherwise, the mapped file is writable.
//!
bool MappedFile::remap(const wchar_t* path, bool readOnly, unsigned int mapSize)
{
    destruct();
    readOnly_ = readOnly;
    bool failIfExists = false;
    unsigned long long size = TBD;
    construct(path, failIfExists, size, mapSize);
    return ok_;
}


//!
//! Save contents in given path. Destroy existing contents at destination if necessary.
//! Return true if successful.
//!
bool MappedFile::saveIn(const wchar_t* path) const
{

    // No-op if saving undefined contents.
    if (!ok_)
    {
        return ok_;
    }

    bool failIfExists = false;
    MappedFile dstFile(path, size_, failIfExists, mapSize_);
    bool ok = dstFile.isOk();
    if (ok)
    {
        unsigned long long remainingBytes = size_;
        for (int i = 0, lastMap = numMaps_ - 1; i <= lastMap; ++i)
        {
            const unsigned char* src = map_[i];
            unsigned int bytesToWrite = (i < lastMap)? mapSize_: static_cast<unsigned int>(remainingBytes);
            unsigned char* dst = dstFile.map_[i];
            memcpy(dst, src, bytesToWrite);
            unsigned int bytesWritten = bytesToWrite;
            remainingBytes -= bytesWritten;
        }
    }

    return ok;
}


//!
//! Return the address of given file byte offset.
//!
unsigned char* MappedFile::addrOf(unsigned long long offset)
{
    if (offset >= size_)
    {
        return &emptyMark_;
    }

    // Given offset resides in first map.
    if ((offset < mapSize_) || (mapSize_ == 0))
    {
        return map_[0] + offset;
    }

    unsigned int i = static_cast<unsigned int>(mapSize2_? (offset >> mapSize2_): (offset / mapSize_));
    return map_[i] + (offset - offset_[i]);
}


//!
//! Copy bytes between two locations in the file. The two locations can overlap.
//! Behavior is unpredictable if current file size is insufficient.
//!
void MappedFile::copyBytes(unsigned long long dst, unsigned long long src, size_t byteCount)
{

    // No-op if copying zero bytes.
    // Also be nice and avoid divide-by-zero if file is empty.
    if ((byteCount == 0) || (size_ == 0))
    {
        return;
    }

    // There's only one map.
    if (numMaps_ == 1)
    {
        const unsigned char* srcAddr = map_[0] + src;
        unsigned char* dstAddr = map_[0] + dst;
        memmove(dstAddr, srcAddr, byteCount);
        return;
    }

    // Locate the housing map for given offset.
    unsigned int i = static_cast<unsigned int>(mapSize2_? (src >> mapSize2_): (src / mapSize_));
    unsigned long long curOffset = offset_[i];
    unsigned long long curEnding = curOffset + mapSize_;

    // Source resides in only one map.
    if ((src + byteCount) <= curEnding)
    {
        const unsigned char* srcAddr = map_[i] + (src - curOffset);
        bool allowOverlaps = true;
        setBytes(dst, byteCount, srcAddr, allowOverlaps);
        return;
    }

    // Source resides in multiple adjacent maps.
    const unsigned char* srcAddr = map_[i] + (src - curOffset);
    size_t copyCount = static_cast<size_t>(curEnding - src);
    for (bool allowOverlaps = true;;)
    {
        setBytes(dst, copyCount, srcAddr, allowOverlaps);
        byteCount -= copyCount;
        if (byteCount == 0)
        {
            break;
        }
        dst += copyCount;
        srcAddr = map_[++i];
        copyCount = (byteCount >= mapSize_)? mapSize_: byteCount;
    }
}


//!
//! Copy bytes, starting at given offset, into given buffer. Behavior is unpredictable
//! if given buffer size is insufficient.
//!
void MappedFile::getBytes(void* dst, unsigned long long offset, size_t byteCount) const
{

    // No-op if getting zero bytes.
    // Also be nice and avoid divide-by-zero if file is empty.
    if ((byteCount == 0) || (size_ == 0))
    {
        return;
    }

    // There's only one map.
    if (numMaps_ == 1)
    {
        const unsigned char* src = map_[0] + offset;
        memcpy(dst, src, byteCount);
        return;
    }

    // Locate the housing map for given offset.
    unsigned int i = static_cast<unsigned int>(mapSize2_? (offset >> mapSize2_): (offset / mapSize_));
    unsigned long long curOffset = offset_[i];
    unsigned long long curEnding = curOffset + mapSize_;

    // Need only one copy from one map.
    if ((offset + byteCount) <= curEnding)
    {
        const unsigned char* src = map_[i] + (offset - curOffset);
        memcpy(dst, src, byteCount);
        return;
    }

    // Copy from multiple adjacent maps.
    const unsigned char* src = map_[i] + (offset - curOffset);
    size_t copyCount = static_cast<size_t>(curEnding - offset);
    for (;;)
    {
        memcpy(dst, src, copyCount);
        byteCount -= copyCount;
        if (byteCount == 0)
        {
            break;
        }
        dst = static_cast<unsigned char*>(dst)+copyCount;
        src = map_[++i];
        copyCount = (byteCount >= mapSize_)? mapSize_: byteCount;
    }
}


//!
//! Copy bytes, from given buffer, into file starting at given offset. If given buffer
//! also resides in the file and might overlap with the destination, allowOverlaps should
//! be set to true. In general, copying bytes between two locations that might overlap is
//! more expensive than copying bytes between two locations that do not overlap. Behavior
//! is unpredictable if current file size is insufficient.
//!
void MappedFile::setBytes(unsigned long long offset, size_t byteCount, const void* src, bool allowOverlaps)
{

    // No-op if setting zero bytes.
    // Also be nice and avoid divide-by-zero if file is empty.
    if ((byteCount == 0) || (size_ == 0))
    {
        return;
    }

    // There's only one map.
    if (numMaps_ == 1)
    {
        unsigned char* dst = map_[0] + offset;
        allowOverlaps? memmove(dst, src, byteCount): memcpy(dst, src, byteCount);
        return;
    }

    // Locate the housing map for given offset.
    unsigned int i = static_cast<unsigned int>(mapSize2_? (offset >> mapSize2_): (offset / mapSize_));
    unsigned long long curOffset = offset_[i];
    unsigned long long curEnding = curOffset + mapSize_;

    // Need only one copy from one map.
    if ((offset + byteCount) <= curEnding)
    {
        unsigned char* dst = map_[i] + (offset - curOffset);
        allowOverlaps? memmove(dst, src, byteCount): memcpy(dst, src, byteCount);
        return;
    }

    // Copy from multiple adjacent maps.
    unsigned char* dst = map_[i] + (offset - curOffset);
    size_t copyCount = static_cast<size_t>(curEnding - offset);
    for (;;)
    {
        allowOverlaps? memmove(dst, src, copyCount): memcpy(dst, src, copyCount);
        byteCount -= copyCount;
        if (byteCount == 0)
        {
            break;
        }
        src = static_cast<const unsigned char*>(src)+copyCount;
        dst = map_[++i];
        copyCount = (byteCount >= mapSize_)? mapSize_: byteCount;
    }
}

END_NAMESPACE1
