#include "appkit/String.hpp"
#include "appkit/StringVec.hpp"
#include "netkit/MacAddr.hpp"
#include "netkit/MacAddrVec.hpp"

#include "netkit-ut-pch.h"
#include "MacAddrSuite.hpp"

using namespace appkit;
using namespace netkit;

typedef struct
{
    const char* s;
    char delim;
    bool isValid;
    unsigned char addr[MacAddr::RawLength];
    unsigned long long n64;
} sample_t;

const sample_t SAMPLE[] =
{
    {"00.00.00.00.00.00", '.', true, {0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U}, 0x0000000000000000ULL},
    {"12.22.33.44.55.766", '.', false, {0x12U, 0x22U, 0x33U, 0x44U, 0x55U, 0x76U}, 0x1222334455760000ULL},
    {"12223@445576", 0, false, {0x12U, 0x22U, 0x33U, 0x44U, 0x55U, 0x76U}, 0x1222334455760000ULL},
    {"1222@3445576", 0, false, {0x12U, 0x22U, 0x33U, 0x44U, 0x55U, 0x76U}, 0x1222334455760000ULL},
    {"12.22.33.44.55.76", '.', true, {0x12U, 0x22U, 0x33U, 0x44U, 0x55U, 0x76U}, 0x1222334455760000ULL},
    {"12.22.33.44.55.67", 0, true, {0x12U, 0x22U, 0x33U, 0x44U, 0x55U, 0x67U}, 0x1222334455670000ULL},
    {"12.22.33.44.55.76", '-', false, {0x12U, 0x22U, 0x33U, 0x44U, 0x55U, 0x76U}, 0x1222334455760000ULL},
    {"122233445577", 0, true, {0x12U, 0x22U, 0x33U, 0x44U, 0x55U, 0x77U}, 0x1222334455770000ULL},
    {"ab:cd:ef:01:23:45", 0, true, {0xabU, 0xcdU, 0xefU, 0x01U, 0x23U, 0x45U}, 0xabcdef0123450000ULL},
    {"AB:CD:EF:01:23:46", ':', true, {0xabU, 0xcdU, 0xefU, 0x01U, 0x23U, 0x46U}, 0xabcdef0123460000ULL},
    {"ABCDef012347", 0, true, {0xabU, 0xcdU, 0xefU, 0x01U, 0x23U, 0x47U}, 0xabcdef0123470000ULL},
    {"ABCDef0123456", 0, false, {0xabU, 0xcdU, 0xefU, 0x01U, 0x23U, 0x45U}, 0xabcdef0123450000ULL},
    {"", 0, false, {0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U}, 0x0000000000000000ULL},
    {"-", 0, false, {0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U}, 0x0000000000000000ULL},
    {"--", 0, false, {0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U}, 0x0000000000000000ULL}
};
const size_t NUM_SAMPLES = sizeof(SAMPLE) / sizeof(SAMPLE[0]);


MacAddrSuite::MacAddrSuite()
{
}


MacAddrSuite::~MacAddrSuite()
{
}


void MacAddrSuite::testCompare00()
{
    MacAddr macAddr0("112233445566");
    MacAddr macAddr1("112233445567");
    bool ok = (MacAddr::compareP(&macAddr0, &macAddr1) < 0);
    CPPUNIT_ASSERT(ok);
    ok = (MacAddr::compareP(&macAddr1, &macAddr0) > 0);
    CPPUNIT_ASSERT(ok);
    ok = (MacAddr::compareP(&macAddr0, &macAddr0) == 0);
    CPPUNIT_ASSERT(ok);
}


void MacAddrSuite::testCtor00()
{
    MacAddr addr0;
    String s = addr0;
    bool ok = (s == "00-00-00-00-00-00");
    CPPUNIT_ASSERT(ok);

    MacAddr addr1("00.00.00.00.00.00");
    ok = (addr0 == addr1);
    CPPUNIT_ASSERT(ok);
    ok = (!(addr0 != addr1));
    CPPUNIT_ASSERT(ok);

    ok = (!addr1.reset("11.22.33.44.55.666")) && (addr1 == addr0);
    CPPUNIT_ASSERT(ok);
    ok = addr1.reset("11.22.33.44.55.66") && (addr1 != addr0);
    CPPUNIT_ASSERT(ok);

    MacAddr addr2(addr0);
    addr0 = addr1.operator const unsigned char*();
    ok = (addr0 == addr1);
    CPPUNIT_ASSERT(ok);
    addr2 = addr1.asRawBytes();
    ok = (addr1 == addr1);
    CPPUNIT_ASSERT(ok);

    MacAddr addr3(addr2.asU64());
    ok = (addr3 == addr2);
    CPPUNIT_ASSERT(ok);

    double rawAddr = addr1.raw();
    addr3 = MacAddr(rawAddr);
    ok = (addr3 == addr1);
    CPPUNIT_ASSERT(ok);
}


