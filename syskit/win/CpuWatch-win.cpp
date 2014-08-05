/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "syskit-pch.h"
#include "syskit/CpuWatch.hpp"
#include "syskit/TickTime.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//
// Collect CPU time used by calling process.
//
void CpuWatch::getProcessTime()
{
    FILETIME createTime;
    FILETIME exitTime;
    union
    {
        FILETIME kernelTime;
        unsigned long long kernelTime64;
    };
    union
    {
        FILETIME usrTime;
        unsigned long long usrTime64;
    };
    HANDLE h = GetCurrentProcess();
    GetProcessTimes(h, &createTime, &exitTime, &kernelTime, &usrTime);

    sysTime_ = kernelTime64;
    usrTime_ = usrTime64;
}


//
// Collect CPU time used by calling thread.
//
void CpuWatch::getThreadTime()
{
    FILETIME createTime;
    FILETIME exitTime;
    union
    {
        FILETIME kernelTime;
        unsigned long long kernelTime64;
    };
    union
    {
        FILETIME usrTime;
        unsigned long long usrTime64;
    };
    HANDLE h = GetCurrentThread();
    GetThreadTimes(h, &createTime, &exitTime, &kernelTime, &usrTime);

    sysTime_ = kernelTime64;
    usrTime_ = usrTime64;
}


void CpuWatch::cpuTimeToTicks(unsigned long long& sysTime, unsigned long long& usrTime) const
{

    // CPU time from GetProcessTimes() and  GetThreadTimes()
    // are in 100-nsecs. Multiply by 100 to get nsecs, etc.
    sysTime = sysTime * TickTime::ticksPerMsec() / 10000;
    usrTime = usrTime * TickTime::ticksPerMsec() / 10000;
}

END_NAMESPACE1
