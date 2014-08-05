/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_DEMON_HPP
#define APPKIT_DEMON_HPP

#include "appkit/App.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)

class CmdLine;
class String;


//! demon application
class Demon: public App
    //!
    //! Demon application. A demon application runs as a service on windows
    //! and can be automatically restarted when desirable. User should provide
    //! application-specific run-time logic by overiding the onStart(), onStop(),
    //! and onSysShutdown() methods. Example:
    //!\code
    //! class MyApp: public Demon
    //! {
    //! public:
    //!   MyApp(const CmdLine& cmdLine);
    //!   virtual ~MyApp();
    //!   virtual void onStart();
    //!   virtual void onStop();
    //!   virtual void onSysShutdown();
    //! };
    //!\endcode
    //!
{

public:
    enum exitCode_e
    {
        AddSvcFailed = 10,
        RmSvcFailed,
        StartFailed,
        StartSvcFailed,
        StopSvcFailed
    };

    Demon(const CmdLine& cmdLine,
        const String& name,
        bool startInExeDir = true,
        bool usesNet = false,
        const wchar_t* confDir = DEFAULT_CONF_DIR,
        const wchar_t* logDir = DEFAULT_LOG_DIR);

    bool runningAsProcess() const;
    bool usesNet() const;
    const String& name() const;
    const wchar_t* nameW() const;

    // Override App.
    virtual const char* usage() const;
    virtual const char* xUsage() const;
    virtual int onRun();
    virtual int run();

    virtual bool stop(bool restart = false);
    virtual void onStart();
    virtual void onStop();
    virtual void onSysShutdown();

    static Demon* instance();
    static size_t findPid(const wchar_t* name);

protected:
    void setDefaultRunMode(bool toBeRunAsProcess);

    virtual ~Demon();

private:
    typedef bool (Demon::*controlSvc_t)() const;

    String* name_;
    const wchar_t* nameW_;

    bool restartOnExit_;
    bool runningAsProcess_;
    bool toBeRunAsProcess_;
    bool usesNet_;

    Demon(const Demon&); //prohibit usage
    const Demon& operator =(const Demon&); //prohibit usage

    bool addSvc(const String*) const;
    bool rmSvc() const;
    bool startSvc() const;
    bool stopSvc() const;
    int start();
    int walk();

};

//! Return true if demon is running as a process (vs. service).
//! Returned value is false if demon has not started running yet.
inline bool Demon::runningAsProcess() const
{
    return runningAsProcess_;
}

//! Return true if demon depends on networking components at service start.
inline bool Demon::usesNet() const
{
    return usesNet_;
}

//! Return the demon's name.
inline const String& Demon::name() const
{
    return *name_;
}

//! Return the demon's name.
inline const wchar_t* Demon::nameW() const
{
    return nameW_;
}

//! Configure default run mode. The run mode can be specified at construction using
//! the "--process" command-line option. This method configures the default run mode
//! if that command-line option is not specified. The setting is meaningless if demon
//! has already started running.
inline void Demon::setDefaultRunMode(bool toBeRunAsProcess)
{
    toBeRunAsProcess_ = toBeRunAsProcess;
}

END_NAMESPACE1

#endif
