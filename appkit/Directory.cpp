/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/DelimitedTxt.hpp"
#include "appkit/Directory.hpp"
#include "appkit/Str.hpp"
#include "appkit/StringVec.hpp"

using namespace syskit;

BEGIN_NAMESPACE1(appkit)

typedef struct
{
    StringVec* childVec;
    const char* ext;
    size_t dotExtLength;
} findArg0_t;

typedef struct
{
    String* children;
    const char* ext;
    size_t dotExtLength;
} findArg1_t;

const char Directory::DELIM = '\n';
const char Directory::MARKER = '/';


Directory::Directory():
path_()
{
    ok_ = true;
}


//!
//! Construct a duplicate instance of the given directory.
//!
Directory::Directory(const Directory& directory):
path_(directory.path_)
{
    ok_ = directory.ok_;
}


//!
//! Construct instance with given directory name. Constructor can fail if
//! given name is not a directory or if given directory is inaccessible.
//! Use isOk() to determine construction status.
//!
Directory::Directory(const String& path):
path_(path)
{
    validate();
}


Directory::Directory(const String& path, bool ok):
path_(path)
{
    ok_ = ok;
}


//!
//! Construct instance with given directory name. Constructor can fail if
//! given name is not a directory or if given directory is inaccessible.
//! Use isOk() to determine construction status.
//!
Directory::Directory(const char* path):
path_(path)
{
    validate();
}


//!
//! Construct instance with given directory name (length characters starting at
//! path). Constructor can fail if given name is not a directory or if given
//! directory is inaccessible. Use isOk() to determine construction status.
//!
Directory::Directory(const char* path, size_t length):
path_(path, length)
{
    validate();
}


Directory::~Directory()
{
}


//!
//! Assignment operator.
//!
const Directory& Directory::operator =(const Directory& directory)
{

    // Prevent self assignment.
    if (this != &directory)
    {
        path_ = directory.path_;
        ok_ = directory.ok_;
    }

    // Return reference to self.
    return *this;
}


//!
//! Return the temporary directory.
//!
String Directory::getTemp()
{
    static String s_tempPath(getTempPath());
    return s_tempPath;
}


//!
//! Recursively apply callback to directory entries. Children first.
//! The callback should return true to continue iterating and should
//! return false to abort iterating. Return false if the callback
//! aborted the iterating. Return true otherwise.
//!
bool Directory::applyChildFirst(cb0_t cb, void* arg) const
{
    String files;
    String subdirs;
    list(files, subdirs);
    if (files.empty() && subdirs.empty())
    {
        return true;
    }

    // Children first. Return as soon as iterating is aborted.
    String subdir;
    Directory child;
    DelimitedTxt txt(subdirs, false, DELIM);
    for (String path(path_);
        txt.next(subdir) && (!txt.trimLine(subdir).empty());
        path.truncate(path_.length()))
    {
        child.setPath(path += subdir);
        if (!child.applyChildFirst(cb, arg))
        {
            return false;
        }
    }

    // Then subdirs. Return as soon as iterating is aborted.
    txt.reset();
    while (txt.next(subdir) && (!txt.trimLine(subdir).empty()))
    {
        if (!cb(arg, *this, subdir))
        {
            return false;
        }
    }

    // Then files. Return as soon as iterating is aborted.
    String file;
    txt.setTxt(files, false);
    while (txt.next(file) && (!txt.trimLine(file).empty()))
    {
        if (!cb(arg, *this, file))
        {
            return false;
        }
    }

    // Return true to indicate iterating was not aborted.
    return true;
}


//!
//! Recursively apply callback to directory entries. Parent first.
//! The callback should return true to continue iterating and should
//! return false to abort iterating. Return false if the callback
//! aborted the iterating. Return true otherwise.
//!
bool Directory::applyParentFirst(cb0_t cb, void* arg) const
{
    String files;
    String subdirs;
    list(files, subdirs);
    if (files.empty() && subdirs.empty())
    {
        return true;
    }

    // Files first. Return as soon as iterating is aborted.
    String file;
    DelimitedTxt txt(files, false, DELIM);
    while (txt.next(file) && (!txt.trimLine(file).empty()))
    {
        if (!cb(arg, *this, file))
        {
            return false;
        }
    }

    // Then subdirs. Return as soon as iterating is aborted.
    String subdir;
    txt.setTxt(subdirs, false);
    while (txt.next(subdir) && (!txt.trimLine(subdir).empty()))
    {
        if (!cb(arg, *this, subdir))
        {
            return false;
        }
    }

    // Then children. Return as soon as iterating is aborted.
    Directory child;
    txt.reset();
    for (String path(path_);
        txt.next(subdir) && (!txt.trimLine(subdir).empty());
        path.truncate(path_.length()))
    {
        child.setPath(path += subdir);
        if (!child.applyParentFirst(cb, arg))
        {
            return false;
        }
    }

    // Return true to indicate iterating was not aborted.
    return true;
}


