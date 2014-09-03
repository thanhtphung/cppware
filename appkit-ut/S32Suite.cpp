#include <ctype.h>
#include "appkit/S32.hpp"

#include "appkit-ut-pch.h"
#include "S32Suite.hpp"

using namespace appkit;

typedef struct
{
    const char* s;
    S32::item_t v;
} sample_t;

const sample_t SAMPLE[] =
{
    {"0", 0},
    {"12", 12},
    {"-123", -123},
    {"+1234", +1234},
    {"0x12345", 0x12345},
    {"-0x123456", -0x123456},
    {"+0X7bcdef01", +0X7bcdef01},
    {"01234567", 01234567},
    {"-076", -076},
    {"+0765", +0765},
    {"0x000000000000000000000000000000000001234ABCD", 0x1234ABCD},
    {"0000000000000000000000000000000000076543210", 076543210}
};

const size_t NUM_SAMPLES = sizeof(SAMPLE) / sizeof(SAMPLE[0]);


S32Suite::S32Suite()
{
}


S32Suite::~S32Suite()
{
}


void S32Suite::testCtor00()
{
    String s0;
    bool ok = true;
    S32::item_t defaultV = 12345;
    for (unsigned int i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        s0 = r.s;
        S32 s32a(r.s);
        S32 s32b(s32a);
        S32 s32c(s0);
        S32 s32d(s0.ascii(), s0.length());
        S32 s32e(s0.ascii(), s0.length() + 1);
        S32 s32f(&s0, defaultV);
        if ((s32a != r.v) || (s32b != r.v) || (s32c != r.v) || (s32d != r.v) || (s32e != r.v) || (s32f != r.v))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void S32Suite::testEof00()
{
    bool ok = (EOF == -1);
    CPPUNIT_ASSERT(ok);
}


void S32Suite::testIsXxx00()
{
    bool ok = true;
    for (int i = -1; i <= 255; ++i)
    {
        if (((isalnum(i) != 0) != S32::isAlnum(i)) ||
            ((isalpha(i) != 0) != S32::isAlpha(i)) ||
            ((isascii(i) != 0) != S32::isAscii(i)) ||
            ((iscntrl(i) != 0) != S32::isCntrl(i)) ||
            ((isdigit(i) != 0) != S32::isDigit(i)) ||
            ((isgraph(i) != 0) != S32::isGraph(i)) ||
            ((islower(i) != 0) != S32::isLower(i)) ||
            ((isprint(i) != 0) != S32::isPrint(i)) ||
            ((ispunct(i) != 0) != S32::isPunct(i)) ||
            ((isspace(i) != 0) != S32::isSpace(i)) ||
            ((isupper(i) != 0) != S32::isUpper(i)) ||
            ((isxdigit(i) != 0) != S32::isXdigit(i)))
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
}


void S32Suite::testToXxx00()
{
    bool ok = true;
    for (int i = -1; i <= 255; ++i)
    {
        if ((tolower(i) != S32::toLower(i)) || (toupper(i) != S32::toUpper(i)))
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
}
