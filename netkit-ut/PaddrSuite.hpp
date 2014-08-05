#ifndef PADDR_SUITE_HPP
#define PADDR_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class PaddrSuite: public CppUnit::TestFixture
{

public:
    PaddrSuite();

    virtual ~PaddrSuite();

private:
    CPPUNIT_TEST_SUITE(PaddrSuite);
    CPPUNIT_TEST(testCompare00);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testReset00);
    CPPUNIT_TEST(testToString00);
    CPPUNIT_TEST(testToString01);
    CPPUNIT_TEST(testVec00);
    CPPUNIT_TEST_SUITE_END();

    PaddrSuite(const PaddrSuite&); //prohibit usage
    const PaddrSuite& operator =(const PaddrSuite&); //prohibit usage

    void testCompare00();
    void testCtor00();
    void testReset00();
    void testToString00();
    void testToString01();
    void testVec00();

};

#endif
