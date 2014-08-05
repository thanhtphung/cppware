/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/StatWatch.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


StatWatch::StatWatch()
{
    delta_ = 0;
    deltaSum_ = 0;
    maxDelta_ = 0;
    minDelta_ = 0xffffffffffffffffULL;
    numDeltas_ = 0;
    startTime_ = 0;
    stopTime_ = 0;
}


//!
//! Merge with given stat watch. This will become the resulting
//! stat watch. The resulting stat watch holds the combined stats.
//!
void StatWatch::mergeWith(const StatWatch& statWatch)
{
    if (statWatch.numDeltas_ > 0)
    {
        if (statWatch.maxDelta_ > maxDelta_)
        {
            maxDelta_ = statWatch.maxDelta_;
        }
        if (statWatch.minDelta_ < minDelta_)
        {
            minDelta_ = statWatch.minDelta_;
        }
        numDeltas_ += statWatch.numDeltas_;
        deltaSum_ += statWatch.deltaSum_;
    }
}


//!
//! Reset the stat watch to its initial state.
//!
void StatWatch::reset()
{
    delta_ = 0;
    maxDelta_ = 0;
    minDelta_ = 0xffffffffffffffffULL;
    numDeltas_ = 0;
    startTime_ = 0;
    stopTime_ = 0;
    deltaSum_ = 0;
}


//
// Compute and save the current delta.
// Adjust the watermarks if necessary.
//
void StatWatch::saveDelta()
{
    delta_ = stopTime_ - startTime_;
    if (delta_ < minDelta_)
    {
        minDelta_ = delta_;
    }
    if (delta_ > maxDelta_)
    {
        maxDelta_ = delta_;
    }

    ++numDeltas_;
    deltaSum_ += delta_;
}


//!
//! Record current time as stop time. Use given time as start time.
//!
void StatWatch::saveDelta(unsigned long long startTime)
{
    startTime_ = startTime;
    stop();
}


//!
//! Record current time as stop time for the current delta.
//! Record another current time after that as start time for
//! the next delta.
//!
void StatWatch::stopAndStart()
{
    stop();
    startTime_ = TickTime::curTime();
}

END_NAMESPACE1
