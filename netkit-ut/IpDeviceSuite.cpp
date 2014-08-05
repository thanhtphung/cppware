#include "netkit/IpAddr.hpp"
#include "netkit/IpDevice.hpp"
#include "netkit/MacAddr.hpp"

#include "netkit-ut-pch.h"
#include "IpDeviceSuite.hpp"

using namespace netkit;


IpDeviceSuite::IpDeviceSuite()
{
}


IpDeviceSuite::~IpDeviceSuite()
{
}


//
// Interfaces under test:
// - unsigned long IpDevice::categorize(unsigned long);
//
void IpDeviceSuite::testCategorize00()
{
    bool ok = (IpDevice::categorize(0x7f000001UL) == IpDevice::Loopback);
    CPPUNIT_ASSERT(ok);

    ok = (IpDevice::categorize(0x7f000000UL) == IpDevice::Broadcast);
    CPPUNIT_ASSERT(ok);

    ok = (IpDevice::categorize(0x7fffffffUL) == IpDevice::Broadcast);
    CPPUNIT_ASSERT(ok);
}


void IpDeviceSuite::testCtor00()
{
    bool ok = true;
    unsigned long numDevices = IpDevice::numDevices();
    IpAddr ipAddr;
    MacAddr macAddr;
    for (unsigned char i = 0; i < numDevices; ++i)
    {

        const IpDevice* device = IpDevice::instance(i);
        macAddr = device->macAddr();
        if (device->isOld() ||
            (device->index() != i) ||
            (IpDevice::find(macAddr.asRawBytes()) != device) ||
            (IpDevice::find(device->name()) != device) ||
            (IpDevice::find(macAddr.toString().ascii()) != device))
        {
            ok = false;
            break;
        }

        char addr[IpAddr::StrLength + 1];
        for (unsigned char j = 0; j < device->numAddrs(); ++j)
        {
            ipAddr = device->addr(j);
            ipAddr.toString(addr);
            if (device->addrIsNear(device->addr(j)) ||
                (IpDevice::find(addr) != device) ||
                ((IpDevice::categorize(device->addr(j)) != IpDevice::Local) &&
                (IpDevice::categorize(device->addr(j)) != IpDevice::Loopback)))
            {
                ok = false;
                i = static_cast<unsigned char>(numDevices - 1); //terminate outer loop
                break;
            }
        }
    }
    CPPUNIT_ASSERT(ok);
}


void IpDeviceSuite::testOp00()
{
    IpDevice device0(*IpDevice::any());
    IpDevice device1(*IpDevice::any());
    device0 = device0; //no-op
    device0 = device1;
    bool ok = (device0.macAddr() == device1.macAddr());
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - unsigned char IpDevice::refresh();
//
void IpDeviceSuite::testRefresh00()
{
    unsigned char numChanges = IpDevice::refresh();
    bool ok = (numChanges == 0);
    CPPUNIT_ASSERT(ok);
}
