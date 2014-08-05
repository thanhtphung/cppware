/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_ATOMIC64_WIN_HPP
#define SYSKIT_ATOMIC64_WIN_HPP
#ifndef SYSKIT_ATOMIC64_HPP
#error "unsupported usage"
#endif

extern "C" long long __cdecl _InterlockedCompareExchange64(long long volatile*, long long, long long);
extern "C" long long __cdecl _InterlockedDecrement64(long long volatile*);
extern "C" long long __cdecl _InterlockedExchange64(long long volatile*, long long);
extern "C" long long __cdecl _InterlockedExchangeAdd64(long long volatile*, long long);
extern "C" long long __cdecl _InterlockedIncrement64(long long volatile*);

#if _M_X64
#pragma intrinsic(_InterlockedCompareExchange64)
#pragma intrinsic(_InterlockedDecrement64)
#pragma intrinsic(_InterlockedExchange64)
#pragma intrinsic(_InterlockedExchangeAdd64)
#pragma intrinsic(_InterlockedIncrement64)
#endif

BEGIN_NAMESPACE1(syskit)

//! Increment value.
//! Return incremented value.
inline Atomic64::item_t Atomic64::operator ++()
{
    return _InterlockedIncrement64(&n_);
}

//! Increment value.
//! Return value before incrementing.
inline Atomic64::item_t Atomic64::operator ++(int)
{
    return static_cast<item_t>(_InterlockedIncrement64(&n_)) - 1;
}

//! Decrement value.
//! Return decremented value.
inline Atomic64::item_t Atomic64::operator --()
{
    return _InterlockedDecrement64(&n_);
}

//! Decrement value.
//! Return value before decrementing.
inline Atomic64::item_t Atomic64::operator --(int)
{
    return static_cast<item_t>(_InterlockedDecrement64(&n_)) + 1;
}

//! Increment value by given delta.
inline void Atomic64::operator +=(item_t delta)
{
    _InterlockedExchangeAdd64(&n_, delta);
}

//! Decrement value by given delta.
inline void Atomic64::operator -=(item_t delta)
{
    _InterlockedExchangeAdd64(&n_, -static_cast<long long>(delta));
}

//! Reset instance with given value.
inline void Atomic64::operator =(item_t v)
{
    _InterlockedExchange64(&n_, v);
}

//! Decrement value.
inline void Atomic64::decrement()
{
    _InterlockedDecrement64(&n_);
}

//! Decrement value.
//! Return value before decrementing in old.
inline void Atomic64::decrement(item_t& old)
{
    old = static_cast<item_t>(_InterlockedDecrement64(&n_)) + 1;
}

//! Decrement value by given delta.
inline void Atomic64::decrementBy(item_t delta)
{
    _InterlockedExchangeAdd64(&n_, -static_cast<long long>(delta));
}

//! Decrement value by given delta.
//! Return value before decrementing in old.
inline void Atomic64::decrementBy(item_t delta, item_t& old)
{
    old = _InterlockedExchangeAdd64(&n_, -static_cast<long long>(delta));
}

//! Increment value.
inline void Atomic64::increment()
{
    _InterlockedIncrement64(&n_);
}

//! Increment value.
//! Return value before incrementing in old.
inline void Atomic64::increment(item_t& old)
{
    old = static_cast<item_t>(_InterlockedIncrement64(&n_)) - 1;
}

//! Increment value by given delta.
inline void Atomic64::incrementBy(item_t delta)
{
    _InterlockedExchangeAdd64(&n_, delta);
}

//! Increment value by given delta.
//! Return value before incrementing in old.
inline void Atomic64::incrementBy(item_t delta, item_t& old)
{
    old = _InterlockedExchangeAdd64(&n_, delta);
}

//! Reset instance with given value.
inline void Atomic64::set(item_t v)
{
    _InterlockedExchange64(&n_, v);
}

//! Reset instance with given value.
//! Return old value in old.
inline void Atomic64::set(item_t v, item_t& old)
{
    old = _InterlockedExchange64(&n_, v);
}

//! Reset instance with given value if current value equals comperand.
inline void Atomic64::setIfEqual(item_t v, item_t comperand)
{
    _InterlockedCompareExchange64(&n_, v, comperand);
}

//! Reset instance with given value if current value equals comperand.
//! Return old value in old.
inline void Atomic64::setIfEqual(item_t v, item_t comperand, item_t& old)
{
    old = _InterlockedCompareExchange64(&n_, v, comperand);
}

END_NAMESPACE1

#endif
