#ifndef ATOMIC64_SUITE_HPP
#define ATOMIC64_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class Atomic64Suite: public CppUnit::TestFixture
{

public:
    Atomic64Suite();

    virtual ~Atomic64Suite();

private:
    CPPUNIT_TEST_SUITE(Atomic64Suite);
    CPPUNIT_TEST(testDecrement00);
    CPPUNIT_TEST(testIncrement00);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST(testSet00);
    CPPUNIT_TEST_SUITE_END();

    Atomic64Suite(const Atomic64Suite&); //prohibit usage
    const Atomic64Suite& operator =(const Atomic64Suite&); //prohibit usage

    void testDecrement00();
    void testIncrement00();
    void testOp00();
    void testSet00();

};

#endif
