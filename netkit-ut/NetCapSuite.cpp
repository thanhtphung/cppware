#include "appkit/CmdLine.hpp"
#include "netkit/CapDevice.hpp"
#include "netkit/CapFile.hpp"
#include "netkit/NetCap.hpp"
#include "syskit/Thread.hpp"

#include "netkit-ut-pch.h"
#include "NetCapSuite.hpp"

using namespace appkit;
using namespace netkit;
using namespace syskit;


NetCapSuite::NetCapSuite()
{
}


NetCapSuite::~NetCapSuite()
{
}


void NetCapSuite::onIpPkt(void* /*arg*/, const IpCap& /*cap*/)
{
}


//
// Interfaces under test:
// - bool CapFile::CapFile(const char*);
//
void NetCapSuite::testCapFile00()
{
    CapFile capFile0("../../../etc/pcap/sample-flows.pcap");
    bool ok = capFile0.isOk();
    CPPUNIT_ASSERT(ok);

    CapFile capFile1("../../../etc/sample-utf8.xml");
    ok = (!capFile1.isOk());
    CPPUNIT_ASSERT(ok);
}


void NetCapSuite::testCtor00()
{
    bool ok = true;
    unsigned char numDevices = CapDevice::numDevices();
    CapConfig config;
    for (unsigned char i = 0; i < numDevices; ++i)
    {
        const CapDevice* device = CapDevice::instance(i);
        NetCap netCap(device, config);
        if ((!netCap.isOk()) || (*netCap.lastErr() != 0) || (netCap.tid() == Thread::INVALID_ID))
        {
            ok = false;
            break;
        }

        NetCap::Stat stat(netCap);
        if (stat.bytesDropped() != 0)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void NetCapSuite::testCtor01()
{
    CapFile capFile("../../../etc/pcap/sample-flows.pcap");
    NetCap netCap(&capFile);
    bool ok = netCap.isOk();
    CPPUNIT_ASSERT(ok);

    void* arg = 0;
    netCap.monitor(onIpPkt, arg);
    NetCap::Stat stat(netCap);
    ok = (stat.bytesCaptured() == 4147403ULL) && (stat.bytesDropped() == 0) &&
        (stat.pktsCaptured() == 11474ULL) && (stat.pktsDropped() == 0) &&
        (stat.pktsFiltered() == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - bool NetCap::repair(bool);
//
void NetCapSuite::testRepair00()
{
    const CapDevice* device = CapDevice::any();
    NetCap netCap(device);
    bool onlyIfBroken = true;
    bool repaired = netCap.repair(onlyIfBroken);
    bool ok = (!repaired);
    CPPUNIT_ASSERT(ok);

    onlyIfBroken = false;
    repaired = netCap.repair(onlyIfBroken);
    ok = repaired && netCap.isOk() && (*netCap.lastErr() == 0) && (netCap.tid() != Thread::INVALID_ID);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - bool NetCap::monitor(pktCb_t,void*);
//
void NetCapSuite::testMonitor00()
{
    CmdLine cmdLine("cmd -capconfig=131072:true:0x1000000:true:9999:true:300");
    CapConfig config(cmdLine);
    CapFile capFile("../../../etc/pcap/sample-jumbo.pcap");
    NetCap netCap(&capFile, config);
    bool ok = netCap.isOk();
    CPPUNIT_ASSERT(ok);

    void* arg = 0;
    netCap.monitor(onIpPkt, arg);
    NetCap::Stat stat(netCap);
    ok = (stat.bytesCaptured() == 8042 * 20ULL) && (stat.bytesDropped() == 0) &&
        (stat.pktsCaptured() == 20ULL) && (stat.pktsDropped() == 0) &&
        (stat.pktsFiltered() == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - bool NetCap::monitor(pktCb_t,void*);
//
void NetCapSuite::testMonitor01()
{
    CmdLine cmdLine("cmd -capconfig=131072:true:0x1000000:true:1518:true:300");
    CapConfig config(cmdLine);
    CapFile capFile("../../../etc/pcap/sample-jumbo.pcap");
    NetCap netCap(&capFile, config);
    bool ok = netCap.isOk();
    CPPUNIT_ASSERT(ok);

    void* arg = 0;
    netCap.monitor(onIpPkt, arg);
    NetCap::Stat stat(netCap);
    ok = (stat.bytesCaptured() == 1518 * 20ULL) && (stat.bytesDropped() == (8042 - 1518) * 20ULL) &&
        (stat.pktsCaptured() == 20ULL) && (stat.pktsDropped() == 0) &&
        (stat.pktsFiltered() == 0);
    CPPUNIT_ASSERT(ok);
}
