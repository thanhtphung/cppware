/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_TICK_TIME_HPP
#define SYSKIT_TICK_TIME_HPP

#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(syskit)


//! time stamp in ticks
class TickTime
    //!
    //! A class representing a time stamp in ticks. These ticks are not synchronized across
    //! CPU boundaries. Tick frequency is also assumed constant. As a result, the class must
    //! be used with care when ticks need to be synchronized across CPU boundaries or when
    //! tick frequency varies due to CPU idling.
    //!
{

public:
    TickTime();
    TickTime(const TickTime& tickTime);
    TickTime(unsigned long long tickTime);

    operator unsigned long long() const;
    const TickTime& operator =(const TickTime& tickTime);

    double asMsecs() const;
    double asSecs() const;
    unsigned long long asU64() const;
    void reset();
    void reset(unsigned long long tickTime);

    static double msecsPerTick();
    static double secsPerTick();
    static unsigned long long curTime();
    static unsigned long long ticksPerMsec();
    static unsigned long long ticksPerSec();

private:
    unsigned long long tickTime_;

    static double msecsPerTick_;
    static double secsPerTick_;
    static unsigned long long ticksPerMsec_;

};

END_NAMESPACE1

#include "syskit/Cpu.hpp"

BEGIN_NAMESPACE1(syskit)

//! Construct instance using current time.
inline TickTime::TickTime()
{
    tickTime_ = rdtsc();
}

//! Construct a duplicate instance of the given time.
inline TickTime::TickTime(const TickTime& tickTime)
{
    tickTime_ = tickTime.tickTime_;
}

//! Construct instance using given time.
inline TickTime::TickTime(unsigned long long tickTime)
{
    tickTime_ = tickTime;
}

//! Return the raw time.
inline TickTime::operator unsigned long long() const
{
    return tickTime_;
}

inline const TickTime& TickTime::operator =(const TickTime& tickTime)
{
    tickTime_ = tickTime.tickTime_;
    return *this;
}

//! Convert to milliseconds.
//! Method should not be used during static data construction/destruction.
inline double TickTime::asMsecs() const
{
    return tickTime_ * msecsPerTick_;
}

//! Convert to seconds.
//! Method should not be used during static data construction/destruction.
inline double TickTime::asSecs() const
{
    return tickTime_ * secsPerTick_;
}

//! Return the tick resolution.
//! Method should not be used during static data construction/destruction.
inline double TickTime::msecsPerTick()
{
    return msecsPerTick_;
}

//! Return the tick resolution.
//! Method should not be used during static data construction/destruction.
inline double TickTime::secsPerTick()
{
    return secsPerTick_;
}

//! Return the raw time.
inline unsigned long long TickTime::asU64() const
{
    return tickTime_;
}

//! Return the current time.
inline unsigned long long TickTime::curTime()
{
    return rdtsc();
}

//! Return the tick resolution.
//! Method should not be used during static data construction/destruction.
inline unsigned long long TickTime::ticksPerMsec()
{
    return ticksPerMsec_;
}

//! Return the tick resolution.
//! Method should not be used during static data construction/destruction.
inline unsigned long long TickTime::ticksPerSec()
{
    return Cpu::myHertz();
}

//! Reset instance with current time.
inline void TickTime::reset()
{
    tickTime_ = rdtsc();
}

//! Reset instance with given time.
inline void TickTime::reset(unsigned long long tickTime)
{
    tickTime_ = tickTime;
}

END_NAMESPACE1

#endif
