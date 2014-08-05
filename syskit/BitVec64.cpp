/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/BitVec64.hpp"
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(syskit)

const unsigned int BitVec64::INVALID_BIT = 0xffffffffU;


//!
//! Construct a duplicate instance of the given bit vector.
//!
BitVec64::BitVec64(const BitVec64& vec)
{
    maxBits_ = vec.maxBits_;
    rawLength_ = vec.rawLength_;
    raw_ = new word_t[rawLength_];
    memcpy(raw_, vec.raw_, rawLength_ * sizeof(*raw_));
}


//!
//! Construct a vector of maxBits bits. The bits are initially clear if
//! initialVal is false and are initially set otherwise.
//!
BitVec64::BitVec64(unsigned int maxBits, bool initialVal)
{
    maxBits_ = maxBits;
    rawLength_ = maxBits_? (((maxBits_ - 1) >> 6) + 1): (0);
    raw_ = new word_t[rawLength_];
    initialVal? setAll(): clearAll();
}


//!
//! Construct a vector of maxBits bits. Initialize the bits with given raw
//! data. Clear remaining bits if necessary.
//!
BitVec64::BitVec64(unsigned int maxBits, const word_t* raw, size_t byteSize)
{
    maxBits_ = maxBits;
    rawLength_ = maxBits_? (((maxBits_ - 1) >> 6) + 1): (0);
    raw_ = new word_t[rawLength_];
    size_t rawBytes = rawLength_ * sizeof(*raw_);
    if (byteSize >= rawBytes)
    {
        memcpy(raw_, raw, rawBytes);
    }
    else
    {
        memcpy(raw_, raw, byteSize);
        memset(reinterpret_cast<unsigned char*>(raw_)+byteSize, 0, rawBytes - byteSize);
    }
}


BitVec64::~BitVec64()
{
    delete[] raw_;
}


//!
//! Perform the bitwise AND operation.
//! Return reference to self.
//!
const BitVec64& BitVec64::operator &=(const BitVec64& vec)
{

    // If ANDing self or given vector has zero bits, make it a no-op.
    if ((this == &vec) || (vec.maxBits_ == 0))
    {
    }

    // Perform the bitwise AND operation, one word at a time.
    // Since unused bits are clear, don't worry if the last word
    // in either operand has more bits.
    else if (maxBits_ <= vec.maxBits_)
    {
        word_t* p0 = raw_;
        const word_t* p1 = vec.raw_;
        for (const word_t* pEnd = p0 + rawLength_; p0 < pEnd; ++p0, ++p1)
        {
            *p0 &= *p1;
        }
    }

    // If the first and resulting operand has more bits, make sure those
    // extra bits are not affected by the operation.
    else
    {
        word_t* p0 = raw_;
        const word_t* p1 = vec.raw_;
        for (const word_t* pEnd = p0 + vec.rawLength_ - 1; p0 < pEnd; ++p0, ++p1)
        {
            *p0 &= *p1;
        }
        *p0 &= setUnusedBits(*p1, vec.maxBits_);
    }

    // Return reference to self.
    return *this;
}


//!
//! Clear all bits, then copy the bit values from given vector.
//! Ignore extra bits in given vector, if any.
//!
const BitVec64& BitVec64::operator =(const BitVec64& vec)
{

    // Prevent self assignment.
    if (this != &vec)
    {
        if (maxBits_ == vec.maxBits_)
        {
            memcpy(raw_, vec.raw_, rawLength_ * sizeof(*raw_));
        }
        else if (maxBits_ < vec.maxBits_)
        {
            memcpy(raw_, vec.raw_, rawLength_ * sizeof(*raw_));
            if (maxBits_ & 0x3fU) //keep unused bits clear
            {
                word_t* p = raw_ + (maxBits_ >> 6);
                *p &= ((1ULL << (maxBits_ & 0x3fU)) - 1);
            }
        }
        else
        {
            memcpy(raw_, vec.raw_, vec.rawLength_ * sizeof(*raw_));
            memset(raw_ + vec.rawLength_, 0, (rawLength_ - vec.rawLength_)*sizeof(*raw_));
        }
    }

    // Return reference to self.
    return *this;
}


//!
//! Perform the bitwise XOR (exclusive or) operation.
//! Return reference to self.
//!
//: use no space after the operator keyword to make doxygen happy.
const BitVec64& BitVec64::operator^=(const BitVec64& vec)
{

    // XORing self is the same as clear all bits.
    if (this == &vec)
    {
        memset(raw_, 0, rawLength_ * sizeof(*raw_));
    }

    // Perform the bitwise XOR operation, one word at a time.
    else if (maxBits_ <= vec.maxBits_)
    {
        if (maxBits_ > 0)
        {
            word_t* p0 = raw_;
            const word_t* p1 = vec.raw_;
            for (const word_t* pEnd = p0 + rawLength_ - 1; p0 < pEnd; ++p0, ++p1)
            {
                *p0 ^= *p1;
            }
            *p0 ^= clearUnusedBits(*p1, maxBits_);
        }
    }

    // If the first and resulting operand has more bits, make
    // sure those extra bits are not affected by the operation.
    else if (vec.maxBits_ > 0)
    {
        word_t* p0 = raw_;
        const word_t* p1 = vec.raw_;
        for (const word_t* pEnd = p0 + vec.rawLength_ - 1; p0 < pEnd; ++p0, ++p1)
        {
            *p0 ^= *p1;
        }
        *p0 ^= clearUnusedBits(*p1, vec.maxBits_);
    }

    // Return reference to self.
    return *this;
}


