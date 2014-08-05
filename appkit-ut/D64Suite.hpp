#ifndef D64_SUITE_HPP
#define D64_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class D64Suite: public CppUnit::TestFixture
{

public:
    D64Suite();

    virtual ~D64Suite();

private:
    CPPUNIT_TEST_SUITE(D64Suite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testDoublet00);
    CPPUNIT_TEST(testIsValid00);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST(testQuartet00);
    CPPUNIT_TEST(testTriplet00);
    CPPUNIT_TEST_SUITE_END();

    D64Suite(const D64Suite&); //prohibit usage
    const D64Suite& operator =(const D64Suite&); //prohibit usage

    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testDoublet00();
    void testIsValid00();
    void testOp00();
    void testQuartet00();
    void testTriplet00();

};

#endif
