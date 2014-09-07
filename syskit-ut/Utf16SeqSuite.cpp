#include <string>
#include "appkit/U16.hpp"
#include "syskit/Utf16.hpp"
#include "syskit/Utf16Seq.hpp"
#include "syskit/Utf8Seq.hpp"

#include "syskit-ut-pch.h"
#include "Utf16SeqSuite.hpp"

using namespace appkit;
using namespace syskit;

const size_t BYTE_SIZE = 40;
const size_t NUM_CHARS = 16;
const size_t NUM_U16S = 20;

const utf32_t SAMPLE[NUM_CHARS] =
{
    0x00000000U, //2-byte value
    0x0000000aU, //2-byte value
    0x0000007fU, //2-byte value
    0x00000080U, //2-byte value
    0x000000abU, //2-byte value
    0x000007ffU, //2-byte value
    0x00000800U, //2-byte value
    0x0000abcdU, //2-byte value
    0x0000ffffU, //2-byte value
    0x00010000U, //4-byte value
    0x0000000aU, //2-byte value
    0x000abcdeU, //4-byte value
    0x0000abcdU, //2-byte value
    0x000000abU, //2-byte value
    0x0010ffffU, //4-byte value
    0x000abcdeU  //4-byte value
};


Utf16SeqSuite::Utf16SeqSuite()
{
    seq0_ = new Utf16Seq;
    seq0_->shrink(SAMPLE, NUM_CHARS);

    seq1_ = new Utf16Seq(3);
    seq1_->shrink(SAMPLE, 3);
}


Utf16SeqSuite::~Utf16SeqSuite()
{
    delete seq1_;
    delete seq0_;
}


bool Utf16SeqSuite::cb0a(void* arg, size_t index, utf32_t c)
{
    size_t& i = *static_cast<size_t *>(arg);
    --i;
    bool ok = (i == index) && (c == SAMPLE[index]);
    CPPUNIT_ASSERT(ok);

    return true;
}


bool Utf16SeqSuite::cb0b(void* arg, size_t index, utf32_t c)
{
    size_t& i = *static_cast<size_t *>(arg);
    --i;
    bool ok = (i == index) && (c == SAMPLE[index]);
    CPPUNIT_ASSERT(ok);

    return (i > 1);
}


bool Utf16SeqSuite::cb0c(void* arg, size_t index, utf32_t c)
{
    size_t& i = *static_cast<size_t *>(arg);
    bool ok = (i == index) && (c == SAMPLE[index]);
    CPPUNIT_ASSERT(ok);
    ++i;

    return true;
}


bool Utf16SeqSuite::cb0d(void* arg, size_t index, utf32_t c)
{
    size_t& i = *static_cast<size_t *>(arg);
    bool ok = (i == index) && (c == SAMPLE[index]);
    CPPUNIT_ASSERT(ok);
    ++i;

    return (i <= 1);
}


void Utf16SeqSuite::cb1a(void* arg, size_t index, utf32_t c)
{
    size_t& i = *static_cast<size_t *>(arg);
    --i;
    bool ok = (i == index) && (c == SAMPLE[index]);
    CPPUNIT_ASSERT(ok);
}


void Utf16SeqSuite::cb1b(void* arg, size_t index, utf32_t c)
{
    size_t& i = *static_cast<size_t *>(arg);
    bool ok = (i == index) && (c == SAMPLE[index]);
    CPPUNIT_ASSERT(ok);
    ++i;
}


//
// Apply callback to each character.
// Use cb0_t.
//
void Utf16SeqSuite::testApply00()
{
    size_t i = NUM_CHARS;
    bool ok = (seq0_->applyHiToLo(cb0a, &i) && (i == 0));
    CPPUNIT_ASSERT(ok);

    i = NUM_CHARS;
    ok = ((!seq0_->applyHiToLo(cb0b, &i)) && (i == 1));
    CPPUNIT_ASSERT(ok);

    i = 0;
    ok = (seq0_->applyLoToHi(cb0c, &i) && (i == NUM_CHARS));
    CPPUNIT_ASSERT(ok);

    i = 0;
    ok = ((!seq0_->applyLoToHi(cb0d, &i)) && (i == 2));
    CPPUNIT_ASSERT(ok);
}


