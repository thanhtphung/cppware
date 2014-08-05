/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "syskit-pch.h"
#include "syskit/Module.hpp"
#include "syskit/macros.h"

#pragma comment(lib,"version")

BEGIN_NAMESPACE1(syskit)


bool Module::getVers(const wchar_t* path)
{
    fileVer_ = 0;
    prodVer_ = 0;

    bool ok = false;
    DWORD h;
    DWORD size0 = GetFileVersionInfoSizeW(path, &h);
    if (size0 == 0)
    {
        return ok;
    }

    unsigned char* buf0 = new unsigned char[size0];
    int rc = GetFileVersionInfoW(path, h, size0, buf0);
    if (rc)
    {
        const VS_FIXEDFILEINFO* info;
        const wchar_t* subBlock = L"\\";
        void* buf1 = 0;
        unsigned int size1 = 0;
        rc = VerQueryValueW(buf0, subBlock, &buf1, &size1);
        if (rc && (size1 == sizeof(*info)))
        {
            info = static_cast<const VS_FIXEDFILEINFO*>(buf1);
            fileVer_ = static_cast<unsigned long long>(info->dwFileVersionMS) << 32 | info->dwFileVersionLS;
            prodVer_ = static_cast<unsigned long long>(info->dwProductVersionMS) << 32 | info->dwProductVersionLS;
            ok = true;
        }
    }

    delete[] buf0;
    return ok;
}

END_NAMESPACE1
