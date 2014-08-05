/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/MappedTxtFile.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//!
//! Map given existing file. Skip byte-order-marker and reveal it in the bom() method
//! if skipBom is set. Don't look for leading byte-order-marker otherwise. The file is
//! mapped in read-only mode if readOnly is true. Otherwise, the mapped file is writable.
//! Constructor can fail if file is not accessible. Use isOk() to determine if the
//! construction is successful.
//!
MappedTxtFile::MappedTxtFile(const wchar_t* path, bool readOnly, bool skipBom):
MappedFile(path, readOnly, 0U /*mapSize*/),
bom_(Bom::None)
{
    skipBom_ = skipBom;
    construct();
}


//!
//! Create and map given file. The file is created to be size bytes long initially. Do not
//! overwrite existing file if failIfExists is true. Do overwrite existing file otherwise.
//! Mark file with given byte-order-mark. Use isOk() to determine if the construction is
//! successful.
//!
MappedTxtFile::MappedTxtFile(const wchar_t* path, unsigned long long size, bool failIfExists, const Bom& bom):
MappedFile(path, size + bom.byteSize(), failIfExists, 0U /*mapSize*/),
bom_(bom)
{
    skipBom_ = true;

    image_ = map(0);
    imageSize_ = MappedFile::size();
    unsigned int bomSize = bom_.byteSize();
    if (bomSize && isOk())
    {
        bom_.encode(image_);
        image_ += bomSize;
        imageSize_ -= bomSize;
    }
}


MappedTxtFile::~MappedTxtFile()
{
}


//!
//! Destroy existing contents. Load new contents from given path.
//! Return true if successful.
//!
bool MappedTxtFile::loadFrom(const wchar_t* path)
{
    bool ok = MappedFile::loadFrom(path);
    bom_ = Bom::None;
    construct();
    return ok;
}


//!
//! Reset and map given file. Given file is mapped in read-only mode if readOnly
//! is true. Otherwise, the mapped file is writable. The mapSize parameter is
//! unused.
//!
bool MappedTxtFile::remap(const wchar_t* path, bool readOnly, unsigned int /*mapSize*/)
{
    bool ok = MappedFile::remap(path, readOnly, 0U /*mapSize*/);
    bom_ = Bom::None;
    construct();
    return ok;
}


//!
//! Resize file. No-op if same size or if mapped file is read-only.
//! Return true if successful.
//!
bool MappedTxtFile::resize(unsigned long long size)
{
    bool ok = MappedFile::resize(size);
    bom_ = Bom::None;
    construct();
    return ok;
}


void MappedTxtFile::construct()
{
    image_ = map(0);
    imageSize_ = size();
    if (skipBom_ && isOk())
    {
        bom_ = Bom::decode(image_, static_cast<size_t>(imageSize_));
        unsigned int bomSize = bom_.byteSize();
        image_ += bomSize;
        imageSize_ -= bomSize;
    }
}

END_NAMESPACE1
