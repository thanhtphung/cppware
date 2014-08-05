/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <signal.h>
#include "netkit/Winsock.hpp"
#include "syskit/MappedTxtFile.hpp"
#include "syskit/SigTrap.hpp"
#include "syskit/Thread.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/CmdAgent.hpp"
#include "appkit/CmdLine.hpp"
#include "appkit/CmdMap.hpp"
#include "appkit/DelimitedTxt.hpp"
#include "appkit/Path.hpp"
#include "appkit/SysIo.hpp"
#include "appkit/U32.hpp"
#include "appkit/WinApp.hpp"
#include "appkit/WinAppCmd.hpp"
#include "appkit/WinCmd.hpp"

using namespace netkit;
using namespace syskit;

const char COLON = ':';
const unsigned int FLUSH_LOG_ENTRIES_M = 0x80U;
const unsigned int LOG_REQ_M = 0x01U;
const unsigned int LOG_RSP_M = 0x02U;

BEGIN_NAMESPACE1(appkit)


WinApp::WinApp(const CmdLine& cmdLine,
bool startInExeDir,
unsigned int messagingId,
const char* defaultIoAddr,
const wchar_t* confDir,
const wchar_t* logDir):
App(cmdLine, startInExeDir, confDir, logDir)
{
    CmdMap::instance()->addRef();
    winsock_ = new Winsock;

    alias_ = 0;
    String alias;
    setAlias(alias);

    cmd_ = 0;
    defaultIoAddr_ = new Paddr(defaultIoAddr);
    enterLoopAtRun_ = false;
    logMask_ = LOG_REQ_M;
    loopbackAgent_ = 0;
    messagingId_ = WM_APP + messagingId;
    messagingThreadId_ = 0;
    messagingWin_ = new MessagingWin(WndProc);
    sigTrap_ = 0;
    sysIo_ = 0;

    exitCode_ = Ok;
    exitMessageLoop_ = false;
}


WinApp::~WinApp()
{
    delete loopbackAgent_;
    delete sysIo_;
    delete sigTrap_;
    delete messagingWin_;
    delete defaultIoAddr_;
    RefCounted::rmRef(cmd_);
    delete alias_;

    delete winsock_;
    CmdMap::instance()->rmRef();
}


//
// This is the backup message handler for WinApp. Messages are usually intercepted
// and handled in the runMessageLoop() messaging loop. However, that loop can be
// bypassed when the UI thread is modal (e.g., user is moving/resizing a window).
// In that scenario, messages can still be dispatched via a message loop internal
// to the modal system. This is why PostMessage() and not PostThreadMessage() must
// be used to communicate with the WinApp messaging thread.
//
LRESULT CALLBACK WinApp::WndProc(HWND hwnd, unsigned int msg, WPARAM wparam, LPARAM lparam)
{
    WinApp* winApp = instance();
    if (msg == winApp->messagingId_)
    {
        WinCmd* cmd = reinterpret_cast<WinCmd*>(wparam);
        const CmdLine* req = reinterpret_cast<const CmdLine*>(lparam);
        size_t i = Cmd::cmdIndex(*req);
        cmd->cmdIsValid_ = cmd->onFinalRun(*req);
        cmd->sem_[i].increment();
        LRESULT lr = 0;
        return lr;
    }

    if (msg == WM_QUIT)
    {
        winApp->exitCode_ = wparam;
        winApp->exitMessageLoop_ = true;
    }

    LRESULT lr = DefWindowProc(hwnd, msg, wparam, lparam);
    return lr;
}


MappedTxtFile* WinApp::mapRcFile() const
{

    // No problem if no accessible rc3 file. Allow use of same rc3 file for both debug
    // and release builds (e.g., x.rc3 can be used by both x.exe and xd.exe).
    String rcPath(confDir());
    rcPath += Path(path()).base();
    rcPath += ".rc3";
    bool readOnly = true;
    bool skipBom = true;
    MappedTxtFile* rcFile = new MappedTxtFile(rcPath.widen(), readOnly, skipBom);
    if (!rcFile->isOk())
    {
        bool ignoreCase = true;
        if (rcPath.endsWith("d.rc3", ignoreCase))
        {
            rcPath.truncate(rcPath.length() - 5);
            rcPath += ".rc3";
            rcFile->remap(rcPath.widen(), readOnly);
        }
    }

    return rcFile;
}


Paddr WinApp::defaultIoAddr() const
{
    return *defaultIoAddr_;
}


//!
//! Return the singleton.
//!
WinApp* WinApp::instance()
{
    return dynamic_cast<WinApp*>(App::instance());
}


