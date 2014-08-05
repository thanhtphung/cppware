/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_MAPPED_TXT_FILE_HPP
#define SYSKIT_MAPPED_TXT_FILE_HPP

#include "syskit/Bom.hpp"
#include "syskit/MappedFile.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! memory-mapped text file
class MappedTxtFile: public MappedFile
    //!
    //! A class representing a memory-mapped text file. For comparison, a MappedTxtFile can
    //! be mapped to one and only one view while a MappedFile can be mapped to multiple views.
    //! Byte-order-marker, if any, can be somewhat transparent with MappedTxtFile using the
    //! skipBom parameter and/or the bom() accessor. Example:
    //!\code
    //! MappedTxtFile file("some-txt-file");
    //! const unsigned char* p = file.image();
    //! const unsigned char* pEnd = p + file.size();
    //! for (; p<pEnd; ++p)
    //! {
    //!   //do something with each byte from the file
    //! }
    //!\endcode
    //!
{

public:
    MappedTxtFile(const wchar_t* path, bool readOnly = true, bool skipBom = true);
    MappedTxtFile(const wchar_t* path, unsigned long long size, bool failIfExists, const Bom& bom);
    bool operator !=(const MappedTxtFile& file) const;
    bool operator ==(const MappedTxtFile& file) const;

    const Bom& bom() const;
    const unsigned char* image() const;
    const unsigned char* image(unsigned long long& imageSize) const;
    unsigned char* image();
    unsigned char* image(unsigned long long& imageSize);
    unsigned long long imageSize() const;

    virtual ~MappedTxtFile();
    virtual bool loadFrom(const wchar_t* path);
    virtual bool remap(const wchar_t* path, bool readOnly, unsigned int mapSize = 0);
    virtual bool resize(unsigned long long size);

private:
    Bom bom_;
    bool skipBom_;
    unsigned char* image_;
    unsigned long long imageSize_;

    MappedTxtFile(const MappedTxtFile&); //prohibit usage
    const MappedTxtFile& operator =(const MappedTxtFile&); //prohibit usage

    void construct();

};

//! Return true if the two files are not identical, content-wise.
inline bool MappedTxtFile::operator !=(const MappedTxtFile& file) const
{
    return (bom_ != file.bom_) || (memcmp(image_, file.image_, static_cast<size_t>(imageSize_)) != 0);
}

//! Return true if the two files are identical, content-wise.
inline bool MappedTxtFile::operator ==(const MappedTxtFile& file) const
{
    return (bom_ == file.bom_) && (memcmp(image_, file.image_, static_cast<size_t>(imageSize_)) == 0);
}

//! Return the leading byte-order-marker. If the memory-mapped file has a
//! byte-order-marker, but it was not being looked for during the mapping
//! operation (i.e., skipBom=false), this method will return Bom::None.
inline const Bom& MappedTxtFile::bom() const
{
    return bom_;
}

//! Return the raw file contents. This does not include the byte-order-marker if
//! skipBom was set at construction. Use imageSize() to obtain the raw file size in
//! bytes if necessary. Return zero if instance was not successfully constructed.
inline const unsigned char* MappedTxtFile::image() const
{
    return image_;
}

//! Return the raw file contents. Also return the raw file size in bytes in imageSize.
//! The returned contents and size do not include the byte-order-marker if skipBom was
//! set at construction. Return zeroes if instance was not successfully constructed.
inline const unsigned char* MappedTxtFile::image(unsigned long long& imageSize) const
{
    imageSize = imageSize_;
    return image_;
}

//! Return the raw file contents. This does not include the byte-order-marker if
//! skipBom was set at construction. Use imageSize() to obtain the raw file size in
//! bytes if necessary. Return zero if instance was not successfully constructed.
//! Behavior is unpredictable if updates are attempted against a read-only file.
inline unsigned char* MappedTxtFile::image()
{
    return image_;
}

//! Return the raw file contents. Also return the raw file size in bytes in imageSize.
//! The returned contents and size do not include the byte-order-marker if skipBom was
//! set at construction. Return zeroes if instance was not successfully constructed.
//! Behavior is unpredictable if updates are attempted against a read-only file.
inline unsigned char* MappedTxtFile::image(unsigned long long& imageSize)
{
    imageSize = imageSize_;
    return image_;
}

//! Return the raw file size in bytes. The size does not include the byte-order-marker
//! if skipBom was set at construction. The size is zero if instance was not successfully
//! constructed.
inline unsigned long long MappedTxtFile::imageSize() const
{
    return imageSize_;
}

END_NAMESPACE1

#endif
