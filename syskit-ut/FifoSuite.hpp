#ifndef FIFO_SUITE_HPP
#define FIFO_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class FifoSuite: public CppUnit::TestFixture
{

public:
    FifoSuite();

    virtual ~FifoSuite();

private:
    CPPUNIT_TEST_SUITE(FifoSuite);
    CPPUNIT_TEST(testAdd00);
    CPPUNIT_TEST(testAdd01);
    CPPUNIT_TEST(testAdd02);
    CPPUNIT_TEST(testAdd03);
    CPPUNIT_TEST(testAdd04);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testCtor03);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST(testOp01);
    CPPUNIT_TEST(testOp02);
    CPPUNIT_TEST(testPeek00);
    CPPUNIT_TEST(testPeek01);
    CPPUNIT_TEST_SUITE_END();

    FifoSuite(const FifoSuite&); //prohibit usage
    const FifoSuite& operator =(const FifoSuite&); //prohibit usage

    void testAdd00();
    void testAdd01();
    void testAdd02();
    void testAdd03();
    void testAdd04();
    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testCtor03();
    void testOp00();
    void testOp01();
    void testOp02();
    void testPeek00();
    void testPeek01();

};

#endif
