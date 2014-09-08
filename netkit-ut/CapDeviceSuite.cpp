#include "netkit/CapDevice.hpp"
#include "netkit/IpAddr.hpp"
#include "netkit/MacAddr.hpp"

#include "netkit-ut-pch.h"
#include "CapDeviceSuite.hpp"

using namespace netkit;


CapDeviceSuite::CapDeviceSuite()
{
}


CapDeviceSuite::~CapDeviceSuite()
{
}


//
// Interfaces under test:
// - unsigned int CapDevice::categorize(unsigned int);
//
void CapDeviceSuite::testCategorize00()
{
    bool ok = (CapDevice::categorize(0x7f000001U) == CapDevice::Loopback);
    CPPUNIT_ASSERT(ok);

    ok = (CapDevice::categorize(0x7f000000U) == CapDevice::Broadcast);
    CPPUNIT_ASSERT(ok);

    ok = (CapDevice::categorize(0x7fffffffU) == CapDevice::Broadcast);
    CPPUNIT_ASSERT(ok);
}


void CapDeviceSuite::testCtor00()
{
    bool ok = true;
    unsigned char numDevices = CapDevice::numDevices();
    IpAddr ipAddr;
    MacAddr macAddr;
    for (unsigned char i = 0; i < numDevices; ++i)
    {

        const CapDevice* device = CapDevice::instance(i);
        macAddr = device->macAddr();
        if (device->isOld() ||
            (device->ifIndex() == 0) ||
            (device->index() != i) ||
            (CapDevice::find(device->macAddr()) != device) ||
            (CapDevice::find(device->name()) != device) ||
            (CapDevice::find(macAddr.toString().ascii()) != device))
        {
            ok = false;
            break;
        }

        char addr[IpAddr::StrLength + 1];
        for (unsigned char j = 0; j < device->numAddrs(); ++j)
        {
            ipAddr = device->addr(j);
            if (ipAddr.asU32())
            {
                ipAddr.toString(addr);
                if (device->addrIsNear(ipAddr.asU32()) ||
                    (CapDevice::find(addr) != device) ||
                    ((CapDevice::categorize(device->addr(j)) != CapDevice::Local) &&
                    (CapDevice::categorize(device->addr(j)) != CapDevice::Loopback)))
                {
                    ok = false;
                    i = numDevices - 1; //terminate outer loop
                    break;
                }
            }
        }

        CapDevice::Stat stat0(*device);
        CapDevice::Stat stat1(*device);
        if (stat0.inBytes() > stat1.inBytes())
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - unsigned char CapDevice::refresh();
//
void CapDeviceSuite::testRefresh00()
{
    unsigned char numChanges = CapDevice::refresh();
    bool ok = (numChanges == 0);
    CPPUNIT_ASSERT(ok);
}
