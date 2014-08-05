/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_PATH_HPP
#define APPKIT_PATH_HPP

#include "appkit/String.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)


//! normalized file path
class Path
    //!
    //! A class representing a normalized fully-specified path. Example:
    //!\code
    //! Path path("c:\\dd\\eee.fff");
    //! assert(path.asString() == "c:/dd/eee.fff");
    //! assert(path.base() == "eee");
    //! assert(path.basename() == "eee.fff");
    //! assert(path.dirname() == "c:/dd/");
    //! assert(path.drive() == 'c');
    //! assert(path.extension() == "fff");
    //! assert(! path.isDir());
    //! Path x("//server/xyz.123");
    //! assert(x.base() == "xyz");
    //! assert(x.basename() == "xyz.123");
    //! assert(x.server() == "server");
    //!\endcode
    //!
{

public:
    Path(const Path& path);
    Path(const String& path, bool skipNormalization = false);
    Path(const char* path, bool skipNormalization = false);
    Path(const char* path, size_t length, bool skipNormalization = false);
    ~Path();

    bool operator !=(const Path& path) const;
    bool operator ==(const Path& path) const;
    const Path& operator =(const Path& Path);

    String base() const;
    String basename() const;
    String dirname() const;
    String extension() const;
    String fullName(bool beautify = true) const;
    String server() const;
    bool isAbsolute() const;
    bool isDir() const;
    bool isRelative() const;
    char drive() const;
    const String& asString() const;

    void reset(const String& path, bool skipNormalization = false);
    void reset(const char* path, bool skipNormalization = false);
    void reset(const char* path, size_t length, bool skipNormalization = false);

private:
    String path_;
    bool hasDrive_;
    bool isDir_;

    void construct(bool);

    static const String& shrink(String&);

};

//! Return true if this path does not equal given path.
inline bool Path::operator !=(const Path& path) const
{
    return (path_ != path.path_);
}

//! Return true if this path equals given path.
inline bool Path::operator ==(const Path& path) const
{
    return (path_ == path.path_);
}

//! Return true if this is a directory name.
inline bool Path::isDir() const
{
    return isDir_;
}

//! Return true if this is a relative path. Return false otherwise.
inline bool Path::isRelative() const
{
    return (!isAbsolute());
}

//! Return the drive letter.
//! Return zero if none.
inline char Path::drive() const
{
    return hasDrive_? *path_.ascii(): 0;
}

//! Return the normalized path. For example, the normalized path of
//! "c:\\abc\\xyz.123" is "c:/abc/xyz.123".
inline const String& Path::asString() const
{
    return path_;
}

//! Reset instance with given path. Assume given path is already
//! normalized if skipNormalization is true. Otherwise, do normalize
//! the given path.
inline void Path::reset(const String& path, bool skipNormalization)
{
    path_ = path;
    construct(skipNormalization);
}

//! Reset instance with given path. Assume given path is already
//! normalized if skipNormalization is true. Otherwise, do normalize
//! the given path.
inline void Path::reset(const char* path, bool skipNormalization)
{
    path_ = path;
    construct(skipNormalization);
}

//! Reset instance with given path (length characters starting at path).
//! Assume given path is already normalized if skipNormalization is true.
//! Otherwise, do normalize the given path.
inline void Path::reset(const char* path, size_t length, bool skipNormalization)
{
    path_.reset(path, length);
    construct(skipNormalization);
}

END_NAMESPACE1

#endif
