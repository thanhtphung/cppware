#include "syskit/Utf16.hpp"
#include "syskit/Utf8.hpp"

#include "syskit-ut-pch.h"
#include "Utf8Suite.hpp"

using namespace syskit;


Utf8Suite::Utf8Suite()
{
}


Utf8Suite::~Utf8Suite()
{
}


void Utf8Suite::testCtor00()
{
    Utf8 utf8a;
    bool ok = (utf8a.asU32() == 0);
    CPPUNIT_ASSERT(ok);

    unsigned char u8 = 0x12U;
    Utf8 utf8b(u8);
    ok = (utf8b.asU32() == u8);
    CPPUNIT_ASSERT(ok);

    unsigned short u16 = 0x1234U;
    Utf8 utf8c(u16);
    ok = (utf8c.asU32() == u16);
    CPPUNIT_ASSERT(ok);

    unsigned int u32 = 0xabcdeU;
    Utf8 utf8d(u32);
    ok = (utf8d.asU32() == u32);
    CPPUNIT_ASSERT(ok);

    u32 = 0x10fedcU;
    Utf16 utf16(u32);
    Utf8 utf8e(utf16);
    ok = (utf8e == u32);
    CPPUNIT_ASSERT(ok);
}


void Utf8Suite::testCtor01()
{
    unsigned int u32 = 0xdeadbeefU;
    Utf8 utf8a(u32);
    bool ok = (utf8a.asU32() == Utf8::DefaultChar);
    CPPUNIT_ASSERT(ok);

    for (unsigned short u16 = Utf8::MinReserved; u16 <= Utf8::MaxReserved; ++u16)
    {
        Utf8 utf8b(u16);
        if (utf8b.asU32() != Utf8::DefaultChar)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


//
// Try various decoding failures. Note that the encode tests have
// already reasonably verified success and insufficient buffer
// scenarios.
//
void Utf8Suite::testDecode00()
{
    const utf8_t someBadSeqs[][Utf8::MaxSeqLength + 1] =
    {
        {0xfeU, 0xffU, 0xffU, 0xffU, 1}, //invalid UTF-8 byte
        {0xabU, 0xffU, 0xffU, 0xffU, 1}, //need more than 1 byte
        {0xdfU, 0x00U, 0xffU, 0xffU, 2}, //2nd byte malformed
        {0xdfU, 0xc0U, 0xffU, 0xffU, 2}, //2nd byte malformed
        {0xc0U, 0x80U, 0xffU, 0xffU, 2}, //does not need 2 bytes
        {0xc1U, 0xbfU, 0xffU, 0xffU, 2}, //does not need 2 bytes
        {0xe1U, 0x40U, 0x81U, 0xffU, 3}, //2nd byte malformed
        {0xe1U, 0x81U, 0xc0U, 0xffU, 3}, //3rd byte malformed
        {0xe0U, 0x80U, 0x80U, 0xffU, 3}, //does not need 3 bytes
        {0xe0U, 0x9fU, 0xbfU, 0xffU, 3}, //does not need 3 bytes
        {0xedU, 0xb2U, 0xbaU, 0xffU, 3}, //reserved value (0xdcba)
        {0xf1U, 0x00U, 0x81U, 0x81U, 4}, //2nd byte malformed
        {0xf1U, 0x81U, 0x40U, 0x81U, 4}, //3rd byte malformed
        {0xf1U, 0x81U, 0x81U, 0xc0U, 4}, //4th byte malformed
        {0xf0U, 0x80U, 0x80U, 0x80U, 4}, //does not need 4 bytes
        {0xf0U, 0x8fU, 0xbfU, 0xbfU, 4}, //does not need 4 bytes
        {0xf4U, 0x90U, 0x80U, 0x80U, 4}  //too big
    };
    unsigned long numBadSeqs = sizeof(someBadSeqs) / sizeof(*someBadSeqs);

    Utf8 c;
    bool ok = true;
    for (unsigned long i = 0; i < numBadSeqs; ++i)
    {
        const utf8_t* seq = someBadSeqs[i];
        if ((c.decode(seq, seq[Utf8::MaxSeqLength]) != 0) ||
            Utf8::isValid(seq, seq[Utf8::MaxSeqLength]) ||
            Utf8::isValid(seq))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = ((c.decode(someBadSeqs[0]) == 0) && (!Utf8::isValid(someBadSeqs[0])));
    CPPUNIT_ASSERT(ok);

    ok = (Utf8::isValid(0x00abcdUL) && Utf8::isValid(0x0abcdeUL));
    CPPUNIT_ASSERT(ok);

    ok = ((!Utf8::isValid(0x00dabcUL)) && (!Utf8::isValid(0xabcdefUL)));
    CPPUNIT_ASSERT(ok);
}


//
// Try encoding all valid one-byte sequences. Along the way, verify
// a few other methods (decode(), isValid(), getSeqLength(), etc.)
// to make comprehensive testing more feasible.
//
void Utf8Suite::testEncode00()
{
    Utf8 i0;
    Utf8 i1;
    Utf8 o0;
    utf8_t seq[Utf8::MaxSeqLength];
    bool ok = true;
    for (unsigned int c = 0; c <= 0x7fU; ++c)
    {

        o0 = c;
        if (o0.getSeqLength() != 1)
        {
            ok = false;
            break;
        }

        utf8_t seq0[Utf8::MaxSeqLength] = {0xffU, 0xffU, 0xffU, 0xffU};
        size_t seq0Length = o0.encode(seq0);
        memset(seq, 0xff, sizeof(seq));
        size_t seqLength = o0.encode(seq, sizeof(seq));
        if ((seqLength != seq0Length) || (seqLength != 1) ||
            (Utf8::getSeqLength(seq0[0]) != 1) ||
            (memcmp(seq, seq0, sizeof(seq)) != 0) ||
            (!Utf8::isValid(seq)) || (!Utf8::isValid(seq, seqLength)) ||
            (i0.decode(seq) != seqLength) || (i0.asU32() != c) ||
            (i1.decode(seq, seqLength) != seqLength) || (i1.asU32() != c))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    // Decode using insufficient buffer.
    ok = (i1.decode(seq, 0) == 0);
    CPPUNIT_ASSERT(ok);

    // Encode using insufficient buffer.
    ok = (o0.encode(seq, 0) == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Try encoding all valid two-byte sequences. Along the way, verify
// a few other methods (decode(), isValid(), getSeqLength(), etc.)
// to make comprehensive testing more feasible.
//
void Utf8Suite::testEncode01()
{
    Utf8 i0;
    Utf8 i1;
    Utf8 o0;
    utf8_t seq[Utf8::MaxSeqLength];
    bool ok = true;
    for (unsigned int c = 0x80U; c <= 0x7ffU; ++c)
    {

        o0 = c;
        if (o0.getSeqLength() != 2)
        {
            ok = false;
            break;
        }

        utf8_t seq0[Utf8::MaxSeqLength] = {0xffU, 0xffU, 0xffU, 0xffU};
        size_t seq0Length = o0.encode(seq0);
        memset(seq, 0xff, sizeof(seq));
        size_t seqLength = o0.encode(seq, sizeof(seq));
        if ((seqLength != seq0Length) || (seqLength != 2) ||
            (Utf8::getSeqLength(seq0[0]) != 2) ||
            (memcmp(seq, seq0, sizeof(seq)) != 0) ||
            (!Utf8::isValid(seq)) || (!Utf8::isValid(seq, seqLength)) ||
            (i0.decode(seq) != seqLength) || (i0.asU32() != c) ||
            (i1.decode(seq, seqLength) != seqLength) || (i1.asU32() != c))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    // Decode using insufficient buffers.
    ok = ((i1.decode(seq, 0) == 0) && (i1.decode(seq, 1) == 0));
    CPPUNIT_ASSERT(ok);

    // Encode using insufficient buffers.
    ok = ((o0.encode(seq, 0) == 0) && (o0.encode(seq, 1) == 0));
    CPPUNIT_ASSERT(ok);
}


//
// Try encoding some valid three-byte sequences. Along the way, verify
// a few other methods (decode(), isValid(), getSeqLength(), etc.)
// to make comprehensive testing more feasible.
//
void Utf8Suite::testEncode02()
{
    const unsigned int some3ByteValues[] =
    {
        0x00000800U,
        0x0000abcdU,
        0x0000d7ffU,
        0x0000ffffU
    };
    const unsigned long num3ByteValues = sizeof(some3ByteValues) / sizeof(*some3ByteValues);

    Utf8 i0;
    Utf8 i1;
    Utf8 o0;
    unsigned char seq[Utf8::MaxSeqLength];
    bool ok = true;
    for (unsigned long i = 0; i < num3ByteValues; ++i)
    {

        unsigned int c = some3ByteValues[i];
        o0 = c;
        if (o0.getSeqLength() != 3)
        {
            ok = false;
            break;
        }

        utf8_t seq0[Utf8::MaxSeqLength] = {0xffU, 0xffU, 0xffU, 0xffU};
        size_t seq0Length = o0.encode(seq0);
        memset(seq, 0xff, sizeof(seq));
        size_t seqLength = o0.encode(seq, sizeof(seq));
        if ((seqLength != seq0Length) || (seqLength != 3) ||
            (Utf8::getSeqLength(seq0[0]) != 3) ||
            (memcmp(seq, seq0, sizeof(seq)) != 0) ||
            (!Utf8::isValid(seq)) || (!Utf8::isValid(seq, seqLength)) ||
            (i0.decode(seq) != seqLength) || (i0.asU32() != c) ||
            (i1.decode(seq, seqLength) != seqLength) || (i1.asU32() != c))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    // Decode using insufficient buffers.
    ok = ((i1.decode(seq, 0) == 0) && (i1.decode(seq, 1) == 0) && (i1.decode(seq, 2) == 0));
    CPPUNIT_ASSERT(ok);

    // Encode using insufficient buffers.
    ok = ((o0.encode(seq, 0) == 0) && (o0.encode(seq, 1) == 0) && (o0.encode(seq, 2) == 0));
    CPPUNIT_ASSERT(ok);
}


//
// Try encoding some valid four-byte sequences. Along the way, verify
// a few other methods (decode(), isValid(), getSeqLength(), etc.)
// to make comprehensive testing more feasible.
//
void Utf8Suite::testEncode03()
{
    const unsigned int some4ByteValues[] =
    {
        0x00010000U,
        0x000abcdeU,
        0x0010ffffU
    };
    const unsigned long num4ByteValues = sizeof(some4ByteValues) / sizeof(*some4ByteValues);

    Utf8 i0;
    Utf8 i1;
    Utf8 o0;
    utf8_t seq[Utf8::MaxSeqLength];
    bool ok = true;
    for (unsigned long i = 0; i < num4ByteValues; ++i)
    {

        unsigned int c = some4ByteValues[i];
        o0 = c;
        if (o0.getSeqLength() != 4)
        {
            ok = false;
            break;
        }

        utf8_t seq0[Utf8::MaxSeqLength] = {0xffU, 0xffU, 0xffU, 0xffU};
        size_t seq0Length = o0.encode(seq0);
        memset(seq, 0xff, sizeof(seq));
        size_t seqLength = o0.encode(seq, sizeof(seq));
        if ((seqLength != seq0Length) || (seqLength != 4) ||
            (Utf8::getSeqLength(seq0[0]) != 4) ||
            (memcmp(seq, seq0, sizeof(seq)) != 0) ||
            (!Utf8::isValid(seq)) || (!Utf8::isValid(seq, seqLength)) ||
            (i0.decode(seq) != seqLength) || (i0.asU32() != c) ||
            (i1.decode(seq, seqLength) != seqLength) || (i1.asU32() != c))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    // Decode using insufficient buffers.
    for (size_t bufSize = 0; bufSize < 4; ++bufSize)
    {
        if (i1.decode(seq, bufSize) != 0)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    // Encode using insufficient buffers.
    for (size_t bufSize = 0; bufSize < 4; ++bufSize)
    {
        if (o0.encode(seq, bufSize) != 0)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void Utf8Suite::testIsValid00()
{
    const utf8_t* seq = 0;
    size_t seqLength = 0;
    bool ok = (!Utf8::isValid(seq, seqLength));
    CPPUNIT_ASSERT(ok);
}


void Utf8Suite::testReset00()
{
    Utf8 utf8;
    unsigned char u8 = 0x12U;
    utf8 = u8;
    bool ok = (utf8.asU32() == u8);
    CPPUNIT_ASSERT(ok);

    unsigned short u16 = 0x1234U;
    utf8 = u16;
    ok = (utf8.asU32() == u16);
    CPPUNIT_ASSERT(ok);

    unsigned int u32 = 0xabcdeU;
    utf8 = u32;
    ok = (utf8.asU32() == u32);
    CPPUNIT_ASSERT(ok);

    u32 = 0x10fedcU;
    Utf16 utf16(u32);
    utf8 = utf16;
    ok = (utf8 == u32);
    CPPUNIT_ASSERT(ok);

    utf8.resetWithValidChar(0);
    ok = (utf8 == 0);
    CPPUNIT_ASSERT(ok);
}


void Utf8Suite::testReset01()
{
    Utf8 utf8;
    unsigned int value = 0xdeadbeefU;
    utf8 = value;
    bool ok = (utf8.asU32() == Utf8::DefaultChar);
    CPPUNIT_ASSERT(ok);

    for (unsigned short u16 = Utf8::MinReserved; u16 <= Utf8::MaxReserved; ++u16)
    {
        utf8 = u16;
        if (utf8.asU32() != Utf8::DefaultChar)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}
