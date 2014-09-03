#include "syskit/BitVec32.hpp"

#include "syskit-ut-pch.h"
#include "BitVec32Suite.hpp"

using namespace syskit;


BitVec32Suite::BitVec32Suite()
{
    vec0_ = new BitVec32(1024, false);
    vec1_ = new BitVec32(1024, true);
    vec2_ = new BitVec32(1000, false);
    for (size_t i = 3; i < 1000; vec2_->set(i), i += 3);
    vec3_ = new BitVec32(1302, true);
    for (size_t i = 2; i < 1302; vec3_->clear(i), i += 2);
}


BitVec32Suite::~BitVec32Suite()
{
    delete vec3_;
    delete vec2_;
    delete vec1_;
    delete vec0_;
}


bool BitVec32Suite::cb0a(void* arg, size_t bit)
{
    BitVec32& vec = *static_cast<BitVec32*>(arg);
    vec.clear(bit);
    return true;
}


bool BitVec32Suite::cb0b(void* arg, size_t bit)
{
    BitVec32& vec = *static_cast<BitVec32*>(arg);
    vec.set(bit);
    return true;
}


bool BitVec32Suite::cb0c(void* /*arg*/, size_t /*bit*/)
{
    return false;
}


bool BitVec32Suite::cb0d(void* arg, size_t bit)
{
    BitVec32& vec = *static_cast<BitVec32*>(arg);
    vec.clear(bit);
    return (bit < (vec.maxBits() - 11));
}


bool BitVec32Suite::cb0e(void* arg, size_t bit)
{
    BitVec32& vec = *static_cast<BitVec32*>(arg);
    vec.clear(bit);
    return (bit >= 9);
}


void BitVec32Suite::cb1a(void* arg, size_t bit)
{
    BitVec32& vec = *static_cast<BitVec32*>(arg);
    vec.clear(bit);
}


void BitVec32Suite::cb1b(void* arg, size_t bit)
{
    BitVec32& vec = *static_cast<BitVec32*>(arg);
    vec.set(bit);
}


//
// Iterate clear bits. Use cb0_t.
//
void BitVec32Suite::testApply00()
{
    BitVec32 vec3(vec3_->maxBits(), true);
    BitVec32::Itor it(*vec3_, false);
    bool ok = (it.applyToClearBits(cb0a, &vec3, true /*loToHi*/) && (vec3 == *vec3_));
    CPPUNIT_ASSERT(ok);

    vec3.setAll();
    ok = (it.applyToClearBits(cb0a, &vec3, false /*loToHi*/) && (vec3 == *vec3_));
    CPPUNIT_ASSERT(ok);

    vec3.setAll();
    it.attach(*vec1_, true);
    ok = (it.applyToClearBits(cb0a, &vec3, true) && (vec3.firstClearBit() == BitVec32::INVALID_BIT));
    CPPUNIT_ASSERT(ok);
    ok = (it.applyToClearBits(cb0a, &vec3, false) && (vec3.firstClearBit() == BitVec32::INVALID_BIT));
    CPPUNIT_ASSERT(ok);

    it.attach(*vec0_, false);
    ok = (!it.applyToClearBits(cb0c, 0, true));
    CPPUNIT_ASSERT(ok);
    ok = (!it.applyToClearBits(cb0c, 0, false));
    CPPUNIT_ASSERT(ok);

    it.attach(*vec3_, false);
    vec3.setAll();
    ok = (!it.applyToClearBits(cb0d, &vec3, true));
    CPPUNIT_ASSERT(ok);
    vec3.setAll();
    ok = (!it.applyToClearBits(cb0e, &vec3, false));
    CPPUNIT_ASSERT(ok);
}


//
// Iterate clear bits. Use cb1_t.
//
void BitVec32Suite::testApply01()
{
    BitVec32 vec3(vec3_->maxBits(), true);
    BitVec32::Itor it(*vec3_, true);
    it.applyToClearBits(cb1a, &vec3, true /*loToHi*/);
    bool ok = (vec3 == *vec3_);
    CPPUNIT_ASSERT(ok);

    vec3.setAll();
    it.applyToClearBits(cb1a, &vec3, false /*loToHi*/);
    ok = (vec3 == *vec3_);
    CPPUNIT_ASSERT(ok);

    vec3.setAll();
    it.attach(*vec1_, false);
    it.applyToClearBits(cb1a, &vec3, true /*loToHi*/);
    it.applyToClearBits(cb1a, &vec3, false /*hiToLo*/);
    ok = (vec3.firstClearBit() == BitVec32::INVALID_BIT);
    CPPUNIT_ASSERT(ok);
}


