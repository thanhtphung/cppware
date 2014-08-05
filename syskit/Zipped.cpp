/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <zlib/contrib/minizip/zip.h>

#include "syskit-pch.h"
#include "syskit/MappedFile.hpp"
#include "syskit/Zipped.hpp"
#include "syskit/sys.hpp"

const size_t MAX_BASENAME_LENGTH = 255;
const size_t MAX_PATH_LENGTH = 4095;
const unsigned long long SIZE0_IN_BYTES = 64 * 1024;
const wchar_t BACKSLASH_W = L'\\';
const wchar_t SLASH_W = L'/';

BEGIN_NAMESPACE1(syskit)


Zipped::Zipped(const wchar_t* path, unsigned int mapSize)
{
    zlib_filefunc64_def cb;
    cb.opaque = this;
    cb.zclose_file = zClose;
    cb.zerror_file = zError;
    cb.zopen64_file = zOpen;
    cb.zread_file = 0;
    cb.zseek64_file = zSeek;
    cb.ztell64_file = zTell;
    cb.zwrite_file = zWrite;

    mapSize_ = mapSize;
    zip_ = zipOpen2_64(path, APPEND_STATUS_CREATE, 0, &cb); //rawOffset_, rawSize_, trueSize_
}


Zipped::~Zipped()
{
    if (zip_ != 0)
    {
        zipClose(zip_, 0);
    }
}


//!
//! Add given file to the zip. A compression level can be specified to dictate how
//! much the file will be compressed. Use -1 for default compression, 0 for no
//! compression, and 1 through 9 for minimal to maximal compression. Behavior is
//! unpredictable for other compression levels. Return true if successful.
//!
bool Zipped::addItem(const MappedFile& item, int compressionLevel)
{
    unsigned long long itemFiletime = getFiletime(item);
    wchar_t normalized[MAX_PATH_LENGTH + MAX_BASENAME_LENGTH + 1];
    const wchar_t* basename = normalizePath(normalized, item.path());
    bool ok = zipItem(item, basename, compressionLevel, itemFiletime);
    return ok;
}


//!
//! Add given file to the zip. A compression level can be specified to dictate how
//! much the file will be compressed. Use -1 for default compression, 0 for no
//! compression, and 1 through 9 for minimal to maximal compression. Behavior is
//! unpredictable for other compression levels. Return true if successful.
//!
bool Zipped::addItem(const wchar_t* path, int compressionLevel)
{
    wchar_t normalized[MAX_PATH_LENGTH + MAX_BASENAME_LENGTH + 1];
    const wchar_t* basename = normalizePath(normalized, path);
    bool readOnly = true;
    MappedFile item(normalized, readOnly, mapSize_);
    unsigned long long itemFiletime = getFiletime(item);
    bool ok = zipItem(item, basename, compressionLevel, itemFiletime);
    return ok;
}


bool Zipped::zGrow(void* arg, void* fp, unsigned int bytesNeeded)
{
    MappedFile* file = static_cast<MappedFile*>(fp);
    unsigned long long curSize = file->size();
    unsigned long long minSize = curSize + bytesNeeded;
    unsigned long long newSize = curSize;
    while ((newSize <<= 1) < minSize);
    bool ok = file->resize(newSize);
    if (ok)
    {
        Zipped& zipped = *static_cast<Zipped*>(arg);
        zipped.rawSize_ += newSize - curSize;
    }

    return ok;
}


size_t Zipped::normalizeDir(wchar_t* normalized, const wchar_t* dir)
{
    wchar_t* w;
    wcscpy_s(normalized, MAX_PATH_LENGTH + 1, dir);
    for (w = normalized; *w; ++w)
    {
        if (*w == BACKSLASH_W)
        {
            *w = SLASH_W;
        }
    }

    if (*(w - 1) != SLASH_W)
    {
        *w++ = SLASH_W;
        *w = 0;
    }

    size_t length = w - normalized;
    return length;
}


//!
//! Save zipped file w/ comment.
//! Return true if successful.
//!
bool Zipped::save(const char* comment)
{
    bool ok = (zipClose(zip_, comment) == ZIP_OK);
    zip_ = 0;
    return ok;
}


