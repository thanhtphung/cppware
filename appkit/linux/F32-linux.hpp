/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_F32_LINUX_HPP
#define APPKIT_F32_LINUX_HPP
#ifndef APPKIT_F32_HPP
#error "unsupported usage"
#endif

#include <math.h>

BEGIN_NAMESPACE1(appkit)

inline int F32::round() const
{
    int s32 = static_cast<int>(roundf(v_));
    return s32;
}

END_NAMESPACE1

#endif
