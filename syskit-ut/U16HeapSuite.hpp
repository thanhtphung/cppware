#ifndef U16_HEAP_SUITE_HPP
#define U16_HEAP_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(syskit, U16Heap)


class U16HeapSuite: public CppUnit::TestFixture
{

public:
    U16HeapSuite();

    virtual ~U16HeapSuite();

private:
    CPPUNIT_TEST_SUITE(U16HeapSuite);
    CPPUNIT_TEST(testAdd00);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST(testOp01);
    CPPUNIT_TEST(testOp02);
    CPPUNIT_TEST(testOp03);
    CPPUNIT_TEST(testResize00);
    CPPUNIT_TEST(testRm00);
    CPPUNIT_TEST(testRm01);
    CPPUNIT_TEST(testSort00);
    CPPUNIT_TEST_SUITE_END();

    U16HeapSuite(const U16HeapSuite&); //prohibit usage
    const U16HeapSuite& operator =(const U16HeapSuite&); //prohibit usage

    void testAdd00();
    void testCtor00();
    void testCtor01();
    void testOp00();
    void testOp01();
    void testOp02();
    void testOp03();
    void testResize00();
    void testRm00();
    void testRm01();
    void testSort00();

    void validate(syskit::U16Heap&);

};

#endif