//!
//! Perform the bitwise OR operation.
//! Return reference to self.
//!
const BitVec64& BitVec64::operator |=(const BitVec64& vec)
{

    // If ORing self or given vector has zero bits, make it a no-op.
    if ((this == &vec) || (vec.maxBits_ == 0))
    {
    }

    // Perform the bitwise OR operation, one word at a time.
    else if (maxBits_ <= vec.maxBits_)
    {
        if (maxBits_ > 0)
        {
            word_t* p0 = raw_;
            const word_t* p1 = vec.raw_;
            for (const word_t* pEnd = p0 + rawLength_ - 1; p0 < pEnd; ++p0, ++p1)
            {
                *p0 |= *p1;
            }
            *p0 |= clearUnusedBits(*p1, maxBits_);
        }
    }

    // If the first and resulting operand has more bits, make sure those
    // extra bits are not affected by the operation. Since unused bits are
    // clear, don't worry if the last word in second operand has
    // unused bits.
    else
    {
        word_t* p0 = raw_;
        const word_t* p1 = vec.raw_;
        for (const word_t* pEnd = p0 + vec.rawLength_; p0 < pEnd; ++p0, ++p1)
        {
            *p0 |= *p1;
        }
    }

    // Return reference to self.
    return *this;
}


//!
//! Clear given bit. Return true if given bit was modified by this operation.
//! Use clear(size_t) to avoid error checking.
//!
bool BitVec64::clearBit(size_t bit)
{

    bool modified = false;
    if (bit < maxBits_)
    {
        size_t vecI = bit >> 6;
        word_t bitM = 1ULL << (bit & 0x3fU);
        if (raw_[vecI] & bitM)
        {
            raw_[vecI] &= ~bitM;
            modified = true;
        }
    }

    return modified;
}


//!
//! Clear given bits. Return true if at least one bit was modified by this operation.
//! Use clear(size_t, size_t) to avoid error checking.
//!
bool BitVec64::clearBits(size_t loBit, size_t hiBit)
{

    // Invalid bit range?
    if (loBit > hiBit)
    {
        return false;
    }

    // Ineffective bit range?
    size_t numBits = (hiBit < maxBits_)? (hiBit - loBit + 1): ((loBit < maxBits_)? (maxBits_ - loBit): 0);
    if (numBits == 0)
    {
        return false;
    }

    // Clear given bits. One word at a time when possible.
    bool modified = false;
    word_t* p = raw_ + (loBit >> 6);
    word_t bitM = 1ULL << (loBit & 0x3fU);
    for (;;)
    {
        if (bitM == 1)
        {
            for (; numBits >= BitsPerWord; ++p, numBits -= BitsPerWord)
            {
                if (*p != 0)
                {
                    *p = 0;
                    modified = true;
                }
            }
            if (numBits == 0) break;
        }
        if (*p & bitM)
        {
            *p &= ~bitM;
            modified = true;
        }
        if (--numBits == 0) break;
        (bitM == 0x8000000000000000ULL)? (++p, bitM = 1): (bitM <<= 1);
    }

    // Return true if at least one bit was modified.
    return modified;
}


//!
//! Return true if all bits are clear.
//!
bool BitVec64::isClear() const
{
    bool answer = true;
    word_t* p = raw_;
    for (const word_t* pEnd = p + rawLength_; p < pEnd; ++p)
    {
        if (*p != 0)
        {
            answer = false;
            break;
        }
    }

    return answer;
}


//!
//! Return true if at least one bit is set.
//!
bool BitVec64::isSet() const
{
    bool answer = false;
    word_t* p = raw_;
    for (const word_t* pEnd = p + rawLength_; p < pEnd; ++p)
    {
        if (*p != 0)
        {
            answer = true;
            break;
        }
    }

    return answer;
}


