/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_TEMP_FILE_HPP
#define APPKIT_TEMP_FILE_HPP

#include "appkit/String.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)

class TempDir;


//! temporary file
class TempFile
    //!
    //! A class representing a temporary file. Temporary files are
    //! automatically deleted once the associated objects go out of
    //! scope.
    //!
{

public:
    TempFile(const TempDir& tempDir, const String& basename);
    ~TempFile();

    const String& path() const;

private:
    String path_;

    TempFile(const TempFile&); //prohibit usage
    const TempFile& operator =(const TempFile&); //prohibit usage

};

//! Return the temporary file's path.
inline const String& TempFile::path() const
{
    return path_;
}

END_NAMESPACE1

#endif