//
// List children. Abort if cannot accumulate results.
//
bool Directory::formList0a(void* arg, const Directory& /*parent*/, const String& childName)
{
    StringVec& childVec = *static_cast<StringVec*>(arg);
    bool keepGoing = childVec.add(childName);
    return keepGoing;
}


//
// List child if extension matched.
//
bool Directory::formList0b(void* arg, const Directory& /*parent*/, const String& childName)
{
    bool keepGoing = true;
    const findArg0_t& findArg = *static_cast<const findArg0_t*>(arg);
    if (childName.byteSize() > findArg.dotExtLength)
    {
        const char DOT = '.';
        const char* s = childName.ascii() + childName.byteSize() - findArg.dotExtLength;
        if ((Str::compareKI(s, findArg.ext) == 0) && (s[-1] == DOT))
        {
            if (!findArg.childVec->add(childName))
            {
                keepGoing = false;
            }
        }
    }

    return keepGoing;
}


//
// List children. Separate files and directories. Abort if cannot accumulate results.
//
bool Directory::formList0c(void* arg, const Directory& /*parent*/, const String& childName)
{
    unsigned int i = nameIsDir(childName)? 1: 0;
    StringVec** pp = static_cast<StringVec**>(arg);
    StringVec* p = pp[i];
    bool keepGoing = p->add(childName);
    if (!keepGoing)
    {
        p = pp[1 - i];
        if (p->canGrow() || (p->numItems() < p->capacity()))
        {
            keepGoing = true;
        }
    }

    return keepGoing;
}


//!
//! Find files with given extension. Save results in children. Return
//! children. Use DELIM to delimit directory entries.
//!
const String& Directory::find(String& children, const char* extension) const
{
    children.reset();
    findArg1_t arg = {&children, extension, strlen(extension) + 1};
    apply(formList1b, &arg);
    return children;
}


//!
//! Find files with given extension. Save results in childVec. Return
//! childVec. Results might be partial if childVec cannot grow.
//!
const StringVec& Directory::find(StringVec& childVec, const char* extension) const
{
    childVec.reset();
    findArg0_t arg = {&childVec, extension, strlen(extension) + 1};
    apply(formList0b, &arg);
    return childVec;
}


//!
//! Recursively apply callback to directory entries. Children first.
//!
void Directory::applyChildFirst(cb1_t cb, void* arg) const
{
    String files;
    String subdirs;
    list(files, subdirs);
    if (files.empty() && subdirs.empty())
    {
        return;
    }

    // Children first.
    String subdir;
    Directory child;
    DelimitedTxt txt(subdirs, false, DELIM);
    for (String path(path_);
        txt.next(subdir) && (!txt.trimLine(subdir).empty());
        path.truncate(path_.length()))
    {
        child.setPath(path += subdir);
        child.applyChildFirst(cb, arg);
    }

    // Then subdirs.
    txt.reset();
    for (; txt.next(subdir) && (!txt.trimLine(subdir).empty()); cb(arg, *this, subdir));

    // Then files.
    String file;
    txt.setTxt(files, false);
    for (; txt.next(file) && (!txt.trimLine(file).empty()); cb(arg, *this, file));
}


//!
//! Recursively apply callback to directory entries. Parent first.
//!
void Directory::applyParentFirst(cb1_t cb, void* arg) const
{
    String files;
    String subdirs;
    list(files, subdirs);
    if (files.empty() && subdirs.empty())
    {
        return;
    }

    // Files first.
    String file;
    DelimitedTxt txt(files, false, DELIM);
    for (; txt.next(file) && (!txt.trimLine(file).empty()); cb(arg, *this, file));

    // Then subdirs.
    String subdir;
    txt.setTxt(subdirs, false);
    for (; txt.next(subdir) && (!txt.trimLine(subdir).empty()); cb(arg, *this, subdir));

    // Then children.
    Directory child;
    txt.reset();
    for (String path(path_);
        txt.next(subdir) && (!txt.trimLine(subdir).empty());
        path.truncate(path_.length()))
    {
        child.setPath(path += subdir);
        child.applyParentFirst(cb, arg);
    }
}


