/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/Thread.hpp"
#include "syskit/macros.h"

#include "netkit-pch.h"
#include "netkit/IpHlpApi.hpp"

using namespace syskit;

const char* const PROC_NAME[] =
{
    "FreeMibTable",
    "GetAdaptersAddresses",
    "GetExtendedUdpTable",
    "GetIfEntry",
    "GetIfEntry2",
    "GetIfTable",
    "GetIfTable2",
    "NotifyAddrChange"
};

const wchar_t* const DLL_NAME = L"Iphlpapi.dll";

BEGIN_NAMESPACE1(netkit)


IpHlpApi::IpHlpApi()
{
    lib_ = LoadLibraryW(DLL_NAME);
    primitive_ = false;
    memset(procAddr_, 0, sizeof(procAddr_));
    memset(savedAddr_, 0, sizeof(savedAddr_));

    if (lib_ != 0)
    {
        for (int i = 0; i < NumProcs; ++i)
        {
            procAddr_[i] = GetProcAddress(lib_, PROC_NAME[i]);
        }
    }
}


IpHlpApi::~IpHlpApi()
{
    if (lib_ != 0)
    {
        FreeLibrary(lib_);
    }
}


IpHlpApi& IpHlpApi::instance()
{
    static IpHlpApi* s_api = new IpHlpApi;
    for (; s_api == 0; Thread::yield());
    return *s_api;
}


//!
//! Be primitive and use the older interfaces (e.g., GetIfEntry(), etc.) even if
//! the newer ones (e.g., GetIfEntry2(), etc.) are available.
//!
void IpHlpApi::bePrimitive(bool primitive)
{
    if (primitive == primitive_)
    {
        return;
    }

    primitive_ = primitive;
    int newerApi[] = {FreeMibTableI, GetIfEntry2I, GetIfTable2I};
    int numNewerApis = sizeof(newerApi) / sizeof(newerApi[0]);

    if (primitive_)
    {
        for (int i = 0; i < numNewerApis; ++i)
        {
            int j = newerApi[i];
            savedAddr_[j] = procAddr_[j];
            procAddr_[j] = 0;
        }
    }

    else
    {
        for (int i = 0; i < numNewerApis; ++i)
        {
            int j = newerApi[i];
            procAddr_[j] = savedAddr_[j];
            savedAddr_[j] = 0;
        }
    }
}

END_NAMESPACE1
