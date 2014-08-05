#include "appkit/U64.hpp"

#include "appkit-ut-pch.h"
#include "U64Suite.hpp"

using namespace appkit;

typedef struct
{
    const char* s;
    unsigned long long v;
} sample_t;

const sample_t SAMPLE[] =
{
    {"0", 0},
    {"12", 12},
    {"-123", 0ULL - 123},
    {"+1234", +1234},
    {"0xabc0000912345", 0xabc0000912345ULL},
    {"-0x123456", 0ULL - 0x123456},
    {"+0Xabcdef01", +0Xabcdef01},
    {"01234567", 01234567},
    {"-076", 0ULL - 076},
    {"+0765", +0765},
    {"0x000000000000000000000000000000000001234ABCD", 0x1234ABCDull},
    {"0000000000000000000000000000000000076543210", 076543210ULL}
};

const unsigned long NUM_SAMPLES = sizeof(SAMPLE) / sizeof(SAMPLE[0]);


U64Suite::U64Suite()
{
}


U64Suite::~U64Suite()
{
}


void U64Suite::testBswap00()
{
    U64::item_t item0[32];
    U64::item_t item1[32];
    for (U64::item_t i = 0; i < 32; ++i)
    {
        item0[i] = ((255 - i) << 8) | i;
    }
    U64::bswap(item0, 32, item1);
    U64::bswap(item0, 32);
    bool ok = (memcmp(item0, item1, sizeof(item0)) == 0);
    CPPUNIT_ASSERT(ok);

    for (U64::item_t i = 0; i < 32; ++i)
    {
        U64::item_t item = (i << 56) | ((255 - i) << 48);
        if (item0[i] != item)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void U64Suite::testCompare00()
{
    unsigned long long u64A = 0x12345678ULL;
    unsigned long long u64B = 0xfedcba98ULL;
    unsigned char* p = 0;
    const void* item0 = p + u64A;
    const void* item1 = p + u64B;
    bool ok = ((U64::compareK(item0, item1) < 0) && (U64::compareP(&u64A, &u64B) < 0));
    CPPUNIT_ASSERT(ok);
    ok = ((U64::compareKR(item0, item1) > 0) && (U64::comparePR(&u64A, &u64B) > 0));
    CPPUNIT_ASSERT(ok);

    u64A = 0xdadULL;
    u64B = 0xbadULL;
    p = 0;
    item0 = p + u64A;
    item1 = p + u64B;
    ok = ((U64::compareK(item0, item1) > 0) && (U64::compareP(&u64A, &u64B) > 0));
    CPPUNIT_ASSERT(ok);
    ok = ((U64::compareKR(item0, item1) < 0) && (U64::comparePR(&u64A, &u64B) < 0));
    CPPUNIT_ASSERT(ok);

    u64A = 0x13579bdfULL;
    u64B = 0x13579bdfULL;
    p = 0;
    item0 = p + u64A;
    item1 = p + u64B;
    ok = ((U64::compareK(item0, item1) == 0) && (U64::compareP(&u64A, &u64B) == 0));
    CPPUNIT_ASSERT(ok);
    ok = ((U64::compareKR(item0, item1) == 0) && (U64::comparePR(&u64A, &u64B) == 0));
    CPPUNIT_ASSERT(ok);
}


void U64Suite::testCtor00()
{
    U64 u32a;
    U64 u32b;
    bool ok = (u32a.asWord() == 0);
    CPPUNIT_ASSERT(ok);

    u32a = SAMPLE[3].v;
    ok = (u32a == SAMPLE[3].v);
    CPPUNIT_ASSERT(ok);

    u32b = u32a;
    ok = (u32b == u32a);
    CPPUNIT_ASSERT(ok);
}


void U64Suite::testCtor01()
{
    const char* s = 0;
    size_t length = 0;
    U64 u32a(s, length);
    bool ok = (u32a == 0);
    CPPUNIT_ASSERT(ok);
    length = 1234;
    U64 u32b(s, length);
    ok = (u32b == 0);
    CPPUNIT_ASSERT(ok);

    size_t bytesUsed = 123ULL;
    U64 u32c(s, length, &bytesUsed);
    ok = ((u32c == 0) && (bytesUsed == 0));
    CPPUNIT_ASSERT(ok);
}


void U64Suite::testCtor02()
{
    String s0;
    bool ok = true;
    U64::item_t defaultV = 12345;
    for (unsigned long i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        s0 = r.s;
        U64 u32a(r.s);
        U64 u32b(u32a);
        U64 u32c(s0);
        U64 u32d(s0.ascii(), s0.length());
        U64 u32e(s0.ascii(), s0.length() + 1);
        U64 u32f(&s0, defaultV);
        if ((u32a != r.v) || (u32b != r.v) || (u32c != r.v) || (u32d != r.v) || (u32e != r.v) || (u32f != r.v))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void U64Suite::testHash00()
{
    U64::item_t u64 = 0xf415b1e43ULL;
    size_t numBuckets = 65521UL;
    unsigned long bucket = U64::hashP(&u64, numBuckets);
    bool ok = (bucket == 3);
    CPPUNIT_ASSERT(ok);

    u64 = 0x415b1e40ULL;
    bucket = U64::hashK(reinterpret_cast<const void*>(u64), numBuckets);
    ok = (bucket == 62146);
    CPPUNIT_ASSERT(ok);
}


void U64Suite::testIsValid00()
{
    bool ok = true;
    for (unsigned long i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        size_t length = strlen(r.s);
        if ((!U64::isValid(r.s)) || (!U64::isValid(r.s, length)) || (!U64::isValid(r.s, length + 1)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    const char* s = 0;
    size_t length = 0;
    ok = (!U64::isValid(s, length));
    CPPUNIT_ASSERT(ok);
    length = 1234;
    ok = (!U64::isValid(s, length));
    CPPUNIT_ASSERT(ok);
}


void U64Suite::testNumDigits00()
{
    bool ok = true;
    unsigned long long u64 = 1223334444ULL;
    for (size_t numDigits = 10; u64 > 0; --numDigits, u64 /= 10)
    {
        if (U64::numDigits(u64) != numDigits)
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
}


void U64Suite::testToDigits00()
{
    U64 u64A;
    bool ok = true;
    const char* good = "4294967295";
    unsigned long u64B = 4294967295ULL;
    for (size_t numDigits = 10; u64B > 0; --numDigits, u64B /= 10)
    {
        u64A = u64B;
        String s = u64A;
        char digit[U64::MaxDigits] = {0};
        if ((U64::toDigits(u64B, digit) != numDigits) ||
            (memcmp(digit, good, numDigits) != 0) ||
            (u64A.toString() != String(good, numDigits)) ||
            (s != String(good, numDigits)))
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
}


void U64Suite::testToXdigits00()
{
    char xdigit[U64::NumXdigits] = {0};
    U64 u64(0xabc0000910FEDCBAull);
    U64::toXDIGITS(u64, xdigit);
    bool ok = (memcmp(xdigit, "ABC0000910FEDCBA", U64::NumXdigits) == 0);
    CPPUNIT_ASSERT(ok);
    ok = (u64.toXDIGITS() == "ABC0000910FEDCBA");
    CPPUNIT_ASSERT(ok);

    u64 = 0x1230000fabcdef01ULL;
    U64::toXdigits(u64, xdigit);
    ok = (memcmp(xdigit, "1230000fabcdef01", U64::NumXdigits) == 0);
    CPPUNIT_ASSERT(ok);
    ok = (u64.toXdigits() == "1230000fabcdef01");
    CPPUNIT_ASSERT(ok);
}
