/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_RO_ZIPPED_HPP
#define SYSKIT_RO_ZIPPED_HPP

#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! read-only zipped file
class RoZipped
    //!
    //! A class representing a read-only zipped file.
    //!
{

public:
    typedef bool(*extractionProgressCb_t)(void* arg,
        unsigned int itemsExtracted,
        unsigned int itemsBeingExtracted,
        unsigned long long bytesExtracted,
        unsigned long long bytesBeingExtracted);

    RoZipped(const wchar_t* path, unsigned int mapSize = 0);
    ~RoZipped();

    bool cancelExtraction();
    bool extract(const wchar_t* outDir, const wchar_t* item, bool flattenDirHierarchy = true) const;
    bool extractAll(const wchar_t* outDir, bool flattenDirHierarchy = true) const;
    bool extractionCanceled() const;
    bool isOk() const;
    const char* comment() const;
    const char* comment(size_t itemIndex) const;
    const wchar_t* item(size_t index) const;
    const wchar_t* path() const;
    unsigned int numItems() const;
    unsigned long long size(size_t itemIndex) const;
    void monitorExtraction(extractionProgressCb_t cb, void* arg);

private:
    typedef struct
    {
        const char* comment;
        const wchar_t* nameW;
        unsigned long long compressedSize;
        unsigned long long uncompressedSize;
    } item_t;

    const char* comment_;
    const wchar_t* path_;
    item_t* item_;
    unsigned int mapSize_;
    unsigned int numItems_;
    unsigned long long mutable rawOffset_;
    unsigned long long mutable rawSize_;
    void* zip_;

    // extraction progress
    bool mutable extractionCanceled_;
    extractionProgressCb_t extractionProgressCb_;
    unsigned int mutable itemsExtracted_;
    unsigned int mutable itemsBeingExtracted_;
    unsigned long long mutable bytesBeingExtracted_;
    unsigned long long mutable bytesExtracted_;
    void* extractionProgressCbArg_;

    RoZipped(const RoZipped&); //prohibit usage
    const RoZipped& operator =(const RoZipped&); //prohibit usage

    bool extractCurItemAs(const wchar_t*, unsigned long long) const;
    void cacheItems();
    void construct(const wchar_t*, unsigned int);

    static bool createItemDir(wchar_t*);
    static bool flattenOutPath(wchar_t*, const wchar_t*);
    static bool itemsMatch(const wchar_t*, const wchar_t*, bool);
    static bool noOp(void*, unsigned int, unsigned int, unsigned long long, unsigned long long);
    static size_t normalizeDir(wchar_t*);
    static void normalizeItem(char*);

    // Mimics of fopen(), fclose(), fwrite(), etc.
    static int zClose(void*, void*);
    static int zError(void*, void*);
    static long zSeek(void*, void*, unsigned long long, int);
    static unsigned long zRead(void*, void*, void*, unsigned long);
    static unsigned long long zTell(void*, void*);
    static void* zOpen(void*, const void*, int);

};

//! Cancel current extract() or extractAll().
//! Return true.
inline bool RoZipped::cancelExtraction()
{
    extractionCanceled_ = true;
    return true;
}

//! Return true if instance was successfully constructed.
inline bool RoZipped::isOk() const
{
    return (zip_ != 0);
}

//! Return true if last extraction was canceled.
inline bool RoZipped::extractionCanceled() const
{
    return extractionCanceled_;
}

//! Return the associated comment.
inline const char* RoZipped::comment() const
{
    return comment_;
}

//! Return the comment for given item.
//! Behavior is unpredictable if given index is invalid.
inline const char* RoZipped::comment(size_t itemIndex) const
{
    return item_[itemIndex].comment;
}

//! Return normalized name for given item.
//! Behavior is unpredictable if given index is invalid.
inline const wchar_t* RoZipped::item(size_t index) const
{
    return item_[index].nameW;
}

//! Return the file path.
inline const wchar_t* RoZipped::path() const
{
    return path_;
}

//! Return the number of items in the zipped file.
inline unsigned int RoZipped::numItems() const
{
    return numItems_;
}

//! Return the uncompressed file size for given item.
//! Behavior is unpredictable if given index is invalid.
inline unsigned long long RoZipped::size(size_t itemIndex) const
{
    return item_[itemIndex].uncompressedSize;
}

END_NAMESPACE1

#include "syskit/win/link-with-syskit.h"
#endif
