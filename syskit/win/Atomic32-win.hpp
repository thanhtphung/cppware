/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_ATOMIC32_WIN_HPP
#define SYSKIT_ATOMIC32_WIN_HPP
#ifndef SYSKIT_ATOMIC32_HPP
#error "unsupported usage"
#endif

extern "C" long __cdecl _InterlockedCompareExchange(long volatile*, long, long);
extern "C" long __cdecl _InterlockedDecrement(long volatile*);
extern "C" long __cdecl _InterlockedExchange(long volatile*, long);
extern "C" long __cdecl _InterlockedExchangeAdd(long volatile*, long);
extern "C" long __cdecl _InterlockedIncrement(long volatile*);

#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_InterlockedExchangeAdd)
#pragma intrinsic(_InterlockedIncrement)

BEGIN_NAMESPACE1(syskit)

//! Increment value.
//! Return incremented value.
inline Atomic32::item_t Atomic32::operator ++()
{
    return _InterlockedIncrement(&n_);
}

//! Increment value.
//! Return value before incrementing.
inline Atomic32::item_t Atomic32::operator ++(int)
{
    return static_cast<item_t>(_InterlockedIncrement(&n_)) - 1;
}

//! Decrement value.
//! Return decremented value.
inline Atomic32::item_t Atomic32::operator --()
{
    return _InterlockedDecrement(&n_);
}

//! Decrement value.
//! Return value before decrementing.
inline Atomic32::item_t Atomic32::operator --(int)
{
    return static_cast<item_t>(_InterlockedDecrement(&n_)) + 1;
}

//! Increment value by given delta.
inline void Atomic32::operator +=(item_t delta)
{
    _InterlockedExchangeAdd(&n_, delta);
}

//! Decrement value by given delta.
inline void Atomic32::operator -=(item_t delta)
{
    _InterlockedExchangeAdd(&n_, -static_cast<long>(delta));
}

//! Reset instance with given value.
inline void Atomic32::operator =(item_t v)
{
    _InterlockedExchange(&n_, v);
}

//! Decrement value.
inline void Atomic32::decrement()
{
    _InterlockedDecrement(&n_);
}

//! Decrement value.
//! Return value before decrementing in old.
inline void Atomic32::decrement(item_t& old)
{
    old = static_cast<item_t>(_InterlockedDecrement(&n_)) + 1;
}

//! Decrement value by given delta.
inline void Atomic32::decrementBy(item_t delta)
{
    _InterlockedExchangeAdd(&n_, -static_cast<long>(delta));
}

//! Decrement value by given delta.
//! Return value before decrementing in old.
inline void Atomic32::decrementBy(item_t delta, item_t& old)
{
    old = _InterlockedExchangeAdd(&n_, -static_cast<long>(delta));
}

//! Increment value.
inline void Atomic32::increment()
{
    _InterlockedIncrement(&n_);
}

//! Increment value.
//! Return value before incrementing in old.
inline void Atomic32::increment(item_t& old)
{
    old = static_cast<item_t>(_InterlockedIncrement(&n_)) - 1;
}

//! Increment value by given delta.
inline void Atomic32::incrementBy(item_t delta)
{
    _InterlockedExchangeAdd(&n_, delta);
}

//! Increment value by given delta.
//! Return value before incrementing in old.
inline void Atomic32::incrementBy(item_t delta, item_t& old)
{
    old = _InterlockedExchangeAdd(&n_, delta);
}

//! Reset instance with given value.
inline void Atomic32::set(item_t v)
{
    _InterlockedExchange(&n_, v);
}

//! Reset instance with given value.
//! Return old value in old.
inline void Atomic32::set(item_t v, item_t& old)
{
    old = _InterlockedExchange(&n_, v);
}

//! Reset instance with given value if current value equals comperand.
inline void Atomic32::setIfEqual(item_t v, item_t comperand)
{
    _InterlockedCompareExchange(&n_, v, comperand);
}

//! Reset instance with given value if current value equals comperand.
//! Return old value in old.
inline void Atomic32::setIfEqual(item_t v, item_t comperand, item_t& old)
{
    old = _InterlockedCompareExchange(&n_, v, comperand);
}

END_NAMESPACE1

#endif
