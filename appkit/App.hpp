/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_APP_HPP
#define APPKIT_APP_HPP

#include "syskit/Singleton.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)

class CmdLine;
class Path;
class String;


//! application singleton
class App: public syskit::Singleton
    //!
    //! Plain-old application. User should provide application-specific run-time
    //! logic by overiding the onRun() method. User should also use the DEFINE_APP1()
    //! macro to define how the App singleton is to be created. Example:
    //!\code
    //! // class declaration
    //! class MyApp: public App
    //! {
    //! public:
    //!   MyApp(const CmdLine& cmdLine);
    //!   virtual ~MyApp();
    //!   virtual int onRun();
    //! };
    //!
    //! // class implementation
    //! DEFINE_APP1(MyApp)
    //!\endcode
    //!
{

public:
    enum exitCode_e
    {
        Ok = 0,
        Fatal
    };

    static const char ID1[];
    static const wchar_t DEFAULT_CONF_DIR[];
    static const wchar_t DEFAULT_LOG_DIR[];

    App(const CmdLine& cmdLine,
        bool startInExeDir = false,
        const wchar_t* confDir = DEFAULT_CONF_DIR,
        const wchar_t* logDir = DEFAULT_LOG_DIR);

    bool isOk() const;
    const CmdLine& cmdLine() const;
    const String& confDir() const;
    const String& logDir() const;
    const String& path() const;
    const char* buildDate() const;
    const char* buildTime() const;
    const wchar_t* dumpPath() const;
    int exitCode() const;

    virtual const char* usage() const;
    virtual const char* xUsage() const;
    virtual const wchar_t* copyright() const;
    virtual int onRun();
    virtual int run();

    static App* instance();
    static App* instance(syskit::Singleton::create_t create, unsigned int initialRefCount = 1U, void* createArg = 0);
    static unsigned int initialRefCount();
    static void setInitialRefCount(unsigned int initialRefCount);

protected:
    virtual ~App();

    bool showUsage() const;
    int usageOpt() const;
    void reset(const CmdLine& cmdLine, const wchar_t* confDir = DEFAULT_CONF_DIR, const wchar_t* logDir = DEFAULT_LOG_DIR);
    void setDumpPath(const String& dumpPath);
    void setExitCode(int exitCode);

private:
    const CmdLine* cmdLine_;
    const String* confDir_;
    const String* logDir_;
    const String* oldDir_;
    const String* path_;
    const char* myBuildDate_;
    const char* myBuildTime_;
    const wchar_t* dumpPath_;
    int exitCode_;
    int usageOpt_; //0=none, 1=-?, -1=-??

    static App* instance_;
    static const char* buildDate_;
    static const char* buildTime_;
    static unsigned int initialRefCount_;

    App(const App&); //prohibit usage
    const App& operator =(const App&); //prohibit usage

    void formDumpPath(const Path&);
    void mkDirs(const Path&, const wchar_t*, const wchar_t*);
    void parseCmdLine(const wchar_t*, const wchar_t*);
    void setCurrentDir(bool, const String&);

    static App* instanceInDll();

};

syskit::Singleton* createApp(const char* id, unsigned int initialRefCount, void* arg);

//! Return the singleton.
//! Return zero if non-existent.
inline App* App::instance()
{
#if _WINDLL
    return instanceInDll();
#else
    if (instance_ == 0) instance_ = dynamic_cast<App*>(getSingleton(App::ID1));
    return instance_;
#endif
}

//! Return the application command-line.
inline const CmdLine& App::cmdLine() const
{
    return *cmdLine_;
}

//! Return the application configuration directory.
inline const String& App::confDir() const
{
    return *confDir_;
}

//! Return the application log directory.
inline const String& App::logDir() const
{
    return *logDir_;
}

//! Return the application executable path (e.g., "C:/usr/bin/app.exe").
//! This is a full path normalized by the Path class.
inline const String& App::path() const
{
    return *path_;
}

//! Return the build date in __DATE__ format.
inline const char* App::buildDate() const
{
    return myBuildDate_;
}

//! Return the build time in __TIME__ format.
inline const char* App::buildTime() const
{
    return myBuildTime_;
}

//! Return the path to be used to dump call stack on crash.
inline const wchar_t* App::dumpPath() const
{
    return dumpPath_;
}

//! Return the exit code. Values with range in exitCode_e are
//! reserved for App usage. All others are application specific.
inline int App::exitCode() const
{
    return exitCode_;
}

//! Return command-line help option (0=none, 1=-?, -1=-??).
inline int App::usageOpt() const
{
    return usageOpt_;
}

//! The App singleton is created with this initial reference count. The default
//! value is 1. Use setInitialRefCount() if the singleton needs to be created with
//! a different initial reference count.
inline unsigned int App::initialRefCount()
{
    return initialRefCount_;
}

//! Set the exit code. Values with range in exitCode_e are
//! reserved for App usage. All others are application specific.
inline void App::setExitCode(int exitCode)
{
    exitCode_ = exitCode;
}

//! The App singleton is created with an initial reference count of 1 by default.
//! Use this method if it needs to be created with a different initial reference
//! count. The usage is meaningless if the singleton has already been created.
inline void App::setInitialRefCount(unsigned int initialRefCount)
{
    initialRefCount_ = initialRefCount;
}

END_NAMESPACE1

#include "appkit/win/link-with-appkit.h"
#endif
