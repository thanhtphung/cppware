/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "syskit-pch.h"
#include "syskit/macros.h"
#include "syskit/win/Janitor-win.hpp"

BEGIN_NAMESPACE1(syskit)


Janitor::Janitor():
cs_()
{
    handle_ = 0;
    numHandles_ = 0;
}


Janitor::~Janitor()
{
    for (size_t i = numHandles_; i > 0; CloseHandle(handle_[--i]));
}


Janitor& Janitor::instance()
{
    static Janitor s_janitor;
    return s_janitor;
}


void Janitor::closeAtExit(HANDLE h)
{
    CriSection::Lock lock(cs_);
    HANDLE* handle = new HANDLE[numHandles_ + 1];
    memcpy(handle, handle_, numHandles_ * sizeof(*handle_));
    handle[numHandles_++] = h;
    delete handle_;
    handle_ = handle;
}

END_NAMESPACE1
