//
// Perform netkit unit tests.
//
#include "appkit/App.hpp"
#include "appkit/UnitTestBed.hpp"
#include "netkit/Winsock.hpp"
#include "syskit/Singleton.hpp"
#include "syskit/sys.hpp"

#include "netkit-ut-pch.h"
#include "IpAddrSetSuite.hpp"
#include "IpAddrSuite.hpp"
#include "IpDeviceSuite.hpp"
#include "MacAddrSuite.hpp"
#include "MacIntfSuite.hpp"
#include "PaddrSuite.hpp"
#include "SubnetSuite.hpp"
#include "UdpClientSuite.hpp"

using namespace appkit;
using namespace netkit;
using namespace syskit;

CPPUNIT_TEST_SUITE_REGISTRATION(IpAddrSetSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(IpAddrSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(IpDeviceSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(MacAddrSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(MacIntfSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(PaddrSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(SubnetSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(UdpClientSuite);

using namespace appkit;
using namespace netkit;
using namespace syskit;

int wmain()
{
    Singleton::create_t create = 0;
    unsigned long initialRefCount = 0UL;
    void* createArg = 0;
    App* app = App::instance(create, initialRefCount, createArg);
    RefCounted::Count lock(*app);

    Winsock winsock;
    UnitTestBed unitTestBed;
    bool ok = unitTestBed.runAll();

    return ok? 0: 1;
}
