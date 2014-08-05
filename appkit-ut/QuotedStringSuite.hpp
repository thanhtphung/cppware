#ifndef QUOTED_STRING_SUITE_HPP
#define QUOTED_STRING_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class QuotedStringSuite: public CppUnit::TestFixture
{

public:
    QuotedStringSuite();

    virtual ~QuotedStringSuite();

private:
    CPPUNIT_TEST_SUITE(QuotedStringSuite);
    CPPUNIT_TEST(testDequote00);
    CPPUNIT_TEST(testDequote01);
    CPPUNIT_TEST(testDequote02);
    CPPUNIT_TEST(testEnquote00);
    CPPUNIT_TEST(testEnquote01);
    CPPUNIT_TEST_SUITE_END();

    QuotedStringSuite(const QuotedStringSuite&); //prohibit usage
    const QuotedStringSuite& operator =(const QuotedStringSuite&); //prohibit usage

    void testDequote00();
    void testDequote01();
    void testDequote02();
    void testEnquote00();
    void testEnquote01();

};

#endif
