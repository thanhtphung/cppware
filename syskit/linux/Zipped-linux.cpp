/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <zlib/contrib/minizip/zip.h>

#include "syskit/MappedFile.hpp"
#include "syskit/Zipped.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//!
//! Add files in directory to the zip. Given file resides in the directory to be added.
//! A compression level can be specified to dictate how much the files will be compressed.
//! Use -1 for default compression, 0 for no compression, and 1 through 9 for minimal to
//! maximal compression. Behavior is unpredictable for other compression levels. Return
//! true if successful.
//!
bool Zipped::addDirectory(const MappedFile& /*sibling*/, int /*compressionLevel*/)
{
    // TODO
    bool ok = false;
    return ok;
}


//!
//! Add files in given directory to the zip. A compression level can be specified to
//! dictate how much the files will be compressed. Use -1 for default compression, 0
//! for no compression, and 1 through 9 for minimal to maximal compression. Behavior
//! is unpredictable for other compression levels. Return true if successful.
//!
bool Zipped::addDirectory(const wchar_t* /*inDir*/, int /*compressionLevel*/)
{
    // TODO
    bool ok = false;
    return ok;
}


bool Zipped::zipItem(const MappedFile& /*item*/, const wchar_t* /*basename*/, int /*compressionLevel*/, unsigned long long /*itemFiletime*/)
{
    // TODO
    bool ok = false;
    return ok;
}

END_NAMESPACE1