//!
//! Resize vector. Given new capacity must not be less than the current capacity.
//! Return true if successful.
//!
bool BitVec64::resize(unsigned int maxBits, bool initialVal)
{

    // Shrinking not allowed.
    bool ok = true;
    unsigned int rawLength = ((maxBits - 1) >> 6) + 1;
    if (maxBits < maxBits_)
    {
        ok = false;
    }

    // No-op.
    else if (maxBits == maxBits_)
    {
    }

    // Existing raw bit vector is too small.
    // Need to allocate a new one and copy bit values over.
    else if (rawLength > rawLength_)
    {
        word_t* raw = new word_t[rawLength];
        memcpy(raw, raw_, rawLength_ * sizeof(*raw));
        if (initialVal)
        {
            raw[rawLength_ - 1] = setUnusedBits(raw[rawLength_ - 1], maxBits_);
            memset(raw + rawLength_, 0xff, (rawLength - rawLength_) * sizeof(*raw));
            raw[rawLength - 1] = clearUnusedBits(raw[rawLength - 1], maxBits);
        }
        else
        {
            memset(raw + rawLength_, 0, (rawLength - rawLength_) * sizeof(*raw));
        }
        delete[] raw_;
        maxBits_ = maxBits;
        rawLength_ = rawLength;
        raw_ = raw;
    }

    // Existing raw bit vector can handle a few more bits.
    else
    {
        if (initialVal)
        {
            raw_[rawLength_ - 1] = setUnusedBits(raw_[rawLength_ - 1], maxBits_);
            raw_[rawLength_ - 1] = clearUnusedBits(raw_[rawLength_ - 1], maxBits);
        }
        maxBits_ = maxBits;
    }

    return ok;
}


//!
//! Set given bit. Return true if given bit was modified by this operation.
//! Use set(size_t) to avoid error checking.
//!
bool BitVec64::setBit(size_t bit)
{
    bool modified = false;
    if (bit < maxBits_)
    {
        size_t vecI = bit >> 6;
        word_t bitM = 1ULL << (bit & 0x3fU);
        if ((raw_[vecI] & bitM) == 0)
        {
            raw_[vecI] |= bitM;
            modified = true;
        }
    }

    return modified;
}


//!
//! Set given bits. Return true if at least one bit was modified by this operation.
//! Use set(size_t, size_t) to avoid error checking.
//!
bool BitVec64::setBits(size_t loBit, size_t hiBit)
{

    // Invalid bit range?
    if (loBit > hiBit)
    {
        return false;
    }

    // Ineffective bit range?
    size_t numBits = (hiBit < maxBits_)? (hiBit - loBit + 1): ((loBit < maxBits_)? (maxBits_ - loBit): 0);
    if (numBits == 0)
    {
        return false;
    }

    // Set given bits. One word at a time when possible.
    bool modified = false;
    word_t* p = raw_ + (loBit >> 6);
    word_t bitM = 1ULL << (loBit & 0x3fU);
    for (;;)
    {
        if (bitM == 1)
        {
            for (; numBits >= BitsPerWord; ++p, numBits -= BitsPerWord)
            {
                if (*p != 0xffffffffffffffffULL)
                {
                    *p = 0xffffffffffffffffULL;
                    modified = true;
                }
            }
            if (numBits == 0) break;
        }
        if ((*p & bitM) == 0)
        {
            *p |= bitM;
            modified = true;
        }
        if (--numBits == 0) break;
        (bitM == 0x8000000000000000ULL)? (++p, bitM = 1): (bitM <<= 1);
    }

    // Return true if at least one bit was modified.
    return modified;
}


//!
//! Count and return the number of set bits.
//!
unsigned int BitVec64::countSetBits() const
{
    static bool s_usePopcnt = popcntIsSupported();

    unsigned int numSetBits = 0;
    if (s_usePopcnt)
    {
        const word_t* p = raw_;
        for (const word_t* pEnd = p + rawLength_; p < pEnd; ++p)
        {
            numSetBits += popcnt(*p);
        }
    }

    else
    {
        const unsigned char* p = reinterpret_cast<const unsigned char*>(raw_);
        for (const unsigned char* pEnd = p + rawLength_*sizeof(*raw_); p < pEnd; p += sizeof(*raw_))
        {
            numSetBits += BIT_COUNT[p[0]] + BIT_COUNT[p[1]] + BIT_COUNT[p[2]] + BIT_COUNT[p[3]];
            numSetBits += BIT_COUNT[p[4]] + BIT_COUNT[p[5]] + BIT_COUNT[p[6]] + BIT_COUNT[p[7]];
        }
    }

    return numSetBits;
}


unsigned int BitVec64::doCountSetBits(unsigned int mask)
{
    static bool s_usePopcnt = popcntIsSupported();

    unsigned int numSetBits;
    if (s_usePopcnt)
    {
        numSetBits = popcnt(mask);
    }
    else
    {
        const unsigned char* p = reinterpret_cast<const unsigned char*>(&mask);
        numSetBits = (BIT_COUNT[p[0]] + BIT_COUNT[p[1]] + BIT_COUNT[p[2]] + BIT_COUNT[p[3]]);
    }

    return numSetBits;
}


