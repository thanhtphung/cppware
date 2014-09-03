#ifndef CSV_ROW_SUITE_HPP
#define CSV_ROW_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(appkit, String)


class CsvRowSuite: public CppUnit::TestFixture
{

public:
    CsvRowSuite();

    virtual ~CsvRowSuite();

private:
    CPPUNIT_TEST_SUITE(CsvRowSuite);
    CPPUNIT_TEST(testApply00);
    CPPUNIT_TEST(testApply01);
    CPPUNIT_TEST(testApply03);
    CPPUNIT_TEST(testApply04);
    CPPUNIT_TEST(testCount00);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testDequoteCol00);
    CPPUNIT_TEST(testNext00);
    CPPUNIT_TEST(testNext01);
    CPPUNIT_TEST(testNext02);
    CPPUNIT_TEST(testNext03);
    CPPUNIT_TEST(testTrim00);
    CPPUNIT_TEST_SUITE_END();

    void testApply00();
    void testApply01();
    void testApply03();
    void testApply04();
    void testCount00();
    void testCtor00();
    void testCtor01();
    void testDequoteCol00();
    void testNext00();
    void testNext01();
    void testNext02();
    void testNext03();
    void testTrim00();

    static bool cb0(void*, const appkit::String&);
    static bool cb1a(void*, const char*, size_t);
    static bool cb1b(void*, const char*, size_t);
    static bool cb1c(void*, const char*, size_t);
    static void cb3(void*, const appkit::String&);
    static void cb4a(void*, const char*, size_t);
    static void cb4b(void*, const char*, size_t);

    static appkit::String formExpectedCol(size_t);

};

#endif