void Directory::formList1a(void* arg, const Directory& /*parent*/, const String& childName)
{
    String& children = *static_cast<String*>(arg);
    children += childName;
    children += DELIM;
}


//
// List child if extension matched.
//
void Directory::formList1b(void* arg, const Directory& /*parent*/, const String& childName)
{
    const findArg1_t& findArg = *static_cast<const findArg1_t*>(arg);
    if (childName.byteSize() > findArg.dotExtLength)
    {
        const char DOT = '.';
        const char* s = childName.ascii() + childName.byteSize() - findArg.dotExtLength;
        if ((Str::compareKI(s, findArg.ext) == 0) && (s[-1] == DOT))
        {
            *findArg.children += childName;
            *findArg.children += DELIM;
        }
    }
}


void Directory::formList1c(void* arg, const Directory& /*parent*/, const String& childName)
{
    unsigned int i = nameIsDir(childName)? 1: 0;
    String** pp = static_cast<String**>(arg);
    String* p = pp[i];
    *p += childName;
    *p += DELIM;
}


//!
//! List directory. Categorize and save results in two given strings:
//! files and subdirs. Use DELIM to delimit directory entries.
//!
void Directory::list(String& files, String& subdirs) const
{
    files.reset();
    subdirs.reset();
    String* arg[2];
    arg[0] = &files;
    arg[1] = &subdirs;
    apply(formList1c, arg);
}


//!
//! List directory. Categorize and save results in two given vectors:
//! fileVec and subdirVec. Results might be partial if the vector(s)
//! cannot grow.
//!
void Directory::list(StringVec& fileVec, StringVec& subdirVec) const
{
    fileVec.reset();
    subdirVec.reset();
    StringVec* arg[2];
    arg[0] = &fileVec;
    arg[1] = &subdirVec;
    apply(formList0c, arg);
}


//!
//! Construct instance with zeroes.
//!
Directory::Attr::Attr()
{
    accessTime_ = Utc::ZERO;
    creationTime_ = Utc::ZERO;
    writeTime_ = Utc::ZERO;
    isDir_ = false;
    size_ = 0;
}


//!
//! Construct instance from given attributes. Use isDir to indicate if these attributes
//! are associated with a directory. The given time array specifies the access, creation,
//! and write times in that order (these windows filetimes are in 100-nsecs since 1/1/1601). The
//! size value specifies the file size in bytes and is meaningless for a directory.
//!
Directory::Attr::Attr(bool isDir, unsigned long long filetime[3], unsigned long long size)
{
    accessTime_ = filetime[0];
    creationTime_ = filetime[1];
    writeTime_ = filetime[2];
    isDir_ = isDir;
    size_ = size;
}


Directory::Attr::Attr(const Attr& attr)
{
    accessTime_ = attr.accessTime_;
    creationTime_ = attr.creationTime_;
    writeTime_ = attr.writeTime_;
    isDir_ = attr.isDir_;
    size_ = attr.size_;
}


const Directory::Attr& Directory::Attr::operator =(const Attr& attr)
{
    if (this != &attr)
    {
        accessTime_ = attr.accessTime_;
        creationTime_ = attr.creationTime_;
        writeTime_ = attr.writeTime_;
        isDir_ = attr.isDir_;
        size_ = attr.size_;
    }

    return *this;
}


//!
//! Reset with zeroes.
//!
void Directory::Attr::reset()
{
    accessTime_ = Utc::ZERO;
    creationTime_ = Utc::ZERO;
    writeTime_ = Utc::ZERO;
    isDir_ = false;
    size_ = 0;
}


//!
//! Reset attributes. Use isDir to indicate if these attributes are associated with
//! a directory. The given time array specifies the access, creation, and write times
//! in that order (these windows filetimes are in 100-nsecs since 1/1/1601).
//!
void Directory::Attr::reset(bool isDir, unsigned long long filetime[3], unsigned long long size)
{
    accessTime_ = filetime[0];
    creationTime_ = filetime[1];
    writeTime_ = filetime[2];
    isDir_ = isDir;
    size_ = size;
}

END_NAMESPACE1
