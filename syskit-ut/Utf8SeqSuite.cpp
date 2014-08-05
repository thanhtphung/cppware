#include <string>
#include "appkit/U16.hpp"
#include "syskit/Utf16Seq.hpp"
#include "syskit/Utf8.hpp"
#include "syskit/Utf8Seq.hpp"
#include "syskit/sys.hpp"

#include "syskit-ut-pch.h"
#include "Utf8SeqSuite.hpp"

BEGIN_NAMESPACE

class MyString: public syskit::Utf8Seq
{
public:
    MyString();
    using Utf8Seq::addNull;
    using Utf8Seq::addNullIfNone;
    using Utf8Seq::rmNull;
    using Utf8Seq::seek;
    virtual ~MyString();
};

MyString::MyString():
syskit::Utf8Seq()
{
}

MyString::~MyString()
{
}

END_NAMESPACE

using namespace appkit;
using namespace syskit;

const size_t BYTE_SIZE = 40;
const size_t NUM_CHARS = 16;
const utf32_t DEFAULT_CHAR = '?';

const utf32_t SAMPLE[NUM_CHARS] =
{
    0x00000000UL, //1-byte value
    0x0000000aUL, //1-byte value
    0x0000007fUL, //1-byte value
    0x00000080UL, //2-byte value
    0x000000abUL, //2-byte value
    0x000007ffUL, //2-byte value
    0x00000800UL, //3-byte value
    0x0000abcdUL, //3-byte value
    0x0000ffffUL, //3-byte value
    0x00010000UL, //4-byte value
    0x000abcdeUL, //4-byte value
    0x0010ffffUL, //4-byte value
    0x000abcdeUL, //4-byte value
    0x0000abcdUL, //3-byte value
    0x0000000aUL, //1-byte value
    0x000000abUL  //2-byte value
};


Utf8SeqSuite::Utf8SeqSuite()
{
    seq0_ = new Utf8Seq;
    seq0_->shrink(SAMPLE, NUM_CHARS);

    seq1_ = new Utf8Seq(3);
    seq1_->shrink(SAMPLE, 3, DEFAULT_CHAR);

    char ascii[128];
    char* s = ascii;
    for (char i = 0; i >= 0; *s++ = i++);
    seq2_ = new Utf8Seq(ascii, 128);
}


Utf8SeqSuite::~Utf8SeqSuite()
{
    delete seq2_;
    delete seq1_;
    delete seq0_;
}


bool Utf8SeqSuite::cb0a(void* arg, size_t index, utf32_t c)
{
    size_t& i = *static_cast<size_t *>(arg);
    --i;
    bool ok = (i == index) && (c == SAMPLE[index]);
    CPPUNIT_ASSERT(ok);

    return true;
}


bool Utf8SeqSuite::cb0b(void* arg, size_t index, utf32_t c)
{
    size_t& i = *static_cast<size_t *>(arg);
    --i;
    bool ok = (i == index) && (c == SAMPLE[index]);
    CPPUNIT_ASSERT(ok);

    return (i > 1);
}


bool Utf8SeqSuite::cb0c(void* arg, size_t index, utf32_t c)
{
    size_t& i = *static_cast<size_t *>(arg);
    bool ok = (i == index) && (c == SAMPLE[index]);
    CPPUNIT_ASSERT(ok);
    ++i;

    return true;
}


bool Utf8SeqSuite::cb0d(void* arg, size_t index, utf32_t c)
{
    size_t& i = *static_cast<size_t *>(arg);
    bool ok = (i == index) && (c == SAMPLE[index]);
    CPPUNIT_ASSERT(ok);
    ++i;

    return (i <= 1);
}


void Utf8SeqSuite::cb1a(void* arg, size_t index, utf32_t c)
{
    size_t& i = *static_cast<size_t *>(arg);
    --i;
    bool ok = (i == index) && (c == SAMPLE[index]);
    CPPUNIT_ASSERT(ok);
}


void Utf8SeqSuite::cb1b(void* arg, size_t index, utf32_t c)
{
    size_t& i = *static_cast<size_t *>(arg);
    bool ok = (i == index) && (c == SAMPLE[index]);
    CPPUNIT_ASSERT(ok);
    ++i;
}


