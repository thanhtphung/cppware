/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_BIT_VEC64_HPP
#define SYSKIT_BIT_VEC64_HPP

#include <string.h>
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! vector of bits
class BitVec64
    //!
    //! A class representing a vector of bits. Most operations are trivial (e.g.,
    //! clear(), set(), reset(), etc.). Implemented using an array of 64-bit words. Bit 0
    //! is the least significant bit of the first word, bit BitsPerWord-1 is the
    //! most significant bit of the first word, and bit BitsPerWord is the least
    //! significant bit of the second word, etc. As a convenience, BitVec is an
    //! alias of BitVec64 in x64 builds. Example:
    //!\code
    //! BitVec64 vec(256, false); //start with 256 clear bits
    //! vec.set(100);            //set bit 100
    //! vec.set(9, 88);          //set bits 9 through 88
    //! vec.clearAll();          //clear all bits
    //! vec.invert();            //invert all bits
    //!\endcode
    //!
{

public:
    typedef unsigned long long word_t;

    enum
    {
        BitsPerWord = 64,
        BytesPerWord = sizeof(word_t),
        DefaultMaxBits = 256
    };

    static const unsigned int INVALID_BIT;

    // Constructors and destructor.
    BitVec64(const BitVec64& vec);
    BitVec64(unsigned int maxBits = DefaultMaxBits, bool initialVal = false);
    BitVec64(unsigned int maxBits, const word_t* raw, size_t byteSize);
    ~BitVec64();

    // Operators.
    bool operator !=(const BitVec64& vec) const;
    bool operator ==(const BitVec64& vec) const;
    bool operator [](size_t bit) const;
    const BitVec64& operator &=(const BitVec64& vec);
    const BitVec64& operator =(const BitVec64& vec);
    const BitVec64& operator ^=(const BitVec64& vec);
    const BitVec64& operator |=(const BitVec64& vec);

    // Getters.
    bool isClear(size_t bit) const;
    bool isSet(size_t bit) const;
    const word_t* raw() const;
    const word_t* raw(unsigned int& byteSize) const;
    unsigned int byteSize() const;
    unsigned int maxBits() const;

    // Setters.
    bool clearBit(size_t bit);
    bool clearBits(size_t loBit, size_t hiBit);
    bool setBit(size_t bit);
    bool setBits(size_t loBit, size_t hiBit);
    void clear(size_t bit);
    void clear(size_t loBit, size_t hiBit);
    void clearAll();
    void invert();
    void reset();
    void set(size_t bit);
    void set(size_t loBit, size_t hiBit);
    void setAll();

    // Iterator support.
    unsigned int firstClearBit() const;
    unsigned int firstSetBit() const;
    unsigned int lastClearBit() const;
    unsigned int lastSetBit() const;
    unsigned int nextClearBit(size_t curBit) const;
    unsigned int nextSetBit(size_t curBit) const;
    unsigned int prevClearBit(size_t curBit) const;
    unsigned int prevSetBit(size_t curBit) const;

    // Utilities.
    bool isClear() const;
    bool isSet() const;
    bool resize(unsigned int maxBits, bool initialVal = false);
    unsigned int countClearBits() const;
    unsigned int countSetBits() const;

    static unsigned int countSetBits(size_t mask);


    //! bit vector iterator
    class Itor
        //!
        //! A class representing a BitVec64 iterator. It provides a scheme to
        //! traverse the bits in a BitVec64 instance. Example:
        //!\code
        //! BitVec64::Itor it(vec);
        //! it.applyToSetBits(cb); //apply cb() to each set bit in vec
        //!\endcode
        //!
    {

    public:
        typedef bool(*cb0_t)(void* arg, size_t bit);
        typedef void(*cb1_t)(void* arg, size_t bit);

        // Constructors and destructor.
        Itor();
        Itor(const BitVec64& vec, bool makeCopy = false);
        ~Itor();

        // Iterator support.
        bool applyToClearBits(cb0_t cb, void* arg = 0, bool loToHi = true) const;
        bool applyToSetBits(cb0_t cb, void* arg = 0, bool loToHi = true) const;
        bool nextClearBit(size_t& bit);
        bool nextSetBit(size_t& bit);
        bool prevClearBit(size_t& bit);
        bool prevSetBit(size_t& bit);
        void applyToClearBits(cb1_t cb, void* arg = 0, bool loToHi = true) const;
        void applyToSetBits(cb1_t cb, void* arg = 0, bool hiToLo = true) const;
        void reset();

        // Utilities.
        const BitVec64* vec() const;
        void attach(const BitVec64& vec, bool makeCopy = false);
        void detach();

    private:
        const BitVec64* vec_;
        size_t curBit_;
        bool copyMade_;

        Itor(const Itor&); //prohibit usage
        const Itor& operator =(const Itor&); //prohibit usage

        bool applyToClearBitsHiToLo(cb0_t, void*) const;
        bool applyToClearBitsLoToHi(cb0_t, void*) const;
        bool applyToSetBitsHiToLo(cb0_t, void*) const;
        bool applyToSetBitsLoToHi(cb0_t, void*) const;
        void applyToClearBitsHiToLo(cb1_t, void*) const;
        void applyToClearBitsLoToHi(cb1_t, void*) const;
        void applyToSetBitsHiToLo(cb1_t, void*) const;
        void applyToSetBitsLoToHi(cb1_t, void*) const;

    };

private:
    word_t* raw_;
    unsigned int maxBits_;
    unsigned int rawLength_;

    static unsigned int doCountSetBits(unsigned int);
    static unsigned int doCountSetBits(unsigned long long);
    static word_t clearUnusedBits(word_t, size_t);
    static word_t setUnusedBits(word_t, size_t);

};

