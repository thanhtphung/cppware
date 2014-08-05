#ifndef HEAP_SUITE_HPP
#define HEAP_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(syskit, Heap)


class HeapSuite: public CppUnit::TestFixture
{

public:
    HeapSuite();

    virtual ~HeapSuite();

private:
    CPPUNIT_TEST_SUITE(HeapSuite);
    CPPUNIT_TEST(testAdd00);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST(testOp01);
    CPPUNIT_TEST(testOp02);
    CPPUNIT_TEST(testOp03);
    CPPUNIT_TEST(testResize00);
    CPPUNIT_TEST(testRm00);
    CPPUNIT_TEST(testRm01);
    CPPUNIT_TEST(testSort00);
    CPPUNIT_TEST_SUITE_END();

    HeapSuite(const HeapSuite&); //prohibit usage
    const HeapSuite& operator =(const HeapSuite&); //prohibit usage

    void testAdd00();
    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testOp00();
    void testOp01();
    void testOp02();
    void testOp03();
    void testResize00();
    void testRm00();
    void testRm01();
    void testSort00();
    void validate(syskit::Heap&);

    static int compare(const void*, const void*);
    static int compareR(const void*, const void*);

};

#endif
