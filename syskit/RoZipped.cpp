/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <zlib/contrib/minizip/ioapi.h>
#include <zlib/contrib/minizip/unzip.h>

#include "syskit-pch.h"
#include "syskit/MappedFile.hpp"
#include "syskit/RoZipped.hpp"
#include "syskit/sys.hpp"

const char BACKSLASH = '\\';
const char SLASH = '/';
const char* const COMMENT = "";
const size_t MAX_BASEPATH_LENGTH = 4095;
const size_t MAX_ZIP_DIR_LENGTH = 4095;
const wchar_t BACKSLASH_W = L'\\';
const wchar_t SLASH_W = L'/';


BEGIN_NAMESPACE1(syskit)


//!
//! Construct an instance assuming given path is a zip file. Construction
//! can fail if the given path cannot be interpreted as a zip file. Use
//! isOk() to determine if the construction is successful.
//!
RoZipped::RoZipped(const wchar_t* path, unsigned int mapSize)
{
    construct(path, mapSize);
    // Initialize rawOffset_ and rawSize_ when used.
}


RoZipped::~RoZipped()
{
    if (zip_ != 0)
    {
        unzClose(zip_);
    }

    if (comment_ != COMMENT)
    {
        delete[] comment_;
    }

    for (unsigned int i = numItems_; i > 0;)
    {
        const item_t& item = item_[--i];
        delete[] item.comment;
        delete[] item.nameW;
    }
    delete[] item_;
    delete[] path_;
}


//!
//! Extract item into given directory. Item must match a value from the
//! item() method. Do flatten directory hierarchy if flattenDirHierarchy
//! is true. For example, item "a/b/c.d" would be extracted as "c.d" if
//! the directory hierarchy is flattened. Return true if successful. Return
//! false if failed or if extraction was canceled. Use extractionCanceled()
//! to clarify failure.
//!
bool RoZipped::extract(const wchar_t* outDir, const wchar_t* item, bool flattenDirHierarchy) const
{
    wchar_t path[MAX_ZIP_DIR_LENGTH + MAX_BASEPATH_LENGTH + 1];
    wcscpy_s(path, MAX_ZIP_DIR_LENGTH + 1, outDir);
    size_t length = normalizeDir(path);

    bool ok = false;
    extractionCanceled_ = false;
    unzGoToFirstFile(zip_);
    for (unsigned int i = 0; i < numItems_; unzGoToNextFile(zip_), ++i)
    {
        if (itemsMatch(item_[i].nameW, item, flattenDirHierarchy))
        {
            flattenDirHierarchy? (void)flattenOutPath(path + length, item): (void)wcscpy_s(path + length, MAX_BASEPATH_LENGTH + 1, item);
            wchar_t* w = wcsrchr(path, SLASH_W);
            *w = 0;
            ok = createItemDir(path);
            *w = SLASH_W;
            if (ok)
            {
                itemsExtracted_ = 0;
                itemsBeingExtracted_ = 1;
                bytesExtracted_ = 0;
                unsigned long long bytesBeingExtracted = item_[i].compressedSize;
                ok = extractCurItemAs(path, bytesBeingExtracted);
                itemsExtracted_ = 1;
            }
            break;
        }
    }

    return ok;
}


//!
//! Extract all items into given directory. Do flatten directory hierarchy if
//! flattenDirHierarchy is true. For example, item "a/b/c.d" would be extracted
//! as "c.d" if the directory hierarchy is flattened. Return true if successful.
//! Return false if failed or if extraction was canceled. Use extractionCanceled()
//! to clarify failure.
//!
bool RoZipped::extractAll(const wchar_t* outDir, bool flattenDirHierarchy) const
{
    wchar_t path[MAX_ZIP_DIR_LENGTH + MAX_BASEPATH_LENGTH + 1];
    wcscpy_s(path, MAX_ZIP_DIR_LENGTH + 1, outDir);
    size_t length = normalizeDir(path);

    bool ok = mkdir(path);
    if (!ok)
    {
        return ok;
    }

    extractionCanceled_ = false;
    itemsExtracted_ = 0;
    itemsBeingExtracted_ = numItems_;
    bytesExtracted_ = 0;
    unsigned long long bytesBeingExtracted = 0;
    for (unsigned int i = 0; i < numItems_; bytesBeingExtracted += item_[i++].compressedSize);

    unzGoToFirstFile(zip_);
    for (unsigned int i = 0; ok && (i < numItems_); unzGoToNextFile(zip_), ++i)
    {
        const wchar_t* item = item_[i].nameW;
        if (flattenDirHierarchy)
        {
            flattenOutPath(path + length, item);
        }
        else
        {
            wcscpy_s(path + length, MAX_BASEPATH_LENGTH + 1, item);
            wchar_t* w = wcsrchr(path, SLASH_W);
            *w = 0;
            createItemDir(path);
            *w = SLASH_W;
        }
        ok = extractCurItemAs(path, bytesBeingExtracted);
        ++itemsExtracted_;
    }

    return ok;
}


bool RoZipped::noOp(void* /*arg*/,
    unsigned int /*itemsExtracted*/,
    unsigned int /*itemsBeingExtracted*/,
    unsigned long long /*bytesExtracted*/,
    unsigned long long /*bytesBeingExtracted*/)
{
    bool keepGoing = true;
    return keepGoing;
}