//
// Iterate set bits. Use cb0_t.
//
void BitVec32Suite::testApply02()
{
    BitVec32 vec2(vec2_->maxBits(), false);
    BitVec32::Itor it(*vec2_, false);
    bool ok = (it.applyToSetBits(cb0b, &vec2, true /*loToHi*/) && (vec2 == *vec2_));
    CPPUNIT_ASSERT(ok);

    vec2.clearAll();
    ok = (it.applyToSetBits(cb0b, &vec2, false /*loToHi*/) && (vec2 == *vec2_));
    CPPUNIT_ASSERT(ok);

    vec2.clearAll();
    it.attach(*vec0_, true);
    ok = (it.applyToSetBits(cb0b, &vec2, true) && (vec2.firstSetBit() == BitVec32::INVALID_BIT));
    CPPUNIT_ASSERT(ok);
    ok = (it.applyToSetBits(cb0b, &vec2, false) && (vec2.firstSetBit() == BitVec32::INVALID_BIT));
    CPPUNIT_ASSERT(ok);

    it.attach(*vec1_, false);
    ok = (!it.applyToSetBits(cb0c, 0, false));
    CPPUNIT_ASSERT(ok);
    ok = (!it.applyToSetBits(cb0c, 0, true));
    CPPUNIT_ASSERT(ok);
}


//
// Iterate set bits. Use cb1_t.
//
void BitVec32Suite::testApply03()
{
    BitVec32 vec2(vec2_->maxBits(), false);
    BitVec32::Itor it(*vec2_, true);
    it.applyToSetBits(cb1b, &vec2, true /*loToHi*/);
    bool ok = (vec2 == *vec2_);
    CPPUNIT_ASSERT(ok);

    vec2.clearAll();
    it.applyToSetBits(cb1b, &vec2, false /*loToHi*/);
    ok = (vec2 == *vec2_);
    CPPUNIT_ASSERT(ok);

    vec2.clearAll();
    it.attach(*vec0_, false);
    it.applyToSetBits(cb1b, &vec2, true /*loToHi*/);
    it.applyToSetBits(cb1b, &vec2, false /*hiToLo*/);
    ok = (vec2.firstSetBit() == BitVec32::INVALID_BIT);
    CPPUNIT_ASSERT(ok);
}


