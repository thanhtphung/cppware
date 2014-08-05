/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/Utc.hpp"
#include "syskit/macros.h"

const unsigned int NSECS_PER_100NSEC = 100U;
const unsigned int NSECS_PER_SEC = 1000000000U;
const unsigned long long FROM_1601_TO_1970_IN_100NSECS = 0x019db1ded53e8000ULL; //134775 days = 280 common years + 89 leap years

BEGIN_NAMESPACE1(syskit)

const unsigned long long Utc::ZERO = FROM_1601_TO_1970_IN_100NSECS;


//!
//! Convert to and return windows filetime (100-nsecs since 1/1/1601).
//!
unsigned long long Utc::toFiletime() const
{
    utc_t u = {d64_};
    unsigned long long nsecsPerSec = NSECS_PER_SEC;
    unsigned long long t = u.u32[Secs] * nsecsPerSec + u.u32[Nsecs];
    t /= NSECS_PER_100NSEC;
    t += FROM_1601_TO_1970_IN_100NSECS;
    return t;
}


//!
//! Reset instance with given windows filetime (100-nsecs since 1/1/1601).
//!
void Utc::reset(unsigned long long filetime)
{
    utc_t u;
    filetime -= FROM_1601_TO_1970_IN_100NSECS;
    filetime *= NSECS_PER_100NSEC;
    u.u32[Secs] = static_cast<unsigned int>(filetime / NSECS_PER_SEC);
    u.u32[Nsecs] = static_cast<unsigned int>(filetime % NSECS_PER_SEC);
    d64_ = u.d64;
}

END_NAMESPACE1
