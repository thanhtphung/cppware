/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "syskit/Thread.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/Demon.hpp"
#include "appkit/String.hpp"

const wchar_t* NET_SVC = L"RPCSS\0"; //double-null-terminated

using namespace appkit;
using namespace syskit;

BEGIN_NAMESPACE


class RunningSvc
{
public:
    RunningSvc(const wchar_t* name, unsigned int scmAccess, unsigned int svcAccess);
    ~RunningSvc();
    operator SC_HANDLE() const;
    bool isOk() const;
private:
    SC_HANDLE scm_;
    SC_HANDLE svc_;
    RunningSvc(const RunningSvc&); //prohibit usage
    const RunningSvc& operator =(const RunningSvc&); //prohibit usage
};

inline RunningSvc::operator SC_HANDLE() const
{
    return svc_;
}

inline bool RunningSvc::isOk() const
{
    bool ok = (svc_ != 0);
    return ok;
}


RunningSvc::RunningSvc(const wchar_t* name, unsigned int scmAccess, unsigned int svcAccess)
{
    const wchar_t* machine = 0;
    const wchar_t* db = 0;
    scm_ = OpenSCManagerW(machine, db, scmAccess);
    svc_ = OpenServiceW(scm_, name, svcAccess);
}


RunningSvc::~RunningSvc()
{
    if (svc_ != 0)
    {
        CloseServiceHandle(svc_);
    }

    if (scm_ != 0)
    {
        CloseServiceHandle(scm_);
    }
}


class Svc
{
public:
    Svc(Demon* app);
    ~Svc();
    bool loop();
private:
    Demon* app_;
    SERVICE_STATUS status_;
    SERVICE_STATUS_HANDLE handle_;
    Svc(const Svc&); //prohibit usage
    const Svc& operator =(const Svc&); //prohibit usage
    bool start();
    void setState(unsigned int, unsigned int);
    static unsigned long WINAPI onControl(unsigned long, unsigned long, void*, void*);
    static void WINAPI main(unsigned long, wchar_t*);
};

static Svc* s_svc = 0;


Svc::Svc(Demon* app)
{
    if (s_svc != 0)
    {
        const char* const TOMBSTONE = "Svc::Svc: s_svc not a singleton!!";
        const char** grave = 0;
        *grave = TOMBSTONE;
    }
    s_svc = this;

    app_ = app;
    handle_ = 0;
    status_.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    status_.dwCurrentState = 0;
    status_.dwControlsAccepted = 0;
    status_.dwWin32ExitCode = 0;
    status_.dwServiceSpecificExitCode = 0;
    status_.dwCheckPoint = 0;
    status_.dwWaitHint = 3000; //msecs
}


Svc::~Svc()
{
    s_svc = 0;
}


bool Svc::loop()
{
    wchar_t* w = app_->name().widen().detachRaw();
    SERVICE_TABLE_ENTRYW svcTbl[] =
    {
        {w, (LPSERVICE_MAIN_FUNCTIONW)&Svc::main},
        {0, 0}
    };

    bool ok = (StartServiceCtrlDispatcherW(svcTbl) != 0);
    delete[] w;
    return ok;
}


bool Svc::start()
{
    bool ok;
    handle_ = RegisterServiceCtrlHandlerExW(app_->nameW(), (LPHANDLER_FUNCTION_EX)&Svc::onControl, 0);
    if (handle_ != 0)
    {
        status_.dwCheckPoint = 1;
        setState(SERVICE_START_PENDING, 0);
        app_->onStart();
        status_.dwCheckPoint = 0;
        setState(SERVICE_RUNNING, SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP);
        ok = true;
    }

    else
    {
        ok = false;
    }

    return ok;
}


unsigned long WINAPI Svc::onControl(unsigned long control, unsigned long /*evType*/, void* /*evData*/, void* /*arg*/)
{
    if (s_svc == 0)
    {
        return 0;
    }

    switch (control)
    {
    case SERVICE_CONTROL_SHUTDOWN:
        s_svc->app_->onSysShutdown();
        break;
    case SERVICE_CONTROL_STOP:
        s_svc->status_.dwCheckPoint = 1;
        s_svc->setState(SERVICE_STOP_PENDING, 0);
        s_svc->app_->onStop();
        s_svc->status_.dwCheckPoint = 0;
        s_svc->setState(SERVICE_STOPPED, 0);
        delete s_svc;
        break;
    default: //SERVICE_CONTROL_INTERROGATE
        break;
    }

    return 0;
}


