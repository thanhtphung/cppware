/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/Atomic64.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE


// Perform (*addend += value) atomically.
// Return *addend value before the addition.
unsigned long long addAndReturnOrig(void volatile* addend, unsigned long long value)
{
    union
    {
        unsigned long long u64;
        unsigned int u32[2];
    };
    u64 = value;
    unsigned int valueLo = u32[0];
    unsigned int valueHi = u32[1];

    __asm
    {
        mov edi, addend
        mov eax, [edi]
        mov edx, [edi + 4] //first read doesn't have to be atomic -- will try again if necessary
again:
        mov ebx, valueLo
        mov ecx, valueHi
        add ebx, eax
        adc ecx, edx
        lock cmpxchg8b qword ptr[edi]
        jne again
    }
    // 64-bit original is in edx:eax
}


// Perform (*addend += value) atomically.
// Return *addend value after the addition.
unsigned long long addAndReturnSum(void volatile* addend, unsigned long long value)
{
    union
    {
        unsigned long long u64;
        unsigned int u32[2];
    };
    u64 = value;
    unsigned int valueLo = u32[0];
    unsigned int valueHi = u32[1];

    __asm
    {
        mov edi, addend
        mov eax, [edi]
        mov edx, [edi + 4] //first read doesn't have to be atomic -- will try again if necessary
again:
        mov ebx, valueLo
        mov ecx, valueHi
        add ebx, eax
        adc ecx, edx
        lock cmpxchg8b qword ptr[edi]
        jne again
        mov eax, ebx
        mov edx, ecx
    }
    // 64-bit sum is in edx:eax
}

END_NAMESPACE


//!
//! Perform (--*addend) atomically.
//! Return *addend value after the decrement.
//!
long long __cdecl _InterlockedDecrement64(long long volatile* addend)
{
    unsigned long long sum = addAndReturnSum(addend, 0ULL - 1);
    return static_cast<long long>(sum);
}


//!
//! Perform (*target = value) atomically.
//! Return *target value before the assignment.
//!
long long __cdecl _InterlockedExchange64(long long volatile* target, long long value)
{
    union
    {
        unsigned long long u64;
        unsigned int u32[2];
    };
    u64 = value;
    unsigned int valueLo = u32[0];
    unsigned int valueHi = u32[1];

    __asm
    {
        mov edi, target
        mov eax, [edi]
        mov edx, [edi + 4] //first read doesn't have to be atomic -- will try again if necessary
again:
        mov ebx, valueLo
        mov ecx, valueHi
        lock cmpxchg8b qword ptr[edi]
        jne again
    }
    // 64-bit original is in edx:eax
}


//!
//! Perform (*addend += value) atomically.
//! Return *addend value before the addition.
//!
long long __cdecl _InterlockedExchangeAdd64(long long volatile* addend, long long value)
{
    unsigned long long orig = addAndReturnOrig(addend, value);
    return static_cast<long long>(orig);
}


//!
//! Perform (++*addend) atomically.
//! Return *addend value after the increment.
//!
long long __cdecl _InterlockedIncrement64(long long volatile* addend)
{
    unsigned long long sum = addAndReturnSum(addend, 1ULL);
    return static_cast<long long>(sum);
}
