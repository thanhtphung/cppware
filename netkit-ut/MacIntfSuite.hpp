#ifndef MAC_INTF_SUITE_HPP
#define MAC_INTF_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class MacIntfSuite: public CppUnit::TestFixture
{

public:
    MacIntfSuite();

    virtual ~MacIntfSuite();

private:
    CPPUNIT_TEST_SUITE(MacIntfSuite);
    CPPUNIT_TEST(testCompare00);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testNew00);
    CPPUNIT_TEST(testReset00);
    CPPUNIT_TEST(testSize00);
    CPPUNIT_TEST(testToString00);
    CPPUNIT_TEST(testVec00);
    CPPUNIT_TEST_SUITE_END();

    MacIntfSuite(const MacIntfSuite&); //prohibit usage
    const MacIntfSuite& operator =(const MacIntfSuite&); //prohibit usage

    void testCompare00();
    void testCtor00();
    void testNew00();
    void testReset00();
    void testSize00();
    void testToString00();
    void testVec00();

};

#endif
