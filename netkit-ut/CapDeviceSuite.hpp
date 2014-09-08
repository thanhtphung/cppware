#ifndef CAP_DEVICE_SUITE_HPP
#define CAP_DEVICE_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class CapDeviceSuite: public CppUnit::TestFixture
{

public:
    CapDeviceSuite();

    virtual ~CapDeviceSuite();

private:
    CPPUNIT_TEST_SUITE(CapDeviceSuite);
    CPPUNIT_TEST(testCategorize00);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testRefresh00);
    CPPUNIT_TEST_SUITE_END();

    CapDeviceSuite(const CapDeviceSuite&); //prohibit usage
    const CapDeviceSuite& operator =(const CapDeviceSuite&); //prohibit usage

    void testCategorize00();
    void testCtor00();
    void testRefresh00();

};

#endif
