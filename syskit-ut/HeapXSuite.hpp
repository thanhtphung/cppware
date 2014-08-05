#ifndef HEAP_X_SUITE_HPP
#define HEAP_X_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(syskit, HeapX)


class HeapXSuite: public CppUnit::TestFixture
{

public:
    HeapXSuite();

    virtual ~HeapXSuite();

private:
    CPPUNIT_TEST_SUITE(HeapXSuite);
    CPPUNIT_TEST(testAdd00);
    CPPUNIT_TEST(testAdd01);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST(testOp01);
    CPPUNIT_TEST(testOp02);
    CPPUNIT_TEST(testOp03);
    CPPUNIT_TEST(testReplace00);
    CPPUNIT_TEST(testResize00);
    CPPUNIT_TEST(testRm00);
    CPPUNIT_TEST(testRm01);
    CPPUNIT_TEST(testRmItem00);
    CPPUNIT_TEST_SUITE_END();

    HeapXSuite(const HeapXSuite&); //prohibit usage
    const HeapXSuite& operator =(const HeapXSuite&); //prohibit usage

    void testAdd00();
    void testAdd01();
    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testOp00();
    void testOp01();
    void testOp02();
    void testOp03();
    void testReplace00();
    void testResize00();
    void testRm00();
    void testRm01();
    void testRmItem00();
    void validate(syskit::HeapX&);

    static int compare(const void*, const void*);
    static int compareR(const void*, const void*);

};

#endif
