/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_BIT_VEC64_X64_HPP
#define SYSKIT_BIT_VEC64_X64_HPP
#ifndef SYSKIT_BIT_VEC64_HPP
#error "unsupported usage"
#endif

BEGIN_NAMESPACE1(syskit)

//! Count and return the number of set bits in given mask.
inline unsigned int BitVec64::countSetBits(size_t mask)
{
    return doCountSetBits(static_cast<unsigned long long>(mask));
}

END_NAMESPACE1

#endif
