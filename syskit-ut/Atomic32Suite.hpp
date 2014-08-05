#ifndef ATOMIC32_SUITE_HPP
#define ATOMIC32_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class Atomic32Suite: public CppUnit::TestFixture
{

public:
    Atomic32Suite();

    virtual ~Atomic32Suite();

private:
    CPPUNIT_TEST_SUITE(Atomic32Suite);
    CPPUNIT_TEST(testDecrement00);
    CPPUNIT_TEST(testIncrement00);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST(testSet00);
    CPPUNIT_TEST_SUITE_END();

    Atomic32Suite(const Atomic32Suite&); //prohibit usage
    const Atomic32Suite& operator =(const Atomic32Suite&); //prohibit usage

    void testDecrement00();
    void testIncrement00();
    void testOp00();
    void testSet00();

};

#endif
