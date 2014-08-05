#include "appkit/StrArray.hpp"
#include "appkit/String.hpp"
#include "appkit/Toeprint.hpp"

#include "appkit-ut-pch.h"
#include "netkit/IpDevice.hpp"
#include "MiscSuite.hpp"

using namespace appkit;
using namespace netkit;


MiscSuite::MiscSuite()
{
}


MiscSuite::~MiscSuite()
{
}


void MiscSuite::testStrArray00()
{
    StrArray a(IpDevice::ADDR_CATEGORY_E);
    String item("Local");
    size_t foundIndex = 0;
    bool ok = (a[2] == item) &&
        a.contains(item) &&
        a.contains("Far") &&
        a.find(item) &&
        a.find("Far") &&
        (a.numItems() == 5) &&
        (a.peek(2) == item) &&
        (a.raw() == IpDevice::ADDR_CATEGORY_E) &&
        a.search(item) &&
        a.search("Far") &&
        (a.search(item, foundIndex) && (foundIndex == 2));
    CPPUNIT_ASSERT(ok);

    foundIndex = 0;
    bool ignoreCase = false;
    bool allowItemAsIndex = false;
    ok = a.find(item, foundIndex, ignoreCase, allowItemAsIndex) && (foundIndex == 2);
    CPPUNIT_ASSERT(ok);

    foundIndex = 0;
    ok = (!a.find("3", foundIndex, ignoreCase, allowItemAsIndex)) && (foundIndex == 0);
    CPPUNIT_ASSERT(ok);
    allowItemAsIndex = true;
    ok = a.find("3", foundIndex, ignoreCase, allowItemAsIndex) && (foundIndex == 3);
    CPPUNIT_ASSERT(ok);
}


void MiscSuite::testToeprint00()
{
    String key1("ANOTHERrANDOMsTRINGuSEDfORtOEPRINTcONSTRUCTION!!!");
    String key0("aRandomStringUsedForToeprintConstruction!!!");
    Toeprint toeprint0(key0, key1);
    String checksum("8dC4vd");
    bool ok = (checksum == toeprint0.asString());
    CPPUNIT_ASSERT(ok);

    Toeprint toeprint1(key0, key1.raw(), key1.byteSize() - 1);
    ok = (checksum == toeprint1.asString());
    CPPUNIT_ASSERT(ok);
}
