#ifndef U32_SUITE_HPP
#define U32_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class U32Suite: public CppUnit::TestFixture
{

public:
    U32Suite();

    virtual ~U32Suite();

private:
    CPPUNIT_TEST_SUITE(U32Suite);
    CPPUNIT_TEST(testCompare00);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testCtor03);
    CPPUNIT_TEST(testHash00);
    CPPUNIT_TEST(testIsValid00);
    CPPUNIT_TEST(testNumDigits00);
    CPPUNIT_TEST(testToDigits00);
    CPPUNIT_TEST(testToXdigits00);
    CPPUNIT_TEST_SUITE_END();

    U32Suite(const U32Suite&); //prohibit usage
    const U32Suite& operator =(const U32Suite&); //prohibit usage

    void testCompare00();
    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testCtor03();
    void testHash00();
    void testIsValid00();
    void testNumDigits00();
    void testToDigits00();
    void testToXdigits00();

};

#endif
