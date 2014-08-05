/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_F32_WIN32_HPP
#define APPKIT_F32_WIN32_HPP
#ifndef APPKIT_F32_HPP
#error "unsupported usage"
#endif

BEGIN_NAMESPACE

inline void roundF32(int* p, float f32)
{
    __asm
    {
        fld f32
        mov edx, p
        FRNDINT
        fistp dword ptr[edx]
    }
}

END_NAMESPACE

BEGIN_NAMESPACE1(appkit)

inline int F32::round() const
{
    int s32;
    roundF32(&s32, v_);
    return s32;
}

END_NAMESPACE1

#endif
