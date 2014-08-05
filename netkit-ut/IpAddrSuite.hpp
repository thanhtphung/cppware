#ifndef IP_ADDR_SUITE_HPP
#define IP_ADDR_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class IpAddrSuite: public CppUnit::TestFixture
{

public:
    IpAddrSuite();

    virtual ~IpAddrSuite();

private:
    CPPUNIT_TEST_SUITE(IpAddrSuite);
    CPPUNIT_TEST(testReset00);
    CPPUNIT_TEST(testToString00);
    CPPUNIT_TEST(testToString01);
    CPPUNIT_TEST(testToU8a);
    CPPUNIT_TEST_SUITE_END();

    IpAddrSuite(const IpAddrSuite&); //prohibit usage
    const IpAddrSuite& operator =(const IpAddrSuite&); //prohibit usage

    void testReset00();
    void testToString00();
    void testToString01();
    void testToU8a();

};

#endif
