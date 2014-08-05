/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_MAPPED_FILE_HPP
#define SYSKIT_MAPPED_FILE_HPP

#include <sys/types.h>
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(syskit)


//! memory-mapped file
class MappedFile
    //!
    //! A class representing a memory-mapped file with fragmentation tolerance. A large
    //! file being mapped to one view can fail due to fragmentation. Fragmentation tolerance
    //! is supported by allowing a file to be mapped to multiple views. The view size can be
    //! specified at construction. A view size of zero is allowed to indicate no limit (i.e.,
    //! use one and only one view regardless of file size). Example:
    //!\code
    //! MappedFile file("some-file");
    //! const unsigned char* p = file.map();
    //! const unsigned char* pEnd = p + file.size();
    //! for (; p<pEnd; ++p)
    //! {
    //!   //do something with each byte from the file
    //! }
    //!\endcode
    //!
{

public:
    MappedFile(const wchar_t* path, bool readOnly = true, unsigned int mapSize = 0);
    MappedFile(const wchar_t* path, unsigned long long size, bool failIfExists, unsigned int mapSize = 0);

    bool grow(unsigned long long size);
    bool isReadOnly() const;
    bool saveIn(const wchar_t* path) const;
    bool truncate(unsigned long long size);
    const unsigned char* map(size_t index = 0) const;
    const wchar_t* path() const;
    fd_t handle() const;
    unsigned char* map(size_t index = 0);
    unsigned int mapSize() const;
    unsigned int numMaps() const;
    unsigned long long offset(size_t index = 0) const;
    unsigned long long size() const;

    const unsigned char* addrOf(unsigned long long offset) const;
    unsigned char* addrOf(unsigned long long offset);
    void copyBytes(unsigned long long dst, unsigned long long src, size_t byteCount);
    void getBytes(void* dst, unsigned long long offset, size_t byteCount) const;
    void setBytes(unsigned long long offset, size_t byteCount, const void* src, bool allowOverlaps = false);

    virtual ~MappedFile();
    virtual bool isOk() const;
    virtual bool loadFrom(const wchar_t* path);
    virtual bool remap(const wchar_t* path, bool readOnly, unsigned int mapSize = 0);
    virtual bool resize(unsigned long long size);

private:
    HANDLE mapHandle_;
    bool ok_;
    bool readOnly_;
    fd_t fileHandle_;
    unsigned int mapSize_;
    unsigned int mapSize2_; //if non-zero, then (2**mapSize2_) equals mapSize_
    unsigned int numMaps_;
    unsigned long long size_;
    wchar_t* path_;

    unsigned char** map_;
    unsigned long long* offset_;

    static unsigned char emptyMark_;

    MappedFile(const MappedFile&); //prohibit usage
    const MappedFile& operator =(const MappedFile&); //prohibit usage

    bool createFile(const wchar_t*, bool, unsigned long long);
    bool openFile(const wchar_t*, unsigned long long&);
    void closeFile();
    void construct(const wchar_t*, bool, unsigned long long, unsigned int);
    void destruct();
    void map1View();
    void mapNViews();
    void unmap();

    static unsigned int adjustMapSize(unsigned int);

};

//! Grow file if it has less than size bytes.
//! Return true if growth successfully occurred.
inline bool MappedFile::grow(unsigned long long size)
{
    bool grown = (size_ >= size)? false: resize(size);
    return grown;
}

//! Return true if file is read-only.
inline bool MappedFile::isReadOnly() const
{
    return readOnly_;
}

//! Truncate file if it has more than size bytes.
//! Return true if truncation successfully occurred.
inline bool MappedFile::truncate(unsigned long long size)
{
    bool truncated = (size_ <= size)? false: resize(size);
    return truncated;
}

//! Return the address of given file byte offset.
inline const unsigned char* MappedFile::addrOf(unsigned long long offset) const
{
    unsigned char* p = const_cast<MappedFile*>(this)->addrOf(offset);
    return p;
}

//! Return the address of the given view. First view has index zero.
inline const unsigned char* MappedFile::map(size_t index) const
{
    return (numMaps_ == 0)? &emptyMark_: map_[index];
}

inline const wchar_t* MappedFile::path() const
{
    return path_;
}

inline fd_t MappedFile::handle() const
{
    return fileHandle_;
}

//! Return the address of the given view. First view has index zero.
//! Behavior is unpredictable if updates are attempted against a read-only file.
inline unsigned char* MappedFile::map(size_t index)
{
    return (numMaps_ == 0)? &emptyMark_: map_[index];
}

//! Return the memory map size (aka view size) in bytes. A zero indicates
//! no limit (i.e., use one and only one view regardless of file size).
//! With a non-zero limit, a file can be mapped into multiple views. Each
//! view except the last covers a contiguous area of this size. The last
//! view covers a contiguous area up to this size.
inline unsigned int MappedFile::mapSize() const
{
    return mapSize_;
}

//! Return the number of views the file is mapped to.
inline unsigned int MappedFile::numMaps() const
{
    return numMaps_;
}

//! Return the offset of the given view. First view has index zero.
//! Return zero if file is empty.
inline unsigned long long MappedFile::offset(size_t index) const
{
    return (numMaps_ == 0)? 0: offset_[index];
}

//! Return the file size in bytes.
inline unsigned long long MappedFile::size() const
{
    return size_;
}

END_NAMESPACE1

#include "syskit/win/link-with-syskit.h"
#endif
