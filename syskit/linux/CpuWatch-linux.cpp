/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include "syskit/CpuWatch.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)

const unsigned int USECS_PER_SEC = 1000000U;


//
// Collect CPU time used by calling process.
//
void CpuWatch::getProcessTime()
{
    struct rusage u;
    getrusage(RUSAGE_SELF, &u);
    sysTime_ = u.ru_stime.tv_sec * USECS_PER_SEC + u.ru_stime.tv_usec;
    usrTime_ = u.ru_utime.tv_sec * USECS_PER_SEC + u.ru_utime.tv_usec;
}


//
// Collect CPU time used by calling thread. This is a Windows-only
// capability. On other platforms, this method would just collect
// CPU time used by calling process.
//
void CpuWatch::getThreadTime()
{
    getProcessTime();
}


void CpuWatch::cpuTimeToTicks(unsigned long long& /*sysTime*/, unsigned long long& /*usrTime*/) const
{

    // CPU times from getrusage() are already in normalized ticks.
    // No further conversion is required.
}

END_NAMESPACE1
