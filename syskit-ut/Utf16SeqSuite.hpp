#ifndef UTF16_SEQ_SUITE_HPP
#define UTF16_SEQ_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"
#include "syskit/sys.hpp"

DECLARE_CLASS1(syskit, Utf16Seq)


class Utf16SeqSuite: public CppUnit::TestFixture
{

public:
    Utf16SeqSuite();

    virtual ~Utf16SeqSuite();

private:
    syskit::Utf16Seq* seq0_;
    syskit::Utf16Seq* seq1_;

    CPPUNIT_TEST_SUITE(Utf16SeqSuite);
    CPPUNIT_TEST(testApply00);
    CPPUNIT_TEST(testApply01);
    CPPUNIT_TEST(testApply02);
    CPPUNIT_TEST(testApply03);
    CPPUNIT_TEST(testConvert00);
    CPPUNIT_TEST(testConvert01);
    CPPUNIT_TEST(testConvert02);
    CPPUNIT_TEST(testConvert03);
    CPPUNIT_TEST(testConvert04);
    CPPUNIT_TEST(testCountChars00);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testCtor03);
    CPPUNIT_TEST(testIsValid00);
    CPPUNIT_TEST(testNext00);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST(testOp01);
    CPPUNIT_TEST(testOp02);
    CPPUNIT_TEST(testOp03);
    CPPUNIT_TEST(testPeek00);
    CPPUNIT_TEST(testPeek01);
    CPPUNIT_TEST(testPeek02);
    CPPUNIT_TEST(testPeek03);
    CPPUNIT_TEST(testPrev00);
    CPPUNIT_TEST(testReset00);
    CPPUNIT_TEST(testReset01);
    CPPUNIT_TEST(testResize00);
    CPPUNIT_TEST(testShrink00);
    CPPUNIT_TEST_SUITE_END();

    Utf16SeqSuite(const Utf16SeqSuite&); //prohibit usage
    const Utf16SeqSuite& operator =(const Utf16SeqSuite&); //prohibit usage

    void testApply00();
    void testApply01();
    void testApply02();
    void testApply03();
    void testConvert00();
    void testConvert01();
    void testConvert02();
    void testConvert03();
    void testConvert04();
    void testCountChars00();
    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testCtor03();
    void testIsValid00();
    void testNext00();
    void testOp00();
    void testOp01();
    void testOp02();
    void testOp03();
    void testPeek00();
    void testPeek01();
    void testPeek02();
    void testPeek03();
    void testPrev00();
    void testReset00();
    void testReset01();
    void testResize00();
    void testShrink00();

    static bool cb0a(void*, size_t, syskit::utf32_t);
    static bool cb0b(void*, size_t, syskit::utf32_t);
    static bool cb0c(void*, size_t, syskit::utf32_t);
    static bool cb0d(void*, size_t, syskit::utf32_t);
    static void cb1a(void*, size_t, syskit::utf32_t);
    static void cb1b(void*, size_t, syskit::utf32_t);

};

#endif
