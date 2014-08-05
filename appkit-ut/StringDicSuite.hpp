#ifndef STRING_DIC_SUITE_HPP
#define STRING_DIC_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(appkit, String)


class StringDicSuite: public CppUnit::TestFixture
{

public:
    StringDicSuite();

    virtual ~StringDicSuite();

private:
    CPPUNIT_TEST_SUITE(StringDicSuite);
    CPPUNIT_TEST(testAdd00);
    CPPUNIT_TEST(testAdd01);
    CPPUNIT_TEST(testApply00);
    CPPUNIT_TEST(testAssociate00);
    CPPUNIT_TEST(testAssociate01);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST(testOp01);
    CPPUNIT_TEST(testRm00);
    CPPUNIT_TEST(testRm01);
    CPPUNIT_TEST_SUITE_END();

    void testAdd00();
    void testAdd01();
    void testApply00();
    void testAssociate00();
    void testAssociate01();
    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testOp00();
    void testOp01();
    void testRm00();
    void testRm01();

    static bool cb0a(void*, const appkit::String&, appkit::String&);
    static bool cb1a(void*, const appkit::String&, const appkit::String&);

};

#endif
