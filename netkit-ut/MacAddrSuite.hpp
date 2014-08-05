#ifndef MAC_ADDR_SUITE_HPP
#define MAC_ADDR_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class MacAddrSuite: public CppUnit::TestFixture
{

public:
    MacAddrSuite();

    virtual ~MacAddrSuite();

private:
    CPPUNIT_TEST_SUITE(MacAddrSuite);
    CPPUNIT_TEST(testCompare00);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testIsXxx00);
    CPPUNIT_TEST(testIsXxx01);
    CPPUNIT_TEST(testNew00);
    CPPUNIT_TEST(testReset00);
    CPPUNIT_TEST(testSize00);
    CPPUNIT_TEST(testToSTRING00);
    CPPUNIT_TEST(testToString00);
    CPPUNIT_TEST(testVec00);
    CPPUNIT_TEST_SUITE_END();

    MacAddrSuite(const MacAddrSuite&); //prohibit usage
    const MacAddrSuite& operator =(const MacAddrSuite&); //prohibit usage

    void testCompare00();
    void testCtor00();
    void testIsXxx00();
    void testIsXxx01();
    void testNew00();
    void testReset00();
    void testSize00();
    void testToSTRING00();
    void testToString00();
    void testVec00();

};

#endif
