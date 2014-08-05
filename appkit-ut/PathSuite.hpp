#ifndef PATH_SUITE_HPP
#define PATH_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class PathSuite: public CppUnit::TestFixture
{

public:
    PathSuite();

    virtual ~PathSuite();

private:
    CPPUNIT_TEST_SUITE(PathSuite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testCtor04);
    CPPUNIT_TEST(testFullName00);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST(testReset00);
    CPPUNIT_TEST_SUITE_END();

    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testCtor04();
    void testFullName00();
    void testOp00();
    void testReset00();

};

#endif