void Utf16SeqSuite::testApply01()
{
    size_t i = seq1_->numChars();
    bool ok = (seq1_->applyHiToLo(cb0a, &i) && (i == 0));
    CPPUNIT_ASSERT(ok);

    i = seq1_->numChars();
    ok = ((!seq1_->applyHiToLo(cb0b, &i)) && (i == 1));
    CPPUNIT_ASSERT(ok);

    i = 0;
    ok = (seq1_->applyLoToHi(cb0c, &i) && (i == seq1_->numChars()));
    CPPUNIT_ASSERT(ok);

    i = 0;
    ok = ((!seq1_->applyLoToHi(cb0d, &i)) && (i == 2));
    CPPUNIT_ASSERT(ok);
}


//
// Apply callback to each character.
// Use cb1_t.
//
void Utf16SeqSuite::testApply02()
{
    size_t i = NUM_CHARS;
    seq0_->applyHiToLo(cb1a, &i);
    bool ok = (i == 0);
    CPPUNIT_ASSERT(ok);

    seq0_->applyLoToHi(cb1b, &i);
    ok = (i == NUM_CHARS);
    CPPUNIT_ASSERT(ok);
}


void Utf16SeqSuite::testApply03()
{
    size_t i = seq1_->numChars();
    seq1_->applyHiToLo(cb1a, &i);
    bool ok = (i == 0);
    CPPUNIT_ASSERT(ok);

    seq1_->applyLoToHi(cb1b, &i);
    ok = (i == seq1_->numChars());
    CPPUNIT_ASSERT(ok);
}


//
// Convert from UTF8.
// All valid 16-bit values.
// Growth required.
//
void Utf16SeqSuite::testConvert00()
{
    utf32_t* sample = new utf32_t[65536];
    for (utf32_t i = 0; i <= 0xffffU; ++i)
    {
        sample[i] = i;
    }
    for (utf32_t i = Utf16::MinReserved; i <= Utf16::MaxReserved; ++i)
    {
        sample[i] = 0xabcd;
    }
    Utf8Seq seq8(7);
    seq8.shrink(sample, 65536);

    Utf16Seq seq16(16384);
    bool ok = (seq16.convert8(seq8.raw(), seq8.byteSize()) == 0);
    CPPUNIT_ASSERT(ok);

    ok = ((seq16.capacity() == 65536) && (seq16.initialCap() == 16384));
    CPPUNIT_ASSERT(ok);

    ok = ((seq16.numChars() == 65536) && (seq16.numU16s() == 65536));
    CPPUNIT_ASSERT(ok);

    utf32_t* s = seq16.expand();
    ok = (memcmp(sample, s, 65536 * sizeof(*sample)) == 0);
    CPPUNIT_ASSERT(ok);
    delete[] s;

    ok = ((seq16[12] == 12) && (seq16[Utf16::MinReserved + 12] == 0xabcd));
    CPPUNIT_ASSERT(ok);

    seq16.reset(seq8.raw(), seq8.byteSize(), seq8.numChars());
    s = seq16.expand();
    ok = (memcmp(sample, s, 65536 * sizeof(*sample)) == 0);
    CPPUNIT_ASSERT(ok);
    delete[] s;
    delete[] sample;
}


