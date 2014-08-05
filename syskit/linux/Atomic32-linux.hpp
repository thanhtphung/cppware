/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_ATOMIC32_LINUX_HPP
#define SYSKIT_ATOMIC32_LINUX_HPP
#ifndef SYSKIT_ATOMIC32_HPP
#error "unsupported usage"
#endif

BEGIN_NAMESPACE1(syskit)

#if GCC_VERSION >= 40800

//! Increment value.
//! Return incremented value.
inline Atomic32::item_t Atomic32::operator ++()
{
    item_t delta = 1;
    return __atomic_add_fetch(&v_, delta, __ATOMIC_SEQ_CST);
}

//! Increment value.
//! Return value before incrementing.
inline Atomic32::item_t Atomic32::operator ++(int)
{
    item_t delta = 1;
    return __atomic_fetch_add(&v_, delta, __ATOMIC_SEQ_CST);
}

//! Decrement value.
//! Return decremented value.
inline Atomic32::item_t Atomic32::operator --()
{
    item_t delta = 1;
    return __atomic_sub_fetch(&v_, delta, __ATOMIC_SEQ_CST);
}

//! Decrement value.
//! Return value before decrementing.
inline Atomic32::item_t Atomic32::operator --(int)
{
    item_t delta = 1;
    return __atomic_fetch_sub(&v_, delta, __ATOMIC_SEQ_CST);
}

//! Increment value by given delta.
inline void Atomic32::operator +=(item_t delta)
{
    __atomic_add_fetch(&v_, delta, __ATOMIC_SEQ_CST);
}

//! Decrement value by given delta.
inline void Atomic32::operator -=(item_t delta)
{
    __atomic_sub_fetch(&v_, delta, __ATOMIC_SEQ_CST);
}

//! Reset instance with given value.
inline void Atomic32::operator =(item_t v)
{
    __atomic_store(&v_, &v, __ATOMIC_SEQ_CST);
}

//! Decrement value.
inline void Atomic32::decrement()
{
    item_t delta = 1;
    __atomic_sub_fetch(&v_, delta, __ATOMIC_SEQ_CST);
}

//! Decrement value.
//! Return value before decrementing in old.
inline void Atomic32::decrement(item_t& old)
{
    item_t delta = 1;
    old = __atomic_fetch_sub(&v_, delta, __ATOMIC_SEQ_CST);
}

//! Decrement value by given delta.
inline void Atomic32::decrementBy(item_t delta)
{
    __atomic_sub_fetch(&v_, delta, __ATOMIC_SEQ_CST);
}

//! Decrement value by given delta.
//! Return value before decrementing in old.
inline void Atomic32::decrementBy(item_t delta, item_t& old)
{
    old = __atomic_fetch_sub(&v_, delta, __ATOMIC_SEQ_CST);
}

//! Increment value.
inline void Atomic32::increment()
{
    item_t delta = 1;
    __atomic_add_fetch(&v_, delta, __ATOMIC_SEQ_CST);
}

//! Increment value.
//! Return value before incrementing in old.
inline void Atomic32::increment(item_t& old)
{
    item_t delta = 1;
    old = __atomic_fetch_add(&v_, delta, __ATOMIC_SEQ_CST);
}

//! Increment value by given delta.
inline void Atomic32::incrementBy(item_t delta)
{
    __atomic_add_fetch(&v_, delta, __ATOMIC_SEQ_CST);
}

//! Increment value by given delta.
//! Return value before incrementing in old.
inline void Atomic32::incrementBy(item_t delta, item_t& old)
{
    old = __atomic_fetch_add(&v_, delta, __ATOMIC_SEQ_CST);
}

//! Reset instance with given value.
inline void Atomic32::set(item_t v)
{
    __atomic_store(&v_, &v, __ATOMIC_SEQ_CST);
}

//! Reset instance with given value.
//! Return old value in old.
inline void Atomic32::set(item_t v, item_t& old)
{
    old = __atomic_exchange_n(&v_, &v, __ATOMIC_SEQ_CST);
}

