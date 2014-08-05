#ifndef D64_HEAP_SUITE_HPP
#define D64_HEAP_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(syskit, D64Heap)


class D64HeapSuite: public CppUnit::TestFixture
{

public:
    D64HeapSuite();

    virtual ~D64HeapSuite();

private:
    CPPUNIT_TEST_SUITE(D64HeapSuite);
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

    D64HeapSuite(const D64HeapSuite&); //prohibit usage
    const D64HeapSuite& operator =(const D64HeapSuite&); //prohibit usage

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

    void validate(syskit::D64Heap&);

};

#endif