#include "appkit/U8.hpp"
#include "syskit/sys.hpp"

#include "appkit-ut-pch.h"
#include "U8Suite.hpp"

using namespace appkit;

typedef struct
{
    const char* s;
    unsigned long v;
} sample_t;

const sample_t SAMPLE[] =
{
    {"0", 0},
    {"12", 12},
    {"+234", +234},
    {"0x45", 0x45},
    {"+0Xab", +0Xab},
    {"067", 067},
    {"+065", +065},
    {"0x00000000000000000000000000000000000CD", 0xCDu},
    {"0000000000000000000000000000000000054", 054U}
};

const unsigned long NUM_SAMPLES = sizeof(SAMPLE) / sizeof(SAMPLE[0]);


U8Suite::U8Suite()
{
}


U8Suite::~U8Suite()
{
}


void U8Suite::testCtor00()
{
    String s0;
    bool ok = true;
    U8::item_t defaultV = 45;
    for (unsigned long i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        s0 = r.s;
        U8 u8a(r.s);
        U8 u8b(u8a);
        U8 u8c(s0);
        U8 u8d(s0.ascii(), s0.length());
        U8 u8e(s0.ascii(), s0.length() + 1);
        U8 u8f(&s0, defaultV);
        if ((u8a != r.v) || (u8b != r.v) || (u8c != r.v) || (u8d != r.v) || (u8e != r.v) || (u8f != r.v))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void U8Suite::testIsValid00()
{
    bool ok = true;
    for (unsigned long i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        size_t length = strlen(r.s);
        if ((!U8::isValid(r.s)) || (!U8::isValid(r.s, length)) || (!U8::isValid(r.s, length + 1)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    const char* s = 0;
    size_t length = 0;
    ok = (!U8::isValid(s, length));
    CPPUNIT_ASSERT(ok);
    length = 1234;
    ok = (!U8::isValid(s, length));
    CPPUNIT_ASSERT(ok);
}


void U8Suite::testNibbleToXdigit00()
{
    bool ok = true;
    const char* const RESULT0 = "0123456789ABCDEF";
    const char* const RESULT1 = "0123456789abcdef";
    for (unsigned char i = 0; i < 16; ++i)
    {
        if ((syskit::XDIGIT[i] != RESULT0[i]) || (syskit::xdigit[i] != RESULT1[i]))
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
}


void U8Suite::testToDigits00()
{
    bool ok = true;
    char digit[U8::MaxDigits] = {0};
    for (unsigned char i = 0; i < 10; ++i)
    {
        if ((U8::toDigits(i, digit) != 1) || (digit[0] != (i + '0')))
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
    for (unsigned char i = 10; i < 100; ++i)
    {
        if ((U8::toDigits(i, digit) != 2) || (digit[0] != ((i / 10) + '0')) || (digit[1] != ((i % 10) + '0')))
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
    for (unsigned char i = 100; i > 99; ++i)
    {
        if ((U8::toDigits(i, digit) != 3) ||
            (digit[0] != ((i / 100) + '0')) ||
            (digit[1] != (((i / 10) % 10) + '0')) ||
            (digit[2] != ((i % 10) + '0')))
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
}


void U8Suite::testToXDigits00()
{
    bool ok = true;
    unsigned char i = 0;
    do
    {

        char xdigits[2];
        U8::toXDIGITS(i, xdigits);
        if ((xdigits[0] != syskit::XDIGIT[i >> 4U]) || (xdigits[1] != syskit::XDIGIT[i & 0x0fU]))
        {
            ok = false;
            break;
        }

        xdigits[0] = 0;
        xdigits[1] = 0;
        U8::toXdigits(i, xdigits);
        if ((xdigits[0] != syskit::xdigit[i >> 4U]) || (xdigits[1] != syskit::xdigit[i & 0x0fU]))
        {
            ok = false;
            break;
        }
    } while (++i != 0xffU);

    CPPUNIT_ASSERT(ok);
}


void U8Suite::testXdigitToNibble00()
{
    bool ok = true;
    for (char xdigit = '0'; xdigit <= '9'; ++xdigit)
    {
        if (U8::xdigitToNibble(xdigit) != (xdigit - '0'))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    for (char xdigit = 'a'; xdigit <= 'f'; ++xdigit)
    {
        if (U8::xdigitToNibble(xdigit) != (xdigit - 'a' + 0x0aU))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    for (char xdigit = 'A'; xdigit <= 'F'; ++xdigit)
    {
        if (U8::xdigitToNibble(xdigit) != (xdigit - 'A' + 0x0aU))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void U8Suite::testXdigitsToU8a()
{
    bool ok = true;
    unsigned char u8 = 0;
    for (char hi = '0'; hi <= 'f'; (hi == '9')? (hi = 'a'): (++hi))
    {
        for (char lo = '0'; lo <= 'F'; (lo == '9')? (lo = 'A'): (++lo))
        {
            if (U8::xdigitsToU8(hi, lo) != u8++)
            {
                ok = false;
                hi = 'f'; //terminate outer loop
                break;
            }
        }
    }
    CPPUNIT_ASSERT(ok);
}