unsigned int BitVec64::doCountSetBits(unsigned long long mask)
{
    static bool s_usePopcnt = popcntIsSupported();

    unsigned int numSetBits;
    if (s_usePopcnt)
    {
        numSetBits = popcnt(mask);
    }
    else
    {
        const unsigned char* p = reinterpret_cast<const unsigned char*>(&mask);
        numSetBits = (BIT_COUNT[p[0]] + BIT_COUNT[p[1]] + BIT_COUNT[p[2]] + BIT_COUNT[p[3]]);
        numSetBits += (BIT_COUNT[p[4]] + BIT_COUNT[p[5]] + BIT_COUNT[p[6]] + BIT_COUNT[p[7]]);
    }

    return numSetBits;
}


//!
//! Given a current bit, return the next clear bit. Return INVALID_BIT if
//! the remaining bits are all set or if the given current bit is not a valid
//! bit number. As an exception, nextClearBit(INVALID_BIT) is equivalent to
//! firstClearBit().
//!
unsigned int BitVec64::nextClearBit(size_t curBit) const
{

    // All remaining bits are set.
    unsigned int curBit32 = static_cast<unsigned int>(curBit);
    if (++curBit32 >= maxBits_)
    {
        return INVALID_BIT;
    }

    // Continue with the inner loop at the next bit. Look for the next
    // clear bit in the current word. Return immediately if
    // found. This loop can potentially look at unused bits, so make
    // sure we check for that.
    curBit = curBit32;
    size_t vecI = curBit >> 6;
    size_t bitI = curBit & 0x3fU;
    const word_t* p = raw_ + vecI;
    if (bitI)
    {
        for (word_t mask = (1ULL << bitI); mask; mask <<= 1, ++curBit)
        {
            if ((*p & mask) == 0)
            {
                return (curBit < maxBits_)? static_cast<unsigned int>(curBit): INVALID_BIT;
            }
        }
        ++p;
    }

    // Look for the next clear bit in the remaining words.
    // Return immediately if found. This loop can potentially look
    // at unused bits, so make sure we check for that.
    for (const word_t* pEnd = raw_ + rawLength_; p < pEnd; ++p, curBit += BitsPerWord)
    {
        if (*p != 0xffffffffffffffffULL)
        {
            for (word_t mask = 0x0000000000000001ULL; mask; mask <<= 1, ++curBit)
            {
                if ((*p & mask) == 0)
                {
                    return (curBit < maxBits_)? static_cast<unsigned int>(curBit): INVALID_BIT;
                }
            }
        }
    }

    // All remaining bits are set.
    return INVALID_BIT;
}


//!
//! Given a current bit, return the next set bit. Return INVALID_BIT if
//! the remaining bits are all clear or if the given current bit is not
//! a valid bit number. As an exception, nextSetBit(INVALID_BIT) is
//! equivalent to firstSetBit().
//!
unsigned int BitVec64::nextSetBit(size_t curBit) const
{

    // All remaining bits are clear.
    unsigned int curBit32 = static_cast<unsigned int>(curBit);
    if (++curBit32 >= maxBits_)
    {
        return INVALID_BIT;
    }

    // Continue with the inner loop at the next bit. Look for the next
    // set bit in the current word. Return immediately if found.
    // This loop can potentially look at unused bits. However, unused
    // bits are clear, and this logic is intentional.
    curBit = curBit32;
    size_t vecI = curBit >> 6;
    size_t bitI = curBit & 0x3fU;
    const word_t* p = raw_ + vecI;
    if (bitI)
    {
        for (word_t mask = (1ULL << bitI); mask; mask <<= 1, ++curBit)
        {
            if (*p & mask)
            {
                return static_cast<unsigned int>(curBit);
            }
        }
        ++p;
    }

    // Look for the next set bit in the remaining words.
    // Return immediately if found. This loop can potentially look
    // at unused bits. However, unused bits are clear, and this
    // logic is intentional.
    for (const word_t* pEnd = raw_ + rawLength_; p < pEnd; ++p, curBit += BitsPerWord)
    {
        ulong32_t index;
        if (_BitScanForward64(&index, *p))
        {
            return static_cast<unsigned int>(curBit + index);
        }
    }

    // All remaining bits are clear.
    return INVALID_BIT;
}


