#ifndef NET_CAP_SUITE_HPP
#define NET_CAP_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(netkit, IpCap)


class NetCapSuite: public CppUnit::TestFixture
{

public:
    NetCapSuite();

    virtual ~NetCapSuite();

private:
    CPPUNIT_TEST_SUITE(NetCapSuite);
    CPPUNIT_TEST(testCapFile00);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testMonitor00);
    CPPUNIT_TEST(testMonitor01);
    CPPUNIT_TEST(testRepair00);
    CPPUNIT_TEST_SUITE_END();

    NetCapSuite(const NetCapSuite&); //prohibit usage
    const NetCapSuite& operator =(const NetCapSuite&); //prohibit usage

    void testCtor00();
    void testCtor01();
    void testCapFile00();
    void testMonitor00();
    void testMonitor01();
    void testRepair00();

    static void onIpPkt(void*, const netkit::IpCap&);

};

#endif