//
// Report state to service control manager.
//
void Svc::setState(unsigned int state, unsigned int controls)
{
    status_.dwCurrentState = state;
    status_.dwControlsAccepted = controls;
    SetServiceStatus(handle_, &status_);
}


void WINAPI Svc::main(unsigned long /*argc*/, wchar_t* /*argv*/)
{
    s_svc->start();
}

END_NAMESPACE

BEGIN_NAMESPACE1(appkit)


//
// Add service (register with service control manager).
//
bool Demon::addSvc(const String* runArg) const
{
    const wchar_t* machine = 0;
    const wchar_t* db = 0;
    unsigned int access = SC_MANAGER_CREATE_SERVICE;
    SC_HANDLE scm = OpenSCManagerW(machine, db, access);
    if (scm == 0)
    {
        bool ok = false;
        return ok;
    }

    HMODULE module = 0;
    wchar_t path[MAX_PATH];
    path[0] = 0;
    GetModuleFileNameW(module, path, MAX_PATH);
    String cmd(1, '"');
    cmd += path;
    if (runArg == 0)
    {
        cmd += '"';
    }
    else
    {
        cmd += "\" ";
        cmd += *runArg;
    }

    const wchar_t* name = nameW();
    access = SERVICE_ALL_ACCESS;
    unsigned int svcType = SERVICE_WIN32_OWN_PROCESS;
    unsigned int startMode = SERVICE_AUTO_START;
    unsigned int errControl = SERVICE_ERROR_NORMAL;
    const wchar_t* dependsOn = usesNet_? NET_SVC: 0;
    SC_HANDLE svc = CreateServiceW(scm, name, name, access, svcType, startMode, errControl, cmd.widen(), 0, 0, dependsOn, 0, 0);
    if (svc == 0)
    {
        CloseServiceHandle(scm);
        bool ok = false;
        return ok;
    }

    CloseServiceHandle(svc);
    CloseServiceHandle(scm);
    bool ok = true;
    return ok;
}


//
// Remove service (unregister from service control manager).
//
bool Demon::rmSvc() const
{
    const wchar_t* name = nameW();
    unsigned int scmAccess = SC_MANAGER_ALL_ACCESS;
    unsigned int svcAccess = DELETE;
    RunningSvc svc(name, scmAccess, svcAccess);
    bool ok = svc.isOk()? (DeleteService(svc) != 0): (false);
    return ok;
}


//
// Start service.
//
bool Demon::startSvc() const
{
    const wchar_t* name = nameW();
    unsigned int scmAccess = SC_MANAGER_ALL_ACCESS;
    unsigned int svcAccess = SERVICE_START;
    RunningSvc svc(name, scmAccess, svcAccess);
    bool ok = svc.isOk()? (StartServiceW(svc, 0, 0) != 0): (false);
    return ok;
}


//
// Stop service.
//
bool Demon::stopSvc() const
{
    const wchar_t* name = nameW();
    unsigned int scmAccess = SC_MANAGER_ALL_ACCESS;
    unsigned int svcAccess = SERVICE_STOP;
    RunningSvc svc(name, scmAccess, svcAccess);
    SERVICE_STATUS ss;
    bool ok = svc.isOk()? (ControlService(svc, SERVICE_CONTROL_STOP, &ss) != 0): (false);
    return ok;
}


//
// Start service.
//
int Demon::start()
{
    Svc* svc = new Svc(this);
    int rc = svc->loop()? exitCode(): StartFailed;
    setExitCode(rc);
    delete s_svc; //non-zero for StartFailed scenario
    return rc;
}


//!
//! Find and return the process identifer of a running service.
//! Return zero if not found.
//!
size_t Demon::findPid(const wchar_t* name)
{
    unsigned int scmAccess = SC_MANAGER_ALL_ACCESS;
    unsigned int svcAccess = SERVICE_QUERY_STATUS;
    RunningSvc svc(name, scmAccess, svcAccess);

    size_t pid = 0;
    if (svc.isOk())
    {
        SERVICE_STATUS_PROCESS ssp = {0};
        unsigned long bufSize = sizeof(ssp);
        int ok = QueryServiceStatusEx(svc, SC_STATUS_PROCESS_INFO, reinterpret_cast<unsigned char*>(&ssp), bufSize, &bufSize);
        if (ok)
        {
            pid = ssp.dwProcessId;
        }
    }

    return pid;
}

END_NAMESPACE1
