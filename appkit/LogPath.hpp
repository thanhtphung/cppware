/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_LOG_PATH_HPP
#define APPKIT_LOG_PATH_HPP

#include <cstdio>
#include "appkit/String.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)

class App;


//! circular log path
class LogPath
    //!
    //! A class representing a path to a primitive circular log. The circular log
    //! resides in the application's log directory. The member log files are named
    //! using this suffix pattern: "<name><index>.txt". The name and index parts
    //! can be specified during construction. As an example, LogPath("xxx-",'0','2')
    //! is a three-member circular log with these suffixes: "xxx-0.txt", "xxx-1.txt",
    //! and "xxx-2.txt".
    //!
{

public:
    LogPath(const char* name, char lo, char hi);
    ~LogPath();

    const String& curPath() const;
    std::FILE* curLog() const;
    std::FILE* reopen();

    static String newerPath(const String& seedPath, char lo, char hi);
    static String newestPath(const char* name, char lo, char hi);
    static String olderPath(const String& seedPath, char lo, char hi);
    static String oldestPath(const char* name, char lo, char hi);

private:
    String path_;
    char hi_;
    char lo_;
    std::FILE* log_;

    LogPath(const LogPath&); //prohibit usage
    const LogPath& operator =(const LogPath&); //prohibit usage

    void closeLog();
    void openLog();

    static String formBasename(const App*, const char*);

};

//! Return the current path (e.g., "/root/bin/Logs/exe-logname-5.txt").
inline const String& LogPath::curPath() const
{
    return path_;
}

//! Return the current log file. Use reopen() to close the current log file
//! and open the next one in the circular log.
inline std::FILE* LogPath::curLog() const
{
    return log_;
}

END_NAMESPACE1

#endif