//
// Clear a single bit.
//
void BitVec32Suite::testClear00()
{
    bool ok = (vec1_->clearBit(175) && (vec1_->countClearBits() == 1));
    CPPUNIT_ASSERT(ok);

    unsigned int maxBits = vec1_->maxBits();
    for (unsigned int i = 0; i < maxBits; ++i)
    {
        if (vec1_->isClear(i) != (i == 175))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = (!vec1_->clearBit(9999));
    CPPUNIT_ASSERT(ok);
    vec1_->setBit(175);
}


//
// Clear all bits.
//
void BitVec32Suite::testClear01()
{
    vec1_->clearAll();
    bool ok = ((*vec1_ == *vec0_) && vec1_->isClear() && (vec1_->countClearBits() == 1024));
    CPPUNIT_ASSERT(ok);

    unsigned int curBit = vec0_->firstClearBit();
    ok = (curBit == 0) && (vec0_->lastClearBit() == 1023 - curBit);
    CPPUNIT_ASSERT(ok);
    for (unsigned int i = 1; curBit < 1023; curBit = i++)
    {
        if ((vec0_->nextClearBit(curBit) != i) || (vec0_->prevClearBit(1023 - curBit) != 1023 - i))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (vec0_->nextClearBit(curBit) == BitVec32::INVALID_BIT) && (vec0_->prevClearBit(1023 - curBit) == BitVec32::INVALID_BIT);
    CPPUNIT_ASSERT(ok);

    vec1_->set(35);
    ok = (!vec1_->isClear());
    CPPUNIT_ASSERT(ok);

    vec1_->reset();
    ok = ((*vec1_ == *vec0_) && vec1_->isClear());
    CPPUNIT_ASSERT(ok);

    vec1_->setAll();
    ok = (!vec1_->isClear());
    CPPUNIT_ASSERT(ok);
}


//
// Clear a range of bits. Invalid bit range. No set bits to clear.
//
void BitVec32Suite::testClear02()
{

    // Invalid bit range.
    bool ok = (!vec1_->clearBits(9999, 8888));
    CPPUNIT_ASSERT(ok);
    ok = (!vec1_->clearBits(8888, 9999));
    CPPUNIT_ASSERT(ok);

    // No set bits to clear.
    BitVec32 vec(vec0_->maxBits(), true);
    vec = *vec0_;
    ok = (!vec.clearBits(100, 200));
    CPPUNIT_ASSERT(ok);
}


//
// Clear a range of bits. Bit range partially valid.
//
void BitVec32Suite::testClear03()
{
    bool ok = (vec1_->clearBits(111, 9999) && (vec1_->countSetBits() == 111));
    CPPUNIT_ASSERT(ok);

    unsigned int maxBits = vec1_->maxBits();
    for (unsigned int i = 0; i < maxBits; ++i)
    {
        if (vec1_->isClear(i) != (i >= 111))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    vec1_->setAll();
}


//
// Clear a range of bits. Valid bit range.
//
void BitVec32Suite::testClear04()
{
    bool ok = (vec1_->clearBits(100, 200) && (vec1_->countClearBits() == 101));
    CPPUNIT_ASSERT(ok);

    unsigned int maxBits = vec1_->maxBits();
    for (unsigned int i = 0; i < maxBits; ++i)
    {
        if (vec1_->isClear(i) != ((i >= 100) && (i <= 200)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    vec1_->setAll();
}


//
// Clear a range of bits. Valid bit range covers all bits.
//
void BitVec32Suite::testClear05()
{
    unsigned int maxBits = vec1_->maxBits();
    bool ok = (vec1_->clearBits(0, maxBits - 1) && (*vec1_ == *vec0_));
    CPPUNIT_ASSERT(ok);
    vec1_->setAll();

    BitVec32 vec3(*vec3_);
    maxBits = vec3.maxBits();
    ok = (vec3.clearBits(0, maxBits - 1) && (vec3.countClearBits() == vec3.maxBits()));
    CPPUNIT_ASSERT(ok);

    vec3 = *vec3_;
    vec3.clear(0, vec3.maxBits() - 1);
    ok = vec3.isClear();
    CPPUNIT_ASSERT(ok);

    vec3.setAll();
    vec3.clear(0, 63);
    ok = (vec3.countClearBits() == 64);
    CPPUNIT_ASSERT(ok);
}


void BitVec32Suite::testCount00()
{
    size_t mask = static_cast<size_t>(0) - 1;
    bool ok = BitVec32::countSetBits(mask) == (sizeof(mask) * 8);
    CPPUNIT_ASSERT(ok);
}


//
// Default constructor.
//
void BitVec32Suite::testCtor00()
{
    BitVec32 vec;
    bool ok = ((vec.countClearBits() == BitVec32::DefaultMaxBits) && (vec.countSetBits() == 0));
    CPPUNIT_ASSERT(ok);

    unsigned int maxBits = vec.maxBits();
    ok = (maxBits == BitVec32::DefaultMaxBits);
    CPPUNIT_ASSERT(ok);

    for (unsigned int i = 0; i < maxBits; ++i)
    {
        if ((!vec.isClear(i)) || vec.isSet(i))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


//
// Constructing w/ non-default capacity.
//
void BitVec32Suite::testCtor01()
{
    BitVec32 vec(11, false);
    bool ok = ((vec.countClearBits() == 11) && (vec.countSetBits() == 0));
    CPPUNIT_ASSERT(ok);

    unsigned int maxBits = vec.maxBits();
    ok = (maxBits == 11);
    CPPUNIT_ASSERT(ok);

    for (unsigned int i = 0; i < maxBits; ++i)
    {
        if ((!vec.isClear(i)) || vec.isSet(i))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


//
// Constructing w/ non-default capacity and non-default initial value.
//
void BitVec32Suite::testCtor02()
{
    BitVec32 vec0(222, true);
    bool ok = ((vec0.countClearBits() == 0) && (vec0.countSetBits() == 222));
    CPPUNIT_ASSERT(ok);

    unsigned int maxBits = vec0.maxBits();
    ok = (maxBits == 222);
    CPPUNIT_ASSERT(ok);

    for (unsigned int i = 0; i < maxBits; ++i)
    {
        if (vec0.isClear(i) || (!vec0.isSet(i)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    BitVec32 vec1(maxBits, vec0.raw(), vec0.byteSize());
    ok = (vec0 == vec1);
    CPPUNIT_ASSERT(ok);

    BitVec32 vec2(maxBits, vec0.raw(), 24);
    ok = (vec0 != vec2);
    CPPUNIT_ASSERT(ok);
    vec2.setBits(192, 221);
    ok = (vec0 == vec2);
    CPPUNIT_ASSERT(ok);
}


//
// Zero capacity.
//
void BitVec32Suite::testCtor03()
{
    BitVec32 vec(0, false);
    unsigned int byteSize = 123;
    bool ok = ((vec.raw() != 0) && (vec.raw(byteSize) != 0) && (byteSize == 0));
    CPPUNIT_ASSERT(ok);

    ok = ((vec.maxBits() == 0) && (vec.byteSize() == 0));
    CPPUNIT_ASSERT(ok);

    const BitVec32::word_t* p = vec.raw();
    vec = vec; //no-op
    ok = (vec.raw() == p);
    CPPUNIT_ASSERT(ok);
}


//
// Invert bits.
//
void BitVec32Suite::testInvert00()
{
    BitVec32 vec0(*vec0_);
    vec0.invert();
    bool ok = (vec0 == *vec1_);
    CPPUNIT_ASSERT(ok);

    BitVec32 vec1(*vec1_);
    vec1.invert();
    ok = (vec1 == *vec0_);
    CPPUNIT_ASSERT(ok);

    BitVec32 vec3(*vec3_);
    vec3.invert();
    vec3.invert();
    CPPUNIT_ASSERT(vec3 == *vec3_);
}


//
// Iterate an all-clear bit vector from lo to hi.
//
void BitVec32Suite::testItor00()
{
    BitVec32::Itor it(*vec0_, false);
    size_t bit = 123;
    size_t numBits = 0;
    size_t prevBit = -1;
    bool ok = true;
    while (it.nextClearBit(bit))
    {
        ++numBits;
        if (bit != (prevBit + 1))
        {
            ok = false;
            break;
        }
        prevBit = bit;
    }
    CPPUNIT_ASSERT(ok);

    ok = (numBits == vec0_->countClearBits());
    CPPUNIT_ASSERT(ok);

    it.reset();
    ok = (!it.nextSetBit(bit));
    CPPUNIT_ASSERT(ok);
}


//
// Iterate an all-clear bit vector from hi to lo.
//
void BitVec32Suite::testItor01()
{
    BitVec32::Itor it;
    bool ok = (it.vec() == 0);
    CPPUNIT_ASSERT(ok);

    it.attach(*vec0_, true);
    size_t bit = 123;
    size_t numBits = 0;
    size_t prevBit = vec0_->maxBits();
    ok = true;
    while (it.prevClearBit(bit))
    {
        ++numBits;
        if (bit != (prevBit - 1))
        {
            ok = false;
            break;
        }
        prevBit = bit;
    }
    CPPUNIT_ASSERT(ok);

    ok = (numBits == vec0_->countClearBits());
    CPPUNIT_ASSERT(ok);

    it.reset();
    ok = (!it.prevSetBit(bit));
    CPPUNIT_ASSERT(ok);
}


//
// Iterate an all-set bit vector from lo to hi.
//
void BitVec32Suite::testItor02()
{
    BitVec32::Itor it(*vec1_, true);
    size_t bit = 123;
    size_t numBits = 0;
    size_t prevBit = -1;
    bool ok = true;
    while (it.nextSetBit(bit))
    {
        ++numBits;
        if (bit != (prevBit + 1))
        {
            ok = false;
            break;
        }
        prevBit = bit;
    }
    CPPUNIT_ASSERT(ok);

    ok = (numBits == vec1_->countSetBits());
    CPPUNIT_ASSERT(ok);

    it.reset();
    ok = (!it.nextClearBit(bit));
    CPPUNIT_ASSERT(ok);
    it.detach();
}


//
// Iterate an all-set bit vector from hi to lo.
//
void BitVec32Suite::testItor03()
{
    BitVec32::Itor it(*vec1_, false);
    size_t bit = 123;
    size_t numBits = 0;
    size_t prevBit = vec1_->maxBits();
    bool ok = true;
    while (it.prevSetBit(bit))
    {
        ++numBits;
        if (bit != (prevBit - 1))
        {
            ok = false;
            break;
        }
        prevBit = bit;
    }
    CPPUNIT_ASSERT(ok);

    ok = (numBits == vec1_->countSetBits());
    CPPUNIT_ASSERT(ok);

    it.reset();
    CPPUNIT_ASSERT(!it.prevClearBit(bit));
}


//
// Iterate clear bits in a mixed bit vector from lo to hi.
//
void BitVec32Suite::testItor04()
{
    BitVec32 vec(*vec3_);
    unsigned int bit = vec.firstClearBit();
    bool ok = (bit == 2);
    CPPUNIT_ASSERT(ok);

    unsigned int maxBits = vec.maxBits();
    for (unsigned int i = bit + 2; i < maxBits; i += 2)
    {
        bit = vec.nextClearBit(bit);
        if (bit != i)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = (vec.nextClearBit(bit) == BitVec32::INVALID_BIT);
    CPPUNIT_ASSERT(ok);
}


//
// Iterate clear bits in a mixed bit vector from hi to lo.
//
void BitVec32Suite::testItor05()
{
    BitVec32 vec(*vec3_);
    size_t bit = vec.lastClearBit();
    bool ok = (bit == (vec.maxBits() - 2));
    CPPUNIT_ASSERT(ok);

    for (size_t i = bit - 2; i >= 2; i -= 2)
    {
        bit = vec.prevClearBit(bit);
        if (bit != i)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = (vec.prevClearBit(bit) == BitVec32::INVALID_BIT);
    CPPUNIT_ASSERT(ok);
}


//
// Iterate set bits in a mixed bit vector from lo to hi.
//
void BitVec32Suite::testItor06()
{
    BitVec32 vec(*vec2_);
    unsigned int bit = vec.firstSetBit();
    bool ok = (bit == 3);
    CPPUNIT_ASSERT(ok);

    unsigned int maxBits = vec.maxBits();
    for (unsigned int i = bit + 3; i < maxBits; i += 3)
    {
        bit = vec.nextSetBit(bit);
        if (bit != i)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = (vec.nextSetBit(bit) == BitVec32::INVALID_BIT);
    CPPUNIT_ASSERT(ok);
}


//
// Iterate set bits in a mixed bit vector from hi to lo.
//
void BitVec32Suite::testItor07()
{
    BitVec32 vec(*vec2_);
    size_t bit = vec.lastSetBit();
    bool ok = (bit == (vec.maxBits() - 1));
    CPPUNIT_ASSERT(ok);

    for (size_t i = bit - 3; i >= 3; i -= 3)
    {
        bit = vec.prevSetBit(bit);
        if (bit != i)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = (vec.prevSetBit(bit) == BitVec32::INVALID_BIT);
    CPPUNIT_ASSERT(ok);
}


//
// ANDing. Same size.
//
void BitVec32Suite::testOp00()
{
    BitVec32 vec0(*vec0_);
    vec0 &= vec0;
    bool ok = (vec0 == *vec0_);
    CPPUNIT_ASSERT(ok);
    vec0 &= *vec1_;
    ok = (vec0 == *vec0_);
    CPPUNIT_ASSERT(ok);

    BitVec32 vec1(*vec1_);
    vec1 &= *vec1_;
    ok = (vec1 == *vec1_);
    CPPUNIT_ASSERT(ok);
    vec1 &= *vec0_;
    ok = (vec1 == *vec0_);
    CPPUNIT_ASSERT(ok);

    ok = (vec1 != *vec3_);
    CPPUNIT_ASSERT(ok);
}


//
// ANDing. Different sizes.
//
void BitVec32Suite::testOp01()
{
    BitVec32 vec2(222, true);
    BitVec32 vec3(111, false);
    vec2 &= vec3;
    bool ok = true;
    for (size_t i = 0; i < 222; ++i)
    {
        if (vec2.isSet(i) != (i >= 111))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    vec2 = *vec1_;
    vec3 = *vec0_;
    vec3.set(23);
    BitVec32 vec3a(vec3);
    vec3 &= vec2;
    ok = (vec3 == vec3a);
    CPPUNIT_ASSERT(ok);
}


//
// ORing. Same size.
//
void BitVec32Suite::testOp02()
{
    BitVec32 vec0(*vec0_);
    vec0 |= vec0;
    bool ok = (vec0 == *vec0_);
    CPPUNIT_ASSERT(ok);
    vec0 |= *vec1_;
    ok = (vec0 == *vec1_);
    CPPUNIT_ASSERT(ok);

    BitVec32 vec1(*vec1_);
    vec1 |= *vec0_;
    ok = (vec1 == *vec1_);
    CPPUNIT_ASSERT(ok);
}


//
// ORing. Different sizes.
//
void BitVec32Suite::testOp03()
{
    BitVec32 vec2(111, false);
    BitVec32 vec3(11, true);
    vec2 |= vec3;
    bool ok = true;
    for (size_t i = 0; i < 111; ++i)
    {
        if (vec2.isSet(i) != (i < 11))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    vec2 = *vec1_;
    vec3 = *vec0_;
    vec3.set(3);
    BitVec32 vec3a(vec3);
    vec3a.setAll();
    vec3 |= vec2;
    ok = (vec3 == vec3a);
    CPPUNIT_ASSERT(ok);
}


//
// XORing. Same size.
//
void BitVec32Suite::testOp04()
{
    BitVec32 vec0(*vec0_);
    vec0 ^= vec0;
    bool ok = (vec0 == *vec0_);
    CPPUNIT_ASSERT(ok);
    vec0 ^= *vec1_;
    ok = (vec0 == *vec1_);
    CPPUNIT_ASSERT(ok);

    BitVec32 vec1(*vec1_);
    vec1 ^= *vec0_;
    ok = (vec1 == *vec1_);
    CPPUNIT_ASSERT(ok);
}


//
// XORing. Different sizes.
//
void BitVec32Suite::testOp05()
{
    BitVec32 vec2(512, false);
    BitVec32 vec3(432, true);
    vec2 ^= vec3;
    bool ok = true;
    for (size_t i = 0; i < 512; ++i)
    {
        if (vec2.isSet(i) != (i < 432))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    vec2 = *vec3_;
    vec3 = *vec1_;
    BitVec32 vec3a(vec3);
    vec3 ^= vec2;
    for (size_t i = 0; i < 432; ++i)
    {
        if (vec3[i] != (vec3a[i] ^ vec2[i]))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void BitVec32Suite::testResize00()
{

    // no shrinking allowed
    BitVec32 vec0(1024 /*maxBits*/, false /*initialVal*/);
    bool ok = ((!vec0.resize(10 /*maxBits*/, false /*initialVal*/)) && vec0.isClear());
    CPPUNIT_ASSERT(ok);

    // no-op
    ok = (vec0.resize(1024 /*maxBits*/, false /*initialVal*/) && vec0.isClear());
    CPPUNIT_ASSERT(ok);

    BitVec32 vec1a(23 /*maxBits*/, true /*initialVal*/);
    BitVec32 vec1b(5 /*maxBits*/, true /*initialVal*/);
    ok = (vec1b.resize(23 /*maxBits*/, true /*initialVal*/) && (vec1b == vec1a));
    CPPUNIT_ASSERT(ok);

    BitVec32 vec1c(50 /*maxBits*/, true /*initialVal*/);
    ok = (vec1b.resize(50 /*maxBits*/, true /*initialVal*/) && (vec1b == vec1c));
    CPPUNIT_ASSERT(ok);

    BitVec32 vec1d(64 /*maxBits*/, true /*initialVal*/);
    ok = (vec1b.resize(64 /*maxBits*/, true /*initialVal*/) && (vec1b == vec1d));
    CPPUNIT_ASSERT(ok);

    BitVec32 vec1e(255 /*maxBits*/, true /*initialVal*/);
    ok = (vec1b.resize(255 /*maxBits*/, true /*initialVal*/) && (vec1b == vec1e));
    CPPUNIT_ASSERT(ok);
    ok = vec1e.resize(1024 /*maxBits*/, false /*initialVal*/) &&
        vec1b.resize(1024 /*maxBits*/, false /*initialVal*/) &&
        (vec1b == vec1e) &&
        vec1e.isClear(1023);
    CPPUNIT_ASSERT(ok);
}


//
// Set a single bit.
//
void BitVec32Suite::testSet00()
{
    bool ok = (vec0_->setBit(3) && (vec0_->countSetBits() == 1));
    CPPUNIT_ASSERT(ok);

    unsigned int maxBits = vec0_->maxBits();
    for (unsigned int i = 0; i < maxBits; ++i)
    {
        if (vec0_->isSet(i) != (i == 3))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = (!vec0_->clearBit(999));
    CPPUNIT_ASSERT(ok);
    vec0_->clearBit(3);
}


//
// Set all bits.
//
void BitVec32Suite::testSet01()
{
    vec0_->setAll();
    bool ok = ((*vec0_ == *vec1_) && vec0_->isSet() && (vec0_->countSetBits() == 1024));
    CPPUNIT_ASSERT(ok);

    unsigned int curBit = vec0_->firstSetBit();
    ok = (curBit == 0) && (vec0_->lastSetBit() == 1023 - curBit);
    CPPUNIT_ASSERT(ok);
    for (unsigned int i = 1; curBit < 1023; curBit = i++)
    {
        if ((vec0_->nextSetBit(curBit) != i) || (vec0_->prevSetBit(1023 - curBit) != 1023 - i))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (vec0_->nextSetBit(curBit) == BitVec32::INVALID_BIT) && (vec0_->prevSetBit(1023 - curBit) == BitVec32::INVALID_BIT);
    CPPUNIT_ASSERT(ok);

    vec0_->clearAll();
    vec0_->set(1);
    ok = vec0_->isSet();
    CPPUNIT_ASSERT(ok);

    vec0_->clear(1);
    ok = (!vec0_->isSet());
    CPPUNIT_ASSERT(ok);
}


//
// Set a range of bits. Invalid bit range. No clear bits to set.
//
void BitVec32Suite::testSet02()
{

    // Invalid bit range.
    bool ok = (!vec0_->setBits(9999, 8888));
    CPPUNIT_ASSERT(ok);
    ok = (!vec0_->setBits(8888, 9999));
    CPPUNIT_ASSERT(ok);

    // No clear bits to set.
    BitVec32 vec(vec1_->maxBits(), false);
    vec = *vec1_;
    ok = (!vec.setBits(100, 200));
    CPPUNIT_ASSERT(ok);
}


//
// Set a range of bits. Bit range partially valid.
//
void BitVec32Suite::testSet03()
{
    bool ok = (vec0_->setBits(123, 9999) && (vec0_->countClearBits() == 123));
    CPPUNIT_ASSERT(ok);

    unsigned int maxBits = vec0_->maxBits();
    for (unsigned int i = 0; i < maxBits; ++i)
    {
        if (vec0_->isSet(i) != (i >= 123))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    vec0_->clearAll();
}


//
// Set a range of bits. Valid bit range.
//
void BitVec32Suite::testSet04()
{
    bool ok = (vec0_->setBits(10, 20) && (vec0_->countSetBits() == 11));
    CPPUNIT_ASSERT(ok);

    unsigned int maxBits = vec0_->maxBits();
    for (unsigned int i = 0; i < maxBits; ++i)
    {
        if (vec0_->isSet(i) != ((i >= 10) && (i <= 20)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    vec0_->clearAll();
}


//
// Set a range of bits. Valid bit range covers all bits.
//
void BitVec32Suite::testSet05()
{
    unsigned int maxBits = vec0_->maxBits();
    bool ok = (vec0_->setBits(0, maxBits - 1) && (*vec0_ == *vec1_));
    CPPUNIT_ASSERT(ok);
    vec0_->clearAll();

    BitVec32 vec2(*vec2_);
    maxBits = vec2.maxBits();
    ok = (vec2.setBits(0, maxBits - 1) && (vec2.countSetBits() == vec2.maxBits()));
    CPPUNIT_ASSERT(ok);

    vec2 = *vec2_;
    vec2.set(0, vec2.maxBits() - 1);
    ok = (vec2.countSetBits() == vec2.maxBits());
    CPPUNIT_ASSERT(ok);

    vec2.clearAll();
    vec2.set(0, 63);
    ok = (vec2.countSetBits() == 64);
    CPPUNIT_ASSERT(ok);
}
