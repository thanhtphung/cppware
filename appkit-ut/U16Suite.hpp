#ifndef U16_SUITE_HPP
#define U16_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class U16Suite: public CppUnit::TestFixture
{

public:
    U16Suite();

    virtual ~U16Suite();

private:
    CPPUNIT_TEST_SUITE(U16Suite);
    CPPUNIT_TEST(testBswap00);
    CPPUNIT_TEST(testChecksum00);
    CPPUNIT_TEST(testChecksum01);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testToDigits00);
    CPPUNIT_TEST(testToXdigits00);
    CPPUNIT_TEST_SUITE_END();

    U16Suite(const U16Suite&); //prohibit usage
    const U16Suite& operator =(const U16Suite&); //prohibit usage

    void testBswap00();
    void testChecksum00();
    void testChecksum01();
    void testCtor00();
    void testToDigits00();
    void testToXdigits00();

};

#endif
