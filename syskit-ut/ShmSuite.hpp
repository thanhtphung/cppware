#ifndef SHM_SUITE_HPP
#define SHM_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class ShmSuite: public CppUnit::TestFixture
{

public:
    ShmSuite();

    virtual ~ShmSuite();

private:
    CPPUNIT_TEST_SUITE(ShmSuite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testCtor03);
    CPPUNIT_TEST(testCtor04);
    CPPUNIT_TEST(testCtor05);
    CPPUNIT_TEST(testRm00);
    CPPUNIT_TEST_SUITE_END();

    ShmSuite(const ShmSuite&); //prohibit usage
    const ShmSuite& operator =(const ShmSuite&); //prohibit usage

    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testCtor03();
    void testCtor04();
    void testCtor05();
    void testRm00();

};

#endif
