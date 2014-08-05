#ifndef TOKENIZER_SUITE_HPP
#define TOKENIZER_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(appkit, String)


class TokenizerSuite: public CppUnit::TestFixture
{

public:
    TokenizerSuite();

    virtual ~TokenizerSuite();

private:
    CPPUNIT_TEST_SUITE(TokenizerSuite);
    CPPUNIT_TEST(testApply00);
    CPPUNIT_TEST(testCountTokens00);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testCtor03);
    CPPUNIT_TEST(testCtor04);
    CPPUNIT_TEST(testNext00);
    CPPUNIT_TEST(testNext01);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST(testPeek00);
    CPPUNIT_TEST(testPeek01);
    CPPUNIT_TEST(testSetDelim00);
    CPPUNIT_TEST(testVectorize00);
    CPPUNIT_TEST_SUITE_END();

    TokenizerSuite(const TokenizerSuite&); //prohibit usage
    const TokenizerSuite& operator =(const TokenizerSuite&); //prohibit usage

    void testApply00();
    void testCountTokens00();
    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testCtor03();
    void testCtor04();
    void testNext00();
    void testNext01();
    void testOp00();
    void testPeek00();
    void testPeek01();
    void testSetDelim00();
    void testVectorize00();

    static bool cb0a(void*, const appkit::String&);
    static bool cb0b(void*, const appkit::String&);
    static bool cb0c(void*, const appkit::String&);
    static bool cb0d(void*, const appkit::String&);
    static void cb1a(void*, const appkit::String&);
    static void cb1b(void*, const appkit::String&);

};

#endif