//
// Mimic fclose() using MappedFile support.
//
int Zipped::zClose(void* arg, void* fp)
{
    Zipped& zipped = *static_cast<Zipped*>(arg);
    MappedFile* file = static_cast<MappedFile*>(fp);
    file->resize(zipped.trueSize_);
    delete file;
    return 0;
}


//
// Mimic ferror() using MappedFile support.
//
int Zipped::zError(void* /*arg*/, void* /*fp*/)
{
    return 0;
}


//
// Mimic fseek() using MappedFile support.
//
long Zipped::zSeek(void* arg, void* fp, unsigned long long offset, int origin)
{
    Zipped& zipped = *static_cast<Zipped*>(arg);
    const MappedFile* file = static_cast<const MappedFile*>(fp);

    switch (origin)
    {
    case ZLIB_FILEFUNC_SEEK_CUR:
        zipped.rawOffset_ += offset;
        zipped.rawSize_ -= offset;
        break;
    case ZLIB_FILEFUNC_SEEK_END:
        zipped.rawOffset_ = file->size();
        zipped.rawSize_ = 0;
        break;
    default: //ZLIB_FILEFUNC_SEEK_SET
        zipped.rawOffset_ = offset;
        zipped.rawSize_ = file->size() - offset;
        break;
    }

    return 0;
}


//
// Mimic fwrite() using MappedFile support.
//
unsigned long Zipped::zWrite(void* arg, void* fp, const void* buf, unsigned long bufSize)
{
    Zipped& zipped = *static_cast<Zipped*>(arg);
    MappedFile* file = static_cast<MappedFile*>(fp);

    unsigned long bytesWritten;
    if ((bufSize <= zipped.rawSize_) || zGrow(arg, fp, bufSize))
    {
        bool allowOverlaps = false;
        file->setBytes(zipped.rawOffset_, bufSize, buf, allowOverlaps);
        zipped.rawOffset_ += bufSize;
        if (zipped.rawOffset_ > zipped.trueSize_)
        {
            zipped.trueSize_ = zipped.rawOffset_;
        }
        zipped.rawSize_ -= bufSize;
        bytesWritten = bufSize;
    }
    else
    {
        bytesWritten = 0;
    }

    return bytesWritten;
}


//
// Mimic ftell() using MappedFile support.
//
unsigned long long Zipped::zTell(void* arg, void* /*fp*/)
{
    const Zipped& zipped = *static_cast<const Zipped*>(arg);
    unsigned long long offset = zipped.rawOffset_;
    return offset;
}


//
// Normalize path by replacing backslashes w/ slashes.
// Return the normalized basename.
//
wchar_t* Zipped::normalizePath(wchar_t* normalized, const wchar_t* path)
{
    wcscpy_s(normalized, MAX_PATH_LENGTH + MAX_BASENAME_LENGTH + 1, path);
    for (wchar_t* w = normalized; *w; ++w)
    {
        if (*w == BACKSLASH_W)
        {
            *w = SLASH_W;
        }
    }

    wchar_t* basename = wcsrchr(normalized, SLASH_W);
    basename = (basename != 0)? (basename + 1): (normalized);
    return basename;
}


//
// Mimic fopen() using MappedFile support.
//
void* Zipped::zOpen(void* arg, const void* path, int mode)
{
    Zipped& zipped = *static_cast<Zipped*>(arg);
    unsigned long long size = SIZE0_IN_BYTES;
    bool failIfExists = ((mode & ZLIB_FILEFUNC_MODE_EXISTING) == 0);
    MappedFile* file = new MappedFile(static_cast<const wchar_t*>(path), size, failIfExists, zipped.mapSize_);
    if (file->isOk())
    {
        zipped.rawSize_ = file->size();
    }
    else
    {
        zipped.rawSize_ = 0;
        delete file;
        file = 0;
    }

    zipped.rawOffset_ = 0;
    zipped.trueSize_ = 0;
    return file;
}

END_NAMESPACE1
