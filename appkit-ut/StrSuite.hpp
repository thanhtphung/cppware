#ifndef STR_SUITE_HPP
#define STR_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class StrSuite: public CppUnit::TestFixture
{

public:
    StrSuite();

    virtual ~StrSuite();

private:
    CPPUNIT_TEST_SUITE(StrSuite);
    CPPUNIT_TEST(testCompare00);
    CPPUNIT_TEST(testCompare01);
    CPPUNIT_TEST(testCompare02);
    CPPUNIT_TEST(testStrcasestr00);
    CPPUNIT_TEST(testStripSpace00);
    CPPUNIT_TEST(testStripSpace01);
    CPPUNIT_TEST(testStripSpace02);
    CPPUNIT_TEST(testTrimSpace00);
    CPPUNIT_TEST(testTrimSpace01);
    CPPUNIT_TEST_SUITE_END();

    StrSuite(const StrSuite&); //prohibit usage
    const StrSuite& operator =(const StrSuite&); //prohibit usage

    void testCompare00();
    void testCompare01();
    void testCompare02();
    void testStrcasestr00();
    void testStripSpace00();
    void testStripSpace01();
    void testStripSpace02();
    void testTrimSpace00();
    void testTrimSpace01();

};

#endif
