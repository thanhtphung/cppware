/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_CPU_WATCH_HPP
#define SYSKIT_CPU_WATCH_HPP

#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)

//! stop watch with CPU time
class CpuWatch
    //!
    //! A class representing a stop watch with CPU time. It holds the
    //! start time when the watch was started, the stop time when the
    //! watch was stopped, and the stop-start time difference. In addition,
    //! it holds the CPU time used by the calling process in the stop-start
    //! duration. The system CPU time is the time spent in kernel mode. The
    //! user CPU time is the time spent in user mode. The time stamps are
    //! 64-bit tick counts. The tick resolution (i.e., ticks per second)
    //! is platform dependent. Use TickTime to interpret the tick counts.
    //! On windows, CPU time used by the calling thread (instead of the
    //! calling process) can be obtained using the useThreadTime parameter
    //! during construction and/or reset. The parameter has no significance
    //! on other platforms.
    //!
{

public:
    CpuWatch(bool useThreadTime = false);
    ~CpuWatch();

    // Stop watch operations.
    void reset(bool useThreadTime = false);
    void start();
    void stop();

    // Collected data.
    unsigned long long delta() const;
    unsigned long long startTime() const;
    unsigned long long stopTime() const;
    unsigned long long sysTime() const;
    unsigned long long usrTime() const;
    void computeUsage(double& sysUsage, double& usrUsage) const;
    void getCpuTime(unsigned long long& sysTime, unsigned long long& usrTime) const;
    void getTimes(unsigned long long& startTime, unsigned long long& stopTime) const;

private:
    typedef void(CpuWatch::*getCpuTime_t)();

    getCpuTime_t getCpuTime_;
    unsigned long long delta_;
    unsigned long long startTime_;
    unsigned long long stopTime_;
    unsigned long long sysTime_;
    unsigned long long sysTime0_;
    unsigned long long usrTime_;
    unsigned long long usrTime0_;

    void cpuTimeToTicks(unsigned long long&, unsigned long long&) const;
    void getProcessTime();
    void getThreadTime();

};

//! Return the stop-start time difference. This method is valid only
//! after a sequence of start()-stop() invocations. It returns zero
//! if the watch was not properly stopped.
inline unsigned long long CpuWatch::delta() const
{
    return delta_;
}

//! Return the time stamp when the stop watch was last started.
//! Return zero if the watch has not been started.
inline unsigned long long CpuWatch::startTime() const
{
    return startTime_;
}

//! Return the time stamp when the stop watch was last stopped.
//! Return zero if the watch was not properly stopped.
inline unsigned long long CpuWatch::stopTime() const
{
    return stopTime_;
}

//! Return the collected system CPU time. This method is valid only
//! after a sequence of start()-stop() invocations. Behavior is
//! unpredictable if the watch was not properly stopped.
inline unsigned long long CpuWatch::sysTime() const
{
    return sysTime_;
}

//! Return the collected user CPU time. This method is valid only
//! after a sequence of start()-stop() invocations. Behavior is
//! unpredictable if the watch was not properly stopped.
inline unsigned long long CpuWatch::usrTime() const
{
    return usrTime_;
}

//! Return the collected CPU time. This method is valid only
//! after a sequence of start()-stop() invocations. Behavior is
//! unpredictable if the watch was not properly stopped.
inline void CpuWatch::getCpuTime(unsigned long long& sysTime, unsigned long long& usrTime) const
{
    sysTime = sysTime_;
    usrTime = usrTime_;
}

//! Return both the start and the stop times. The start time is
//! zero if the watch has not been started. The stop time is zero
//! zero if the watch was not properly stopped.
inline void CpuWatch::getTimes(unsigned long long& startTime, unsigned long long& stopTime) const
{
    startTime = startTime_;
    stopTime = stopTime_;
}

END_NAMESPACE1

#endif
