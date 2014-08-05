#include "appkit/U32.hpp"

#include "appkit-ut-pch.h"
#include "U32Suite.hpp"

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
    {"-123", 0UL - 123},
    {"+1234", +1234},
    {"0x12345", 0x12345},
    {"-0x123456", 0UL - 0x123456},
    {"+0Xabcdef01", +0Xabcdef01},
    {"01234567", 01234567},
    {"-076", 0UL - 076},
    {"+0765", +0765},
    {"0x000000000000000000000000000000000001234ABCD", 0x1234ABCDul},
    {"0000000000000000000000000000000000076543210", 076543210UL}
};

const unsigned long NUM_SAMPLES = sizeof(SAMPLE) / sizeof(SAMPLE[0]);


U32Suite::U32Suite()
{
}


U32Suite::~U32Suite()
{
}


void U32Suite::testCompare00()
{
    unsigned long u32A = 0x12345678UL;
    unsigned long u32B = 0xfedcba98UL;
    unsigned char* p = 0;
    const void* item0 = p + u32A;
    const void* item1 = p + u32B;
    bool ok = ((U32::compareK(item0, item1) < 0) && (U32::compareP(&u32A, &u32B) < 0));
    CPPUNIT_ASSERT(ok);
    ok = ((U32::compareKR(item0, item1) > 0) && (U32::comparePR(&u32A, &u32B) > 0));
    CPPUNIT_ASSERT(ok);

    u32A = 0xdadUL;
    u32B = 0xbadUL;
    p = 0;
    item0 = p + u32A;
    item1 = p + u32B;
    ok = ((U32::compareK(item0, item1) > 0) && (U32::compareP(&u32A, &u32B) > 0));
    CPPUNIT_ASSERT(ok);
    ok = ((U32::compareKR(item0, item1) < 0) && (U32::comparePR(&u32A, &u32B) < 0));
    CPPUNIT_ASSERT(ok);

    u32A = 0x13579bdfUL;
    u32B = 0x13579bdfUL;
    p = 0;
    item0 = p + u32A;
    item1 = p + u32B;
    ok = ((U32::compareK(item0, item1) == 0) && (U32::compareP(&u32A, &u32B) == 0));
    CPPUNIT_ASSERT(ok);
    ok = ((U32::compareKR(item0, item1) == 0) && (U32::comparePR(&u32A, &u32B) == 0));
    CPPUNIT_ASSERT(ok);
}


void U32Suite::testCtor00()
{
    U32 u32a;
    U32 u32b;
    bool ok = (u32a.asWord() == 0);
    CPPUNIT_ASSERT(ok);

    u32a = SAMPLE[3].v;
    ok = (u32a == SAMPLE[3].v);
    CPPUNIT_ASSERT(ok);

    u32b = u32a;
    ok = (u32b == u32a);
    CPPUNIT_ASSERT(ok);
}


void U32Suite::testCtor01()
{
    const char* s = 0;
    size_t length = 0;
    U32 u32a(s, length);
    bool ok = (u32a == 0);
    CPPUNIT_ASSERT(ok);
    length = 1234;
    U32 u32b(s, length);
    ok = (u32b == 0);
    CPPUNIT_ASSERT(ok);

    size_t bytesUsed = 123UL;
    U32 u32c(s, length, &bytesUsed);
    ok = ((u32c == 0) && (bytesUsed == 0));
    CPPUNIT_ASSERT(ok);
}


void U32Suite::testCtor02()
{
    String s0;
    bool ok = true;
    U32::item_t defaultV = 12345;
    for (unsigned long i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        s0 = r.s;
        U32 u32a(r.s);
        U32 u32b(u32a);
        U32 u32c(s0);
        U32 u32d(s0.ascii(), s0.length());
        U32 u32e(s0.ascii(), s0.length() + 1);
        U32 u32f(&s0, defaultV);
        if ((u32a != r.v) || (u32b != r.v) || (u32c != r.v) || (u32d != r.v) || (u32e != r.v) || (u32f != r.v))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void U32Suite::testCtor03()
{
    String s("1234x");
    U32 u32(&s, 0);
    bool ok = (u32.asWord() == 0);
    CPPUNIT_ASSERT(ok);
}


void U32Suite::testIsValid00()
{
    bool ok = true;
    for (unsigned long i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        size_t length = strlen(r.s);
        if ((!U32::isValid(r.s)) || (!U32::isValid(r.s, length)) || (!U32::isValid(r.s, length + 1)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    const char* s = 0;
    size_t length = 0;
    ok = (!U32::isValid(s, length));
    CPPUNIT_ASSERT(ok);
    length = 1234;
    ok = (!U32::isValid(s, length));
    CPPUNIT_ASSERT(ok);
}


void U32Suite::testHash00()
{
    unsigned long u32 = 396;
    size_t numBuckets = 131;
    unsigned long bucket = U32::hashP(&u32, numBuckets);
    bool ok = (bucket == 3);
    CPPUNIT_ASSERT(ok);

    u32 = 393;
    bucket = U32::hashK(reinterpret_cast<const void*>(u32), numBuckets);
    ok = (bucket == 0);
    CPPUNIT_ASSERT(ok);
}


void U32Suite::testNumDigits00()
{
    bool ok = true;
    unsigned long u32 = 1223334444UL;
    for (size_t numDigits = 10; u32 > 0; --numDigits, u32 /= 10)
    {
        if (U32::numDigits(u32) != numDigits)
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
}


void U32Suite::testToDigits00()
{
    U32 u32A;
    bool ok = true;
    const char* good = "4294967295";
    unsigned long u32B = 4294967295UL;
    for (size_t numDigits = 10; u32B > 0; --numDigits, u32B /= 10)
    {
        u32A = u32B;
        String s = u32A;
        char digit[U32::MaxDigits] = {0};
        if ((U32::toDigits(u32B, digit) != numDigits) ||
            (memcmp(digit, good, numDigits) != 0) ||
            (u32A.toString() != String(good, numDigits)) ||
            (s != String(good, numDigits)))
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
}


void U32Suite::testToXdigits00()
{
    char xdigit[U32::NumXdigits] = {0};
    U32 u32(0x10FEDCBAul);
    U32::toXDIGITS(u32, xdigit);
    bool ok = (memcmp(xdigit, "10FEDCBA", U32::NumXdigits) == 0);
    CPPUNIT_ASSERT(ok);
    ok = (u32.toXDIGITS() == "10FEDCBA");
    CPPUNIT_ASSERT(ok);

    u32 = 0xabcdef01UL;
    U32::toXdigits(u32, xdigit);
    ok = (memcmp(xdigit, "abcdef01", U32::NumXdigits) == 0);
    CPPUNIT_ASSERT(ok);
    ok = (u32.toXdigits() == "abcdef01");
    CPPUNIT_ASSERT(ok);
}
