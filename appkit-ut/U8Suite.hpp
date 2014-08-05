#ifndef U8_SUITE_HPP
#define U8_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class U8Suite: public CppUnit::TestFixture
{

public:
    U8Suite();

    virtual ~U8Suite();

private:
    CPPUNIT_TEST_SUITE(U8Suite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testIsValid00);
    CPPUNIT_TEST(testNibbleToXdigit00);
    CPPUNIT_TEST(testToDigits00);
    CPPUNIT_TEST(testToXDigits00);
    CPPUNIT_TEST(testXdigitToNibble00);
    CPPUNIT_TEST(testXdigitsToU8a);
    CPPUNIT_TEST_SUITE_END();

    U8Suite(const U8Suite&); //prohibit usage
    const U8Suite& operator =(const U8Suite&); //prohibit usage

    void testCtor00();
    void testIsValid00();
    void testNibbleToXdigit00();
    void testToDigits00();
    void testToXDigits00();
    void testXdigitToNibble00();
    void testXdigitsToU8a();

};

#endif
