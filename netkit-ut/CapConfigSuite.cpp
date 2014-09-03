#include "appkit/CmdLine.hpp"
#include "netkit/CapConfig.hpp"

#include "netkit-ut-pch.h"
#include "CapConfigSuite.hpp"

using namespace appkit;
using namespace netkit;


CapConfigSuite::CapConfigSuite()
{
}


CapConfigSuite::~CapConfigSuite()
{
}


void CapConfigSuite::testCtor00()
{
    CapConfig config;
    bool ok = (config.bePromiscuous() &&
        config.capIcPkts() &&
        config.capOgPkts() &&
        (config.filter()[0] == 0) &&
        config.name().empty() &&
        (config.bufSize() == 0x1000000) &&
        (config.capLength() == 1518) &&
        (config.loopTimeout() == 300) &&
        (config.agentStackSize() == 131072));
    CPPUNIT_ASSERT(ok);

    const char* s = "capconfig=131072:true:16777216:true:1518:true:300" "\n" "capfilter=\"\"" "\n";
    bool includeFilter = true;
    bool includeName = false;
    ok = (config.asString(includeFilter, includeName) == s);
    CPPUNIT_ASSERT(ok);

    s = "capconfig=131072:true:16777216:true:1518:true:300" "\n";
    includeFilter = false;
    ok = (config.asString(includeFilter, includeName) == s);
    CPPUNIT_ASSERT(ok);
}


void CapConfigSuite::testCtor01()
{
    CmdLine cmdLine("cmd --capconfig='1:false:2::3::456'");
    CapConfig config(cmdLine);
    bool ok = ((!config.bePromiscuous()) &&
        config.capIcPkts() &&
        config.capOgPkts() &&
        (config.filter()[0] == 0) &&
        config.name().empty() &&
        (config.bufSize() == 2) &&
        (config.capLength() == 1518) && //3 is out-of-range
        (config.loopTimeout() == 456) &&
        (config.agentStackSize() == 1));
    CPPUNIT_ASSERT(ok);

    cmdLine = "cmd --capconfig='11:TRUE:22::123::4567' --capfilter=\"ip.addr == 192.168.3.209\"";
    config.reset(cmdLine);
    ok = (config.bePromiscuous() &&
        config.capIcPkts() &&
        config.capOgPkts() &&
        (String(config.filter()) == "ip.addr == 192.168.3.209") &&
        config.name().empty() &&
        (config.bufSize() == 22) &&
        (config.capLength() == 123) &&
        (config.loopTimeout() == 4567) &&
        (config.agentStackSize() == 11));
    CPPUNIT_ASSERT(ok);

    const char* s = "capconfig=11:true:22:true:123:true:4567" "\n" "capfilter=\"ip.addr == 192.168.3.209\"" "\n";
    bool includeFilter = true;
    bool includeName = false;
    ok = (config.asString(includeFilter, includeName) == s);
    CPPUNIT_ASSERT(ok);

    cmdLine = "cmd --capfilter=\"xyz\"";
    config.reset(cmdLine);
    ok = (config.bePromiscuous() &&
        config.capIcPkts() &&
        config.capOgPkts() &&
        (String(config.filter()) == "xyz") &&
        config.name().empty() &&
        (config.bufSize() == 0x1000000) &&
        (config.capLength() == 1518) &&
        (config.loopTimeout() == 300) &&
        (config.agentStackSize() == 131072));
    CPPUNIT_ASSERT(ok);
}


void CapConfigSuite::testName00()
{
    CmdLine cmdLine("cmd --capname='a-name'");
    CapConfig config(cmdLine);
    bool ok = (config.name() == "a-name");
    CPPUNIT_ASSERT(ok);

    const char* s = "capconfig=131072:true:16777216:true:1518:true:300" "\n" "capfilter=\"\"" "\n" "capname=\"a-name\"" "\n";
    bool includeFilter = true;
    bool includeName = true;
    ok = (config.asString(includeFilter, includeName) == s);
    CPPUNIT_ASSERT(ok);

    String name = "just-another-name";
    config.setName(name);
    ok = (config.name() == name);
    CPPUNIT_ASSERT(ok);

    s = "capconfig=131072:true:16777216:true:1518:true:300" "\n" "capname=\"just-another-name\"" "\n";
    includeFilter = false;
    ok = (config.asString(includeFilter, includeName) == s);
    CPPUNIT_ASSERT(ok);
}
