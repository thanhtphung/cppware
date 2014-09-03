#ifndef DELIMITED_TXT_SUITE_HPP
#define DELIMITED_TXT_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(appkit, String)


class DelimitedTxtSuite: public CppUnit::TestFixture
{

public:
    DelimitedTxtSuite();

    virtual ~DelimitedTxtSuite();

private:
    CPPUNIT_TEST_SUITE(DelimitedTxtSuite);
    CPPUNIT_TEST(testApply00);
    CPPUNIT_TEST(testApply01);
    CPPUNIT_TEST(testApply03);
    CPPUNIT_TEST(testApply04);
    CPPUNIT_TEST(testCount00);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testNext00);
    CPPUNIT_TEST(testNext01);
    CPPUNIT_TEST(testNext02);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST(testPeek00);
    CPPUNIT_TEST(testPeek01);
    CPPUNIT_TEST(testPeek02);
    CPPUNIT_TEST(testPeek03);
    CPPUNIT_TEST(testPeek04);
    CPPUNIT_TEST(testPeek05);
    CPPUNIT_TEST(testPrev00);
    CPPUNIT_TEST(testPrev01);
    CPPUNIT_TEST(testPrev02);
    CPPUNIT_TEST(testTrim00);
    CPPUNIT_TEST(testTrim01);
    CPPUNIT_TEST(testVectorize00);
    CPPUNIT_TEST(testVectorize01);
    CPPUNIT_TEST_SUITE_END();

    void testApply00();
    void testApply01();
    void testApply03();
    void testApply04();
    void testCount00();
    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testNext00();
    void testNext01();
    void testNext02();
    void testOp00();
    void testPeek00();
    void testPeek01();
    void testPeek02();
    void testPeek03();
    void testPeek04();
    void testPeek05();
    void testPrev00();
    void testPrev01();
    void testPrev02();
    void testTrim00();
    void testTrim01();
    void testVectorize00();
    void testVectorize01();

    static bool cb0a(void*, const appkit::String&);
    static bool cb0b(void*, const appkit::String&);
    static bool cb1a(void*, const char*, size_t);
    static bool cb1b(void*, const char*, size_t);
    static bool cb1c(void*, const char*, size_t);
    static bool cb1d(void*, const char*, size_t);
    static void cb3a(void*, const appkit::String&);
    static void cb3b(void*, const appkit::String&);
    static void cb4a(void*, const char*, size_t);
    static void cb4b(void*, const char*, size_t);

    static appkit::String formExpectedLine(size_t);

};

#endif
