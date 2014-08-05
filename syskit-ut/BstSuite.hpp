#ifndef BST_SUITE_HPP
#define BST_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(syskit, Bst)
DECLARE_CLASS1(syskit, Vec)


class BstSuite: public CppUnit::TestFixture
{

public:
    BstSuite();

    virtual ~BstSuite();

private:
    CPPUNIT_TEST_SUITE(BstSuite);
    CPPUNIT_TEST(testAdd00);
    CPPUNIT_TEST(testAdd01);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testFind00);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST(testOp01);
    CPPUNIT_TEST(testOp02);
    CPPUNIT_TEST(testOp03);
    CPPUNIT_TEST(testOp04);
    CPPUNIT_TEST(testOp05);
    CPPUNIT_TEST(testOp06);
    CPPUNIT_TEST(testResize00);
    CPPUNIT_TEST(testRm00);
    CPPUNIT_TEST_SUITE_END();

    BstSuite(const BstSuite&); //prohibit usage
    const BstSuite& operator =(const BstSuite&); //prohibit usage

    void testAdd00();
    void testAdd01();
    void testCtor00();
    void testCtor01();
    void testFind00();
    void testOp00();
    void testOp01();
    void testOp02();
    void testOp03();
    void testOp04();
    void testOp05();
    void testOp06();
    void testResize00();
    void testRm00();

    void validateBst(const syskit::Bst&, const syskit::Vec&, size_t);
    void validateBst(const syskit::Bst&, size_t);

};

#endif
