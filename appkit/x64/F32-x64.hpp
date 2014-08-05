/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_F32_X64_HPP
#define APPKIT_F32_X64_HPP
#ifndef APPKIT_F32_HPP
#error "unsupported usage"
#endif

BEGIN_NAMESPACE1(appkit)

inline int F32::round() const
{
    float x = (v_ < 0.0f)? (v_ - 0.5f): (v_ + 0.5f);
    return static_cast<int>(x);
}

END_NAMESPACE1

#endif