//
// Interfaces under test:
// - void Utf8Seq::append(const Utf8Seq& seq, size_t startAt, size_t charCount);
// - void Utf8Seq::append(const char* s, size_t length);
// - void Utf8Seq::append(size_t count, char c);
//
void Utf8SeqSuite::testAppend00()
{
    Utf8Seq seq(*seq1_);
    seq.append(seq, 0, 0);
    bool ok = (seq == *seq1_);
    CPPUNIT_ASSERT(ok);

    seq.append(*seq0_, 3, NUM_CHARS - 3);
    ok = (seq == *seq0_);
    CPPUNIT_ASSERT(ok);

    seq.append(0, 'z');
    ok = (seq == *seq0_);
    CPPUNIT_ASSERT(ok);

    size_t i = seq.byteSize();
    seq.append(12, 'z');
    const char* z = "zzzzzzzzzzzz";
    ok = ((seq.byteSize() == i + 12) && (memcmp(z, seq.raw() + i, 12) == 0));
    CPPUNIT_ASSERT(ok);

    seq.reset();
    size_t byteSize = 0;
    size_t numChars = 0;
    for (size_t count = 0; count < 256; ++count)
    {
        unsigned char c = static_cast<unsigned char>(count);
        byteSize += (count < 128)? (count): (count * 2);
        numChars += count;
        seq.append(count, c);
        if ((seq.byteSize() != byteSize) || (seq.numChars() != numChars))
        {
            ok = false;
            break;
        }
        if (count && (seq[numChars - 1] != c))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - void Utf8Seq::append(const utf8_t* s, size_t numU8s, size_t numChars);
//
void Utf8SeqSuite::testAppend01()
{
    Utf8Seq s0;
    const utf8_t* s = seq0_->raw();
    size_t numU8s = seq0_->byteSize();
    size_t numChars = seq0_->numChars();
    s0.append(s, numU8s, numChars);
    bool ok = (s0 == *seq0_);
    CPPUNIT_ASSERT(ok);

    numU8s = 0;
    numChars = 0;
    s0.append(s, numU8s, numChars);
    ok = (s0 == *seq0_);
    CPPUNIT_ASSERT(ok);

    numU8s = s0.byteSize();
    numChars = s0.numChars();
    utf8_t* p = s0.detachRaw();
    Utf8Seq s1;
    s1.attachRaw(p, numU8s, numChars);
    ok = (s1 == *seq0_);
    CPPUNIT_ASSERT(ok);
}


//
// Apply callback to each character.
// Use cb0_t.
//
void Utf8SeqSuite::testApply00()
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


void Utf8SeqSuite::testApply01()
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
void Utf8SeqSuite::testApply02()
{
    size_t i = NUM_CHARS;
    seq0_->applyHiToLo(cb1a, &i);
    bool ok = (i == 0);
    CPPUNIT_ASSERT(ok);

    seq0_->applyLoToHi(cb1b, &i);
    ok = (i == NUM_CHARS);
    CPPUNIT_ASSERT(ok);
}


void Utf8SeqSuite::testApply03()
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
// Convert from UTF16.
// All ASCII.
// Growth required.
//
void Utf8SeqSuite::testConvert00()
{
    utf32_t sample[128];
    for (unsigned long i = 0; i < 128; ++i)
    {
        sample[i] = i;
    }
    Utf16Seq seq16(7);
    seq16.shrink(sample, 128);

    Utf8Seq seq8A(32);
    bool ok = (seq8A.convert16(seq16.raw(), seq16.numU16s()) == 0);
    CPPUNIT_ASSERT(ok);

    ok = ((seq8A.capacity() == 128) && (seq8A.initialCap() == 32));
    CPPUNIT_ASSERT(ok);

    ok = ((seq8A.numChars() == 128) && (seq8A.byteSize() == 128));
    CPPUNIT_ASSERT(ok);

    utf32_t* s = seq8A.expand();
    ok = (memcmp(sample, s, sizeof(sample)) == 0);
    CPPUNIT_ASSERT(ok);
    delete[] s;

    for (unsigned long i = 0; i < 128; ++i)
    {
        if (seq8A[i] != sample[i])
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    Utf8Seq seq8B(32);
    utf16_t* u61 = new utf16_t[seq16.numU16s()];
    U16::bswap(seq16.raw(), seq16.numU16s(), u61);
    ok = ((seq8B.convert61(u61, seq16.numU16s()) == 0) && (seq8B == seq8A));
    CPPUNIT_ASSERT(ok);
    delete[] u61;
}


//
// Convert from UTF16.
// Some valid codes.
// No growth.
//
void Utf8SeqSuite::testConvert01()
{
    utf32_t sample[8] =
    {
        0x0000000aUL, //1-byte value
        0x000000abUL, //2-byte value
        0x0000abcdUL, //3-byte value
        0x000abcdeUL, //4-byte value
        0x000abcdeUL, //4-byte value
        0x0000abcdUL, //3-byte value
        0x000000abUL, //2-byte value
        0x0000000aUL  //1-byte value
    };
    Utf16Seq seq16;
    seq16.shrink(sample, 8);

    Utf8Seq seq8A(20);
    bool ok = (seq8A.convert16(seq16.raw(), seq16.numU16s()) == 0);
    CPPUNIT_ASSERT(ok);

    ok = ((seq8A.capacity() == 20) && (seq8A.initialCap() == 20));
    CPPUNIT_ASSERT(ok);

    ok = ((seq8A.numChars() == 8) && (seq8A.byteSize() == 20));
    CPPUNIT_ASSERT(ok);

    utf32_t* s = seq8A.expand();
    ok = (memcmp(sample, s, sizeof(sample)) == 0);
    CPPUNIT_ASSERT(ok);
    delete[] s;

    for (unsigned long i = 0; i < 8; ++i)
    {
        if (seq8A[i] != sample[i])
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    Utf8Seq seq8B(20);
    utf16_t* u61 = new utf16_t[seq16.numU16s()];
    U16::bswap(seq16.raw(), seq16.numU16s(), u61);
    ok = ((seq8B.convert61(u61, seq16.numU16s()) == 0) && (seq8B == seq8A));
    CPPUNIT_ASSERT(ok);
    delete[] u61;
}


//
// Convert from UTF16.
// Some invalid codes.
//
void
Utf8SeqSuite::testConvert02()
{

    //TODO
#if 0
    const utf16_t someBadSeqs[][2] =
    {
        {0xdc00U, 0xdcbaU}, //1st 2 bytes malformed
        {0xdba8U, 0x0000U}, //2nd 2 bytes malformed
        {0xdba8U, 0xd800U}  //2nd 2 bytes malformed
    };
    unsigned long numBadSeqs = sizeof(someBadSeqs) / sizeof(*someBadSeqs);

    Utf8Seq seq8A;
    Utf8Seq seq8B;
    utf16_t seq16A[128] = {0};
    utf16_t seq16B[128] = {0};
    bool ok = true;
    for (unsigned long i = 0; i < numBadSeqs; ++i)
    {
        seq16A[45] = someBadSeqs[i][0];
        seq16A[46] = someBadSeqs[i][1];
        seq16B[45] = bswap16(someBadSeqs[i][0]);
        seq16B[46] = bswap16(someBadSeqs[i][1]);
        if ((seq8A.convert16(seq16A, 128) != 1) || (seq8B.convert61(seq16B, 128) != 1) || (seq8A != seq8B))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
#endif
}


//
// Convert from UTF32.
//
void Utf8SeqSuite::testConvert03()
{
    Utf8Seq seq;
    bool ok = ((seq.shrink(SAMPLE, NUM_CHARS, DEFAULT_CHAR) == 0) && (seq == *seq0_));
    CPPUNIT_ASSERT(ok);

    const utf32_t someInvalidSeq[5] = {0UL, 1UL, 0xabcddcbaUL, 1UL, 0UL};
    ok = ((seq.shrink(someInvalidSeq, 5, DEFAULT_CHAR) == 1) && (seq.numChars() == 5));
    CPPUNIT_ASSERT(ok);
    ok = (seq[2] == DEFAULT_CHAR);
    CPPUNIT_ASSERT(ok);

    seq.reset();
    seq.resize(1);
    ok = ((seq.shrink(SAMPLE, NUM_CHARS, DEFAULT_CHAR) == 0) && (seq == *seq0_));
    CPPUNIT_ASSERT(ok);
}


void Utf8SeqSuite::testCountChars00()
{
    unsigned int numChars = 1234567;
    bool ok = Utf8Seq::countChars(0, 0, numChars) && (numChars == 0);
    CPPUNIT_ASSERT(ok);

    ok = (Utf8Seq::countChars(seq0_->raw(), seq0_->byteSize(), numChars) && (numChars == NUM_CHARS));
    CPPUNIT_ASSERT(ok);

    const utf8_t someInvalidSeq[7] = {0U, 1U, 2U, 255U, 2U, 1U, 0U};
    ok = ((!Utf8Seq::countChars(someInvalidSeq, 7, numChars)) && (numChars == 3));
    CPPUNIT_ASSERT(ok);
}


//
// Default constructor.
//
void Utf8SeqSuite::testCtor00()
{
    Utf8Seq seq;
    bool ok = ((seq.capacity() == Utf8Seq::DefaultCap) && (seq.growthFactor() < 0));
    CPPUNIT_ASSERT(ok);

    ok = ((seq.byteSize() == 0) && (seq.numChars() == 0));
    CPPUNIT_ASSERT(ok);

    unsigned int byteSize = 123;
    ok = ((seq.raw() != 0) && (seq.raw(byteSize) != 0) && (byteSize == 0));
    CPPUNIT_ASSERT(ok);

    ok = ((!seq.setGrowth(100)) && seq.setGrowth(-1));
    CPPUNIT_ASSERT(ok);
}


//
// Copy constructor.
//
void Utf8SeqSuite::testCtor01()
{
    Utf8Seq seq0(*seq0_);
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
void Utf8SeqSuite::testCtor02()
{
    unsigned long capacity = 0;
    Utf8Seq seq(capacity);
    bool ok = ((seq.capacity() == 1) && (seq.growthFactor() < 0));
    CPPUNIT_ASSERT(ok);

    ok = ((seq.byteSize() == 0) && (seq.numChars() == 0));
    CPPUNIT_ASSERT(ok);

    unsigned int byteSize = 123;
    ok = ((seq.raw() != 0) && (seq.raw(byteSize) != 0) && (byteSize == 0));
    CPPUNIT_ASSERT(ok);
}


//
// ASCII.
//
void Utf8SeqSuite::testCtor03()
{
    Utf8Seq seq("abc123", 6);
    bool ok = ((seq.byteSize() == 6) && (seq.numChars() == 6) && seq.isAscii());
    CPPUNIT_ASSERT(ok);

    ok = (memcmp(seq.raw(), "abc123", 6) == 0);
    CPPUNIT_ASSERT(ok);

    seq += '4';
    ok = ((seq.byteSize() == 7) && (seq.numChars() == 7));
    CPPUNIT_ASSERT(ok);

    ok = (memcmp(seq.raw(), "abc1234", 7) == 0);
    CPPUNIT_ASSERT(ok);

    MyString str;
    unsigned long length = 7;
    str.reset("abc1234", length);
    str.addNull();
    ok = (memcmp(str.raw(), "abc1234", 7 + 1) == 0);
    CPPUNIT_ASSERT(ok);

    str.addNullIfNone();
    str.rmNull();
    str.addNullIfNone();
    ok = (memcmp(str.raw(), "abc1234", 7 + 1) == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Subsequence.
//
void Utf8SeqSuite::testCtor04()
{
    Utf8Seq seq0;
    seq0.reset(*seq0_, 0, 3);
    Utf8Seq seq1(*seq0_, 0, 3);
    bool ok = ((seq0 == *seq1_) && (seq1 == *seq1_));
    CPPUNIT_ASSERT(ok);

    seq0.reset(*seq0_, NUM_CHARS - 2, 2);
    Utf8Seq seq2(*seq0_, NUM_CHARS - 2, 2);
    ok = ((seq0.numChars() == 2) && (seq0[0] == SAMPLE[NUM_CHARS - 2]) && (seq0[1] == SAMPLE[NUM_CHARS - 1]));
    CPPUNIT_ASSERT(ok);
    ok = ((seq2.numChars() == 2) && (seq2[0] == SAMPLE[NUM_CHARS - 2]) && (seq2[1] == SAMPLE[NUM_CHARS - 1]));
    CPPUNIT_ASSERT(ok);

    seq0.reset(*seq0_, 5, 0);
    Utf8Seq seq3(*seq0_, 5, 0);
    ok = ((seq0.numChars() == 0) && (seq3.numChars() == 0));
    CPPUNIT_ASSERT(ok);

    seq0.reset(*seq0_, 5, 1);
    Utf8Seq seq4(*seq0_, 5, 1);
    ok = ((seq0.numChars() == 1) && (seq0[0] == SAMPLE[5]));
    CPPUNIT_ASSERT(ok);
    ok = ((seq4.numChars() == 1) && (seq4[0] == SAMPLE[5]));
    CPPUNIT_ASSERT(ok);

    seq0.reset(*seq2_, 'a', 26);
    Utf8Seq seq5(*seq2_, 'a', 26);
    ok = ((seq0.numChars() == 26) && (seq5.numChars() == 26));
    CPPUNIT_ASSERT(ok);
    const utf8_t* p0 = seq0.raw();
    const utf8_t* p1 = seq5.raw();
    for (char c = 'a'; c <= 'z'; ++c)
    {
        if ((*p0++ != c) || (*p1++ != c))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    seq0.reset();
    seq5.reset(seq4, 999, 9999);
    ok = (seq5 == seq0);
    CPPUNIT_ASSERT(ok);

    seq5.reset(seq4, 0, 9999);
    ok = (seq5 == seq0);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - Utf8Seq(utf8_t*, size_t, size_t);
// - utf8_t* Utf8Seq::detachRaw();
// - utf8_t* Utf8Seq::detachRaw(size_t&);
//
void Utf8SeqSuite::testDetachRaw00()
{
    Utf8Seq seq0("abc", 4);
    unsigned int byteSize = 0;
    const utf8_t* raw = seq0.raw();
    utf8_t* s = seq0.detachRaw(byteSize);
    bool ok = ((s == raw) && (byteSize == 4) && (seq0.raw() != raw));
    CPPUNIT_ASSERT(ok);

    delete[] s;
    Utf8Seq seq1("abc123", 6);
    size_t numChars = seq1.numChars();
    byteSize = seq1.byteSize();
    raw = seq1.raw();
    s = seq1.detachRaw();
    ok = ((s == raw) && (seq1.raw() != raw));
    CPPUNIT_ASSERT(ok);

    Utf8Seq seq2(s, byteSize, numChars);
    ok = ((seq2.raw() == raw) && (seq2.byteSize() == byteSize) && (seq2.numChars() == numChars));
    CPPUNIT_ASSERT(ok);
}


void Utf8SeqSuite::testIsValid00()
{

    // An empty raw sequence is a valid sequence.
    bool ok = Utf8Seq::isValid(0, 0, 0);
    CPPUNIT_ASSERT(ok);

    // A valid Utf8Seq instance should have a valid raw sequence.
    ok = Utf8Seq::isValid(seq0_->raw(), seq0_->byteSize(), 0);
    CPPUNIT_ASSERT(ok);

    // 255 should not be valid in any raw sequence.
    const utf8_t someInvalidSeq[7] = {0U, 1U, 2U, 255U, 2U, 1U, 0U};
    ok = (!Utf8Seq::isValid(someInvalidSeq, sizeof(someInvalidSeq), 0));
    CPPUNIT_ASSERT(ok);

    const utf8_t* badSeq = 0;
    ok = (!Utf8Seq::isValid(someInvalidSeq, 7, &badSeq)) && (badSeq == &someInvalidSeq[3]);
    CPPUNIT_ASSERT(ok);
}


//
// Iterate left to right.
//
void Utf8SeqSuite::testNext00()
{
    Utf8Seq::Itor it(*seq0_, false /*makeCopy*/);
    bool ok = (it.curOffset() == Utf8Seq::INVALID_INDEX);
    CPPUNIT_ASSERT(ok);

    Utf8 utf8;
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
        utf8.resetWithValidChar(c);
        offset += utf8.getSeqLength();
    }
    CPPUNIT_ASSERT(ok);

    ok = ((i == NUM_CHARS) && (!it.next(c)));
    CPPUNIT_ASSERT(ok);

    utf8.resetWithValidChar(SAMPLE[NUM_CHARS - 1]);
    offset -= utf8.getSeqLength();
    ok = (it.curOffset() == offset);
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator.
// No growth.
//
void Utf8SeqSuite::testOp00()
{
    Utf8Seq seq;
    seq = *seq0_;
    bool ok = (seq == *seq0_);
    CPPUNIT_ASSERT(ok);

    ok = ((seq.capacity() == Utf8Seq::DefaultCap) && (seq.initialCap() == Utf8Seq::DefaultCap));
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator.
// Growth required.
//
void Utf8SeqSuite::testOp01()
{
    Utf8Seq seq(16 /*capacity*/);
    seq = *seq0_;
    bool ok = (seq == *seq0_);
    CPPUNIT_ASSERT(ok);

    ok = ((seq.capacity() == 64) && (seq.initialCap() == 16));
    CPPUNIT_ASSERT(ok);
}


void Utf8SeqSuite::testOp02()
{
    Utf8Seq seq(21);
    bool ok = (seq != *seq0_);
    CPPUNIT_ASSERT(ok);

    seq += *seq0_;
    ok = (seq == *seq0_);
    CPPUNIT_ASSERT(ok);

    seq += *seq0_;
    seq += *seq0_;
    seq += *seq0_;
    seq += seq;
    ok = ((seq.byteSize() == BYTE_SIZE * 8) && (seq.numChars() == NUM_CHARS * 8));
    CPPUNIT_ASSERT(ok);

    const utf8_t* p = seq.raw();
    for (unsigned long i = 0; i < 8; ++i)
    {
        if (memcmp(p, seq0_->raw(), seq0_->byteSize()) != 0)
        {
            ok = false;
            break;
        }
        p += seq0_->byteSize();
    }
    CPPUNIT_ASSERT(ok);
}


//
// Same byte size.
// Same character count.
//
void Utf8SeqSuite::testOp03()
{
    utf32_t s[3] = {1UL, 2UL, 3UL};
    Utf8Seq seq;
    seq.shrink(s, 3);
    bool ok = (seq != *seq1_);
    CPPUNIT_ASSERT(ok);
}


void Utf8SeqSuite::testOp04()
{

    // Both sequences are empty.
    Utf8Seq seq0;
    Utf8Seq seq1;
    bool ok = (!(seq0 < seq1));
    CPPUNIT_ASSERT(ok);

    // First sequence is empty.
    ok = (seq0 < *seq0_);
    CPPUNIT_ASSERT(ok);

    // Second sequence is empty.
    ok = (!(*seq1_ < seq1));
    CPPUNIT_ASSERT(ok);

    ok = (*seq0_ > *seq1_) && (Utf8Seq::compareP(seq0_, seq1_) > 0) && (Utf8Seq::comparePR(seq0_, seq1_) < 0);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - Utf8Seq::Utf8Seq(Utf8Seq* seq);
// - const Utf8Seq& Utf8Seq::operator +=(const Utf8& c);
//
void Utf8SeqSuite::testOp05()
{
    unsigned long capacity = 3;
    Utf8Seq s0(capacity);
    bool skipValidation = true;
    for (size_t i = 0; i < NUM_CHARS; ++i)
    {
        Utf8 c(SAMPLE[i], skipValidation);
        s0 += c;
    }

    bool ok = (s0 == *seq0_);
    CPPUNIT_ASSERT(ok);

    Utf8Seq s1(&s0);
    ok = (s1 == *seq0_);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - const Utf8Seq& Utf8Seq::operator +=(char c);
//
void Utf8SeqSuite::testOp06()
{
    unsigned long capacity = 9;
    Utf8Seq s0(capacity);
    for (unsigned char c = 0;;)
    {
        s0 += c++;
        if (c == 0) break;
    }

    bool ok = (s0.numChars() == 256) && (s0.byteSize() == 128 + 128 * 2);
    CPPUNIT_ASSERT(ok);

    for (unsigned char c = 0;;)
    {
        if (s0[c] != c)
        {
            ok = false;
            break;
        }
        if (c++ == 0) break;
    }
    CPPUNIT_ASSERT(ok);
}


//
// Iterate left to right.
// Make sure we can look ahead to the right.
//
void Utf8SeqSuite::testPeek00()
{
    Utf8Seq::Itor it(*seq0_, false /*makeCopy*/);

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
void Utf8SeqSuite::testPeek01()
{
    Utf8Seq::Itor it(*seq0_, false /*makeCopy*/);

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
void Utf8SeqSuite::testPeek02()
{
    Utf8Seq::Itor it(*seq1_, true /*makeCopy*/);
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
void Utf8SeqSuite::testPeek03()
{
    Utf8Seq::Itor it;
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
void Utf8SeqSuite::testPrev00()
{
    Utf8Seq::Itor it(*seq0_, false /*makeCopy*/);
    bool ok = (it.curOffset() == Utf8Seq::INVALID_INDEX);
    CPPUNIT_ASSERT(ok);

    Utf8 utf8;
    utf32_t c;
    int i;
    ok = true;
    size_t offset = seq0_->byteSize();
    for (i = NUM_CHARS - 1; it.prev(c); --i)
    {
        utf8.resetWithValidChar(c);
        offset -= utf8.getSeqLength();
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
void Utf8SeqSuite::testReset00()
{
    Utf8Seq seq;
    seq.reset(seq0_->raw(), seq0_->byteSize(), seq0_->numChars());
    bool ok = (seq == *seq0_);
    CPPUNIT_ASSERT(ok);

    ok = ((seq.capacity() == Utf8Seq::DefaultCap) && (seq.initialCap() == Utf8Seq::DefaultCap));
    CPPUNIT_ASSERT(ok);
}


//
// Growth required.
//
void Utf8SeqSuite::testReset01()
{
    Utf8Seq seq0;
    Utf8Seq seq1(8);
    seq1.reset(seq0_->raw(), seq0_->byteSize(), seq0_->numChars());
    bool ok = (seq1 == *seq0_);
    CPPUNIT_ASSERT(ok);

    ok = ((seq1.capacity() == 64) && (seq1.initialCap() == 8));
    CPPUNIT_ASSERT(ok);

    // 255 should not be valid in any raw sequence.
    const utf8_t someInvalidSeq[7] = {0U, 1U, 2U, 255U, 2U, 1U, 0U};
    ok = (seq1.convert8(someInvalidSeq, 7, DEFAULT_CHAR) == 1);
    CPPUNIT_ASSERT(ok);
    ok = ((seq1.byteSize() == 7) && (seq1.numChars() == 7) && (seq1[3] == DEFAULT_CHAR));
    CPPUNIT_ASSERT(ok);

    seq1.reset(seq1.raw(), 0, 0);
    ok = (seq1 == seq0);
    CPPUNIT_ASSERT(ok);

    seq1 = *seq2_;
    seq1.convert8(0, 0, 0);
    ok = (seq1 == seq0);
    CPPUNIT_ASSERT(ok);

    seq1 = *seq2_;
    seq1.convert16(0, 0, 0);
    ok = (seq1 == seq0);
    CPPUNIT_ASSERT(ok);

    seq1 = *seq2_;
    seq1.convert61(0, 0, 0);
    ok = (seq1 == seq0);
    CPPUNIT_ASSERT(ok);

    Utf8Seq seq2(1);
    ok = ((seq2.convert8(seq1_->raw(), seq1_->byteSize(), DEFAULT_CHAR) == 0) && (seq2 == *seq1_));
    CPPUNIT_ASSERT(ok);

    utf8_t* buf = new utf8_t[seq0_->byteSize() + 1];
    buf[0] = 255U;
    memcpy(buf + 1, seq0_->raw(), seq0_->byteSize());
    ok = (seq2.convert8(buf, seq0_->byteSize() + 1, DEFAULT_CHAR) == 1);
    CPPUNIT_ASSERT(ok);
    delete[] buf;

    seq2.reset(seq2, 1, seq2.numChars() - 1);
    ok = (seq2 == *seq0_);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - void Utf8Seq::append(const char* s, size_t length);
// - void Utf8Seq::reset(const char* s, size_t length);
//
void Utf8SeqSuite::testReset02()
{
    Utf8Seq seq0;
    Utf8Seq seq1(*seq2_);
    const char* s = 0;
    unsigned long length = 0;
    seq1.reset(s, length);
    bool ok = (seq1 == seq0);
    CPPUNIT_ASSERT(ok);
    seq1.append(s, 0);
    ok = (seq1 == seq0);
    CPPUNIT_ASSERT(ok);

    length = 3;
    seq1.reset("abc123", length);
    seq0.append("abc123", length);
    ok = ((seq0 == seq1) && (seq1.numChars() == 3) && (memcmp(seq1.raw(), "abc", 3) == 0));
    CPPUNIT_ASSERT(ok);

    seq0.reset();
    seq1.reset();
    unsigned char ascii8[256];
    for (long i = -1; ++i < 256; ascii8[i] = static_cast<unsigned char>(i));
    length = 256;
    seq0.reset(reinterpret_cast<const char*>(ascii8), length);
    seq1.append(reinterpret_cast<const char*>(ascii8), length);
    ok = (seq0 == seq1) && (seq0.numChars() == 256) && (seq0.byteSize() == 128 + 128 * 2);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - void Utf8Seq::reset(const utf16_t* s, size_t numU16s, size_t numChars);
//
void Utf8SeqSuite::testReset03()
{
    Utf16Seq seq16;
    seq16.shrink(SAMPLE, NUM_CHARS);
    Utf8Seq seq0;
    Utf8Seq seq1;
    seq1.reset(seq16.raw(), seq16.numU16s(), seq16.numChars());
    utf32_t* s = seq1.expand();
    bool ok = (memcmp(s, SAMPLE, sizeof(SAMPLE)) == 0);
    CPPUNIT_ASSERT(ok);
    delete[] s;

    seq1.reset(seq16.raw(), seq16.numU16s(), 0);
    seq0.reset(seq16.raw(), 0, seq16.numChars());
    s = seq1.expand();
    ok = (seq0 == seq1) && (memcmp(s, SAMPLE, sizeof(SAMPLE)) == 0);
    CPPUNIT_ASSERT(ok);
    delete[] s;

    seq0.reset();
    seq1.reset(seq16.raw(), 0, 0);
    ok = (seq1 == seq0);
    CPPUNIT_ASSERT(ok);

    seq16.shrink(SAMPLE, 3);
    seq1.reset(seq16.raw(), seq16.numU16s(), seq16.numChars());
    s = seq1.expand();
    ok = (memcmp(s, SAMPLE, 3 * sizeof(SAMPLE[0])) == 0);
    CPPUNIT_ASSERT(ok);
    delete[] s;

    ok = (seq1.getByteSize(0, 3) == 3);
    CPPUNIT_ASSERT(ok);

    seq1.reset(seq16.raw(), 0, 0);
    ok = (seq1 == seq0);
    CPPUNIT_ASSERT(ok);
}


void Utf8SeqSuite::testResize00()
{

    // Resize succeeds if no data loss would occur.
    Utf8Seq seq(*seq0_);
    bool ok = seq.resize(seq.byteSize());
    CPPUNIT_ASSERT(ok);

    ok = (seq == *seq0_);
    CPPUNIT_ASSERT(ok);

    ok = ((seq.capacity() == seq.byteSize()) && (seq.initialCap() == Utf8Seq::DefaultCap));
    CPPUNIT_ASSERT(ok);

    // Resize fails if data loss would occur.
    ok = (!seq.resize(1));
    CPPUNIT_ASSERT(ok);

    // Resize is a no-op if capacity does not change.
    ok = seq.resize(seq.capacity());
    CPPUNIT_ASSERT(ok);
}


void Utf8SeqSuite::testSeek00()
{
    MyString str;
    unsigned long length = 7;
    str.reset("abc1234", length);
    const utf8_t* p0 = str.raw();
    const utf8_t* p = str.seek(3);
    bool ok = (p == (p0 + 3));
    CPPUNIT_ASSERT(ok);
}


void Utf8SeqSuite::testShrink00()
{
    Utf8Seq seq0;
    Utf8Seq seq1(*seq1_);
    seq1.shrink(0, 0);
    bool ok = (seq1 == seq0);
    CPPUNIT_ASSERT(ok);

    seq1 = *seq2_;
    ok = ((seq1.shrink(0, 0, 0) == 0) && (seq1 == seq0));
    CPPUNIT_ASSERT(ok);
}


void Utf8SeqSuite::testTruncate00()
{
    Utf8Seq seq(*seq0_);
    bool ok = (!seq.truncate(9999));
    CPPUNIT_ASSERT(ok);

    ok = (seq.truncate(0) && (seq.numChars() == 0));
    CPPUNIT_ASSERT(ok);

    seq = *seq0_;
    ok = (seq.truncate(3) && (seq == *seq1_));
    CPPUNIT_ASSERT(ok);

    seq = *seq2_;
    ok = (seq.truncate(1) && (seq.numChars() == 1) && (seq[0] == 0));
    CPPUNIT_ASSERT(ok);

    seq.reset();
    ok = (!seq.truncate(0));
    CPPUNIT_ASSERT(ok);
}
