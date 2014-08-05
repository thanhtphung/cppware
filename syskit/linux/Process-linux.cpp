/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <sched.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "syskit/Process.hpp"
#include "syskit/Utc.hpp"
#include "syskit/Utf8Seq.hpp"
#include "syskit/sys.hpp"

const size_t MAX_CPUS = sizeof(void*) * 8; //32 for x86, 64 for x64, etc.

BEGIN_NAMESPACE1(syskit)


//!
//! Return start time of the current process.
//!
Utc Process::startTime()
{
    unsigned long long createTime = 0;
    return createTime;
}


//!
//! Apply given callback to loaded modules including ones with inaccessible versions.
//! The callback should return true to continue iterating and should return false to
//! abort iterating. Return false if the callback aborted the iterating. Return true
//! otherwise.
//!
bool Process::apply(cb0_t /*cb*/, void* /*arg*/) const
{
    // TODO
    bool ok = false;
    return ok;
}


//!
//! Apply given callback to the first maxPids running processes found.
//! The callback should return true to continue iterating and should
//! return false to abort iterating. Return false if the callback aborted
//! the iterating. Return true otherwise.
//!
bool Process::apply(unsigned int /*maxPids*/, cb1_t /*cb*/, void* /*arg*/)
{
    // TODO
    bool ok = false;
    return ok;
}


//!
//! Return true if given process identifier identifies an active process.
//!
bool Process::idIsActive(size_t id)
{

    // Form pathname to process.
    char pathname[6 + 20 + 1] = "/proc/"; //pid=0..18446744073709551615
    sprintf(pathname + 6, SIZE_T_SPEC, id);

    struct stat buf;
    return ((stat(pathname, &buf) == 0) && (buf.st_mode & S_IFDIR));
}


//!
//! Set the caller's process affinity mask.
//! Return true if successful.
//!
bool Process::setAffinityMask(size_t affinityMask)
{
#if __CYGWIN__
    bool ok = true;

#else
    cpu_set_t procMask = {{0}};
    for (size_t cpu = 0, mask = 1; cpu < MAX_CPUS; ++cpu, mask <<= 1)
    {
        if ((affinityMask & mask) != 0)
        {
            CPU_SET(cpu, &procMask);
        }
    }

    bool ok = (sched_setaffinity(0 /*pid*/, sizeof(procMask), &procMask) >= 0);
#endif

    return ok;
}


//!
//! Return the caller's process affinity mask.
//!
size_t Process::affinityMask()
{
#if __CYGWIN__
    size_t mask = 1;

#else
    size_t mask = 0;
    cpu_set_t procMask = {{0}};
    if (sched_getaffinity(0 /*pid*/, sizeof(procMask), &procMask) >= 0)
    {
        for (size_t cpu = 0; cpu < MAX_CPUS; ++cpu)
        {
            if (CPU_ISSET(cpu, &procMask))
            {
                mask |= (1UL << cpu);
            }
        }
    }
#endif

    return mask;
}


//!
//! Return the process identifier of the caller.
//!
size_t Process::myId()
{
    size_t id = getpid();
    return id;
}


//!
//! Return associated user name.
//! The returned string is to be freed by caller using the delete[] operator when done.
//!
wchar_t* Process::user() const
{
    char buf[31 + 1];
    buf[0] = 0;
    getlogin_r(buf, sizeof(buf));

    Utf8Seq seq(buf, strlen(buf));
    utf32_t* w = seq.expand();
    return reinterpret_cast<wchar_t*>(w);
}

END_NAMESPACE1
