#ifndef PRIME_SUITE_HPP
#define PRIME_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class PrimeSuite: public CppUnit::TestFixture
{

public:
    PrimeSuite();

    virtual ~PrimeSuite();

private:
    CPPUNIT_TEST_SUITE(PrimeSuite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testFindHi00);
    CPPUNIT_TEST_SUITE_END();

    void testCtor00();
    void testFindHi00();

};

#endif
