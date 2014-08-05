/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/CpuWatch.hpp"
#include "syskit/TickTime.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//!
//! Construct a CPU watch. Attach it to the calling process if
//! <i>useThreadTime</i> is false or the calling thread if
//! <i>useThreadTime</i> is true. A watch attached to the
//! calling process will collect CPU time used by the calling
//! process, and a watch attached to the calling thread will
//! collect CPU time used by the calling thread. Obtaining CPU
//! time used by the calling thread is a Windows-only capability.
//! On other platforms, the <i>useThreadTime</i> has no significance
//! and the watch would be attached to the calling process only.
//!
CpuWatch::CpuWatch(bool useThreadTime)
{
    reset(useThreadTime);
}


//!
//! Destruct the CPU watch.
//!
CpuWatch::~CpuWatch()
{
}


//!
//! Compute the CPU usage. This method is valid only after a sequence
//! of start()-stop() invocations. It returns zeroes if the watch was
//! not properly stopped. Upon return, <i>sysUsage</i> holds the
//! system CPU time as a percentage of the elapsed time, <i>usrUsage</i>
//! holds the user CPU time as a percentage of the elapsed time. The
//! elapsed time is the stop-start time difference.
//!
void CpuWatch::computeUsage(double& sysUsage, double& usrUsage) const
{
    if (delta_)
    {
        sysUsage = static_cast<double>(sysTime_);
        sysUsage /= static_cast<double>(delta_);
        sysUsage *= 100.0;
        usrUsage = static_cast<double>(usrTime_);
        usrUsage /= static_cast<double>(delta_);
        usrUsage *= 100.0;
    }
    else
    {
        sysUsage = 0.0;
        usrUsage = 0.0;
    }
}


//!
//! Reset the watch to its initial state. Then attach it to the
//! calling process if <i>useThreadTime</i> is false or the calling
//! thread if <i>useThreadTime</i> is true. A watch attached to the
//! calling process will collect CPU time used by the calling process,
//! and a watch attached to the calling thread will collect CPU time
//! used by the calling thread. Obtaining CPU time used by the calling
//! thread is a Windows-only capability. On other platforms, the
//! <i>useThreadTime</i> has no significance and the watch would
//! be attached to the calling process only.
//!
void CpuWatch::reset(bool useThreadTime)
{
    getCpuTime_ = useThreadTime? &CpuWatch::getThreadTime: &CpuWatch::getProcessTime;
    delta_ = 0;
    startTime_ = 0;
    stopTime_ = 0;
    sysTime_ = 0;
    sysTime0_ = 0;
    usrTime_ = 0;
    usrTime0_ = 0;
}


//!
//! Start the CPU watch. Reset and record the start time.
//! 
void CpuWatch::start()
{
    delta_ = 0;
    stopTime_ = 0;
    (this->*getCpuTime_)();
    startTime_ = TickTime::curTime();
    sysTime0_ = sysTime_;
    usrTime0_ = usrTime_;
}


//!
//! Stop the CPU watch. Record the stop time and compute
//! the stop-start time difference. Also collect the CPU
//! time used in the stop-start duration. This operation
//! does not reset the start time. A subsequent consecutive
//! stop would overwrite the earlier stop time.
//! 
void CpuWatch::stop()
{
    stopTime_ = TickTime::curTime();
    (this->*getCpuTime_)();
    sysTime_ -= sysTime0_;
    usrTime_ -= usrTime0_;
    cpuTimeToTicks(sysTime_, usrTime_);
    delta_ = stopTime_ - startTime_;
}

END_NAMESPACE1