//!
//! This method performs second-phase initialization to support the command
//! handling scheme used by Cmd. This is invoked right before onRun() and
//! allows applications to abort running if necessary. Return true to keep
//! going. A false value causes run() to return InitFailed ASAP.
//!
bool WinApp::onWalk()
{
    sigTrap_ = new SigTrap;
    const CmdLine& config = App::cmdLine();
    sysIo_ = new SysIo(config, defaultIoAddr_->addr(), defaultIoAddr_->port());
    setLogMask(logMask_); //sync w/ sysIo_ logMask
    cmd_ = new WinAppCmd(this, sysIo_);
    loopbackAgent_ = CmdAgent::createLoopback(sysIo_);

    bool keepGoing = true;
    return keepGoing;
}


//!
//! Stop application. Return true if successful.
//!
bool WinApp::stop()
{
    WPARAM wparam = Ok; //exit code
    LPARAM lparam = 0;
    int ok = PostMessageW(*messagingWin_, WM_QUIT, wparam, lparam);
    if (!ok)
    {
        raise(SIGTERM);
    }

    return true;
}


//!
//! Return the application usage.
//!
const char* WinApp::usage() const
{
    const char* s = App::usage();
    return s;
}


//!
//! Return the application extended usage.
//!
const char* WinApp::xUsage() const
{
    const char* s = App::xUsage();
    return s;
}


//!
//! This method is invoked when the application is to start running. The method
//! is expected to return an exit code. The default implementation initiates
//! start-up command processing. A basic window messaging loop can also be entered
//! here if the application is configured to do so via setDefaultMessageLoop().
//! Start-up commands reside in corresponding rc3 file located in the confDir()
//! directory with name "Path(path()).base().rc3". For example, xxx.rc3 would be
//! the corresponding rc3 file for xxx.exe. The rc3 file is optional.
//!
int WinApp::onRun()
{
    bool keepGoing = onWalk();
    if (!keepGoing)
    {
        int rc = InitFailed;
        return rc;
    }

    // Allow users to implement their own onWalk() version. Initiate start-up
    // command processing. Run the commands using a separate thread as they
    // might need to interact with this thread which will enter a basic window
    // messaging loop momentarily.
    if (sysIo_ != 0)
    {
        const MappedTxtFile* rcFile = mapRcFile();
        if (rcFile->isOk())
        {
            runArg_t* arg = new runArg_t(this, rcFile);
            unsigned int stackSizeInBytes = 0; //default
            bool startSuspended = true;
            Thread* t = new Thread(agentEntry, arg, stackSizeInBytes, startSuspended);
            arg->t = t;
            if ((!t->isOk()) || (!t->resume()))
            {
                delete arg;
                delete t;
            }
            else
            {
                rcFile = 0; //to be deleted by agent thread at end of start-up command processing
            }
        }
        delete rcFile;
    }

    int rc = enterLoopAtRun_? runMessageLoop(): Ok;
    return rc;
}


//!
//! Run application.
//!
int WinApp::run()
{
    messagingThreadId_ = Thread::myId();
    int exitCode = App::run();
    return exitCode;
}


