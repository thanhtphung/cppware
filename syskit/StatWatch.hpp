/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_STAT_WATCH_HPP
#define SYSKIT_STAT_WATCH_HPP

#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! stop watch with stats tracking
class StatWatch
    //!
    //! A class representing a stop watch with stats tracking. A delta is
    //! formed for every start and stop time pair. Deltas are accumulated
    //! to provide the basic stats (min, max, and average) when requested.
    //! The time stamps are 64-bit tick counts. The tick resolution (i.e.,
    //! ticks per second) is platform dependent. Use TickTime to interpret
    //! the tick counts. If there's no need for the provided stats, use
    //! StopWatch. Example:
    //!\code
    //! StatWatch statWatch;
    //! for (;;)
    //! {
    //!   statWatch.start();
    //!   :
    //!   statWatch.stop();
    //! }
    //! size_t numSamples = statWatch.numDeltas();
    //! unsigned long long avgTicks = statWatch.computeAvgDelta();
    //! unsigned long long minTicks = statWatch.minDelta();
    //! unsigned long long maxTicks = statWatch.maxDelta();
    //!\endcode
    //!
{

public:
    StatWatch();

    // Stat watch operations.
    void mergeWith(const StatWatch& statWatch);
    void reset();
    void saveDelta(unsigned long long startTime);
    void saveDelta(unsigned long long startTime, unsigned long long stopTime);
    void start();
    void stop();
    void stopAndStart();

    // Collected data.
    unsigned int numDeltas() const;
    unsigned long long computeAvgDelta() const;
    unsigned long long computeAvgDelta(unsigned long long& minDelta, unsigned long long& maxDelta) const;
    unsigned long long deltaSum() const;
    unsigned long long delta() const;
    unsigned long long maxDelta() const;
    unsigned long long minDelta() const;
    unsigned long long startTime() const;
    unsigned long long stopTime() const;
    void getTimes(unsigned long long& startTime, unsigned long long& stopTime) const;

private:
    unsigned int numDeltas_;
    unsigned long long delta_;
    unsigned long long deltaSum_;
    unsigned long long maxDelta_;
    unsigned long long minDelta_;
    unsigned long long startTime_;
    unsigned long long stopTime_;

    void saveDelta();

};

END_NAMESPACE1

#include "syskit/TickTime.hpp"

BEGIN_NAMESPACE1(syskit)

//! Compute and return the average delta. This is the average
//! of all recorded deltas.
inline unsigned long long StatWatch::computeAvgDelta() const
{
    return numDeltas_? (deltaSum_ / numDeltas_): 0;
}

//! Compute and return the average delta. This is the average
//! of all recorded deltas. Also return the current minimum
//! and maximum deltas. The minimum delta is 0xffffffffffffffffULL
//! if no deltas have been recorded. The maximum delta is zero
//! if no deltas have been recorded.
inline unsigned long long StatWatch::computeAvgDelta(unsigned long long& minDelta, unsigned long long& maxDelta) const
{
    maxDelta = maxDelta_;
    minDelta = minDelta_;
    return numDeltas_? (deltaSum_ / numDeltas_): 0;
}

//! Return the number of recorded deltas.
inline unsigned int StatWatch::numDeltas() const
{
    return numDeltas_;
}

//! Return the most recent delta.
//! Return zero if no deltas has been recorded.
inline unsigned long long StatWatch::delta() const
{
    return delta_;
}

//! Return the sum of all recorded deltas.
inline unsigned long long StatWatch::deltaSum() const
{
    return deltaSum_;
}

//! Return the current maximum delta.
//! Return zero if no deltas has been recorded.
inline unsigned long long StatWatch::maxDelta() const
{
    return maxDelta_;
}

//! Return the current minimum delta.
//! Return 0xffffffffffffffffULL if no deltas has been recorded.
inline unsigned long long StatWatch::minDelta() const
{
    return minDelta_;
}

//! Return the most recent start time.
//! Return zero if no start times has been recorded.
inline unsigned long long StatWatch::startTime() const
{
    return startTime_;
}

//! Return the most recent stop time.
//! Return zero if no deltas has been recorded.
inline unsigned long long StatWatch::stopTime() const
{
    return stopTime_;
}

//! Return both the most recent start times and the most recent
//! stop times. The most recent start time is zero if no start
//! times has been recorded. The most recent stop time is zero
//! if no deltas has been recorded.
inline void StatWatch::getTimes(unsigned long long& startTime, unsigned long long& stopTime) const
{
    startTime = startTime_;
    stopTime = stopTime_;
}

//! Record given delta.
inline void StatWatch::saveDelta(unsigned long long startTime, unsigned long long stopTime)
{
    startTime_ = startTime;
    stopTime_ = stopTime;
    saveDelta();
}

//! Record current time as start time.
inline void StatWatch::start()
{
    startTime_ = TickTime::curTime();
}

//! Record current time as stop time. Compute the start-stop delta and
//! accumulate the result. This operation does not reset the start time.
//! A subsequent consecutive stop would result in a new delta using the
//! same start time. For example, a sequence of start-stop-start-stop
//! would result in two deltas, and a sequence of start-stop-stop would
//! also result in two deltas.
inline void StatWatch::stop()
{
    stopTime_ = TickTime::curTime();
    saveDelta();
}

END_NAMESPACE1

#endif