//!
//! Given a current bit, return the previous clear bit. Return INVALID_BIT if
//! the remaining bits are all set or if the given current bit is not a valid
//! bit number. As an exception, prevClearBit(maxBits()) is equivalent to
//! lastClearBit().
//!
unsigned int BitVec64::prevClearBit(size_t curBit) const
{

    // All remaining bits are set.
    if (--curBit >= maxBits_)
    {
        return INVALID_BIT;
    }

    // Continue with the inner loop at the previous bit. Look
    // for the previous clear bit in the current word.
    // Return immediately if found.
    size_t vecI = curBit >> 6;
    size_t bitI = curBit & 0x3fU;
    const word_t* p = raw_ + vecI;
    if (bitI < BitsPerWord - 1)
    {
        for (word_t mask = (1ULL << bitI); mask; mask >>= 1, --curBit)
        {
            if ((*p & mask) == 0)
            {
                return static_cast<unsigned int>(curBit);
            }
        }
        --p;
    }

    // Look for the previous clear bit in the remaining words.
    // Return immediately if found.
    for (; p >= raw_; --p, curBit -= BitsPerWord)
    {
        if (*p != 0xffffffffffffffffULL)
        {
            for (word_t mask = 0x8000000000000000ULL; mask; mask >>= 1, --curBit)
            {
                if ((*p & mask) == 0)
                {
                    return static_cast<unsigned int>(curBit);
                }
            }
        }
    }

    // All remaining bits are set.
    return INVALID_BIT;
}


//!
//! Given a current bit, return the previous set bit. Return INVALID_BIT if
//! the remaining bits are all clear or if the given current bit is not a
//! valid bit number. As an exception, prevSetBit(maxBits()) is equivalent
//! to lastSetBit().
//!
unsigned int BitVec64::prevSetBit(size_t curBit) const
{

    // All remaining bits are clear.
    if (--curBit >= maxBits_)
    {
        return INVALID_BIT;
    }

    // Continue with the inner loop at the previous bit. Look
    // for the previous set bit in the current word.
    // Return immediately if found.
    size_t vecI = curBit >> 6;
    size_t bitI = curBit & 0x3fU;
    const word_t* p = raw_ + vecI;
    if (bitI < BitsPerWord - 1)
    {
        for (word_t mask = (1ULL << bitI); mask; mask >>= 1, --curBit)
        {
            if (*p & mask)
            {
                return static_cast<unsigned int>(curBit);
            }
        }
        --p;
    }

    // Look for the previous set bit in the remaining words.
    // Return immediately if found.
    for (; p >= raw_; --p, curBit -= BitsPerWord)
    {
        ulong32_t index;
        if (_BitScanReverse64(&index, *p))
        {
            return static_cast<unsigned int>(curBit + index - 63);
        }
    }

    // All remaining bits are clear.
    return INVALID_BIT;
}


//!
//! Clear given bits. Don't do any error checking.
//!
void BitVec64::clear(size_t loBit, size_t hiBit)
{
    size_t numBits = hiBit - loBit + 1;
    word_t* p = raw_ + (loBit >> 6);
    word_t bitM = 1ULL << (loBit & 0x3fU);
    for (;;)
    {
        if (bitM == 1)
        {
            for (; numBits >= BitsPerWord; ++p, numBits -= BitsPerWord)
            {
                *p = 0;
            }
            if (numBits == 0) break;
        }
        *p &= ~bitM;
        if (--numBits == 0) break;
        (bitM == 0x8000000000000000ULL)? (++p, bitM = 1): (bitM <<= 1);
    }
}


//!
//! Invert all bits.
//!
void BitVec64::invert()
{
    word_t* p = raw_;
    for (const word_t* pEnd = p + rawLength_; p < pEnd; ++p)
    {
        *p = ~*p;
    }

    if (maxBits_ & 0x3fU) //keep unused bits clear
    {
        word_t* p = raw_ + (maxBits_ >> 6);
        *p &= ((1ULL << (maxBits_ & 0x3fU)) - 1);
    }
}


//!
//! Set given bits. Don't do any error checking.
//!
void BitVec64::set(size_t loBit, size_t hiBit)
{
    size_t numBits = hiBit - loBit + 1;
    word_t* p = raw_ + (loBit >> 6);
    word_t bitM = 1ULL << (loBit & 0x3fU);
    for (;;)
    {
        if (bitM == 1)
        {
            for (; numBits >= BitsPerWord; ++p, numBits -= BitsPerWord)
            {
                *p = 0xffffffffffffffffULL;
            }
            if (numBits == 0) break;
        }
        *p |= bitM;
        if (--numBits == 0) break;
        (bitM == 0x8000000000000000ULL)? (++p, bitM = 1): (bitM <<= 1);
    }
}


//!
//! Set all bits.
//!
void BitVec64::setAll()
{
    memset(raw_, 0xff, rawLength_ * sizeof(*raw_));
    if (maxBits_ & 0x3fU) //keep unused bits clear
    {
        word_t* p = raw_ + (maxBits_ >> 6);
        *p &= ((1ULL << (maxBits_ & 0x3fU)) - 1);
    }
}


//!
//! Construct an unattached BitVec64 iterator.
//!
BitVec64::Itor::Itor()
{

    copyMade_ = false;
    curBit_ = INVALID_BIT;
    vec_ = 0;
}


//!
//! Construct a BitVec64 iterator. Attach iterator to given bit vector.
//! A deep copy of the given bit vector is made if makeCopy is true.
//! Normally, a deep copy should be made only if the given vector can
//! change during iterating.
//!
BitVec64::Itor::Itor(const BitVec64& vec, bool makeCopy)
{

    copyMade_ = false;
    vec_ = 0;

    // attach() is also responsible for resetting the iterator and
    // that's why curBit_ does not have to be initialized here.
    attach(vec, makeCopy);
}


