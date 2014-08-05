#ifndef GROWABLE_SUITE_HPP
#define GROWABLE_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class GrowableSuite: public CppUnit::TestFixture
{

public:
    GrowableSuite();

    virtual ~GrowableSuite();

private:
    CPPUNIT_TEST_SUITE(GrowableSuite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testCtor03);
    CPPUNIT_TEST(testNextCap00);
    CPPUNIT_TEST_SUITE_END();

    GrowableSuite(const GrowableSuite&); //prohibit usage
    const GrowableSuite& operator =(const GrowableSuite&); //prohibit usage

    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testCtor03();
    void testNextCap00();

};

#endif
