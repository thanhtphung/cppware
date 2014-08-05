/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <sys/time.h>

#include "syskit/Utc.hpp"
#include "syskit/macros.h"

const unsigned int NSECS_PER_USEC = 1000;

BEGIN_NAMESPACE1(syskit)


//!
//! Return current time as windows filetime (100-nsecs since 1/1/1601).
//!
unsigned long long Utc::nowAsFiletime()
{
    struct timeval tv;
    struct timezone* tz = 0;
    gettimeofday(&tv, tz);
    Utc now(tv.tv_sec, tv.tv_usec * NSECS_PER_USEC);
    return now.toFiletime();
}

END_NAMESPACE1