BitVec64::Itor::~Itor()
{

    detach();
}


//!
//! Iterate the bit vector from low to high if loToHi is true.
//! Iterate the bit vector from high to low otherwise. Invoke
//! callback at each clear bit. The callback should return true to
//! continue iterating and should return false to abort iterating.
//! Return false if the callback aborted the iterating. Return
//! true otherwise. Ignore if iterator is unattached.
//!
bool BitVec64::Itor::applyToClearBits(cb0_t cb, void* arg, bool loToHi) const
{
    bool ok = true;
    if (vec_ != 0)
    {
        ok = loToHi? applyToClearBitsLoToHi(cb, arg): applyToClearBitsHiToLo(cb, arg);
    }

    return ok;
}


//
// Iterate the bit vector from high to low. Apply callback to
// each clear bit. Return true if the iterating was not aborted.
//
bool BitVec64::Itor::applyToClearBitsHiToLo(cb0_t cb, void* arg) const
{

    // Last word might have unused bits.
    const word_t* pEnd = vec_->raw_;
    const word_t* p = pEnd + vec_->rawLength_ - 1;
    size_t bit = (vec_->rawLength_ << 6) - 1;
    word_t lastWord = setUnusedBits(*p, vec_->maxBits_);
    if (lastWord != 0xffffffffffffffffULL)
    {
        for (word_t mask = 0x8000000000000000ULL; mask; mask >>= 1, --bit)
        {
            if ((lastWord & mask) == 0)
            {
                if (!cb(arg, bit))
                {
                    return false;
                }
            }
        }
    }
    else
    {
        bit -= BitsPerWord;
    }

    // Look at one word at a time. Skip current word
    // if all current bits are set. Invoke callback at each clear bit.
    for (--p; p >= pEnd; --p)
    {
        if (*p != 0xffffffffffffffffULL)
        {
            for (word_t mask = 0x8000000000000000ULL; mask; mask >>= 1, --bit)
            {
                if ((*p & mask) == 0)
                {
                    if (!cb(arg, bit))
                    {
                        return false;
                    }
                }
            }
        }
        else
        {
            bit -= BitsPerWord;
        }
    }

    // Return true to indicate the iterating was not aborted.
    return true;
}


//
// Iterate the bit vector from low to high. Apply callback to
// each clear bit. Return true if the iterating was not aborted.
//
bool BitVec64::Itor::applyToClearBitsLoToHi(cb0_t cb, void* arg) const
{

    // Look at one word at a time. Skip current word
    // if all current bits are set. Invoke callback at each clear bit.
    const word_t* p = vec_->raw_;
    size_t bit = 0;
    for (const word_t* pEnd = p + vec_->rawLength_ - 1; p < pEnd; ++p)
    {
        if (*p != 0xffffffffffffffffULL)
        {
            for (word_t mask = 0x0000000000000001ULL; mask; mask <<= 1, ++bit)
            {
                if ((*p & mask) == 0)
                {
                    if (!cb(arg, bit))
                    {
                        return false;
                    }
                }
            }
        }
        else
        {
            bit += BitsPerWord;
        }
    }

    // Last word might have unused bits.
    word_t lastWord = setUnusedBits(*p, vec_->maxBits_);
    if (lastWord != 0xffffffffffffffffULL)
    {
        for (word_t mask = 0x0000000000000001ULL; mask; mask <<= 1, ++bit)
        {
            if ((lastWord & mask) == 0)
            {
                if (!cb(arg, bit))
                {
                    return false;
                }
            }
        }
    }

    // Return true to indicate the iterating was not aborted.
    return true;
}


//!
//! Iterate the bit vector from low to high if loToHi is true.
//! Iterate the bit vector from high to low otherwise. Invoke
//! callback at each set bit. The callback should return true to
//! continue iterating and should return false to abort iterating.
//! Return false if the callback aborted the iterating. Return
//! true otherwise. Ignore if iterator is unattached.
//!
bool BitVec64::Itor::applyToSetBits(cb0_t cb, void* arg, bool loToHi) const
{
    bool ok = true;
    if (vec_ != 0)
    {
        ok = loToHi? applyToSetBitsLoToHi(cb, arg): applyToSetBitsHiToLo(cb, arg);
    }

    return ok;
}


