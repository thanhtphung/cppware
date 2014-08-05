#ifndef BIT_VEC_SUITE_HPP
#define BIT_VEC_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(syskit, BitVec)


class BitVecSuite: public CppUnit::TestFixture
{

public:
    BitVecSuite();

    virtual ~BitVecSuite();

private:
    syskit::BitVec* vec0_;
    syskit::BitVec* vec1_;
    syskit::BitVec* vec2_;
    syskit::BitVec* vec3_;

    CPPUNIT_TEST_SUITE(BitVecSuite);
    CPPUNIT_TEST(testApply00);
    CPPUNIT_TEST(testApply01);
    CPPUNIT_TEST(testApply02);
    CPPUNIT_TEST(testApply03);
    CPPUNIT_TEST(testClear00);
    CPPUNIT_TEST(testClear01);
    CPPUNIT_TEST(testClear02);
    CPPUNIT_TEST(testClear03);
    CPPUNIT_TEST(testClear04);
    CPPUNIT_TEST(testClear05);
    CPPUNIT_TEST(testCount00);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testCtor03);
    CPPUNIT_TEST(testInvert00);
    CPPUNIT_TEST(testItor00);
    CPPUNIT_TEST(testItor01);
    CPPUNIT_TEST(testItor02);
    CPPUNIT_TEST(testItor03);
    CPPUNIT_TEST(testItor04);
    CPPUNIT_TEST(testItor05);
    CPPUNIT_TEST(testItor06);
    CPPUNIT_TEST(testItor07);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST(testOp01);
    CPPUNIT_TEST(testOp02);
    CPPUNIT_TEST(testOp03);
    CPPUNIT_TEST(testOp04);
    CPPUNIT_TEST(testOp05);
    CPPUNIT_TEST(testResize00);
    CPPUNIT_TEST(testSet00);
    CPPUNIT_TEST(testSet01);
    CPPUNIT_TEST(testSet02);
    CPPUNIT_TEST(testSet03);
    CPPUNIT_TEST(testSet04);
    CPPUNIT_TEST(testSet05);
    CPPUNIT_TEST_SUITE_END();

    BitVecSuite(const BitVecSuite&); //prohibit usage
    const BitVecSuite& operator =(const BitVecSuite&); //prohibit usage

    void testApply00();
    void testApply01();
    void testApply02();
    void testApply03();
    void testClear00();
    void testClear01();
    void testClear02();
    void testClear03();
    void testClear04();
    void testClear05();
    void testCount00();
    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testCtor03();
    void testInvert00();
    void testItor00();
    void testItor01();
    void testItor02();
    void testItor03();
    void testItor04();
    void testItor05();
    void testItor06();
    void testItor07();
    void testOp00();
    void testOp01();
    void testOp02();
    void testOp03();
    void testOp04();
    void testOp05();
    void testResize00();
    void testSet00();
    void testSet01();
    void testSet02();
    void testSet03();
    void testSet04();
    void testSet05();

    static bool cb0a(void*, size_t);
    static bool cb0b(void*, size_t);
    static bool cb0c(void*, size_t);
    static bool cb0d(void*, size_t);
    static bool cb0e(void*, size_t);
    static void cb1a(void*, size_t);
    static void cb1b(void*, size_t);

};

#endif
