#include "syskit/Utf16.hpp"

#include "syskit-ut-pch.h"
#include "Utf16Suite.hpp"

using namespace syskit;


Utf16Suite::Utf16Suite()
{
}


Utf16Suite::~Utf16Suite()
{
}


void Utf16Suite::testCtor00()
{
    Utf16 utf16a;
    bool ok = (utf16a.asU32() == 0);
    CPPUNIT_ASSERT(ok);

    unsigned char u8 = 0x12U;
    Utf16 utf16b(u8);
    ok = ((utf16b.asU32() == u8) && (utf16b.getSeqLength() == 1));
    CPPUNIT_ASSERT(ok);

    unsigned short u16 = 0x1234U;
    Utf16 utf16c(u16);
    ok = (utf16c.asU32() == u16);
    CPPUNIT_ASSERT(ok);

    unsigned int u32 = 0xabcdeU;
    Utf16 utf16d(u32);
    ok = ((utf16d.asU32() == u32) && (utf16d.getSeqLength() == 2));
    CPPUNIT_ASSERT(ok);

    u32 = 0x10fedcU;
    Utf8 utf8(u32);
    Utf16 utf16e(utf8);
    ok = (utf16e == u32);
    CPPUNIT_ASSERT(ok);
}


void Utf16Suite::testCtor01()
{
    unsigned int u32 = 0xdeadbeefU;
    Utf16 utf16a(u32);
    bool ok = (utf16a.asU32() == Utf16::DefaultChar);
    CPPUNIT_ASSERT(ok);

    for (unsigned short u16 = Utf8::MinReserved; u16 <= Utf8::MaxReserved; ++u16)
    {
        Utf16 utf16b(u16);
        if (utf16b.asU32() != Utf16::DefaultChar)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


//
// Try various decoding failures. Note that the encode tests have
// already reasonably verified success scenarios.
//
void Utf16Suite::testDecode00()
{
    const utf16_t someBadSeqs[][2] =
    {
        {0xdc00U, 0xdcbaU}, //1st 2 bytes malformed
        {0xdba8U, 0x0000U}, //2nd 2 bytes malformed
        {0xdba8U, 0xd800U}  //2nd 2 bytes malformed
    };
    unsigned long numBadSeqs = sizeof(someBadSeqs) / sizeof(*someBadSeqs);

    Utf16 c;
    bool ok = true;
    for (unsigned long i = 0; i < numBadSeqs; ++i)
    {
        const utf16_t* seq = someBadSeqs[i];
        if (c.decode(seq) != 0)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = (!Utf16::isValid(0x00dabcUL));
    CPPUNIT_ASSERT(ok);

    ok = (!Utf16::isValid(0xabcdefUL));
    CPPUNIT_ASSERT(ok);
}


void Utf16Suite::testEncode00()
{
    const unsigned int someValues[] =
    {
        0x000000U,
        0x00abcdU,
        0x0abcdeU,
        0x10ffffU
    };
    const unsigned long numValues = sizeof(someValues) / sizeof(*someValues);

    Utf16 i0;
    Utf16 i1;
    Utf16 o0;
    bool ok = true;
    for (unsigned long i = 0; i < numValues; ++i)
    {

        unsigned int c = someValues[i];
        size_t seqLength = (c <= 0xffffUL)? 1: 2;
        if (!Utf16::isValid(c))
        {
            ok = false;
            break;
        }

        utf32_t u32 = 0x12345678UL;
        utf16_t seq[2] = {0xdcbaU, 0xdcbaU};
        o0 = c;
        if ((o0.encode(seq) != seqLength) ||
            (i0.decode(seq) != seqLength) || (i0.asU32() != c) ||
            (i1.decodeValidSeq(seq, u32) != seqLength) || (u32 != c) ||
            (Utf16::convertValidSeq(seq) != c))
        {
            ok = false;
            break;
        }

        if (seqLength == 2)
        {
            memset(seq, 0xff, sizeof(seq));
            Utf16::convertSurrogate(c, seq);
            if ((Utf16::convertSurrogate(seq) != c) || (Utf16::convertSurrogate(seq[0], seq[1]) != c))
            {
                ok = false;
                break;
            }
        }
    }
    CPPUNIT_ASSERT(ok);
}


void Utf16Suite::testReset00()
{
    Utf16 utf16;
    unsigned char u8 = 0x12U;
    utf16 = u8;
    bool ok = (utf16.asU32() == u8);
    CPPUNIT_ASSERT(ok);

    unsigned short u16 = 0x1234U;
    utf16 = u16;
    ok = (utf16.asU32() == u16);
    CPPUNIT_ASSERT(ok);

    unsigned int u32 = 0xabcdeUL;
    utf16 = u32;
    ok = (utf16.asU32() == u32);
    CPPUNIT_ASSERT(ok);

    u32 = 0x10fedcU;
    Utf8 utf8(u32);
    utf16 = utf8;
    ok = (utf16 == u32);
    CPPUNIT_ASSERT(ok);

    utf16.resetWithValidChar(0);
    ok = (utf16 == 0);
    CPPUNIT_ASSERT(ok);
}


void Utf16Suite::testReset01()
{
    Utf16 utf16;
    unsigned int u32 = 0xdeadbeefU;
    utf16 = u32;
    bool ok = (utf16.asU32() == Utf16::DefaultChar);
    CPPUNIT_ASSERT(ok);

    for (unsigned short u16 = Utf16::MinReserved; u16 <= Utf16::MaxReserved; ++u16)
    {
        utf16 = u16;
        if (utf16.asU32() != Utf16::DefaultChar)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}
