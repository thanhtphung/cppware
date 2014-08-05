/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_TEMP_DIR_H
#define APPKIT_TEMP_DIR_H

#include "appkit/String.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)

class Directory;


//! temporary directory
class TempDir
    //!
    //! A class representing a temporary directory. A temporary directory is
    //! created at instance construction, and the directory is deleted during
    //! instance destruction.
    //!
{

public:
    TempDir(bool deleteAllOnDestruction = false);
    ~TempDir();

    const String& path() const;

private:
    String path_;
    bool deleteAll_;

    TempDir(const TempDir&); //prohibit usage
    const TempDir& operator =(const TempDir&); //prohibit usage

    static String mkMyDir();
    static void deleteAll(void*, const Directory&, const String&);

};

//! Return the temporary directory's path.
inline const String& TempDir::path() const
{
    return path_;
}

END_NAMESPACE1

#endif
