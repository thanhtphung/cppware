#ifndef UNIT_TEST_BED_SUITE_HPP
#define UNIT_TEST_BED_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include <sstream>


class UnitTestBedSuite: public CppUnit::TestFixture
{

public:
    UnitTestBedSuite();

    virtual ~UnitTestBedSuite();

private:
    std::ostringstream display_;

    CPPUNIT_TEST_SUITE(UnitTestBedSuite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testCtor03);
    CPPUNIT_TEST(testCtor04);
    CPPUNIT_TEST(testCtor05);
    CPPUNIT_TEST_SUITE_END();

    UnitTestBedSuite(const UnitTestBedSuite&); //prohibit usage
    const UnitTestBedSuite& operator =(const UnitTestBedSuite&); //prohibit usage

    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testCtor03();
    void testCtor04();
    void testCtor05();

};

#endif
