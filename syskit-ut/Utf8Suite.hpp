#ifndef UTF8_SUITE_HPP
#define UTF8_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class Utf8Suite: public CppUnit::TestFixture
{

public:
    Utf8Suite();

    virtual ~Utf8Suite();

private:
    CPPUNIT_TEST_SUITE(Utf8Suite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testDecode00);
    CPPUNIT_TEST(testEncode00);
    CPPUNIT_TEST(testEncode01);
    CPPUNIT_TEST(testEncode02);
    CPPUNIT_TEST(testEncode03);
    CPPUNIT_TEST(testIsValid00);
    CPPUNIT_TEST(testReset00);
    CPPUNIT_TEST(testReset01);
    CPPUNIT_TEST_SUITE_END();

    Utf8Suite(const Utf8Suite&); //prohibit usage
    const Utf8Suite& operator =(const Utf8Suite&); //prohibit usage

    void testCtor00();
    void testCtor01();
    void testDecode00();
    void testEncode00();
    void testEncode01();
    void testEncode02();
    void testEncode03();
    void testIsValid00();
    void testReset00();
    void testReset01();

};

#endif
