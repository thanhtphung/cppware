/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_DIRECTORY_HPP
#define APPKIT_DIRECTORY_HPP

#include "appkit/String.hpp"
#include "syskit/macros.h"

DECLARE_CLASS1(syskit, Utc)

BEGIN_NAMESPACE1(appkit)

class StringVec;


//! directory (aka folder)
class Directory
    //!
    //! A class representing a directory. Example:
    //!\code
    //! :
    //! String files;
    //! String subdirs;
    //! Directory dir("./");
    //! dir.list(files, subdirs);
    //! :
    //!\endcode
    //!
{

public:
    class Attr;

    typedef bool(*cb0_t)(void* arg, const Directory& parent, const String& childName);
    typedef void(*cb1_t)(void* arg, const Directory& parent, const String& childName);

    static const char DELIM;
    static const char MARKER;

    Directory(const Directory& directory);
    Directory(const String& path);
    Directory(const char* path);
    Directory(const char* path, size_t length);

    const Directory& operator =(const Directory& directory);

    bool getChildAttr(Attr& attr, const String& childName) const;
    bool hasChild(const String& childName) const;
    bool isOk() const;
    const String& find(String& children, const char* extension) const;
    const String& list(String& children) const;
    const String& path() const;
    const StringVec& find(StringVec& childVec, const char* extension) const;
    const StringVec& list(StringVec& childVec) const;
    void list(String& files, String& subdirs) const;
    void list(StringVec& fileVec, StringVec& subdirVec) const;

    bool apply(cb0_t cb, void* arg = 0) const;
    bool applyChildFirst(cb0_t cb, void* arg = 0) const;
    bool applyParentFirst(cb0_t cb, void* arg = 0) const;
    void apply(cb1_t cb, void* arg = 0) const;
    void applyChildFirst(cb1_t cb, void* arg = 0) const;
    void applyParentFirst(cb1_t cb, void* arg = 0) const;

    virtual ~Directory();

    static Directory current();
    static String getCurrent();
    static String getTemp();
    static bool nameIsDir(const String& name);
    static bool setCurrent(const String& path);
    static const String& normalizeSlashes(String& path);


    //! file/directory attributes
    class Attr
    {
    public:
        Attr();
        Attr(bool isDir, unsigned long long filetime[3], unsigned long long size);
        Attr(const Attr& attr);
        bool operator ==(const Attr& attr) const;
        const Attr& operator =(const Attr& attr);
        bool isDir() const;
        syskit::Utc accessTime() const;
        syskit::Utc creationTime() const;
        syskit::Utc writeTime() const;
        unsigned long long size() const;
        void reset();
        void reset(bool isDir, unsigned long long filetime[3], unsigned long long size);
    private:
        bool isDir_;
        unsigned long long accessTime_;
        unsigned long long creationTime_;
        unsigned long long size_;
        unsigned long long writeTime_;
    };

private:
    String path_;
    bool ok_;

    Directory();
    Directory(const String&, bool);
    void setPath(const String&);
    void validate();

    static String getTempPath();
    static bool formList0a(void*, const Directory&, const String&);
    static bool formList0b(void*, const Directory&, const String&);
    static bool formList0c(void*, const Directory&, const String&);
    static void formList1a(void*, const Directory&, const String&);
    static void formList1b(void*, const Directory&, const String&);
    static void formList1c(void*, const Directory&, const String&);
    static void normalizeSlashes(wchar_t*, size_t);

};

END_NAMESPACE1

#include "appkit/StringVec.hpp"
#include "syskit/Utc.hpp"

BEGIN_NAMESPACE1(appkit)

//! Return the current directory.
inline Directory Directory::current()
{
    Directory dir(getCurrent(), true);
    return dir;
}

//! Return true if instance was successfully constructed.
inline bool Directory::isOk() const
{
    return ok_;
}

//! Return true if given name specifies a directory.
inline bool Directory::nameIsDir(const String& name)
{
    const char* s = name.ascii();
    return ((*s != 0) && (s[name.byteSize() - 2] == MARKER));
}

//! Return directory path.
inline const String& Directory::path() const
{
    return path_;
}

//! List directory. Save results in children. Return children. Use DELIM to
//! delimit directory entries. Append MARKER to directory names to distinguish
//! subdirectory entries from file entries.
inline const String& Directory::list(String& children) const
{
    children.reset();
    apply(formList1a, &children);
    return children;
}

//! Normalize given path by replacing backslashes with slashes.
//! Return updated path.
inline const String& Directory::normalizeSlashes(String& path)
{
    const char BACKSLASH = '\\';
    const char SLASH = '/';
    path.replace(BACKSLASH, SLASH);
    return path;
}

//! List directory. Save results in childVec. Return childVec. Append MARKER
//! to directory names to distinguish subdirectory entries from file entries.
//! Results might be partial if childVec cannot grow.
inline const StringVec& Directory::list(StringVec& childVec) const
{
    childVec.reset();
    apply(formList0a, &childVec);
    return childVec;
}

inline void Directory::setPath(const String& path)
{
    path_ = path;
}

inline bool Directory::Attr::operator ==(const Attr& attr) const
{
    bool eq = (isDir_ == attr.isDir_) &&
        (size_ == attr.size_) &&
        (accessTime_ == attr.accessTime_) &&
        (creationTime_ == attr.creationTime_) &&
        (writeTime_ == attr.writeTime_);
    return eq;
}

//! Return true if these attributes are associated with a directory.
inline bool Directory::Attr::isDir() const
{
    return isDir_;
}

//! Return the last access (read or write) time.
//! Return zero if unavailable.
inline syskit::Utc Directory::Attr::accessTime() const
{
    return accessTime_;
}

//! Return the creation time.
//! Return zero if unavailable.
inline syskit::Utc Directory::Attr::creationTime() const
{
    return creationTime_;
}

//! Return the last write time.
//! Return zero if unavailable.
inline syskit::Utc Directory::Attr::writeTime() const
{
    return writeTime_;
}

//! Return the file size in bytes. Returned value is meaningless for a directory.
inline unsigned long long Directory::Attr::size() const
{
    return size_;
}

END_NAMESPACE1

#endif
