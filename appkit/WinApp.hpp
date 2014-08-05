/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_WIN_APP_HPP
#define APPKIT_WIN_APP_HPP

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include "appkit/App.hpp"
#include "syskit/macros.h"

DECLARE_CLASS1(netkit, Paddr)
DECLARE_CLASS1(netkit, Winsock)
DECLARE_CLASS1(syskit, MappedTxtFile)
DECLARE_CLASS1(syskit, SigTrap)
DECLARE_CLASS1(syskit, Thread)

BEGIN_NAMESPACE1(appkit)

class CmdAgent;
class SysIo;
class WinAppCmd;


//! windows application w/ built-in command handlers
class WinApp: public App
{

public:
    enum exitCode_e
    {
        InitFailed = 20,
        MessageLoopFailed
    };

    WinApp(const CmdLine& cmdLine,
        bool startInExeDir = false,
        unsigned int messagingId = 0,
        const char* defaultIoAddr = "127.0.0.1:0",
        const wchar_t* confDir = DEFAULT_CONF_DIR,
        const wchar_t* logDir = DEFAULT_LOG_DIR);

    CmdAgent* loopbackAgent() const;
    HWND messagingWin() const;
    const String& alias() const;
    const SysIo* sysIo() const;
    size_t messagingThreadId() const;
    unsigned int logMask() const;
    unsigned int messagingId() const;
    void setAlias(const String& alias);
    void setLogMask(unsigned int logMask);

    // Override App.
    virtual const char* usage() const;
    virtual const char* xUsage() const;
    virtual int onRun();
    virtual int run();

    virtual bool onWalk();
    virtual bool stop();
    virtual int runMessageLoop();
    virtual void onConfig(const CmdLine& config);

    static WinApp* instance();

protected:
    netkit::Paddr defaultIoAddr() const;
    bool enterLoopAtRun() const;
    void setDefaultIoAddr(const char* defaultIoAddr);
    void setDefaultMessageLoop(bool enterLoopAtRun);

    virtual ~WinApp();

private:
    typedef struct runArg_s
    {
        runArg_s(const WinApp*, const syskit::MappedTxtFile*);
        syskit::Thread* t;
        const syskit::MappedTxtFile* rcFile;
        const WinApp* winApp;
    } runArg_t;

    class MessagingWin
    {
    public:
        MessagingWin(WNDPROC wndProc);
        ~MessagingWin();
        operator HWND() const;
    private:
        HINSTANCE module_;
        HWND hwnd_;
        MessagingWin(const MessagingWin&); //prohibit usage
        const MessagingWin& operator =(const MessagingWin&); //prohibit usage
    };

    CmdAgent* loopbackAgent_;
    MessagingWin* messagingWin_;
    netkit::Paddr* defaultIoAddr_;
    syskit::SigTrap* sigTrap_;
    String* alias_;
    SysIo* sysIo_;
    WinAppCmd* cmd_;
    netkit::Winsock* winsock_;
    bool enterLoopAtRun_;
    size_t messagingThreadId_;
    unsigned int volatile logMask_;
    unsigned int messagingId_;

    WPARAM exitCode_;
    bool volatile exitMessageLoop_;

    WinApp(const WinApp&); //prohibit usage
    const WinApp& operator =(const WinApp&); //prohibit usage

    syskit::MappedTxtFile* mapRcFile() const;
    void runStartUpCmds(const syskit::MappedTxtFile&) const;

    static LRESULT CALLBACK WndProc(HWND, unsigned int, WPARAM, LPARAM);
    static void* agentEntry(void*);

};

inline CmdAgent* WinApp::loopbackAgent() const
{
    return loopbackAgent_;
}

//! This implementation can enter a message loop when the application runs via the
//! run() method. The messaging thread can be identified using messagingThreadId().
//! Messages to be sent to this thread should be addressed using messagingWin().
inline HWND WinApp::messagingWin() const
{
    HWND hwnd = *messagingWin_;
    return hwnd;
}

//! Return true if application will enter a message loop via the run() method.
//! Return false otherwise. This has little meaning when application has already
//! started running. With the default behavior, run() does not enter a message
//! loop. Use setDefaultMessageLoop() to change the behavior.
inline bool WinApp::enterLoopAtRun() const
{
    return enterLoopAtRun_;
}

//! Return the application's alias.
inline const String& WinApp::alias() const
{
    return *alias_;
}

inline const SysIo* WinApp::sysIo() const
{
    return sysIo_;
}

//! This implementation can enter a windows message loop when the application
//! runs via the run() method. Return the identifer of the messaging thread.
//! The returned value is zero if the application has not run yet.
inline size_t WinApp::messagingThreadId() const
{
    return messagingThreadId_;
}

//! Return current log mask. Logging is minimal if most or all bits are
//! off and maximal if most or all bits are on.
inline unsigned int WinApp::logMask() const
{
    return logMask_;
}

//! This implementation uses a WM_APP+i message to interop w/ the windows
//! messaging mechanism. The i index can be specified at construction.
//! Return the WM_APP+i value.
inline unsigned int WinApp::messagingId() const
{
    return messagingId_;
}

//! With the default behavior, run() does not enter a message loop. Use this
//! method to change the behavior.
inline void WinApp::setDefaultMessageLoop(bool enterLoopAtRun)
{
    enterLoopAtRun_ = enterLoopAtRun;
}

inline WinApp::MessagingWin::operator HWND() const
{
    return hwnd_;
}

inline WinApp::runArg_s::runArg_s(const WinApp* winApp, const syskit::MappedTxtFile* rcFile)
{
    runArg_s::rcFile = rcFile;
    runArg_s::t = 0;
    runArg_s::winApp = winApp;
}

END_NAMESPACE1

#endif