void MacAddrSuite::testIsXxx00()
{
    const char* v[5] =
    {
        "00-05-69-11-11-11", "00.0c.29.22.22.22", "00:1c:14:33:33:33", "005056444444",
        "ff-ff-ff-ff-ff-ff"
    };

    bool ok = true;
    MacAddr macAddr;
    for (size_t i = 0; i < 4; ++i)
    {
        macAddr = v[i];
        if ((!macAddr.isVmware()) || macAddr.isBroadcast())
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    macAddr = v[4];
    ok = (!macAddr.isVmware()) && macAddr.isBroadcast();
    CPPUNIT_ASSERT(ok);
}


void MacAddrSuite::testIsXxx01()
{
    const char* v[8] =
    {
        "00-05-6a-11-11-11",
        "00-06-69-11-11-11",
        "00.0c.2a.22.22.22",
        "00.0d.29.22.22.22",
        "00:1c:15:33:33:33",
        "00:1d:14:33:33:33",
        "005057444444",
        "005156444444"
    };

    bool ok = true;
    MacAddr macAddr;
    for (size_t i = 0; i < 8; ++i)
    {
        macAddr = v[i];
        if (macAddr.isVmware())
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void MacAddrSuite::testNew00()
{
    MacAddr* macAddr = new MacAddr;
    bool ok = (macAddr != 0);
    CPPUNIT_ASSERT(ok);
    delete macAddr;

    unsigned char buf[sizeof(*macAddr)];
    macAddr = new(buf)MacAddr;
    ok = (reinterpret_cast<unsigned char*>(macAddr) == buf);
    CPPUNIT_ASSERT(ok);
    macAddr->MacAddr::~MacAddr();
}


void MacAddrSuite::testReset00()
{
    bool ok = true;
    MacAddr macAddr;
    for (unsigned long i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        if ((macAddr.reset(r.s, r.delim) != r.isValid) ||
            (r.isValid && ((macAddr != r.addr) || (macAddr.toN64() != r.n64) || (MacAddr::fromN64(r.n64) != macAddr))))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void MacAddrSuite::testSize00()
{
    bool ok = (sizeof(MacAddr) == 8);
    CPPUNIT_ASSERT(ok);
}


void MacAddrSuite::testToSTRING00()
{
    char s[MacAddr::StrLength + 1];
    unsigned char addr[MacAddr::RawLength] = {0xabU, 0xcdU, 0xefU, 0x01U, 0x23U, 0x45U};
    MacAddr macAddr(addr);
    bool ok = (macAddr.toSTRING(s, ':' /*delim*/) == s);
    CPPUNIT_ASSERT(ok);
    ok = (strcmp(s, "AB:CD:EF:01:23:45") == 0);
    CPPUNIT_ASSERT(ok);

    ok = (macAddr.toSTRING(s, 0 /*delim*/) == s);
    CPPUNIT_ASSERT(ok);
    ok = (strcmp(s, "ABCDEF012345") == 0);
    CPPUNIT_ASSERT(ok);
}


void MacAddrSuite::testToString00()
{
    char s[MacAddr::StrLength + 1];
    unsigned char addr[MacAddr::RawLength] = {0xabU, 0xcdU, 0xefU, 0x01U, 0x23U, 0x45U};
    MacAddr macAddr(addr);
    bool ok = (macAddr.toString(s, '-' /*delim*/) == s);
    CPPUNIT_ASSERT(ok);
    ok = (strcmp(s, "ab-cd-ef-01-23-45") == 0);
    CPPUNIT_ASSERT(ok);

    ok = (macAddr.toString(s, 0 /*delim*/) == s);
    CPPUNIT_ASSERT(ok);
    ok = (strcmp(s, "abcdef012345") == 0);
    CPPUNIT_ASSERT(ok);

    ok = (macAddr.toString(':') == "ab:cd:ef:01:23:45");
    CPPUNIT_ASSERT(ok);
}


void MacAddrSuite::testVec00()
{
    bool ok = true;
    MacAddr macAddr;
    MacAddrVec vec0;
    String mac;
    StringVec macs;
    for (unsigned long i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        if (r.isValid)
        {
            macAddr = r.addr;
            unsigned long foundAt = vec0.numItems();
            if ((!vec0.add(macAddr)) || (vec0.findIndex(macAddr) != foundAt))
            {
                ok = false;
                break;
            }
            mac = macAddr;
            macs.add(mac);
        }
    }
    CPPUNIT_ASSERT(ok);

    MacAddrVec vec1(vec0);
    ok = (vec1 == vec0);
    CPPUNIT_ASSERT(ok);
    vec1.reset();
    ok = (vec1 != vec0) && (vec1.numItems() == 0);
    CPPUNIT_ASSERT(ok);

    ok = vec1.reset(macs) && (vec1 == vec0);
    CPPUNIT_ASSERT(ok);
    mac = "not-a-mac";
    macs.add(mac);
    ok = (!vec1.reset(macs)) && (vec1 == vec0);
    CPPUNIT_ASSERT(ok);

    bool reverseOrder = false;
    vec0.sort(reverseOrder);
    ok = vec1.find(vec0.peek(0));
    CPPUNIT_ASSERT(ok);
    for (size_t i = 1, numItems = vec0.numItems(); i < numItems; ++i)
    {
        if ((!vec1.find(vec0.peek(i))) || (vec0.peek(i - 1) > vec0.peek(i)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    reverseOrder = true;
    vec0.sort(reverseOrder);
    ok = vec1.find(vec0.peek(0));
    CPPUNIT_ASSERT(ok);
    for (size_t i = 1, numItems = vec0.numItems(); i < numItems; ++i)
    {
        if ((!vec1.find(vec0.peek(i))) || (vec0.peek(i - 1) < vec0.peek(i)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}
