/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_ATOMIC64_WIN32_HPP
#define SYSKIT_ATOMIC64_WIN32_HPP
#ifndef SYSKIT_ATOMIC64_HPP
#error "unsupported usage"
#endif

BEGIN_NAMESPACE

// Atomically copy a 64-bit integer using FPU instructions.
inline void copy64(void* dst, unsigned long long src)
{
    __asm
    {
        mov edi, dst
        fild qword ptr[src]
        fistp qword ptr[edi]
    }
}

END_NAMESPACE

BEGIN_NAMESPACE1(syskit)

//! Construct an instance with given initial value.
inline Atomic64::Atomic64(item_t v)
{
    copy64(&v_, v);
}

//! Return the unsigned long long value.
inline Atomic64::operator item_t() const
{
    item_t v;
    copy64(&v, v_);
    return v;
}

//! Return the unsigned long long value.
inline Atomic64::item_t Atomic64::asWord() const
{
    item_t v;
    copy64(&v, v_);
    return v;
}

END_NAMESPACE1

#endif
