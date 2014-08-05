#ifndef F32_VEC_SUITE_HPP
#define F32_VEC_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class F32VecSuite: public CppUnit::TestFixture
{

public:
    F32VecSuite();

    virtual ~F32VecSuite();

private:
    CPPUNIT_TEST_SUITE(F32VecSuite);
    CPPUNIT_TEST(testAdd00);
    CPPUNIT_TEST(testAdd01);
    CPPUNIT_TEST(testAdd02);
    CPPUNIT_TEST(testAdd03);
    CPPUNIT_TEST(testAdd04);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testFind00);
    CPPUNIT_TEST(testFindKthSmallest00);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST(testOp01);
    CPPUNIT_TEST(testOp02);
    CPPUNIT_TEST(testOp03);
    CPPUNIT_TEST(testResize00);
    CPPUNIT_TEST(testRm00);
    CPPUNIT_TEST(testSort00);
    CPPUNIT_TEST(testSort01);
    CPPUNIT_TEST_SUITE_END();

    F32VecSuite(const F32VecSuite&); //prohibit usage
    const F32VecSuite& operator =(const F32VecSuite&); //prohibit usage

    void testAdd00();
    void testAdd01();
    void testAdd02();
    void testAdd03();
    void testAdd04();
    void testCtor00();
    void testFind00();
    void testFindKthSmallest00();
    void testOp00();
    void testOp01();
    void testOp02();
    void testOp03();
    void testResize00();
    void testRm00();
    void testSort00();
    void testSort01();

};

#endif