//! Reset instance with given value if current value equals comperand.
inline void Atomic32::setIfEqual(item_t v, item_t comperand)
{
    bool weak = false;
    __atomic_compare_exchange_n(&v_, &comperand, v, weak, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

//! Reset instance with given value if current value equals comperand.
//! Return old value in old.
inline void Atomic32::setIfEqual(item_t v, item_t comperand, item_t& old)
{
    bool weak = false;
    __atomic_compare_exchange_n(&v_, &comperand, v, weak, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    old = comperand;
}

#else

//! Increment value.
//! Return incremented value.
inline Atomic32::item_t Atomic32::operator ++()
{
    item_t delta = 1;
    return __sync_add_and_fetch(&v_, delta);
}

//! Increment value.
//! Return value before incrementing.
inline Atomic32::item_t Atomic32::operator ++(int)
{
    item_t delta = 1;
    return __sync_fetch_and_add(&v_, delta);
}

//! Decrement value.
//! Return decremented value.
inline Atomic32::item_t Atomic32::operator --()
{
    item_t delta = 1;
    return __sync_sub_and_fetch(&v_, delta);
}

//! Decrement value.
//! Return value before decrementing.
inline Atomic32::item_t Atomic32::operator --(int)
{
    item_t delta = 1;
    return __sync_fetch_and_sub(&v_, delta);
}

//! Increment value by given delta.
inline void Atomic32::operator +=(item_t delta)
{
    __sync_add_and_fetch(&v_, delta);
}

//! Decrement value by given delta.
inline void Atomic32::operator -=(item_t delta)
{
    __sync_sub_and_fetch(&v_, delta);
}

//! Reset instance with given value.
inline void Atomic32::operator =(item_t v)
{
    __sync_lock_test_and_set(&v_, v);
}

//! Decrement value.
inline void Atomic32::decrement()
{
    item_t delta = 1;
    __sync_sub_and_fetch(&v_, delta);
}

//! Decrement value.
//! Return value before decrementing in old.
inline void Atomic32::decrement(item_t& old)
{
    item_t delta = 1;
    old = __sync_fetch_and_sub(&v_, delta);
}

//! Decrement value by given delta.
inline void Atomic32::decrementBy(item_t delta)
{
    __sync_sub_and_fetch(&v_, delta);
}

//! Decrement value by given delta.
//! Return value before decrementing in old.
inline void Atomic32::decrementBy(item_t delta, item_t& old)
{
    old = __sync_fetch_and_sub(&v_, delta);
}

//! Increment value.
inline void Atomic32::increment()
{
    item_t delta = 1;
    __sync_add_and_fetch(&v_, delta);
}

//! Increment value.
//! Return value before incrementing in old.
inline void Atomic32::increment(item_t& old)
{
    item_t delta = 1;
    old = __sync_fetch_and_add(&v_, delta);
}

//! Increment value by given delta.
inline void Atomic32::incrementBy(item_t delta)
{
    __sync_add_and_fetch(&v_, delta);
}

//! Increment value by given delta.
//! Return value before incrementing in old.
inline void Atomic32::incrementBy(item_t delta, item_t& old)
{
    old = __sync_fetch_and_add(&v_, delta);
}

//! Reset instance with given value.
inline void Atomic32::set(item_t v)
{
    __sync_lock_test_and_set(&v_, v);
}

//! Reset instance with given value.
//! Return old value in old.
inline void Atomic32::set(item_t v, item_t& old)
{
    old = __sync_lock_test_and_set(&v_, v);
}

//! Reset instance with given value if current value equals comperand.
inline void Atomic32::setIfEqual(item_t v, item_t comperand)
{
    __sync_val_compare_and_swap(&v_, comperand, v);
}

//! Reset instance with given value if current value equals comperand.
//! Return old value in old.
inline void Atomic32::setIfEqual(item_t v, item_t comperand, item_t& old)
{
    old = __sync_val_compare_and_swap(&v_, comperand, v);
}

#endif

END_NAMESPACE1

#endif