//
// Iterate the bit vector from high to low. Apply callback to
// each set bit. Return true if the iterating was not aborted.
//
bool BitVec64::Itor::applyToSetBitsHiToLo(cb0_t cb, void* arg) const
{

    // Look at one word at a time. Skip current word
    // if all current bits are clear. Invoke callback at each set bit.
    // The loops will be looking at unused bits. However, unused bits
    // are clear, and this logic is intentional.
    const word_t* pEnd = vec_->raw_;
    size_t bit = (vec_->rawLength_ << 6) - 1;
    for (const word_t* p = pEnd + vec_->rawLength_ - 1; p >= pEnd; --p)
    {
        if (*p)
        {
            for (word_t mask = 0x8000000000000000ULL; mask; mask >>= 1, --bit)
            {
                if (*p & mask)
                {
                    if (!cb(arg, bit))
                    {
                        return false;
                    }
                }
            }
        }
        else
        {
            bit -= BitsPerWord;
        }
    }

    // Return true to indicate the iterating was not aborted.
    return true;
}


//
// Iterate the bit vector from low to high. Apply callback to
// each set bit. Return true if the iterating was not aborted.
//
bool BitVec64::Itor::applyToSetBitsLoToHi(cb0_t cb, void* arg) const
{

    // Look at one word at a time. Skip current word
    // if all current bits are clear. Invoke callback at each set bit.
    // The loops will be looking at unused bits. However, unused bits
    // are clear, and this logic is intentional.
    const word_t* p = vec_->raw_;
    size_t bit = 0;
    for (const word_t* pEnd = p + vec_->rawLength_; p < pEnd; ++p)
    {
        if (*p)
        {
            for (word_t mask = 0x0000000000000001ULL; mask; mask <<= 1, ++bit)
            {
                if (*p & mask)
                {
                    if (!cb(arg, bit))
                    {
                        return false;
                    }
                }
            }
        }
        else
        {
            bit += BitsPerWord;
        }
    }

    // Return true to indicate the iterating was not aborted.
    return true;
}


//!
//! Determine the next clear bit. Return true if there's one. Return false
//! otherwise (if remaining bits are all set or if there's no more bits to
//! iterate). Behavior is unpredictable if the iterator is unattached. The
//! first invocation after construction or reset() will find the first clear
//! bit.
//!
bool BitVec64::Itor::nextClearBit(size_t& bit)
{
    bit = vec_->nextClearBit(curBit_);
    return (bit == INVALID_BIT)? (false): ((curBit_ = bit), true);
}


//!
//! Determine the next set bit. Return true if there's one. Return false
//! otherwise (if remaining bits are all clear or if there's no more bits to
//! iterate). Behavior is unpredictable if the iterator is unattached. The
//! first invocation after construction or reset() will find the first set
//! bit.
//!
bool BitVec64::Itor::nextSetBit(size_t& bit)
{
    bit = vec_->nextSetBit(curBit_);
    return (bit == INVALID_BIT)? (false): ((curBit_ = bit), true);
}


//!
//! Determine the previous clear bit. Return true if there's one. Return false
//! otherwise (if remaining bits are all set or if there's no more bits to
//! iterate). Behavior is unpredictable if the iterator is unattached. The
//! first invocation after construction or reset() will find the last clear
//! bit.
//!
bool BitVec64::Itor::prevClearBit(size_t& bit)
{
    bit = (curBit_ == INVALID_BIT)? vec_->lastClearBit(): vec_->prevClearBit(curBit_);
    return (bit == INVALID_BIT)? (false): ((curBit_ = bit), true);
}


//!
//! Determine the previous set bit. Return true if there's one. Return false
//! otherwise (if remaining bits are all clear or if there's no more bits to
//! iterate). Behavior is unpredictable if the iterator is unattached. The
//! first invocation after construction or reset() will find the last set
//! bit.
//!
bool BitVec64::Itor::prevSetBit(size_t& bit)
{
    bit = (curBit_ == INVALID_BIT)? vec_->lastSetBit(): vec_->prevSetBit(curBit_);
    return (bit == INVALID_BIT)? (false): ((curBit_ = bit), true);
}


//!
//! Iterate the bit vector from low to high if loToHi is true.
//! Iterate the bit vector from high to low otherwise. Invoke
//! callback at each clear bit. Ignore if iterator is unattached.
//!
void BitVec64::Itor::applyToClearBits(cb1_t cb, void* arg, bool loToHi) const
{
    if (vec_ != 0)
    {
        loToHi? applyToClearBitsLoToHi(cb, arg): applyToClearBitsHiToLo(cb, arg);
    }
}


//
// Iterate the bit vector from high to low.
// Apply callback to each clear bit.
//
void BitVec64::Itor::applyToClearBitsHiToLo(cb1_t cb, void* arg) const
{

    // Last word might have unused bits.
    const word_t* pEnd = vec_->raw_;
    const word_t* p = pEnd + vec_->rawLength_ - 1;
    size_t bit = (vec_->rawLength_ << 6) - 1;
    word_t lastWord = setUnusedBits(*p, vec_->maxBits_);
    if (lastWord != 0xffffffffffffffffULL)
    {
        for (word_t mask = 0x8000000000000000ULL; mask; mask >>= 1, --bit)
        {
            if ((lastWord & mask) == 0)
            {
                cb(arg, bit);
            }
        }
    }
    else
    {
        bit -= BitsPerWord;
    }

    // Look at one word at a time. Skip current word
    // if all current bits are set. Invoke callback at each clear bit.
    for (--p; p >= pEnd; --p)
    {
        if (*p != 0xffffffffffffffffULL)
        {
            for (word_t mask = 0x8000000000000000ULL; mask; mask >>= 1, --bit)
            {
                if ((*p & mask) == 0)
                {
                    cb(arg, bit);
                }
            }
        }
        else
        {
            bit -= BitsPerWord;
        }
    }
}