// Given the last word of a raw bit vector, clear the unused bits
// and return the result. This raw bit vector has maxBits bits.
inline BitVec64::word_t BitVec64::clearUnusedBits(word_t word, size_t maxBits)
{
    return ((maxBits & 0x3fU) != 0)? (word & ((1ULL << (maxBits & 0x3fU)) - 1)): word;
}

// Given the last word of a raw bit vector, set the unused bits
// and return the result. This raw bit vector has maxBits bits.
inline BitVec64::word_t BitVec64::setUnusedBits(word_t word, size_t maxBits)
{
    return ((maxBits & 0x3fU) != 0)? (word | ~((1ULL << (maxBits & 0x3fU)) - 1)): word;
}

//! Return true if this vector equals given vector. That is, if both
//! have the same number of bits and if corresponding bits are equal.
inline bool BitVec64::operator ==(const BitVec64& vec) const
{
    return (maxBits_ == vec.maxBits_) && (memcmp(raw_, vec.raw_, rawLength_*sizeof(*raw_)) == 0);
}

//! Return true if this vector does not equal given vector.
inline bool BitVec64::operator !=(const BitVec64& vec) const
{
    return !(operator ==(vec));
}

//! Return true/false if given bit is set/clear. Don't do any error checking.
inline bool BitVec64::operator [](size_t bit) const
{
    size_t vecI = bit >> 6;
    size_t bitI = bit & 0x3fU;
    return ((raw_[vecI] & (1ULL << bitI)) != 0);
}

//! Return true if given bit is clear. Don't do any error checking.
inline bool BitVec64::isClear(size_t bit) const
{
    size_t vecI = bit >> 6;
    size_t bitI = bit & 0x3fU;
    return ((raw_[vecI] & (1ULL << bitI)) == 0);
}

//! Return true if given bit is set. Don't do any error checking.
inline bool BitVec64::isSet(size_t bit) const
{
    size_t vecI = bit >> 6;
    size_t bitI = bit & 0x3fU;
    return ((raw_[vecI] & (1ULL << bitI)) != 0);
}

//! Return the raw bit vector. The vector size in bytes can be obtained using
//! byteSize(). Bit 0 is the least significant bit of the first word,
//! bit BitsPerWord-1 is the most significant bit of the first word, and bit BitsPerWord
//! is the least significant bit of the second word, etc.
inline const BitVec64::word_t* BitVec64::raw() const
{
    return raw_;
}

//! Return the raw bit vector. Also return its size in bytes via byteSize.
//! Bit 0 is the least significant bit of the first word, bit BitsPerWord-1 is the
//! most significant bit of the first word, and bit BitsPerWord is the least
//! significant bit of the second word, etc.
inline const BitVec64::word_t* BitVec64::raw(unsigned int& byteSize) const
{
    byteSize = rawLength_ * sizeof(*raw_);
    return raw_;
}

//! Return the raw bit vector size in bytes.
inline unsigned int BitVec64::byteSize() const
{
    return rawLength_ * sizeof(*raw_);
}

//! Count and return the number of clear bits.
inline unsigned int BitVec64::countClearBits() const
{
    return maxBits_ - countSetBits();
}

//! Return the first clear bit. Return INVALID_BIT if all bits are set.
inline unsigned int BitVec64::firstClearBit() const
{
    return nextClearBit(INVALID_BIT);
}

//! Return the first set bit. Return INVALID_BIT if all bits are clear.
inline unsigned int BitVec64::firstSetBit() const
{
    return nextSetBit(INVALID_BIT);
}

//! Return the last clear bit. Return INVALID_BIT if all bits are set.
inline unsigned int BitVec64::lastClearBit() const
{
    return prevClearBit(maxBits_);
}

//! Return the last set bit. Return INVALID_BIT if all bits are clear.
inline unsigned int BitVec64::lastSetBit() const
{
    return prevSetBit(maxBits_);
}

//! Return the bit vector capacity.
inline unsigned int BitVec64::maxBits() const
{
    return maxBits_;
}

//! Clear given bit. Don't do any error checking.
inline void BitVec64::clear(size_t bit)
{
    size_t vecI = bit >> 6;
    word_t bitM = 1ULL << (bit & 0x3fU);
    raw_[vecI] &= ~bitM;
}

//! Clear all bits.
inline void BitVec64::clearAll()
{
    memset(raw_, 0, rawLength_ * sizeof(*raw_));
}

//! Clear all bits.
inline void BitVec64::reset()
{
    memset(raw_, 0, rawLength_ * sizeof(*raw_));
}

//! Set given bit. Don't do any error checking.
inline void BitVec64::set(size_t bit)
{
    size_t vecI = bit >> 6;
    word_t bitM = 1ULL << (bit & 0x3fU);
    raw_[vecI] |= bitM;
}

//! Return the attached bit vector. Return zero if unattached.
inline const BitVec64* BitVec64::Itor::vec() const
{
    return vec_;
}

//! Reset the iterator to its initial state. That is, nextClearBit()
//! will return the first clear bit and nextSetBit() will return the
//! first set bit, etc.
inline void BitVec64::Itor::reset()
{
    curBit_ = INVALID_BIT;
}

END_NAMESPACE1

#if __linux || __CYGWIN__ || __FREERTOS__
#include "syskit/linux/BitVec64-linux.hpp"

#elif _WIN32
#if _M_X64
#include "syskit/x64/BitVec64-x64.hpp"
#else
#include "syskit/win32/BitVec64-win32.hpp"
#endif

#else
#error "unsupported architecture"

#endif
#endif
