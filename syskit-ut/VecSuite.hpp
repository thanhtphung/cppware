#ifndef VEC_SUITE_HPP
#define VEC_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class VecSuite: public CppUnit::TestFixture
{

public:
    VecSuite();

    virtual ~VecSuite();

private:
    CPPUNIT_TEST_SUITE(VecSuite);
    CPPUNIT_TEST(testAdd00);
    CPPUNIT_TEST(testAdd01);
    CPPUNIT_TEST(testAdd02);
    CPPUNIT_TEST(testAdd03);
    CPPUNIT_TEST(testAdd04);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testFind00);
    CPPUNIT_TEST(testFindKthSmallest00);
    CPPUNIT_TEST(testNew00);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST(testOp01);
    CPPUNIT_TEST(testOp02);
    CPPUNIT_TEST(testOp03);
    CPPUNIT_TEST(testResize00);
    CPPUNIT_TEST(testRm00);
    CPPUNIT_TEST(testRm01);
    CPPUNIT_TEST(testRm02);
    CPPUNIT_TEST(testSize00);
    CPPUNIT_TEST(testSort00);
    CPPUNIT_TEST_SUITE_END();

    VecSuite(const VecSuite&); //prohibit usage
    const VecSuite& operator =(const VecSuite&); //prohibit usage

    void testAdd00();
    void testAdd01();
    void testAdd02();
    void testAdd03();
    void testAdd04();
    void testCtor00();
    void testCtor01();
    void testFind00();
    void testFindKthSmallest00();
    void testNew00();
    void testOp00();
    void testOp01();
    void testOp02();
    void testOp03();
    void testResize00();
    void testRm00();
    void testRm01();
    void testRm02();
    void testSize00();
    void testSort00();

};

#endif
