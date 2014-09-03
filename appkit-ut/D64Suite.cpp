#include "appkit/D64.hpp"

#include "appkit-ut-pch.h"
#include "D64Suite.hpp"

using namespace appkit;

typedef struct
{
    const char* s;
    D64::item_t v;
} sample_t;

const sample_t SAMPLE[] =
{
    {"0", 0.0},
    {"1.2", 1.2},
    {"-1.23", -1.23},
    {"0.123456789012345", 0.123456789012345},
    {"2.22044604925031e-016", 2.22044604925031e-016 /*~DBL_EPSILON*/},
    {"1.79769313486231e+308", 1.79769313486231e+308 /*~DBL_MAX*/},
    {"2.2250738585072e-308", 2.2250738585072e-308 /*~DBL_MIN*/},
    {"-9e+111", -9e+111},
    {"4.56e-123", 4.56e-123},
    {"1.23E4", 1.23e4},
    {"+123.", 123.0},
    {".123", 0.123}
};

const size_t NUM_SAMPLES = sizeof(SAMPLE) / sizeof(SAMPLE[0]);


D64Suite::D64Suite()
{
}


D64Suite::~D64Suite()
{
}


void D64Suite::testCtor00()
{
    D64 item0;
    D64 item1;
    bool ok = (item0.asReal() == 0.0);
    CPPUNIT_ASSERT(ok);

    item0 = SAMPLE[3].v;
    ok = (item0 == SAMPLE[3].v);
    CPPUNIT_ASSERT(ok);

    item1 = item0;
    ok = (item1 == item0);
    CPPUNIT_ASSERT(ok);
}


void D64Suite::testCtor01()
{
    const char* s0 = 0;
    size_t length = 0;
    D64 item0(s0, length);
    bool ok = (item0 == 0.0);
    CPPUNIT_ASSERT(ok);

    size_t bytesUsed = 123U;
    D64 item1(s0, length, &bytesUsed);
    ok = ((item1 == 0.0) && (bytesUsed == 0));
    CPPUNIT_ASSERT(ok);

    String s1;
    D64::item_t defaultV = 1.23;
    D64 item2(&s1, defaultV);
    ok = (item2 == 1.23);
    CPPUNIT_ASSERT(ok);

    const String* s2 = 0;
    defaultV = 4.56;
    item2 = D64(s2, defaultV);
    ok = (item2 == 4.56);
    CPPUNIT_ASSERT(ok);
}


void D64Suite::testCtor02()
{
    String s0;
    bool ok = true;
    D64::item_t defaultV = 1.2345;
    for (unsigned int i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        s0 = r.s;
        D64 item0(r.s);
        D64 item1(item0);
        D64 item2(s0);
        D64 item3(s0.ascii(), s0.length());
        D64 item4(s0.ascii(), s0.length() + 1);
        D64 item5(&s0, defaultV);
        if ((item0 != r.v) || (item1 != r.v) || (item2 != r.v) || (item3 != r.v) || (item4 != r.v) || (item5 != r.v))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void D64Suite::testDoublet00()
{
    String s("|0.1 22.345|");
    D64::Doublet doublet(s);
    bool ok = (doublet[0] == 0.1) && (doublet[1] == 22.345);
    CPPUNIT_ASSERT(ok);

    ok = (s == doublet);
    CPPUNIT_ASSERT(ok);

    doublet[0] = -6.7;
    s = doublet.toString('(');
    ok = (s == "(-6.7 22.345)");
    CPPUNIT_ASSERT(ok);
    doublet = D64::Doublet(" [ -6.7   22345e-3  ] ");
    ok = (s == doublet.toString(')'));
    CPPUNIT_ASSERT(ok);

    doublet[1] = 0.123456;
    s = doublet.toString(']');
    ok = (s == "[-6.7 0.123456]");
    CPPUNIT_ASSERT(ok);
    doublet = D64::Doublet("-6.7 0.123456");
    ok = (s == doublet.toString('['));
    CPPUNIT_ASSERT(ok);

    D64::Vec vec(s);
    ok = (s == vec.toString('['));
    CPPUNIT_ASSERT(ok);
    ok = (vec.numItems() == 2) && (vec[0] == doublet[0]) && (vec[1] == doublet[1]);
    CPPUNIT_ASSERT(ok);
}


void D64Suite::testIsValid00()
{
    bool ok = true;
    for (unsigned int i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        if (!D64::isValid(r.s))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    const char* s = "";
    ok = (!D64::isValid(s));
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - D64::operator String() const;
//
void D64Suite::testOp00()
{
    D64 item;
    String s;
    bool ok = true;
    for (unsigned int i = 0; i < 9; ++i)
    {
        const sample_t& r = SAMPLE[i];
        item = r.v;
        s = item;
        if (s != r.s)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    double real = 0.123456789012345;
    ok = (D64::compareP(&SAMPLE[3].v, &real) == 0);
    CPPUNIT_ASSERT(ok);
    ok = (D64::compareK((void*)(long long)(SAMPLE[3].v), (void*)(long long)(real)) == 0);
    CPPUNIT_ASSERT(ok);
}


void D64Suite::testTriplet00()
{
    String s("|6789 0.1 22.345|");
    D64::Triplet triplet(s);
    bool ok = (triplet[0] == 6789.0) && (triplet[1] == 0.1) && (triplet[2] == 22.345);
    CPPUNIT_ASSERT(ok);

    ok = (s == triplet);
    CPPUNIT_ASSERT(ok);

    triplet[0] = 1;
    triplet[1] = 2;
    triplet[2] = 3;
    s = triplet.toString('<');
    ok = (s == "<1 2 3>");
    CPPUNIT_ASSERT(ok);
    triplet = D64::Triplet(" { 1. 2. 3.} ");
    ok = (s == triplet.toString('>'));
    CPPUNIT_ASSERT(ok);

    D64::Vec vec(s);
    ok = (s == vec.toString('>'));
    CPPUNIT_ASSERT(ok);
    ok = (vec.numItems() == 3) && (vec[0] == triplet[0]) && (vec[1] == triplet[1]) && (vec[2] == triplet[2]);
    CPPUNIT_ASSERT(ok);
}


void D64Suite::testQuartet00()
{
    String s("\"6789 0.1 22.345  9e-11 \" ");
    D64::Quartet quartet(s);
    bool ok = (quartet[0] == 6789.0) && (quartet[1] == 0.1) && (quartet[2] == 22.345) && (quartet[3] == 9e-11);
    CPPUNIT_ASSERT(ok);

    s = "6789 0.1 22.345 9e-011";
    ok = (s == quartet.toString(0));
    CPPUNIT_ASSERT(ok);

    s = "\'6789 0.1 22.345 9e-011\'";
    quartet = D64::Quartet(s);
    ok = (s == quartet.toString('\''));
    CPPUNIT_ASSERT(ok);

    D64::Vec vec(s);
    ok = (s == vec.toString('\''));
    CPPUNIT_ASSERT(ok);
    ok = (vec.numItems() == 4) && (vec[0] == quartet[0]) && (vec[1] == quartet[1]) && (vec[2] == quartet[2]) && (vec[3] == quartet[3]);
    CPPUNIT_ASSERT(ok);
}
