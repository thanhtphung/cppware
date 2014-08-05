#ifndef TREE_SUITE_HPP
#define TREE_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class TreeSuite: public CppUnit::TestFixture
{

public:
    TreeSuite();

    virtual ~TreeSuite();

private:
    CPPUNIT_TEST_SUITE(TreeSuite);
    CPPUNIT_TEST(testAdd00);
    CPPUNIT_TEST(testAdd01);
    CPPUNIT_TEST(testApply00);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testNew00);
    CPPUNIT_TEST(testRm00);
    CPPUNIT_TEST(testSize00);
    CPPUNIT_TEST_SUITE_END();

    TreeSuite(const TreeSuite&); //prohibit usage
    const TreeSuite& operator =(const TreeSuite&); //prohibit usage

    void testAdd00();
    void testAdd01();
    void testApply00();
    void testCtor00();
    void testCtor01();
    void testNew00();
    void testRm00();
    void testSize00();

    static bool checkItem(void*, void*);
    static bool rmItem(void*, void*);
    static void deleteItem(void*, void*);

};

#endif