//
// Iterate the bit vector from low to high.
// Apply callback to each clear bit.
//
void BitVec64::Itor::applyToClearBitsLoToHi(cb1_t cb, void* arg) const
{

    // Look at one word at a time. Skip current word
    // if all current bits are set. Invoke callback at each clear bit.
    const word_t* p = vec_->raw_;
    size_t bit = 0;
    for (const word_t* pEnd = p + vec_->rawLength_ - 1; p < pEnd; ++p)
    {
        if (*p != 0xffffffffffffffffULL)
        {
            for (word_t mask = 0x0000000000000001ULL; mask; mask <<= 1, ++bit)
            {
                if ((*p & mask) == 0)
                {
                    cb(arg, bit);
                }
            }
        }
        else
        {
            bit += BitsPerWord;
        }
    }

    // Last word might have unused bits.
    word_t lastWord = setUnusedBits(*p, vec_->maxBits_);
    if (lastWord != 0xffffffffffffffffULL)
    {
        for (word_t mask = 0x0000000000000001ULL; mask; mask <<= 1, ++bit)
        {
            if ((lastWord & mask) == 0)
            {
                cb(arg, bit);
            }
        }
    }
}


//!
//! Iterate the bit vector from low to high if loToHi is true.
//! Iterate the bit vector from high to low otherwise. Invoke
//! callback at each set bit. Ignore if iterator is unattached.
//!
void BitVec64::Itor::applyToSetBits(cb1_t cb, void* arg, bool loToHi) const
{
    if (vec_ != 0)
    {
        loToHi? applyToSetBitsLoToHi(cb, arg): applyToSetBitsHiToLo(cb, arg);
    }
}


//
// Iterate the bit vector from high to low.
// Apply callback to each set bit.
//
void BitVec64::Itor::applyToSetBitsHiToLo(cb1_t cb, void* arg) const
{

    // Look at one word at a time. Skip current word
    // if all current bits are clear. Invoke callback at each set bit.
    // The loops will be looking at unused bits. However, unused bits
    // are clear, and this logic is intentional.
    const word_t* pEnd = vec_->raw_;
    const word_t* p = pEnd + vec_->rawLength_ - 1;
    size_t bit = (vec_->rawLength_ << 6) - 1;
    for (; p >= pEnd; --p)
    {
        if (*p)
        {
            for (word_t mask = 0x8000000000000000ULL; mask; mask >>= 1, --bit)
            {
                if (*p & mask)
                {
                    cb(arg, bit);
                }
            }
        }
        else
        {
            bit -= BitsPerWord;
        }
    }
}


//
// Iterate the bit vector from low to high.
// Apply callback to each set bit.
//
void BitVec64::Itor::applyToSetBitsLoToHi(cb1_t cb, void* arg) const
{

    // Look at one word at a time. Skip current word
    // if all current bits are clear. Invoke callback at each set bit.
    // The loops will be looking at unused bits. However, unused bits
    // are clear, and this logic is intentional.
    const word_t* p = vec_->raw_;
    size_t bit = 0;
    for (const word_t* pEnd = p + vec_->rawLength_; p < pEnd; ++p)
    {
        if (*p)
        {
            for (word_t mask = 0x0000000000000001ULL; mask; mask <<= 1, ++bit)
            {
                if (*p & mask)
                {
                    cb(arg, bit);
                }
            }
        }
        else
        {
            bit += BitsPerWord;
        }
    }
}


//!
//! Attach iterator to given bit vector. A deep copy of the given bit
//! vector is made if makeCopy is true. Normally, a deep copy should be
//! made only if the given vector can change during iterating.
//!
void BitVec64::Itor::attach(const BitVec64& vec, bool makeCopy)
{

    // Destroy old copy.
    if (copyMade_)
    {
        delete vec_;
    }

    // Make private copy of given bit vector if necessary.
    vec_ = makeCopy? new BitVec64(vec): &vec;
    copyMade_ = makeCopy;

    // Reset iterator.
    reset();
}


//!
//! Detach from any attached bit vector. Ignore if iterator is unattached.
//!
void BitVec64::Itor::detach()
{
    if (copyMade_)
    {
        delete vec_;
        copyMade_ = false;
    }

    vec_ = 0;
}

END_NAMESPACE1
