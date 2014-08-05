#ifndef U16_SET_SUITE_HPP
#define U16_SET_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(appkit, U16Set)


class U16SetSuite: public CppUnit::TestFixture
{

public:
    U16SetSuite();

    virtual ~U16SetSuite();

private:
    CPPUNIT_TEST_SUITE(U16SetSuite);
    CPPUNIT_TEST(testAdd00);
    CPPUNIT_TEST(testAdd01);
    CPPUNIT_TEST(testAdd02);
    CPPUNIT_TEST(testAdd03);
    CPPUNIT_TEST(testAdd04);
    CPPUNIT_TEST(testAdd05);
    CPPUNIT_TEST(testCmp00);
    CPPUNIT_TEST(testCmp01);
    CPPUNIT_TEST(testCmp02);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testCtor03);
    CPPUNIT_TEST(testCtor04);
    CPPUNIT_TEST(testItor00);
    CPPUNIT_TEST(testItor01);
    CPPUNIT_TEST(testItor02);
    CPPUNIT_TEST(testItor03);
    CPPUNIT_TEST(testNew00);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST(testOp01);
    CPPUNIT_TEST(testOp02);
    CPPUNIT_TEST(testOp03);
    CPPUNIT_TEST(testOp04);
    CPPUNIT_TEST(testOp05);
    CPPUNIT_TEST(testRm00);
    CPPUNIT_TEST(testRm01);
    CPPUNIT_TEST(testRm02);
    CPPUNIT_TEST(testSize00);
    CPPUNIT_TEST_SUITE_END();

    U16SetSuite(const U16SetSuite&); //prohibit usage
    const U16SetSuite& operator =(const U16SetSuite&); //prohibit usage

    void testAdd00();
    void testAdd01();
    void testAdd02();
    void testAdd03();
    void testAdd04();
    void testAdd05();
    void testCmp00();
    void testCmp01();
    void testCmp02();
    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testCtor03();
    void testCtor04();
    void testItor00();
    void testItor01();
    void testItor02();
    void testItor03();
    void testNew00();
    void testOp00();
    void testOp01();
    void testOp02();
    void testOp03();
    void testOp04();
    void testOp05();
    void testRm00();
    void testRm01();
    void testRm02();
    void testSize00();
    void validateSet(const appkit::U16Set&);

};

#endif
