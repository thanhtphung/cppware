/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/DelimitedTxt.hpp"
#include "appkit/Directory.hpp"
#include "appkit/Path.hpp"
#include "appkit/S32.hpp"
#include "appkit/StringVec.hpp"

const char COLON = ':';
const char DOT = '.';

BEGIN_NAMESPACE1(appkit)


//!
//! Construct a duplicate instance of the given path.
//!
Path::Path(const Path& path):
path_(path.path_)
{
    hasDrive_ = path.hasDrive_;
    isDir_ = path.isDir_;
}


//!
//! Construct instance with given path. Assume given path is already
//! normalized if skipNormalization is true. Otherwise, do normalize
//! the given path.
//!
Path::Path(const String& path, bool skipNormalization):
path_(path)
{
    construct(skipNormalization);
}


//!
//! Construct instance with given path. Assume given path is already
//! normalized if skipNormalization is true. Otherwise, do normalize
//! the given path.
//!
Path::Path(const char* path, bool skipNormalization):
path_(path)
{
    construct(skipNormalization);
}


//!
//! Construct instance with given path (length characters starting at path).
//! Assume given path is already normalized if skipNormalization is true.
//! Otherwise, do normalize the given path.
//!
Path::Path(const char* path, size_t length, bool skipNormalization):
path_(path, length)
{
    construct(skipNormalization);
}


Path::~Path()
{
}


const Path& Path::operator =(const Path& path)
{
    if (this != &path)
    {
        path_ = path.path_;
        hasDrive_ = path.hasDrive_;
        isDir_ = path.isDir_;
    }

    return *this;
}


//!
//! Return the path's basename without the extension. For example,
//! Path("c:/abc/xyz.123").base() would return "xyz". Use basename()
//! if the extension is required.
//!
String Path::base() const
{
    String base;
    DelimitedTxt txt(path_, false, Directory::MARKER);
    txt.prev(base);
    size_t length = base.rfind(DOT);
    if (length != String::INVALID_INDEX)
    {
        base.truncate(length);
    }

    return base;
}


//!
//! Return the path's basename. For example, Path("c:\\abc\\xyz.123").basename()
//! would return "xyz.123". The returned basename is the path itself if no
//! directory delimiters are seen. For example, Path("basename").basename()
//! would return "basename". Use base() if the extension is not required.
//!
String Path::basename() const
{
    String base;
    DelimitedTxt txt(path_, false, Directory::MARKER);
    txt.prev(base);
    return base;
}


//!
//! Return the path's directory name. For example, Path("c:\\abc\\xyz.123").dirname()
//! would return "c:/abc/". The returned name is an empty string if no directory
//! delimiters are seen. For example, Path("filename").dirname() would be empty.
//!
String Path::dirname() const
{
    String base;
    DelimitedTxt txt(path_, false, Directory::MARKER);
    txt.prev(base);
    String dir(path_);
    dir.truncate(path_.length() - base.length());
    return dir;
}


//!
//! Return the path's extension. For example, Path("c:\\abc\\xyz.123").extension()
//! would return "123". The returned extension is an empty string if no dots are
//! seen in the path's basename. For example, Path("./kid/grandkid").extension()
//! would return an empty string.
//!
String Path::extension() const
{
    String ext;
    DelimitedTxt txt(path_, false, DOT);
    txt.prev(ext);
    if (ext.find(Directory::MARKER) != String::INVALID_INDEX)
    {
        ext.reset();
    }

    return ext;
}


//!
//! Return the full name. If current path is relative, the full name is formed using
//! the absolute path of the current directory as its prefix (e.g., "xyz.123" -->
//! "d:/current-directory/xyz.123"). If beautify is true and current path contains
//! redundant parts, the full name is formed w/o the redundant parts (e.g., ".././x/y/../../" -->
//! "../".
//!
String Path::fullName(bool beautify) const
{
    String fullName;
    if (isAbsolute())
    {
        fullName = path_;
    }
    else
    {
        fullName = Directory::getCurrent();
        fullName += path_;
    }

    if (beautify)
    {
        shrink(fullName);
    }

    return fullName;
}


//!
//! Return the path's server. For example, Path("//server/xyz.123").server() would
//! return "server". The returned server is an empty string if this is not a UNC
//! (universal naming convention) path. For example, Path("c:/").server() would
//! return an empty string.
//!
String Path::server() const
{
    String name;
    if (path_.length() >= 4) //"//x/"
    {
        const char* s = path_.ascii();
        if ((s[0] == Directory::MARKER) && (s[1] == Directory::MARKER))
        {
            DelimitedTxt txt(path_, false, Directory::MARKER);
            const char* line;
            size_t length;
            txt.next(line, length);
            txt.next(line, length);
            txt.next(name);
            length = name.length();
            if (txt.trimLine(name).length() == length)
            {
                name.reset();
            }
        }
    }

    return name;
}


//!
//! Return true if this is an absolute path. Return false otherwise.
//!
bool Path::isAbsolute() const
{
    bool answer;
    if (hasDrive_)
    {
        answer = true;
    }
    else if (path_.length() >= 4)
    {
        const char* s = path_.ascii();
        answer = ((s[0] == Directory::MARKER) && (s[1] == Directory::MARKER));
    }
    else
    {
        answer = false;
    }

    return answer;
}


const String& Path::shrink(String& fullName)
{

    // Split full name into subdirectories.
    // Optimize out the "." and ".." parts.
    String line;
    StringVec parts;
    DelimitedTxt txt(fullName, false, Directory::MARKER);
    while (txt.next(line))
    {
        if ((line.byteSize() == 2) || ((line.byteSize() == 3) && (*line.ascii() == DOT))) //"/" or "./"
        {
            continue;
        }
        if (line == "../")
        {
            if (parts.rmTail())
            {
                continue;
            }
        }
        parts.add(line);
    }

    // Check for UNC path (//server/xxx).
    const char* s = fullName.ascii();
    bool isUnc = (s[0] == Directory::MARKER) && (s[1] == Directory::MARKER) && (s[2] != Directory::MARKER);
    isUnc? fullName.reset(2, Directory::MARKER): fullName.reset();

    // Merge subdirectories back into a full name.
    for (size_t i = 0, numParts = parts.numItems(); i < numParts; ++i)
    {
        fullName += parts.peek(i);
    }

    return fullName;
}


void Path::construct(bool skipNormalization)
{
    if (!skipNormalization)
    {
        Directory::normalizeSlashes(path_);
        bool ignoreCase = true;
        const char* prefix = "file:///";
        if (path_.startsWith(prefix, ignoreCase))
        {
            size_t charCount = static_cast<size_t>(0) - 1;
            size_t startAt = 8;
            path_ = path_.substr(startAt, charCount); //file:///xxx --> xxx
        }
    }

    isDir_ = Directory::nameIsDir(path_);
    hasDrive_ = (path_.length() > 1) && (path_.ascii()[1] == COLON) && S32::isAlpha(*path_.ascii());
}

END_NAMESPACE1
