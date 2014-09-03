#include "appkit/F32.hpp"

#include "appkit-ut-pch.h"
#include "F32Suite.hpp"

using namespace appkit;

typedef struct
{
    const char* s;
    F32::item_t v;
} sample_t;

const sample_t SAMPLE[] =
{
    {"0", 0.0f},
    {"1.2", 1.2f},
    {"-1.23", -1.23f},
    {"0.123456", 0.123456f},
    {"1.19209e-007", 1.19209e-07f /*~FLT_EPSILON*/},
    {"3.40282e+038", 3.40282e+38f /*~FLT_MAX*/},
    {"1.17549e-038", 1.17549e-38f /*~FLT_MIN*/},
    {"-9e+011", -9e+11f},
    {"4.56e-023", 4.56e-23f},
    {"1.23E4", 1.23e4f},
    {"+123.", 123.0f},
    {".123", 0.123f}
};

const size_t NUM_SAMPLES = sizeof(SAMPLE) / sizeof(SAMPLE[0]);


F32Suite::F32Suite()
{
}


F32Suite::~F32Suite()
{
}


void F32Suite::testCtor00()
{
    F32 item0;
    F32 item1;
    bool ok = (item0.asReal() == 0.0);
    CPPUNIT_ASSERT(ok);

    item0 = SAMPLE[3].v;
    ok = (item0 == SAMPLE[3].v);
    CPPUNIT_ASSERT(ok);

    item1 = item0;
    ok = (item1 == item0);
    CPPUNIT_ASSERT(ok);
}


void F32Suite::testCtor01()
{
    const char* s0 = 0;
    size_t length = 0;
    F32 item0(s0, length);
    bool ok = (item0 == 0.0);
    CPPUNIT_ASSERT(ok);

    size_t bytesUsed = 123U;
    F32 item1(s0, length, &bytesUsed);
    ok = ((item1 == 0.0) && (bytesUsed == 0));
    CPPUNIT_ASSERT(ok);

    String s1;
    F32::item_t defaultV = 1.23f;
    F32 item2(&s1, defaultV);
    ok = (item2 == 1.23f);
    CPPUNIT_ASSERT(ok);

    const String* s2 = 0;
    defaultV = 4.56f;
    item2 = F32(s2, defaultV);
    ok = (item2 == 4.56f);
    CPPUNIT_ASSERT(ok);
}


void F32Suite::testCtor02()
{
    String s0;
    bool ok = true;
    F32::item_t defaultV = 1.2345f;
    for (unsigned int i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        s0 = r.s;
        F32 item0(r.s);
        F32 item1(item0);
        F32 item2(s0);
        F32 item3(s0.ascii(), s0.length());
        F32 item4(s0.ascii(), s0.length() + 1);
        F32 item5(&s0, defaultV);
        if ((item0 != r.v) || (item1 != r.v) || (item2 != r.v) || (item3 != r.v) || (item4 != r.v) || (item5 != r.v))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void F32Suite::testDoublet00()
{
    String s("|0.1 22.345|");
    F32::Doublet doublet(s);
    bool ok = (doublet[0] == 0.1f) && (doublet[1] == 22.345f);
    CPPUNIT_ASSERT(ok);

    ok = (s == doublet);
    CPPUNIT_ASSERT(ok);

    doublet[0] = -6.7f;
    s = doublet.toString('(');
    ok = (s == "(-6.7 22.345)");
    CPPUNIT_ASSERT(ok);
    doublet = F32::Doublet(" [ -6.7   22345e-3  ] ");
    ok = (s == doublet.toString(')'));
    CPPUNIT_ASSERT(ok);

    doublet[1] = 0.123456f;
    s = doublet.toString(']');
    ok = (s == "[-6.7 0.123456]");
    CPPUNIT_ASSERT(ok);
    doublet = F32::Doublet("-6.7 0.123456");
    ok = (s == doublet.toString('['));
    CPPUNIT_ASSERT(ok);

    F32::Vec vec(s);
    ok = (s == vec.toString('['));
    CPPUNIT_ASSERT(ok);
    ok = (vec.numItems() == 2) && (vec[0] == doublet[0]) && (vec[1] == doublet[1]);
    CPPUNIT_ASSERT(ok);
}


void F32Suite::testIsValid00()
{
    bool ok = true;
    for (unsigned int i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        if (!F32::isValid(r.s))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    const char* s = "";
    ok = (!F32::isValid(s));
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - F32::operator String() const;
//
void F32Suite::testOp00()
{
    F32 item;
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

    float real = 0.123456f;
    ok = (F32::compareP(&SAMPLE[3].v, &real) == 0);
    CPPUNIT_ASSERT(ok);
    ok = (F32::compareK((void*)(int)(SAMPLE[3].v), (void*)(int)(real)) == 0);
    CPPUNIT_ASSERT(ok);
}


void F32Suite::testQuartet00()
{
    String s("\"6789 0.1 22.345  9e-11 \" ");
    F32::Quartet quartet(s);
    bool ok = (quartet[0] == 6789.0f) && (quartet[1] == 0.1f) && (quartet[2] == 22.345f) && (quartet[3] == 9e-11f);
    CPPUNIT_ASSERT(ok);

    s = "6789 0.1 22.345 9e-011";
    ok = (s == quartet.toString(0));
    CPPUNIT_ASSERT(ok);

    s = "\'6789 0.1 22.345 9e-011\'";
    quartet = F32::Quartet(s);
    ok = (s == quartet.toString('\''));
    CPPUNIT_ASSERT(ok);

    F32::Vec vec(s);
    ok = (s == vec.toString('\''));
    CPPUNIT_ASSERT(ok);
    ok = (vec.numItems() == 4) && (vec[0] == quartet[0]) && (vec[1] == quartet[1]) && (vec[2] == quartet[2]) && (vec[3] == quartet[3]);
    CPPUNIT_ASSERT(ok);
}


void F32Suite::testRound00()
{
    F32 item(12.345f);
    bool ok = (item.round() == 12);
    CPPUNIT_ASSERT(ok);

    item = 34.567f;
    ok = (item.round() == 35);
    CPPUNIT_ASSERT(ok);

    item = -34.567f;
    ok = (item.round() == -35);
    CPPUNIT_ASSERT(ok);

    item = -12.34f;
    ok = (item.round() == -12);
    CPPUNIT_ASSERT(ok);
}


void F32Suite::testTriplet00()
{
    String s("|6789 0.1 22.345|");
    F32::Triplet triplet(s);
    bool ok = (triplet[0] == 6789.0f) && (triplet[1] == 0.1f) && (triplet[2] == 22.345f);
    CPPUNIT_ASSERT(ok);

    ok = (s == triplet);
    CPPUNIT_ASSERT(ok);

    triplet[0] = 1;
    triplet[1] = 2;
    triplet[2] = 3;
    s = triplet.toString('<');
    ok = (s == "<1 2 3>");
    CPPUNIT_ASSERT(ok);
    triplet = F32::Triplet(" { 1. 2. 3.} ");
    ok = (s == triplet.toString('>'));
    CPPUNIT_ASSERT(ok);

    F32::Vec vec(s);
    ok = (s == vec.toString('>'));
    CPPUNIT_ASSERT(ok);
    ok = (vec.numItems() == 3) && (vec[0] == triplet[0]) && (vec[1] == triplet[1]) && (vec[2] == triplet[2]);
    CPPUNIT_ASSERT(ok);
}
