#include "appkit/Str.hpp"
#include "appkit/String.hpp"

#include "appkit-ut-pch.h"
#include "StrSuite.hpp"

using namespace appkit;

const char IGNORE0[] = "should be ignored \t";
const char IGNORE1[] = "should - be - ignored,\t";

typedef struct
{
    const char* i;
    const char* o;
} stripCase_t;

typedef struct
{
    bool trimL;
    bool trimR;
    const char* i;
    const char* o;
} trimCase_t;

static const stripCase_t s_stripCase[] =
{
    {"a,b-c", "a,b-c"},
    {" -a\r\n,b    -\t\v\fc-   ", "-a,b-c-"},
    {"a,\r\n \t\v\fb    -\t\v\fc", "a,b-c"},
    {"", ""},
    {"abc", "abc"}
};
const unsigned long NUM_STRIP_CASES = sizeof(s_stripCase) / sizeof(*s_stripCase);

static const trimCase_t s_trimCase[] =
{
    {true, true, "a,b-c", "a,b-c"},
    {true, true, "", ""},
    {true, true, " \t\n\r\v\f", ""},
    {true, true, " \t\n\r\v\f\f\v\r\n\t a,b-c \t\n\r\v\f\f\v\r\n\t ", "a,b-c"},
    {false, false, " a,b-c ", " a,b-c "},
    {true, false, " \t\n\r\v\f", ""},
    {true, false, "  a,b-c  ", "a,b-c  "},
    {false, true, " \t\n\r\v\f", ""},
    {false, true, "  a,b-c  ", "  a,b-c"}
};
const unsigned long NUM_TRIM_CASES = sizeof(s_trimCase) / sizeof(*s_trimCase);


StrSuite::StrSuite()
{
}


StrSuite::~StrSuite()
{
}


void StrSuite::testCompare00()
{
    const char* item0 = "xyz";
    const char* item1 = "yzabc";
    bool ok = ((Str::compareK(item0, item1) < 0) && (Str::compareP(&item0, &item1) < 0));
    CPPUNIT_ASSERT(ok);
    ok = ((Str::compareKR(item0, item1) > 0) && (Str::comparePR(&item0, &item1) > 0));
    CPPUNIT_ASSERT(ok);

    item0 = "xy";
    item1 = "abc";
    ok = ((Str::compareK(item0, item1) > 0) && (Str::compareP(&item0, &item1) > 0));
    CPPUNIT_ASSERT(ok);
    ok = ((Str::compareKR(item0, item1) < 0) && (Str::comparePR(&item0, &item1) < 0));
    CPPUNIT_ASSERT(ok);

    item0 = "xyz123";
    item1 = "xyz123";
    ok = ((Str::compareK(item0, item1) == 0) && (Str::compareP(&item0, &item1) == 0));
    CPPUNIT_ASSERT(ok);
    ok = ((Str::compareKR(item0, item1) == 0) && (Str::comparePR(&item0, &item1) == 0));
    CPPUNIT_ASSERT(ok);
}


void StrSuite::testCompare01()
{
    const char* item0 = "XyZ";
    const char* item1 = "yZaBc";
    bool ok = ((Str::compareKI(item0, item1) < 0) && (Str::comparePI(&item0, &item1) < 0));
    CPPUNIT_ASSERT(ok);
    ok = ((Str::compareKIR(item0, item1) > 0) && (Str::comparePIR(&item0, &item1) > 0));
    CPPUNIT_ASSERT(ok);

    item0 = "Xy";
    item1 = "ABC";
    ok = ((Str::compareKI(item0, item1) > 0) && (Str::comparePI(&item0, &item1) > 0));
    CPPUNIT_ASSERT(ok);
    ok = ((Str::compareKIR(item0, item1) < 0) && (Str::comparePIR(&item0, &item1) < 0));
    CPPUNIT_ASSERT(ok);

    item0 = "xyZ123";
    item1 = "XYz123";
    ok = ((Str::compareKI(item0, item1) == 0) && (Str::comparePI(&item0, &item1) == 0));
    CPPUNIT_ASSERT(ok);
    ok = ((Str::compareKIR(item0, item1) == 0) && (Str::comparePIR(&item0, &item1) == 0));
    CPPUNIT_ASSERT(ok);
}


