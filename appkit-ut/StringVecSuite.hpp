#ifndef STRING_VEC_SUITE_HPP
#define STRING_VEC_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class StringVecSuite: public CppUnit::TestFixture
{

public:
    StringVecSuite();

    virtual ~StringVecSuite();

private:
    CPPUNIT_TEST_SUITE(StringVecSuite);
    CPPUNIT_TEST(testAdd00);
    CPPUNIT_TEST(testAdd01);
    CPPUNIT_TEST(testAdd02);
    CPPUNIT_TEST(testAdd03);
    CPPUNIT_TEST(testAdd04);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testFind00);
    CPPUNIT_TEST(testFindMaxLength00);
    CPPUNIT_TEST(testFindMinLength00);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST(testOp01);
    CPPUNIT_TEST(testOp02);
    CPPUNIT_TEST(testOp03);
    CPPUNIT_TEST(testResize00);
    CPPUNIT_TEST(testRm00);
    CPPUNIT_TEST(testRm01);
    CPPUNIT_TEST(testSort00);
    CPPUNIT_TEST(testSort01);
    CPPUNIT_TEST_SUITE_END();

    StringVecSuite(const StringVecSuite&); //prohibit usage
    const StringVecSuite& operator =(const StringVecSuite&); //prohibit usage

    void testAdd00();
    void testAdd01();
    void testAdd02();
    void testAdd03();
    void testAdd04();
    void testCtor00();
    void testCtor01();
    void testFind00();
    void testFindMaxLength00();
    void testFindMinLength00();
    void testOp00();
    void testOp01();
    void testOp02();
    void testOp03();
    void testResize00();
    void testRm00();
    void testRm01();
    void testSort00();
    void testSort01();

    static int compareNumericStrings(const void*, const void*);

};

#endif
