#ifndef IP_DEVICE_SUITE_HPP
#define IP_DEVICE_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class IpDeviceSuite: public CppUnit::TestFixture
{

public:
    IpDeviceSuite();

    virtual ~IpDeviceSuite();

private:
    CPPUNIT_TEST_SUITE(IpDeviceSuite);
    CPPUNIT_TEST(testCategorize00);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST(testRefresh00);
    CPPUNIT_TEST_SUITE_END();

    IpDeviceSuite(const IpDeviceSuite&); //prohibit usage
    const IpDeviceSuite& operator =(const IpDeviceSuite&); //prohibit usage

    void testCategorize00();
    void testCtor00();
    void testOp00();
    void testRefresh00();

};

#endif
