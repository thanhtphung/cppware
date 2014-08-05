/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <lmcons.h>
#include <stdio.h>
#include <tlhelp32.h>

#include "syskit-pch.h"

#include <psapi.h>
#pragma comment(lib,"psapi")

// The following mask from "v7.0a/include/winnt.h" is mainly for v90 builds.
#ifndef VER_SUITE_WH_SERVER
#define VER_SUITE_WH_SERVER 0x00008000
#endif

#include "syskit/Module.hpp"
#include "syskit/Process.hpp"
#include "syskit/Utc.hpp"
#include "syskit/macros.h"

const wchar_t BACKSLASH = L'\\';
const wchar_t SLASH = L'/';

//
// Clarify OS version with a name.
//
static const wchar_t* osName(const OSVERSIONINFOEXW& ver)
{
    const wchar_t* name = L"windows";
    if (ver.dwMajorVersion == 6)
    {
        if (ver.dwMinorVersion == 2)
        {
            name = (ver.wProductType == VER_NT_WORKSTATION)? L"win-8": L"win-2012";
        }
        else if (ver.dwMinorVersion == 1)
        {
            name = (ver.wProductType == VER_NT_WORKSTATION)? L"win-7": L"win-2008-r2";
        }
        else if (ver.dwMinorVersion == 0)
        {
            name = (ver.wProductType == VER_NT_WORKSTATION)? L"win-vista": L"win-2008";
        }
    }

    else if (ver.dwMajorVersion == 5)
    {
        if (ver.dwMinorVersion == 2)
        {
            if ((ver.wSuiteMask & VER_SUITE_WH_SERVER) != 0)
            {
                name = L"win-home";
            }
            else if (ver.wProductType == VER_NT_WORKSTATION)
            {
                name = L"win-xp64";
            }
            else
            {
                name = (GetSystemMetrics(SM_SERVERR2) == 0)? L"win-2003": L"win-2003-r2";
            }
        }
        else if (ver.dwMinorVersion == 1)
        {
            name = L"win-xp";
        }
        else if (ver.dwMinorVersion == 0)
        {
            name = L"win-2k";
        }
    }

    return name;
}

static void normalizeSlashes(wchar_t* path)
{
    for (wchar_t* w = path; *w; ++w)
    {
        if (*w == BACKSLASH)
        {
            *w = SLASH;
        }
    }
}


BEGIN_NAMESPACE1(syskit)


//!
//! Return hosting OS version (e.g., "6.1.7601-1.0 (win-7 -- Service Pack 1)").
//! The returned string is to be freed by caller using the delete[] operator when done.
//!
wchar_t* Process::osVer()
{
    OSVERSIONINFOEXW ver = {0};
    ver.dwOSVersionInfoSize = sizeof(ver);

#pragma warning(push)
#pragma warning(disable: 4996)
    GetVersionExW(reinterpret_cast<OSVERSIONINFOW*>(&ver));
#pragma warning(pop)

    wchar_t buf[1023 + 1];
    size_t bufSizeInChars = sizeof(buf) / sizeof(buf[0]);
    size_t n = swprintf(buf, bufSizeInChars, L"%lu.%lu.%lu-%lu.%lu (%s",
        ver.dwMajorVersion, ver.dwMinorVersion, ver.dwBuildNumber,
        ver.wServicePackMajor, ver.wServicePackMinor,
        osName(ver));
    wchar_t* w = buf + n;
    if (ver.szCSDVersion[0] != 0)
    {
        bufSizeInChars -= n;
        w += swprintf(w, bufSizeInChars, L" -- %s", ver.szCSDVersion);
    }
    *w++ = L')';
    *w = 0;

    n = w - buf + 1;
    w = new wchar_t[n];
    memcpy(w, buf, n * sizeof(w[0]));
    return w;
}


//!
//! Return associated user name.
//! The returned string is to be freed by caller using the delete[] operator when done.
//!
wchar_t* Process::user() const
{
    wchar_t buf[UNLEN + 1];
    buf[0] = 0;
    DWORD size = UNLEN + 1;
    GetUserNameW(buf, &size);

    wchar_t* w = new wchar_t[size];
    memcpy(w, buf, size*sizeof(w[0]));
    return w;
}


//!
//! Return start time of the current process.
//!
Utc Process::startTime()
{
    union
    {
        FILETIME createTime;
        unsigned long long createTime64;
    };
    FILETIME exitTime;
    FILETIME kernelTime;
    FILETIME usrTime;
    HANDLE h = GetCurrentProcess();
    GetProcessTimes(h, &createTime, &exitTime, &kernelTime, &usrTime);
    return createTime64;
}


//!
//! Apply given callback to loaded modules including ones with inaccessible versions.
//! The callback should return true to continue iterating and should return false to
//! abort iterating. Return false if the callback aborted the iterating. Return true
//! otherwise.
//!
bool Process::apply(cb0_t cb, void* arg) const
{
    Module module;
    bool ok = true;

    DWORD flags = TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32;
    HANDLE h = CreateToolhelp32Snapshot(flags, static_cast<DWORD>(id_));

    MODULEENTRY32W me;
    me.dwSize = sizeof(me);
    for (BOOL more = Module32FirstW(h, &me); more; more = Module32NextW(h, &me))
    {
        normalizeSlashes(me.szExePath);
        module.reset(me.szExePath, me.modBaseAddr, me.modBaseSize);
        bool keepGoing = cb(arg, module);
        if (!keepGoing)
        {
            ok = false;
            break;
        }
    }

    CloseHandle(h);
    return ok;
}


//!
//! Apply given callback to the first maxPids running processes found.
//! The callback should return true to continue iterating and should
//! return false to abort iterating. Return false if the callback aborted
//! the iterating. Return true otherwise.
//!
bool Process::apply(unsigned int maxPids, cb1_t cb, void* arg)
{
    bool ok = true;
    DWORD* pids = new DWORD[maxPids];

    DWORD bufSizeIn = maxPids * sizeof(*pids);
    DWORD bufSizeOut = 0;
    BOOL success = EnumProcesses(pids, bufSizeIn, &bufSizeOut);
    if (success)
    {
        Process proc(0UL);
        for (unsigned int i = 0, numPids = bufSizeOut / sizeof(*pids); i < numPids; ++i)
        {
            proc.id_ = pids[i];
            bool keepGoing = cb(arg, proc);
            if (!keepGoing)
            {
                ok = false;
                break;
            }
        }
    }

    delete[] pids;
    return ok;
}


//!
//! Return true if given process identifier identifies an active process.
//!
bool Process::idIsActive(size_t id)
{
    HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION, 0, static_cast<DWORD>(id));
    if (h == 0)
    {
        h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, 0, static_cast<DWORD>(id));
    }

    return (h != 0)? (CloseHandle(h), true): (GetLastError() == ERROR_ACCESS_DENIED);
}


//!
//! Set the caller's process affinity mask.
//! Return true if successful.
//!
bool Process::setAffinityMask(size_t affinityMask)
{
    HANDLE h = GetCurrentProcess();
    return SetProcessAffinityMask(h, affinityMask) != 0;
}


//!
//! Return the caller's process affinity mask.
//!
size_t Process::affinityMask()
{
    HANDLE h = GetCurrentProcess();
    DWORD_PTR procMask = 0UL;
    DWORD_PTR sysMask = 0UL;
    GetProcessAffinityMask(h, &procMask, &sysMask);
    return procMask;
}


//!
//! Return the process identifier of the caller.
//!
size_t Process::myId()
{
    size_t id = GetCurrentProcessId();
    return id;
}

END_NAMESPACE1
