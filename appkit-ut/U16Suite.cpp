#include "appkit/U16.hpp"

#include "appkit-ut-pch.h"
#include "U16Suite.hpp"

using namespace appkit;

typedef struct
{
    const char* s;
    U16::item_t v;
} sample_t;

const sample_t SAMPLE[] =
{
    {"0", 0},
    {"123", 123},
    {"+1234", +1234},
    {"0x2345", 0x2345},
    {"+0Xabcd", +0Xabcd},
    {"034567", 034567},
    {"+076", +076},
    {"0x0000000000000000000000000000000000034AB", 0x34ABu},
    {"0000000000000000000000000000000000076543", 076543U}
};

const size_t NUM_SAMPLES = sizeof(SAMPLE) / sizeof(SAMPLE[0]);


U16Suite::U16Suite()
{
}


U16Suite::~U16Suite()
{
}


void U16Suite::testBswap00()
{
    U16::item_t item0[32];
    U16::item_t item1[32];
    for (U16::item_t i = 0; i < 32; ++i)
    {
        item0[i] = ((255 - i) << 8) | i;
    }
    U16::bswap(item0, 32, item1);
    U16::bswap(item0, 32);
    bool ok = (memcmp(item0, item1, sizeof(item0)) == 0);
    CPPUNIT_ASSERT(ok);

    for (U16::item_t i = 0; i < 32; ++i)
    {
        U16::item_t item = (i << 8) | (255 - i);
        if (item0[i] != item)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void U16Suite::testChecksum00()
{
    unsigned short sum = U16::checksum(0, 0);
    bool ok = (sum == 0xffffU);
    CPPUNIT_ASSERT(ok);

    unsigned short u16[4] = {1U, 2U, 0xfffeU, 0xfffeU};
    sum = U16::checksum(u16, 2);
    ok = (sum == 0xfffcU);
    CPPUNIT_ASSERT(ok);

    sum = U16::checksum(u16, 3);
    ok = (sum == 0xfffdU);
    CPPUNIT_ASSERT(ok);

    sum = U16::checksum(u16, 4);
    ok = (sum == 0xfffeU);
    CPPUNIT_ASSERT(ok);
}


void U16Suite::testChecksum01()
{
    unsigned short sum = U16::checksum(0, 0, 0, 0);
    bool ok = (sum == 0xffffU);
    CPPUNIT_ASSERT(ok);

    unsigned short u16[4] = {1U, 2U, 0xfffeU, 0xfffeU};
    sum = U16::checksum(u16, 2, u16, 2);
    ok = (sum == 0xfff9U);
    CPPUNIT_ASSERT(ok);

    sum = U16::checksum(u16, 1, u16 + 1, 2);
    ok = (sum == 0xfffdU);
    CPPUNIT_ASSERT(ok);

    sum = U16::checksum(u16, 2, u16 + 2, 2);
    ok = (sum == 0xfffeU);
    CPPUNIT_ASSERT(ok);
}


void U16Suite::testCtor00()
{
    String s0;
    bool ok = true;
    U16::item_t defaultV = 12345;
    for (unsigned int i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        s0 = r.s;
        U16 u16a(r.s);
        U16 u16b(u16a);
        U16 u16c(s0);
        U16 u16d(s0.ascii(), s0.length());
        U16 u16e(s0.ascii(), s0.length() + 1);
        U16 u16f(&s0, defaultV);
        if ((u16a != r.v) || (u16b != r.v) || (u16c != r.v) || (u16d != r.v) || (u16e != r.v) || (u16f != r.v))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void U16Suite::testToDigits00()
{
    bool ok = true;
    const char* good = "65535";
    unsigned short u16 = 65535U;
    for (size_t numDigits = 5; u16 > 0; --numDigits, u16 /= 10)
    {
        char digit[U16::MaxDigits] = {0};
        if ((U16::toDigits(u16, digit) != numDigits) || (memcmp(digit, good, numDigits) != 0))
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
}


void U16Suite::testToXdigits00()
{
    char xdigit[U16::NumXdigits] = {0};
    unsigned short u16 = 0xDCBAu;
    U16::toXDIGITS(u16, xdigit);
    bool ok = (memcmp(xdigit, "DCBA", U16::NumXdigits) == 0);
    CPPUNIT_ASSERT(ok);

    u16 = 0xabcdU;
    U16::toXdigits(u16, xdigit);
    ok = (memcmp(xdigit, "abcd", U16::NumXdigits) == 0);
    CPPUNIT_ASSERT(ok);
}
