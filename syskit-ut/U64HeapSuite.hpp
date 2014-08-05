#ifndef U64_HEAP_SUITE_HPP
#define U64_HEAP_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(syskit, U64Heap)


class U64HeapSuite: public CppUnit::TestFixture
{

public:
    U64HeapSuite();

    virtual ~U64HeapSuite();

private:
    CPPUNIT_TEST_SUITE(U64HeapSuite);
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

    U64HeapSuite(const U64HeapSuite&); //prohibit usage
    const U64HeapSuite& operator =(const U64HeapSuite&); //prohibit usage

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

    void validate(syskit::U64Heap&);

};

#endif
