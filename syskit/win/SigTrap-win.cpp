/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <signal.h>

#include "syskit-pch.h"
#include "syskit/macros.h"
#include "syskit/SigTrap.hpp"

static HANDLE s_thread = 0;
static bool* s_empty = 0;

static void CALLBACK apc(ULONG_PTR /*arg*/)
{
}

static void handleSig(int /*sig*/)
{
    ULONG_PTR arg = 0;
    QueueUserAPC(apc, s_thread, arg);

    *s_empty = false;
}

BEGIN_NAMESPACE1(syskit)


SigTrap::SigTrap()
{
    empty_ = true;
    s_empty = &empty_;

    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();
    DuplicateHandle(process, thread, process, &s_thread, 0, 0, DUPLICATE_SAME_ACCESS);
    signal(SIGINT, handleSig);
    signal(SIGTERM, handleSig);
}


SigTrap::~SigTrap()
{
    CloseHandle(s_thread);
}

END_NAMESPACE1