//
// Mimic fclose() using MappedFile support.
//
int RoZipped::zClose(void* /*arg*/, void* fp)
{
    const MappedFile* file = static_cast<const MappedFile*>(fp);
    delete file;
    return 0;
}


//
// Mimic ferror() using MappedFile support.
//
int RoZipped::zError(void* /*arg*/, void* /*fp*/)
{
    return 0;
}


//
// Mimic fseek() using MappedFile support.
//
long RoZipped::zSeek(void* arg, void* fp, unsigned long long offset, int origin)
{
    const RoZipped& zipped = *static_cast<const RoZipped*>(arg);
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


size_t RoZipped::normalizeDir(wchar_t* path)
{
    wchar_t* w;
    for (w = path; *w; ++w)
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

    size_t length = w - path;
    return length;
}


//
// Mimic fread() using MappedFile support.
//
unsigned long RoZipped::zRead(void* arg, void* fp, void* buf, unsigned long bufSize)
{
    const RoZipped& zipped = *static_cast<const RoZipped*>(arg);
    const MappedFile* file = static_cast<const MappedFile*>(fp);

    size_t bytesRead = (zipped.rawSize_ >= bufSize)? bufSize: static_cast<size_t>(zipped.rawSize_);
    file->getBytes(buf, zipped.rawOffset_, bytesRead);
    zipped.rawOffset_ += bytesRead;
    zipped.rawSize_ -= bytesRead;

    // provide extraction progress
    // allow extraction cancellation
    if (zipped.bytesBeingExtracted_ > 0)
    {
        zipped.bytesExtracted_ += bytesRead;
        bool keepGoing = zipped.extractionProgressCb_(zipped.extractionProgressCbArg_,
            zipped.itemsExtracted_,
            zipped.itemsBeingExtracted_,
            zipped.bytesExtracted_,
            zipped.bytesBeingExtracted_);
        if (!keepGoing) zipped.extractionCanceled_ = true;
        if (zipped.extractionCanceled_) bytesRead = 0;
    }

    return static_cast<unsigned long>(bytesRead);
}


//
// Mimic ftell() using MappedFile support.
//
unsigned long long RoZipped::zTell(void* arg, void* fp)
{
    const RoZipped& zipped = *static_cast<const RoZipped*>(arg);
    const MappedFile* file = static_cast<const MappedFile*>(fp);
    unsigned long long offset = file->size() - zipped.rawSize_;
    return offset;
}


void RoZipped::construct(const wchar_t* path, unsigned int mapSize)
{
    comment_ = COMMENT;
    mapSize_ = mapSize;
    numItems_ = 0;
    path_ = strdup(path);
    zip_ = 0;

    // extraction progress
    itemsExtracted_ = 0;
    itemsBeingExtracted_ = 0;
    bytesExtracted_ = 0;
    bytesBeingExtracted_ = 0;
    extractionCanceled_ = false;
    extractionProgressCb_ = noOp;
    extractionProgressCbArg_ = 0;

    zlib_filefunc64_def cb;
    cb.opaque = this;
    cb.zclose_file = zClose;
    cb.zerror_file = zError;
    cb.zopen64_file = zOpen;
    cb.zread_file = zRead;
    cb.zseek64_file = zSeek;
    cb.ztell64_file = zTell;
    cb.zwrite_file = 0;
    unz_global_info64 hdr;
    unzFile zip = unzOpen2_64(path_, &cb);
    if (unzGetGlobalInfo64(zip, &hdr) != UNZ_OK)
    {
        unzClose(zip);
        item_ = new item_t[numItems_]; //create empty cache
        return;
    }

    if (hdr.size_comment > 0)
    {
        char* comment = new char[hdr.size_comment + 1];
        unzGetGlobalComment(zip, comment, hdr.size_comment + 1);
        comment_ = comment;
    }
    numItems_ = static_cast<unsigned int>(hdr.number_entry);
    zip_ = zip;
    cacheItems();
}


//!
//! Monitor subsequent extracts. During subsequent extract() or extractAll(), given
//! callback will be invoked with itemsExtracted, itemsBeingExtracted, bytesExtracted
//! and bytesBeingExtracted specifying the extraction progress. The callback invocation
//! frequency is not configurable, however. Given callback can be zero to disable
//! extraction monitoring. Extraction monitoring is initially disabled by default.
//! The callback should return true to continue extraction and should return false
//! to abort extraction.
//!
void RoZipped::monitorExtraction(extractionProgressCb_t cb, void* arg)
{
    extractionProgressCbArg_ = arg;
    extractionProgressCb_ = (cb == 0)? noOp: cb;
}


void RoZipped::normalizeItem(char* item)
{
    for (char* p = item; *p; ++p)
    {
        if (*p == BACKSLASH)
        {
            *p = SLASH;
        }
    }
}


//
// Mimic fopen() using MappedFile support.
//
void* RoZipped::zOpen(void* arg, const void* path, int mode)
{
    const RoZipped& zipped = *static_cast<const RoZipped*>(arg);
    bool readOnly = ((mode & ZLIB_FILEFUNC_MODE_WRITE) == 0);
    MappedFile* file = new MappedFile(static_cast<const wchar_t*>(path), readOnly, zipped.mapSize_);
    if (file->isOk())
    {
        zipped.rawOffset_ = 0;
        zipped.rawSize_ = file->size();
    }
    else
    {
        delete file;
        file = 0;
    }

    return file;
}

END_NAMESPACE1
