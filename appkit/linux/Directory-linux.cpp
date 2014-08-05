/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstdlib>
#include "appkit/Directory.hpp"
#include "syskit/sys.hpp"

using namespace syskit;

const unsigned long BUF_SIZE = 8192;

inline bool isDotOrDotDot(const char* name)
{
    if (name[0] == '.')
    {
        if ((name[1] == 0) || ((name[1] == '.') && (name[2] == 0)))
        {
            return true;
        }
    }

    return false;
}

#if __CYGWIN__
struct dirent64
{
    unsigned char d_type;
    unsigned short d_reclen;
    char d_name[255 + 1];
};

int getdents64(unsigned int fd, struct dirent64* /*dirp*/, unsigned int /*count*/)
{
    DIR* dir = fdopendir(fd);
    closedir(dir);
    return 0;
}

#else
#include <linux/unistd.h>
inline int getdents64(unsigned int fd, struct dirent64* dirp, unsigned int count)
{
    return syscall(__NR_getdents64, fd, dirp, count);
}

#endif


static int openDir(const appkit::String& path)
{
    int fd = open(path.ascii(), O_RDONLY);
    if (fd >= 0)
    {
        int flags = fcntl(fd, F_GETFD);
        if ((flags >= 0) && ((flags | FD_CLOEXEC) != 0))
        {
            flags |= FD_CLOEXEC;
            fcntl(fd, F_SETFD, flags);
        }
    }

    return fd;
}


static long typeOf(const appkit::String& parentPath, const char* childName)
{
    unsigned char type = DT_UNKNOWN;
    appkit::String childPath(parentPath);
    childPath += childName;
    struct stat childStat;
    if (stat(childPath.ascii(), &childStat) == 0)
    {
        if (childStat.st_mode & S_IFDIR)
        {
            type = DT_DIR;
        }
        else if (childStat.st_mode & S_IFREG)
        {
            type = DT_REG;
        }
    }

    return type;
}


BEGIN_NAMESPACE1(appkit)


//!
//! Return the current directory.
//!
String Directory::getCurrent()
{
    String path;
    char path8[MAX_PATH];
    path8[0] = 0;
    getcwd(path8, sizeof(path8));
    path.reset8(reinterpret_cast<const utf8_t*>(path8), strlen(path8));
    if (!nameIsDir(path))
    {
        path += MARKER;
    }

    return path;
}


String Directory::getTempPath()
{
    char path[] = "appkit-XXXXXX";
    mkdtemp(path);
    return path;
}


//!
//! Apply callback to directory entries. The callback should return true to
//! continue iterating and should return false to abort iterating. Return
//! false if the callback aborted the iterating. Return true otherwise.
//!
bool Directory::apply(cb0_t cb, void* arg) const
{

    // Assume normal iterating.
    bool ok = true;
    int fd = openDir(path_);
    if (fd < 0)
    {
        return ok;
    }

    String child;
    for (unsigned char* buf = new unsigned char[BUF_SIZE];;)
    {

        // Obtain directory entries.
        // One buffer at a time.
        const unsigned char* pEnd = buf + getdents64(fd, reinterpret_cast<struct dirent64*>(buf), BUF_SIZE);
        if (pEnd <= buf)
        {
            delete[] buf;
            break;
        }

        // Perform callback on each directory entry.
        const struct dirent64* c;
        for (const unsigned char* p = buf; p < pEnd; p += c->d_reclen)
        {
            c = reinterpret_cast<const struct dirent64*>(p);
            long type = ((c->d_type == DT_UNKNOWN) || (c->d_type == DT_LNK))? typeOf(path_, c->d_name): c->d_type;
            if (type == DT_DIR)
            {
                if (isDotOrDotDot(c->d_name)) continue;
                child.reset8(reinterpret_cast<const utf8_t*>(c->d_name), strlen(c->d_name));
                child += MARKER;
            }
            else
            {
                child.reset8(reinterpret_cast<const utf8_t*>(c->d_name), strlen(c->d_name));
            }
            if (!cb(arg, *this, child))
            {
                ok = false;
                break;
            }
        }
    }

    // Return true if the iterating was not aborted.
    close(fd);
    return ok;
}


//!
//! Return true if directory contains given child.
//!
bool Directory::hasChild(const String& childName) const
{
    if (!childName.empty())
    {
        String childPath(path_);
        childPath += childName;
        struct stat childStat;
        bool ok = (stat(childPath.ascii(), &childStat) == 0);
        if (ok)
        {
            bool isDir = ((childStat.st_mode & S_IFDIR) != 0);
            return (isDir == nameIsDir(childName));
        }
    }

    return false;
}


//!
//! Set current directory.
//! Return true if successful.
//!
bool Directory::setCurrent(const String& path)
{
    bool ok = (chdir(path.ascii()) == 0);
    return ok;
}


//!
//! Apply callback to directory entries.
//!
void Directory::apply(cb1_t cb, void* arg) const
{
    int fd = openDir(path_);
    if (fd < 0)
    {
        return;
    }

    String child;
    for (unsigned char* buf = new unsigned char[BUF_SIZE];;)
    {

        // Obtain directory entries.
        // One buffer at a time.
        const unsigned char* pEnd = buf + getdents64(fd, reinterpret_cast<struct dirent64*>(buf), BUF_SIZE);
        if (pEnd <= buf)
        {
            delete[] buf;
            break;
        }

        // Perform callback on each directory entry.
        const struct dirent64* c;
        for (const unsigned char* p = buf; p < pEnd; p += c->d_reclen)
        {
            c = reinterpret_cast<const struct dirent64*>(p);
            long type = ((c->d_type == DT_UNKNOWN) || (c->d_type == DT_LNK))? typeOf(path_, c->d_name): c->d_type;
            if (type == DT_DIR)
            {
                if (isDotOrDotDot(c->d_name)) continue;
                child.reset8(reinterpret_cast<const utf8_t*>(c->d_name), strlen(c->d_name));
                child += MARKER;
            }
            else
            {
                child.reset8(reinterpret_cast<const utf8_t*>(c->d_name), strlen(c->d_name));
            }
            cb(arg, *this, child);
        }
    }

    close(fd);
}


void Directory::validate()
{
    struct stat pathStat;
    ok_ = ((stat(path_.ascii(), &pathStat) == 0) && ((pathStat.st_mode & S_IFDIR) != 0));

    // Normalize pathname.
    if (ok_ && (!nameIsDir(path_)))
    {
        path_ += MARKER;
    }
}

END_NAMESPACE1
