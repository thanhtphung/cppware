/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_ATOMIC64_X64_HPP
#define SYSKIT_ATOMIC64_X64_HPP
#ifndef SYSKIT_ATOMIC64_HPP
#error "unsupported usage"
#endif

BEGIN_NAMESPACE1(syskit)

//! Construct an instance with given initial value.
inline Atomic64::Atomic64(item_t v)
{
    v_ = v;
}

//! Return the unsigned long long value.
inline Atomic64::operator item_t() const
{
    return v_;
}

//! Return the unsigned long long value.
inline Atomic64::item_t Atomic64::asWord() const
{
    return v_;
}

END_NAMESPACE1

#endif
