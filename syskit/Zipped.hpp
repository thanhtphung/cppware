/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_ZIPPED_HPP
#define SYSKIT_ZIPPED_HPP

#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)

class MappedFile;


//! write-only zipped file
class Zipped
    //!
    //! A class representing a write-only zipped file.
    //!
{

public:
    Zipped(const wchar_t* path, unsigned int mapSize = 0);
    ~Zipped();

    bool addDirectory(const MappedFile& sibling, int compressionLevel = -1);
    bool addDirectory(const wchar_t* inDir, int compressionLevel = -1);
    bool addItem(const MappedFile& item, int compressionLevel = -1);
    bool addItem(const wchar_t* path, int compressionLevel = -1);
    bool isOk() const;
    bool save(const char* comment);

private:
    unsigned int mapSize_;
    unsigned long long mutable rawOffset_;
    unsigned long long mutable rawSize_;
    unsigned long long mutable trueSize_;
    void* zip_;

    Zipped(const Zipped&); //prohibit usage
    const Zipped& operator =(const Zipped&); //prohibit usage

    bool zipItem(const MappedFile&, const wchar_t*, int, unsigned long long);

    static size_t normalizeDir(wchar_t*, const wchar_t*);
    static unsigned long long getFiletime(const MappedFile&);
    static wchar_t* normalizePath(wchar_t*, const wchar_t*);

    // Mimics of fopen(), fclose(), fwrite(), etc.
    static bool zGrow(void*, void*, unsigned int);
    static int zClose(void*, void*);
    static int zError(void*, void*);
    static long zSeek(void*, void*, unsigned long long, int);
    static unsigned long zWrite(void*, void*, const void*, unsigned long);
    static unsigned long long zTell(void*, void*);
    static void* zOpen(void*, const void*, int);

};

//! Return true if instance was successfully constructed.
inline bool Zipped::isOk() const
{
    return (zip_ != 0);
}

END_NAMESPACE1

#include "syskit/win/link-with-syskit.h"
#endif