void StrSuite::testCompare02()
{
    const char* item0 = "XyZ123456789";
    const char* item1 = "yZa123456789";
    bool ok = (Str::compareKIN(item0, item1, 99) < 0);
    CPPUNIT_ASSERT(ok);

    item0 = "XyA";
    item1 = "XBC";
    ok = (Str::compareKIN(item0, item1, 2) > 0);
    CPPUNIT_ASSERT(ok);

    item0 = "xyZ123";
    item1 = "XYz456";
    ok = (Str::compareKIN(item0, item1, 3) == 0);
    CPPUNIT_ASSERT(ok);

    item0 = "xyZ123";
    item1 = "XYz123";
    ok = (Str::compareKIN(item0, item1, 7) == 0);
    CPPUNIT_ASSERT(ok);

    item0 = "xyZ123";
    item1 = "XYz123ABC";
    ok = (Str::compareKIN(item0, item1, 9) < 0);
    CPPUNIT_ASSERT(ok);
}


void StrSuite::testStrcasestr00()
{
    const char* haystack = "HaystackHaystackNeedleHaystack";
    const char* found = appkit::strcasestr(haystack, "needle");
    bool ok = (found == haystack + 16);
    CPPUNIT_ASSERT(ok);

    found = appkit::strcasestr(haystack, "x" "haystack");
    ok = (found == 0);
    CPPUNIT_ASSERT(ok);

    found = appkit::strcasestr(haystack, "HaystackHaystackNeedleHaystack" "x");
    ok = (found == 0);
    CPPUNIT_ASSERT(ok);

    found = appkit::strcasestr(haystack, "needlehaystack" "x");
    ok = (found == 0);
    CPPUNIT_ASSERT(ok);

    found = appkit::strcasestr(haystack, "haystackhaystackneedle");
    ok = (found == haystack);
    CPPUNIT_ASSERT(ok);
}


void StrSuite::testStripSpace00()
{
    String result;
    bool ok = true;
    for (unsigned long i = 0; i < NUM_STRIP_CASES; ++i)
    {
        const stripCase_t* p = &s_stripCase[i];
        String* rc = &Str::stripSpace(result, p->i);
        if ((rc != &result) || (result != p->o))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void StrSuite::testStripSpace01()
{
    char s[127 + 1];
    String result;
    bool ok = true;
    for (unsigned long i = 0; i < NUM_STRIP_CASES; ++i)
    {
        const stripCase_t* p = &s_stripCase[i];
        size_t length = strlen(p->i);
        memcpy(s, p->i, length);
        memcpy(s + length, IGNORE0, sizeof(IGNORE0));
        String* rc = &Str::stripSpace(result, s, length);
        if ((rc != &result) || (result != p->o))
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
}


void StrSuite::testStripSpace02()
{
    char s[63 + 1];
    bool ok = true;
    for (unsigned long i = 0; i < NUM_STRIP_CASES; ++i)
    {
        const stripCase_t* p = &s_stripCase[i];
        size_t length = strlen(p->i);
        memcpy(s, p->i, length + 1);
        if ((Str::stripSpace(s) != s) || (strcmp(s, p->o) != 0))
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
}


void StrSuite::testTrimSpace00()
{
    String result;
    bool ok = true;
    for (unsigned long i = 0; i < NUM_TRIM_CASES; ++i)
    {
        const trimCase_t* p = &s_trimCase[i];
        String* rc = &Str::trimSpace(result, p->i, p->trimL, p->trimR);
        if ((rc != &result) || (result != p->o))
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
}


void StrSuite::testTrimSpace01()
{
    char s[127 + 1];
    String result;
    bool ok = true;
    for (unsigned long i = 0; i < NUM_TRIM_CASES; ++i)
    {
        const trimCase_t* p = &s_trimCase[i];
        size_t length = strlen(p->i);
        memcpy(s, p->i, length);
        memcpy(s + length, IGNORE1, sizeof(IGNORE1));
        String* rc = &Str::trimSpace(result, s, length, p->trimL, p->trimR);
        if ((rc != &result) || (result != p->o))
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
}
