#ifndef F32_SUITE_HPP
#define F32_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class F32Suite: public CppUnit::TestFixture
{

public:
    F32Suite();

    virtual ~F32Suite();

private:
    CPPUNIT_TEST_SUITE(F32Suite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testDoublet00);
    CPPUNIT_TEST(testIsValid00);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST(testQuartet00);
    CPPUNIT_TEST(testRound00);
    CPPUNIT_TEST(testTriplet00);
    CPPUNIT_TEST_SUITE_END();

    F32Suite(const F32Suite&); //prohibit usage
    const F32Suite& operator =(const F32Suite&); //prohibit usage

    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testDoublet00();
    void testIsValid00();
    void testOp00();
    void testQuartet00();
    void testRound00();
    void testTriplet00();

};

#endif
