/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/App.hpp"
#include "appkit/Directory.hpp"
#include "appkit/LogPath.hpp"
#include "appkit/Path.hpp"
#include "appkit/crt.hpp"

using namespace syskit;

BEGIN_NAMESPACE1(appkit)


//!
//! Construct path to a primitive circular log. The circular log resides in the
//! application's log directory. The member log files are named using this suffix
//! pattern: "<name><index>.txt". The index ranges from lo to hi. As an example,
//! LogPath("xxx-",'0','2') is a three-member circular log with these suffixes:
//! "xxx-0.txt", "xxx-1.txt", and "xxx-2.txt".
//!
LogPath::LogPath(const char* name, char lo, char hi):
path_()
{
    const App* app = App::instance();
    String childName(formBasename(app, name));
    size_t nameIndex = childName.byteSize() - 6; //refers to the x before the file extension "x.txt"

    // If member log files exist, start with overwriting the oldest one.
    Directory dir(app->logDir());
    Directory::Attr attr;
    Utc oldest;
    char startAt = lo;
    for (char c = lo; c <= hi; ++c)
    {
        childName.setAscii(nameIndex, c);
        bool ok = dir.getChildAttr(attr, childName) && (!attr.isDir());
        if (!ok)
        {
            startAt = c;
            break;
        }
        Utc t(attr.writeTime());
        if (t < oldest)
        {
            oldest = t;
            startAt = c;
        }
    }

    childName.setAscii(nameIndex, startAt);
    path_ = app->logDir();
    path_ += childName;

    lo_ = lo;
    hi_ = hi;
    openLog(); //log_
}


LogPath::~LogPath()
{
    closeLog();
}


//!
//! Close the current log file and open the next one in the circular log.
//! Return the newly open log file.
//!
std::FILE* LogPath::reopen()
{
    path_ = newerPath(path_, lo_, hi_);
    closeLog();
    openLog();
    return log_;
}


String LogPath::formBasename(const App* app, const char* name)
{
    bool skipNormalization = true;
    Path appPath(app->path(), skipNormalization);
    String basename(appPath.base());
    basename += name;
    basename += "x.txt";
    return basename;
}


//!
//! Return the path of the log file with the most recent write time. Refer to the
//! LogPath constructor for info on the parameters identifying the circular log.
//! Return an empty string if nothing found.
//!
String LogPath::newestPath(const char* name, char lo, char hi)
{
    const App* app = App::instance();
    String childName(formBasename(app, name));
    size_t nameIndex = childName.byteSize() - 6; //refers to the x before the file extension "x.txt"

    Directory dir(app->logDir());
    Directory::Attr attr;
    Utc newestTime(0U, 0U);
    String newest;
    for (char c = lo; c <= hi; ++c)
    {
        childName.setAscii(nameIndex, c);
        bool ok = dir.getChildAttr(attr, childName) && (!attr.isDir());
        if (ok)
        {
            Utc t(attr.writeTime());
            if (t > newestTime)
            {
                newestTime = t;
                newest = childName;
            }
        }
    }

    return newest.empty()? (newest): (dir.path() + newest);
}


//!
//! Return the path of the log file with the least recent write time. Refer to the
//! LogPath constructor for info on the parameters identifying the circular log.
//! Return an empty string if nothing found.
//!
String LogPath::oldestPath(const char* name, char lo, char hi)
{
    const App* app = App::instance();
    String childName(formBasename(app, name));
    size_t nameIndex = childName.byteSize() - 6; //refers to the x before the file extension "x.txt"

    // Look for oldest log file.
    Directory dir(app->logDir());
    Directory::Attr attr;
    Utc oldestTime(0xffffffffffffffffULL);
    String oldest;
    for (char c = lo; c <= hi; ++c)
    {
        childName.setAscii(nameIndex, c);
        bool ok = dir.getChildAttr(attr, childName) && (!attr.isDir());
        if (ok)
        {
            Utc t(attr.writeTime());
            if (t < oldestTime)
            {
                oldestTime = t;
                oldest = childName;
            }
        }
    }

    return oldest.empty()? (oldest): (dir.path() + oldest);
}


//!
//! Return the newer path relative to given seed path (e.g., "/root/bin/Logs/exe-logname-3.txt"
//! if seed path is "/root/bin/Logs/exe-logname-2.txt"). Refer to the LogPath constructor for
//! info on the parameters (lo, hi) describing the circular log.
//!
String LogPath::newerPath(const String& seedPath, char lo, char hi)
{
    size_t i = seedPath.byteSize() - 6; //refers to the x before the file extension "x.txt"
    char cur = seedPath.ascii()[i];
    char newer = (cur == hi)? (lo): (cur + 1);
    String path(seedPath);
    path.setAscii(i, newer);
    return path;
}


//!
//! Return the older path relative to given seed path (e.g., "/root/bin/Logs/exe-logname-3.txt"
//! if seed path is "/root/bin/Logs/exe-logname-4.txt"). Refer to the LogPath constructor for
//! info on the parameters (lo, hi) describing the circular log.
//!
String LogPath::olderPath(const String& seedPath, char lo, char hi)
{
    size_t i = seedPath.byteSize() - 6; //refers to the x before the file extension "x.txt"
    char cur = seedPath.ascii()[i];
    char older = (cur == lo)? (hi): (cur - 1);
    String path(seedPath);
    path.setAscii(i, older);
    return path;
}


void LogPath::closeLog()
{
    if (log_ != stdout)
    {
        fclose(log_);
    }
}


void LogPath::openLog()
{
    String mode("wb+");
    log_ = fopen(path_, mode);
    if (log_ == 0)
    {
        log_ = stdout;
    }
}

END_NAMESPACE1
