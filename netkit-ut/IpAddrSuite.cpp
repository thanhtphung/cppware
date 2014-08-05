#include "appkit/String.hpp"
#include "netkit/IpAddr.hpp"
#include "netkit/IpDevice.hpp"
#include "netkit/net.hpp"

#include "netkit-ut-pch.h"
#include "IpAddrSuite.hpp"

using namespace appkit;
using namespace netkit;


IpAddrSuite::IpAddrSuite()
{
}


IpAddrSuite::~IpAddrSuite()
{
}


void IpAddrSuite::testReset00()
{
    IpAddr addr("1.not-an-ip-addr");
    bool ok = (addr == 0UL);
    CPPUNIT_ASSERT(ok);

    ok = addr.reset("1.2.3.4") && (addr == 0x01020304UL);
    CPPUNIT_ASSERT(ok);

    ok = (!addr.reset("1.23.45.      67")) && (addr == 0x01020304UL);
    CPPUNIT_ASSERT(ok);

    const char* s = 0;
    ok = (!addr.reset(s)) && (addr == 0x01020304UL);
    CPPUNIT_ASSERT(ok);

    char host[255 + 1];
    host[0] = 0;
    gethostname(host, sizeof(host));
    ok = addr.reset(host) && (IpDevice::find(addr) != 0);
    CPPUNIT_ASSERT(ok);
    ok = addr.reset("") && (IpDevice::find(addr) != 0);
    CPPUNIT_ASSERT(ok);
    ok = addr.reset("localhost") && (addr == INADDR_LOOPBACK);
    CPPUNIT_ASSERT(ok);
}


void IpAddrSuite::testToString00()
{
    char s[IpAddr::StrLength + 1] = "";
    IpAddr ipAddr0(0x01233204UL);
    bool ok = (ipAddr0.toString(s) == s) && (strcmp(s, "1.35.50.4") == 0);
    CPPUNIT_ASSERT(ok);

    IpAddr ipAddr1(ipAddr0);
    ok = (ipAddr1 == ipAddr0) && (ipAddr1.asU32() == 0x01233204UL);
    CPPUNIT_ASSERT(ok);

    ipAddr1.reset("11.22.33.44");
    ok = (ipAddr1.toDigits(s) == 11) && (memcmp(s, "11.22.33.44", 11) == 0);
    CPPUNIT_ASSERT(ok);
}


void IpAddrSuite::testToString01()
{
    IpAddr addr(0x01020304UL);
    String s(addr.toString());
    bool ok = (s == "1.2.3.4") && (addr == 0x01020304UL) && (IpAddr(s.ascii()) == addr);
    CPPUNIT_ASSERT(ok);

    addr = 0x11223344UL;
    ok = (addr.toString() == "17.34.51.68");
    CPPUNIT_ASSERT(ok);
}


void IpAddrSuite::testToU8a()
{
    unsigned char addr[4] = {0};
    bool ok = IpAddr::toU8(addr, "1.2.3.4") && (addr[0] == 1) && (addr[1] == 2) && (addr[2] == 3) && (addr[3] == 4);
    CPPUNIT_ASSERT(ok);

    ok = (!IpAddr::toU8(addr, "1. 23.45.67"));
    CPPUNIT_ASSERT(ok);

    const char* s = 0;
    ok = (!IpAddr::toU8(addr, s));
    CPPUNIT_ASSERT(ok);

    char host[255 + 1];
    host[0] = 0;
    gethostname(host, sizeof(host));
    ok = IpAddr::toU8(addr, host);
    CPPUNIT_ASSERT(ok);
    ok = IpAddr::toU8(addr, "");
    CPPUNIT_ASSERT(ok);
    ok = IpAddr::toU8(addr, "localhost");
    CPPUNIT_ASSERT(ok);
}
