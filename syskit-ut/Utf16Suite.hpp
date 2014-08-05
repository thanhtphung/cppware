#ifndef UTF16_SUITE_HPP
#define UTF16_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class Utf16Suite: public CppUnit::TestFixture
{

public:
    Utf16Suite();

    virtual ~Utf16Suite();

private:
    CPPUNIT_TEST_SUITE(Utf16Suite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testDecode00);
    CPPUNIT_TEST(testEncode00);
    CPPUNIT_TEST(testReset00);
    CPPUNIT_TEST(testReset01);
    CPPUNIT_TEST_SUITE_END();

    Utf16Suite(const Utf16Suite&); //prohibit usage
    const Utf16Suite& operator =(const Utf16Suite&); //prohibit usage

    void testCtor00();
    void testCtor01();
    void testDecode00();
    void testEncode00();
    void testReset00();
    void testReset01();

};

#endif