//
// Convert from UTF8.
// Some valid codes.
// No growth.
//
void Utf16SeqSuite::testConvert01()
{
    utf32_t sample[9] =
    {
        0x0000000aU, //2-byte value
        0x000000abU, //2-byte value
        0x0000abcdU, //2-byte value
        0x000abcdeU, //4-byte value
        0x000abcdeU, //4-byte value
        0x0000abcdU, //2-byte value
        0x000000abU, //2-byte value
        0x0000000aU, //2-byte value
        0x00000000U  //2-byte value
    };
    Utf8Seq seq8;
    seq8.shrink(sample, 9);

    Utf16Seq seq16(11);
    bool ok = (seq16.convert8(seq8.raw(), seq8.byteSize()) == 0);
    CPPUNIT_ASSERT(ok);

    ok = ((seq16.capacity() == 11) && (seq16.initialCap() == 11));
    CPPUNIT_ASSERT(ok);

    ok = ((seq16.numChars() == 9) && (seq16.numU16s() == 11));
    CPPUNIT_ASSERT(ok);

    // This serves as reminder that wcslen() does not give character count.
    ok = (wcslen(reinterpret_cast<const wchar_t*>(seq16.raw())) == 10);
    CPPUNIT_ASSERT(ok);

    utf32_t* s = seq16.expand();
    ok = (memcmp(sample, s, sizeof(sample)) == 0);
    CPPUNIT_ASSERT(ok);
    delete[] s;

    for (unsigned int i = 0; i < 9; ++i)
    {
        if (seq16[i] != sample[i])
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


//
// Convert from UTF8.
// Some invalid codes.
//
void Utf16SeqSuite::testConvert02()
{
    const utf8_t someBadSeqs[][6] =
    {
        {0xfeU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU}, //invalid UTF8 byte
        {0xabU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU}, //need more than 1 byte
        {0xdfU, 0x00U, 0xffU, 0xffU, 0xffU, 0xffU}, //2nd byte malformed
        {0xc0U, 0x80U, 0xffU, 0xffU, 0xffU, 0xffU}, //does not need 2 bytes
        {0xc1U, 0xbfU, 0xffU, 0xffU, 0xffU, 0xffU}, //does not need 2 bytes
        {0xe1U, 0x40U, 0x81U, 0xffU, 0xffU, 0xffU}, //2nd byte malformed
        {0xe1U, 0x81U, 0xc0U, 0xffU, 0xffU, 0xffU}, //3rd byte malformed
        {0xe0U, 0x80U, 0x80U, 0xffU, 0xffU, 0xffU}, //does not need 3 bytes
        {0xe0U, 0x9fU, 0xbfU, 0xffU, 0xffU, 0xffU}, //does not need 3 bytes
        {0xf1U, 0x00U, 0x81U, 0x81U, 0xffU, 0xffU}, //2nd byte malformed
        {0xf1U, 0x81U, 0x40U, 0x81U, 0xffU, 0xffU}, //3rd byte malformed
        {0xf1U, 0x81U, 0x81U, 0xc0U, 0xffU, 0xffU}, //4th byte malformed
        {0xf0U, 0x80U, 0x80U, 0x80U, 0xffU, 0xffU}, //does not need 4 bytes
        {0xf0U, 0x8fU, 0xbfU, 0xbfU, 0xffU, 0xffU}, //does not need 4 bytes
        {0xf9U, 0x00U, 0x81U, 0x81U, 0x81U, 0xffU}, //2nd byte malformed
        {0xf9U, 0x81U, 0x40U, 0x81U, 0x81U, 0xffU}, //3rd byte malformed
        {0xf9U, 0x81U, 0x81U, 0xc0U, 0x81U, 0xffU}, //4th byte malformed
        {0xf9U, 0x81U, 0x81U, 0x81U, 0x00U, 0xffU}, //5th byte malformed
        {0xf8U, 0x80U, 0x80U, 0x80U, 0x80U, 0xffU}, //does not need 5 bytes
        {0xf8U, 0x87U, 0xbfU, 0xbfU, 0xbfU, 0xffU}, //does not need 5 bytes
        {0xfdU, 0x40U, 0x81U, 0x81U, 0x81U, 0x81U}, //2nd byte malformed
        {0xfdU, 0x81U, 0xc0U, 0x81U, 0x81U, 0x81U}, //3rd byte malformed
        {0xfdU, 0x81U, 0x81U, 0x00U, 0x81U, 0x81U}, //4th byte malformed
        {0xfdU, 0x81U, 0x81U, 0x81U, 0x40U, 0x81U}, //5th byte malformed
        {0xfdU, 0x81U, 0x81U, 0x81U, 0x81U, 0xc0U}, //6th byte malformed
        {0xfcU, 0x80U, 0x80U, 0x80U, 0x80U, 0x80U}, //does not need 6 bytes
        {0xfcU, 0x83U, 0xbfU, 0xbfU, 0xbfU, 0xbfU}  //does not need 6 bytes
    };
    size_t numBadSeqs = sizeof(someBadSeqs) / sizeof(*someBadSeqs);

    Utf16Seq seq16;
    utf8_t seq8[128] = {0};
    bool ok = true;
    for (unsigned int i = 0; i < numBadSeqs; ++i)
    {
        memcpy(seq8 + 13, someBadSeqs[i], 6);
        if (seq16.convert8(seq8, 128) == 0)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


//
// Convert from UTF32.
//
void Utf16SeqSuite::testConvert03()
{
    Utf16Seq seq;
    utf32_t defaultChar = '?';
    bool ok = ((seq.shrink(SAMPLE, NUM_CHARS, defaultChar) == 0) && (seq == *seq0_));
    CPPUNIT_ASSERT(ok);

    const utf32_t someInvalidSeq[5] = {0U, 1U, 0xabcddcbaU, 1U, 0U};
    ok = ((seq.shrink(someInvalidSeq, 5, defaultChar) == 1) && (seq.numChars() == 5));
    CPPUNIT_ASSERT(ok);
    ok = (seq[2] == defaultChar);
    CPPUNIT_ASSERT(ok);

    seq.reset();
    seq.resize(1);
    ok = ((seq.shrink(SAMPLE, NUM_CHARS, defaultChar) == 0) && (seq == *seq0_));
    CPPUNIT_ASSERT(ok);
}


//
// Convert from non-native-endian UTF16.
//
void Utf16SeqSuite::testConvert04()
{

    // 0xdc00U is not a valid leading value.
    utf16_t someInvalidSeq[9] = {0U, 1U, 2U, 0xdc00U, 2U, 1U, 0U, 0xd800U, 0xdc00};
    U16::bswap(someInvalidSeq, 9);
    Utf16Seq seq0(8);
    bool ok = (seq0.convert61(someInvalidSeq, 9) == 1);
    CPPUNIT_ASSERT(ok);

    ok = ((seq0.byteSize() == 18) && (seq0.numChars() == 8) && (seq0.numU16s() == 9));
    CPPUNIT_ASSERT(ok);

    Utf16Seq seq1(1);
    utf16_t* u61 = new utf16_t[seq0_->numU16s()];
    U16::bswap(seq0_->raw(), seq0_->numU16s(), u61);
    ok = ((seq1.convert61(u61, seq0_->numU16s()) == 0) && (seq1 == *seq0_));
    CPPUNIT_ASSERT(ok);
    delete[] u61;

    Utf16Seq seq2;
    ok = ((seq1.convert61(0, 0, 0) == 0) && (seq1 == seq2));
    CPPUNIT_ASSERT(ok);
}


void Utf16SeqSuite::testCountChars00()
{
    unsigned int numChars = 0x12345678U;
    bool ok = Utf16Seq::countChars(0, 0, numChars) && (numChars == 0);
    CPPUNIT_ASSERT(ok);

    ok = (Utf16Seq::countChars(seq0_->raw(), seq0_->numU16s(), numChars) && (numChars == NUM_CHARS));
    CPPUNIT_ASSERT(ok);

    const utf16_t someInvalidSeq[7] = {0U, 1U, 2U, 0xdc00U, 2U, 1U, 0U};
    ok = ((!Utf16Seq::countChars(someInvalidSeq, 7, numChars)) && (numChars == 3));
    CPPUNIT_ASSERT(ok);
}


//
// Default constructor.
//
void Utf16SeqSuite::testCtor00()
{
    Utf16Seq seq;
    bool ok = ((seq.capacity() == Utf16Seq::DefaultCap) && (seq.growthFactor() < 0));
    CPPUNIT_ASSERT(ok);

    ok = ((seq.byteSize() == 0) && (seq.numChars() == 0) && (seq.numU16s() == 0));
    CPPUNIT_ASSERT(ok);

    unsigned int numU16s = 123;
    ok = ((seq.raw() != 0) && (seq.raw(numU16s) != 0) && (numU16s == 0));
    CPPUNIT_ASSERT(ok);

    ok = ((!seq.setGrowth(100)) && seq.setGrowth(-1));
    CPPUNIT_ASSERT(ok);
}


//
// Copy constructor.
//
void Utf16SeqSuite::testCtor01()
{
    Utf16Seq seq0(*seq0_);
    bool ok = (seq0 == *seq0_);
    CPPUNIT_ASSERT(ok);

    UtfSeq* seq1 = seq0_->clone();
    utf32_t s[NUM_CHARS];
    seq1->expand(s);
    ok = (memcmp(s, SAMPLE, BYTE_SIZE) == 0);
    CPPUNIT_ASSERT(ok);

    delete seq1;
}


//
// Zero capacity.
//
void Utf16SeqSuite::testCtor02()
{
    unsigned int capacity = 0;
    Utf16Seq seq(capacity);
    bool ok = ((seq.capacity() == 1) && (seq.growthFactor() < 0));
    CPPUNIT_ASSERT(ok);

    ok = ((seq.byteSize() == 0) && (seq.numChars() == 0) && (seq.numU16s() == 0));
    CPPUNIT_ASSERT(ok);

    unsigned int numU16s = 123;
    ok = ((seq.raw() != 0) && (seq.raw(numU16s) != 0) && (numU16s == 0));
    CPPUNIT_ASSERT(ok);
}


void Utf16SeqSuite::testCtor03()
{
    Utf16Seq seqA(*seq0_);
    Utf16Seq seqB(&seqA);
    bool ok = (seqB == *seq0_);
    CPPUNIT_ASSERT(ok);

    Utf16Seq seqC(*seq1_);
    size_t numU16s = seqC.numU16s();
    size_t numChars = seqC.numChars();
    utf16_t* raw = seqC.detachRaw();
    Utf16Seq seqD(raw, numU16s, numChars);
    ok = (seqD == *seq1_);
    CPPUNIT_ASSERT(ok);
}


void Utf16SeqSuite::testIsValid00()
{

    // An empty raw sequence is a valid sequence.
    bool ok = Utf16Seq::isValid(0, 0, 0);
    CPPUNIT_ASSERT(ok);

    // A valid Utf16Seq instance should have a valid raw sequence.
    ok = Utf16Seq::isValid(seq0_->raw(), seq0_->numU16s(), 0);
    CPPUNIT_ASSERT(ok);

    // 0xdc00U is not a valid leading value.
    const utf16_t someInvalidSeq[7] = {0U, 1U, 2U, 0xdc00U, 2U, 1U, 0U};
    ok = (!Utf16Seq::isValid(someInvalidSeq, sizeof(someInvalidSeq), 0));
    CPPUNIT_ASSERT(ok);

    const utf16_t* badSeq = 0;
    ok = (!Utf16Seq::isValid(someInvalidSeq, 7, &badSeq)) && (badSeq == &someInvalidSeq[3]);
    CPPUNIT_ASSERT(ok);
}


//
// Iterate left to right.
//
void Utf16SeqSuite::testNext00()
{
    Utf16Seq::Itor it(*seq0_, false /*makeCopy*/);
    bool ok = (it.curOffset() == Utf16Seq::INVALID_INDEX);
    CPPUNIT_ASSERT(ok);

    Utf16 utf16;
    utf32_t c;
    size_t i;
    ok = true;
    size_t offset = 0;
    for (i = 0; it.next(c); ++i)
    {
        if ((c != SAMPLE[i]) || (it.curOffset() != offset))
        {
            ok = false;
            break;
        }
        utf16.resetWithValidChar(c);
        offset += utf16.getSeqLength();
    }
    CPPUNIT_ASSERT(ok);

    ok = ((i == NUM_CHARS) && (!it.next(c)));
    CPPUNIT_ASSERT(ok);

    utf16.resetWithValidChar(SAMPLE[NUM_CHARS - 1]);
    offset -= utf16.getSeqLength();
    ok = (it.curOffset() == offset);
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator.
// No growth.
//
void Utf16SeqSuite::testOp00()
{
    Utf16Seq seq;
    seq = *seq0_;
    bool ok = (seq == *seq0_);
    CPPUNIT_ASSERT(ok);

    ok = ((seq.capacity() == Utf16Seq::DefaultCap) && (seq.initialCap() == Utf16Seq::DefaultCap));
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator.
// Growth required.
//
void Utf16SeqSuite::testOp01()
{
    Utf16Seq seq(16 /*capacity*/);
    seq = *seq0_;
    bool ok = (seq == *seq0_);
    CPPUNIT_ASSERT(ok);

    ok = ((seq.capacity() == 32) && (seq.initialCap() == 16));
    CPPUNIT_ASSERT(ok);
}


void Utf16SeqSuite::testOp02()
{
    Utf16Seq seq(21);
    bool ok = (seq != *seq0_);
    CPPUNIT_ASSERT(ok);

    seq += *seq0_;
    ok = (seq == *seq0_);
    CPPUNIT_ASSERT(ok);

    seq += *seq0_;
    seq += *seq0_;
    seq += *seq0_;
    seq += seq;
    ok = ((seq.byteSize() == BYTE_SIZE * 8) && (seq.numChars() == NUM_CHARS * 8) && (seq.numU16s() == NUM_U16S * 8));
    CPPUNIT_ASSERT(ok);

    const utf16_t* p = seq.raw();
    for (unsigned int i = 0; i < 8; ++i)
    {
        if (memcmp(p, seq0_->raw(), seq0_->byteSize()) != 0)
        {
            ok = false;
            break;
        }
        p += seq0_->numU16s();
    }
    CPPUNIT_ASSERT(ok);
}


//
// Same byte size.
// Same character count.
//
void Utf16SeqSuite::testOp03()
{
    utf32_t s[3] = {1U, 2U, 3U};
    Utf16Seq seq;
    seq.shrink(s, 3);
    bool ok = (seq != *seq1_);
    CPPUNIT_ASSERT(ok);
}


//
// Iterate left to right.
// Make sure we can look ahead to the right.
//
void Utf16SeqSuite::testPeek00()
{
    Utf16Seq::Itor it(*seq0_, false /*makeCopy*/);

    utf32_t curC;
    utf32_t oldC;
    bool ok = true;
    for (it.peekRight(oldC); it.next(curC); it.peekRight(oldC))
    {
        if (curC != oldC)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


//
// Iterate left to right.
// Make sure we can look back to the left.
//
void Utf16SeqSuite::testPeek01()
{
    Utf16Seq::Itor it(*seq0_, false /*makeCopy*/);

    utf32_t c;
    utf32_t curC;
    it.next(curC);
    bool ok = (!it.peekLeft(c));
    CPPUNIT_ASSERT(ok);

    ok = true;
    for (utf32_t oldC = curC; it.next(curC); oldC = curC)
    {
        if ((!it.peekLeft(c)) || (c != oldC))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


//
// Iterate right to left.
// Make sure we can look ahead to the left.
//
void Utf16SeqSuite::testPeek02()
{
    Utf16Seq::Itor it(*seq1_, true /*makeCopy*/);
    bool ok = ((it.seq() != seq1_) && (*it.seq() == *seq1_));
    CPPUNIT_ASSERT(ok);
    it.attach(*seq0_, true /*makeCopy*/);
    ok = ((it.seq() != seq0_) && (*it.seq() == *seq0_));
    CPPUNIT_ASSERT(ok);

    utf32_t curC;
    utf32_t oldC;
    for (it.peekLeft(oldC); it.prev(curC); it.peekLeft(oldC))
    {
        if (curC != oldC)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    it.attach(*seq1_, true /*makeCopy*/);
    ok = it.peekLeft(oldC);
    CPPUNIT_ASSERT(ok);
}


//
// Iterate right to left.
// Make sure we can look back to the right.
//
void Utf16SeqSuite::testPeek03()
{
    Utf16Seq::Itor it;
    it.attach(*seq0_, false /*makeCopy*/);
    bool ok = (it.seq() == seq0_);
    CPPUNIT_ASSERT(ok);

    utf32_t c;
    utf32_t curC;
    it.prev(curC);
    ok = (!it.peekRight(c));
    CPPUNIT_ASSERT(ok);

    ok = true;
    for (utf32_t oldC = curC; it.prev(curC); oldC = curC)
    {
        if ((!it.peekRight(c)) || (c != oldC))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    it.detach();
}


//
// Iterate right to left.
//
void Utf16SeqSuite::testPrev00()
{
    Utf16Seq::Itor it(*seq0_, false /*makeCopy*/);
    bool ok = (it.curOffset() == Utf16Seq::INVALID_INDEX);
    CPPUNIT_ASSERT(ok);

    Utf16 utf16;
    utf32_t c;
    int i;
    ok = true;
    size_t offset = seq0_->numU16s();
    for (i = NUM_CHARS - 1; it.prev(c); --i)
    {
        utf16.resetWithValidChar(c);
        offset -= utf16.getSeqLength();
        if ((c != SAMPLE[i]) || (it.curOffset() != offset))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = ((i == -1) && (!it.prev(c)) && (it.curOffset() == offset));
    CPPUNIT_ASSERT(ok);
}


//
// No growth.
//
void Utf16SeqSuite::testReset00()
{
    Utf16Seq seq;
    seq.reset(seq0_->raw(), seq0_->numU16s(), seq0_->numChars());
    bool ok = (seq == *seq0_);
    CPPUNIT_ASSERT(ok);

    ok = ((seq.capacity() == Utf16Seq::DefaultCap) && (seq.initialCap() == Utf16Seq::DefaultCap));
    CPPUNIT_ASSERT(ok);

    utf8_t ascii[128];
    utf8_t* p = ascii;
    for (utf8_t i = 128; i > 0; *p++ = --i);
    seq.reset(ascii, 128, 128);
    utf32_t* s = seq.expand();
    for (size_t i = 0; i < 128; ++i)
    {
        if (s[i] != ascii[i])
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    delete[] s;
}


//
// Growth required.
//
void Utf16SeqSuite::testReset01()
{
    Utf16Seq seq0;
    Utf16Seq seq1(8);
    seq1.reset(seq0_->raw(), seq0_->numU16s(), seq0_->numChars());
    bool ok = (seq1 == *seq0_);
    CPPUNIT_ASSERT(ok);

    ok = ((seq1.capacity() == 32) && (seq1.initialCap() == 8));
    CPPUNIT_ASSERT(ok);

    // 0xdc00U is not a valid leading value.
    const utf16_t someInvalidSeq[9] = {0U, 1U, 2U, 0xdc00U, 2U, 1U, 0U, 0xd800U, 0xdc00};
    ok = (seq1.convert16(someInvalidSeq, 9) == 1);
    CPPUNIT_ASSERT(ok);
    ok = ((seq1.byteSize() == 18) && (seq1.numChars() == 8) && (seq1.numU16s() == 9));
    CPPUNIT_ASSERT(ok);

    seq1 = *seq0_;
    ok = ((seq1.convert8(0, 0, 0) == 0) && (seq1 == seq0));
    CPPUNIT_ASSERT(ok);

    seq1 = *seq0_;
    const utf8_t* s = 0;
    seq1.reset(s, 0, 0);
    ok = (seq1 == seq0);
    CPPUNIT_ASSERT(ok);

    Utf16Seq seq2(1);
    ok = ((seq2.convert16(seq0_->raw(), seq0_->numU16s()) == 0) && (seq2 == *seq0_));
    CPPUNIT_ASSERT(ok);

    ok = ((seq2.convert16(0, 0, 0) == 0) && (seq2 == seq0));
    CPPUNIT_ASSERT(ok);
}


void Utf16SeqSuite::testResize00()
{

    // Resize succeeds if no data loss would occur.
    Utf16Seq seq(*seq0_);
    bool ok = seq.resize(seq.numU16s());
    CPPUNIT_ASSERT(ok);

    ok = (seq == *seq0_);
    CPPUNIT_ASSERT(ok);

    ok = ((seq.capacity() == seq.numU16s()) && (seq.initialCap() == Utf16Seq::DefaultCap));
    CPPUNIT_ASSERT(ok);

    // Resize fails if data loss would occur.
    ok = (!seq.resize(1));
    CPPUNIT_ASSERT(ok);

    // Resize is a no-op if capacity does not change.
    ok = seq.resize(seq.capacity());
    CPPUNIT_ASSERT(ok);
}


void Utf16SeqSuite::testShrink00()
{
    Utf16Seq seq0;
    Utf16Seq seq1(*seq1_);
    seq1.shrink(0, 0);
    bool ok = (seq1 == seq0);
    CPPUNIT_ASSERT(ok);

    seq1 = *seq1_;
    ok = ((seq1.shrink(0, 0, 0) == 0) && (seq1 == seq0));
    CPPUNIT_ASSERT(ok);
}
