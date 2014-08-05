#ifndef U64_SUITE_HPP
#define U64_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class U64Suite: public CppUnit::TestFixture
{

public:
    U64Suite();

    virtual ~U64Suite();

private:
    CPPUNIT_TEST_SUITE(U64Suite);
    CPPUNIT_TEST(testBswap00);
    CPPUNIT_TEST(testCompare00);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testHash00);
    CPPUNIT_TEST(testIsValid00);
    CPPUNIT_TEST(testNumDigits00);
    CPPUNIT_TEST(testToDigits00);
    CPPUNIT_TEST(testToXdigits00);
    CPPUNIT_TEST_SUITE_END();

    U64Suite(const U64Suite&); //prohibit usage
    const U64Suite& operator =(const U64Suite&); //prohibit usage

    void testBswap00();
    void testCompare00();
    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testHash00();
    void testIsValid00();
    void testNumDigits00();
    void testToDigits00();
    void testToXdigits00();

};

#endif
