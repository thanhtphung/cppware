/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_UTC_HPP
#define SYSKIT_UTC_HPP

#include <sys/types.h>
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! coordinated universal time
class Utc
    //!
    //! A class representing a high-resolution current time. In its native form, time is
    //! nanoseconds since 1/1/1970. Conversion to and from windows filetime is supported.
    //! The windows filetime is 100-nanoseconds since 1/1/1601.
    //!
{

public:
    static const unsigned long long ZERO;

    Utc();
    Utc(const Utc& utc);
    Utc(double rawTime);
    Utc(unsigned int secs, unsigned int nsecs);
    Utc(unsigned long long filetime);

    bool operator !=(const Utc& utc) const;
    bool operator <(const Utc& utc) const;
    bool operator >(const Utc& utc) const;
    bool operator ==(const Utc& utc) const;
    const Utc& operator =(const Utc& utc);
    static void operator delete(void* p, size_t size);
    static void operator delete(void* p, void* buf);
    static void* operator new(size_t size);
    static void* operator new(size_t size, void* buf);

    double asSecs() const;
    double raw() const;
    unsigned int secs() const;
    unsigned int nsecs() const;
    unsigned long long asNsecs() const;
    unsigned long long toFiletime() const;
    void reset();
    void reset(unsigned int secs, unsigned int nsecs);
    void reset(unsigned long long filetime);

    static unsigned long long nowAsFiletime();

private:
    enum
    {
        Nsecs = 0,
        Secs
    };

    typedef union
    {
        double d64; //must be first to allow some code to initialize the union w/ a double
        unsigned int u32[2];
        unsigned long long u64;
    } utc_t;

    union
    {
        double d64_;
        unsigned int u32_[2];
        unsigned long long u64_;
    };

};

END_NAMESPACE1

#include "syskit/BufPool.hpp"

BEGIN_NAMESPACE1(syskit)

//! Construct instance using a high-resolution current time.
inline Utc::Utc()
{
    unsigned long long now64 = nowAsFiletime();
    reset(now64);
}

inline Utc::Utc(const Utc& utc)
{
    d64_ = utc.d64_;
}

//! Construct instance using given raw time.
//! Given raw time must have been obtained from raw().
inline Utc::Utc(double rawTime)
{
    d64_ = rawTime;
}

//! Construct instance using given time (since 1/1/1970). Don't do any error checking.
inline Utc::Utc(unsigned int secs, unsigned int nsecs)
{
    utc_t u;
    u.u32[Secs] = secs;
    u.u32[Nsecs] = nsecs;
    d64_ = u.d64;
}

//! Construct instance using given filetime (100-nsecs since 1/1/1601).
inline Utc::Utc(unsigned long long filetime)
{
    reset(filetime);
}

//! Return true if this time and that time are different.
inline bool Utc::operator !=(const Utc& utc) const
{
    bool ne = (u64_ != utc.u64_);
    return ne;
}

//! Return true if this time occurs before that time.
inline bool Utc::operator <(const Utc& utc) const
{
    utc_t lhs = {d64_};
    utc_t rhs = {utc.d64_};
    bool lt = (lhs.u32[Secs] < rhs.u32[Secs]) || ((lhs.u32[Secs] == rhs.u32[Secs]) && (lhs.u32[Nsecs] < rhs.u32[Nsecs]));
    return lt;
}

//! Return true if this time occurs after that time.
inline bool Utc::operator >(const Utc& utc) const
{
    utc_t lhs = {d64_};
    utc_t rhs = {utc.d64_};
    bool gt = (lhs.u32[Secs] > rhs.u32[Secs]) || ((lhs.u32[Secs] == rhs.u32[Secs]) && (lhs.u32[Nsecs] > rhs.u32[Nsecs]));
    return gt;
}

//! Return true if this time equals that time.
inline bool Utc::operator ==(const Utc& utc) const
{
    bool eq = (u64_ == utc.u64_);
    return eq;
}

inline const Utc& Utc::operator =(const Utc& utc)
{
    d64_ = utc.d64_;
    return *this;
}

inline void Utc::operator delete(void* p, size_t size)
{
    BufPool::freeBuf(p, size);
}

inline void Utc::operator delete(void* /*p*/, void* /*buf*/)
{
}

inline void* Utc::operator new(size_t size)
{
    void* buf = BufPool::allocateBuf(size);
    return buf;
}

inline void* Utc::operator new(size_t /*size*/, void* buf)
{
    return buf;
}

//! Return meaningless raw time.
//! It's meant to be used for constructing an identical Utc instance.
inline double Utc::raw() const
{
    return d64_;
}

//! Return time value as seconds since 1/1/1970.
inline double Utc::asSecs() const
{
    utc_t u = {d64_};
    const double SECS_PER_NSEC = 1e-9;
    double t = u.u32[Nsecs] * SECS_PER_NSEC + u.u32[Secs];
    return t;
}

//! Return the second part (seconds since 1/1/1970).
inline unsigned int Utc::secs() const
{
    return u32_[Secs];
}

//! Return the residual nanosecond part.
//! (secs() * nsecsPerSec + nsecs() == nanoseconds since 1/1/1970).
inline unsigned int Utc::nsecs() const
{
    return u32_[Nsecs];
}

//! Return time value as nanoseconds since 1/1/1970.
inline unsigned long long Utc::asNsecs() const
{
    utc_t u = {d64_};
    const unsigned long long NSECS_PER_SEC = 1000000000ULL;
    unsigned long long t = u.u32[Secs] * NSECS_PER_SEC + u.u32[Nsecs];
    return t;
}

//! Reset instance with a high-resolution current time.
inline void Utc::reset()
{
    unsigned long long now64 = nowAsFiletime();
    reset(now64);
}

//! Reset instance using given time (since 1/1/1970). Don't do any error checking.
inline void Utc::reset(unsigned int secs, unsigned int nsecs)
{
    utc_t u;
    u.u32[Secs] = secs;
    u.u32[Nsecs] = nsecs;
    d64_ = u.d64;
}

END_NAMESPACE1

#endif