//!
//! Custom windows messaging loop. Allow WinCmd commands to be handled by the
//! messaging thread. Basically, WinCmd::onRun() is invoked by a dedicated agent
//! task. That task then waits for WinCmd::onFinalRun() to be invoked by this
//! windows messaging thread. A semaphore is used to synchronize the two agents.
//! A derived class can override this to implement its own loop. To support WinCmd
//! command handling, application code must still dispatch windows messages timely
//! using its own scheme.
//!
int WinApp::runMessageLoop()
{

    // The loop terminates when a WM_QUIT message is received or if GetMessageW() fails.
    int rc;
    size_t oldMessagingThreadId = messagingThreadId_;
    messagingThreadId_ = Thread::myId();
    MSG msg = {0};
    while (((rc = GetMessageW(&msg, 0, 0, 0)) > 0) && (!exitMessageLoop_))
    {

        if (msg.message == messagingId_)
        {
            WinCmd* cmd = reinterpret_cast<WinCmd*>(msg.wParam);
            const CmdLine* req = reinterpret_cast<const CmdLine*>(msg.lParam);
            size_t i = Cmd::cmdIndex(*req);
            cmd->cmdIsValid_ = cmd->onFinalRun(*req);
            cmd->sem_[i].increment();
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // WM_QUIT received in the backup message handler: WndProc().
    if (exitMessageLoop_)
    {
        msg.wParam = exitCode_;
        rc = 0;
    }

    messagingThreadId_ = oldMessagingThreadId;
    rc = (rc == 0)? static_cast<int>(msg.wParam): Fatal;
    return rc;
}


//!
//! This method is invoked when the application is being configured via the app-config command.
//! The config parameter holds the full app-config command being processed.
//!
void WinApp::onConfig(const CmdLine& config)
{
    String optK("alias");
    const String* alias = config.opt(optK);
    if (alias != 0)
    {
        setAlias(*alias);
    }

    optK = "logmask";
    U32 logMask(config.opt(optK), logMask_ /*defaultV*/);
    if (logMask_ != logMask)
    {
        setLogMask(logMask);
    }
}


//
// Run start-up commands from rc3 file, if any.
// This is done in the background by a separate thread.
//
void WinApp::runStartUpCmds(const MappedTxtFile& rcFile) const
{

    // Run start-up commands from rc3 file using loopback agent.
    bool makeCopy = false;
    DelimitedTxt rcTxt(reinterpret_cast<const char*>(rcFile.image()), static_cast<size_t>(rcFile.imageSize()), makeCopy);
    String cmd;
    bool doTrimLine = true;
    while (rcTxt.next(cmd, doTrimLine) && (!cmd_->exitInProgress()))
    {
        if (!cmd.empty())
        {
            loopbackAgent_->runCmd(cmd);
        }
    }
}


//!
//! Override the default alias. By default, each WinApp instance has an alias
//! of "<hostname>:<exeBasename>". Default alias is used if given alias is empty.
//!
void WinApp::setAlias(const String& alias)
{
    if (alias.empty())
    {
        char host[255 + 1];
        host[0] = 0;
        gethostname(host, sizeof(host));
        setAlias(String(host) + COLON + Path(path(), true /*skipNormalization*/).base()); //recursively
        return;
    }

    delete alias_;
    alias_ = new String(alias);
}


//!
//! Set the default home address of the I/O subsystem. This is used during second-phase
//! initialization occurring right before onRun(). Setting this address after that point
//! has no practical meaning.
//!
void WinApp::setDefaultIoAddr(const char* defaultIoAddr)
{
    delete defaultIoAddr_;
    defaultIoAddr_ = new Paddr(defaultIoAddr);
}


//!
//! Update the current log mask. Logging is minimal if most or all bits are off
//! and maximal if most or all bits are on.
//!
void WinApp::setLogMask(unsigned int logMask)
{
    logMask_ = logMask;
    if (sysIo_)
    {
        bool logReq = ((logMask_ & LOG_REQ_M) != 0);
        bool logRsp = ((logMask_ & LOG_RSP_M) != 0);
        bool flushLogEntries = ((logMask_ & FLUSH_LOG_ENTRIES_M) != 0);
        sysIo_->setLogMode(logReq, logRsp, flushLogEntries);
    }
}


//
// Start-up commands need to be run in the background by a separate thread.
// This is its entry point.
//
void* WinApp::agentEntry(void* arg)
{
    const runArg_t* runArg = static_cast<const runArg_t*>(arg);
    Thread* t = runArg->t;
    const MappedTxtFile* rcFile = runArg->rcFile;
    const WinApp* winApp = runArg->winApp;
    winApp->runStartUpCmds(*rcFile);
    delete runArg;
    delete rcFile;
    t->detach(); //avoid waiting for this thread itself to exit
    delete t;
    return 0;
}


WinApp::MessagingWin::MessagingWin(WNDPROC wndProc)
{
    const wchar_t* className = L"appkit::messaging";
    const wchar_t* moduleName = 0;
    module_ = GetModuleHandleW(moduleName);

    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(wc);
    wc.style = 0;
    wc.lpfnWndProc = wndProc;
    wc.cbWndExtra = 0;
    wc.hInstance = module_;
    wc.hIcon = 0;
    wc.hCursor = 0;
    wc.lpszClassName = className;
    RegisterClassExW(&wc);

    unsigned int exStyle = 0;
    const wchar_t* name = 0;
    unsigned int style = 0;
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    HWND parent = HWND_MESSAGE;
    HMENU menu = 0;
    void* createParam = 0;
    hwnd_ = CreateWindowExW(exStyle, className, name, style, x, y, width, height, parent, menu, module_, createParam);
}


WinApp::MessagingWin::~MessagingWin()
{
    if (hwnd_ != 0)
    {
        DestroyWindow(hwnd_);
    }

    const wchar_t* className = L"appkit::messaging";
    UnregisterClassW(className, module_);
}

END_NAMESPACE1
