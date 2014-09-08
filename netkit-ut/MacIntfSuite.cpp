#include "appkit/Str.hpp"
#include "appkit/StringVec.hpp"
#include "netkit/MacIntf.hpp"
#include "netkit/MacIntfVec.hpp"

#include "netkit-ut-pch.h"
#include "MacIntfSuite.hpp"

using namespace appkit;
using namespace netkit;

typedef struct
{
    const char* s;
    bool isValid;
    char delim0;
    char delim1;
    unsigned char addr[MacAddr::RawLength];
    unsigned short intf;
} sample_t;

const sample_t SAMPLE[] =
{
    {"00.00.00.00.00.00:0", true, '.', ':', {0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U}, 0},
    {"12.22.33.44.55.766:1", false, 0, 0, {0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U}, 0},
    {"12.22.33.44.55.76:89", true, '.', ':', {0x12U, 0x22U, 0x33U, 0x44U, 0x55U, 0x76U}, 89},
    {"12.22.33.44.55.76@90", true, '.', '@', {0x12U, 0x22U, 0x33U, 0x44U, 0x55U, 0x76U}, 90},
    {"12.22.33.44.55.76.65536", false, 0, 0, {0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U}, 0},
    {"122233445576", false, 0, 0, {0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U}, 0},
    {"ab:cd:ef:01:23:45.0xffff", true, 0, 0, {0xabU, 0xcdU, 0xefU, 0x01U, 0x23U, 0x45U}, 65535},
    {"AB:CD:EF:01:23:45.010", true, 0, 0, {0xabU, 0xcdU, 0xefU, 0x01U, 0x23U, 0x45U}, 8},
    {"ABCDef012345-12345", true, 0, '-', {0xabU, 0xcdU, 0xefU, 0x01U, 0x23U, 0x45U}, 12345},
    {"ABCDef0123456", false, 0, 0, {0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U}, 0},
    {"ABCDef012345.9", true, 0, '.', {0xabU, 0xcdU, 0xefU, 0x01U, 0x23U, 0x45U}, 9}
};
const size_t NUM_SAMPLES = sizeof(SAMPLE) / sizeof(SAMPLE[0]);


MacIntfSuite::MacIntfSuite()
{
}


MacIntfSuite::~MacIntfSuite()
{
}


void MacIntfSuite::testCompare00()
{
    MacIntf macIntf0("112233445566.210");
    MacIntf macIntf1("112233445566.123");
    MacIntf macIntf2("112233495566.123");
    MacIntf macIntf3(macIntf2);
    bool ok = (MacIntf::compareP(&macIntf0, &macIntf1) > 0) && (MacIntf::compareP(&macIntf0, &macIntf2) < 0);
    CPPUNIT_ASSERT(ok);
    ok = (MacIntf::compareP(&macIntf1, &macIntf0) < 0) && (MacIntf::compareP(&macIntf2, &macIntf0) > 0);
    CPPUNIT_ASSERT(ok);
    ok = (MacIntf::compareP(&macIntf0, &macIntf0) == 0) && (MacIntf::compareP(&macIntf2, &macIntf3) == 0);
    CPPUNIT_ASSERT(ok);
}


void MacIntfSuite::testCtor00()
{
    MacAddr macAddr("11.22.33.44.55.66");
    MacIntf macIntf0(macAddr, 0x7788U);
    unsigned long long n64 = macIntf0.toN64();
    bool ok = (n64 == 0x1122334455667788ULL);
    CPPUNIT_ASSERT(ok);

    MacIntf macIntf1(MacIntf::fromN64(n64));
    ok = (macIntf0 == macIntf1);
    CPPUNIT_ASSERT(ok);

    double rawMacIntf = macIntf0.raw();
    macIntf1 = MacIntf(rawMacIntf);
    ok = (macIntf1 == macIntf0);
    CPPUNIT_ASSERT(ok);
}


void MacIntfSuite::testNew00()
{
    MacIntf* macIntf = new MacIntf;
    bool ok = (macIntf != 0);
    CPPUNIT_ASSERT(ok);
    delete macIntf;

    unsigned char buf[sizeof(*macIntf)];
    macIntf = new(buf)MacIntf;
    ok = (reinterpret_cast<unsigned char*>(macIntf) == buf);
    CPPUNIT_ASSERT(ok);
    macIntf->MacIntf::~MacIntf();
}


void MacIntfSuite::testReset00()
{
    bool ok = true;
    MacIntf macIntf;
    for (unsigned int i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        if ((macIntf.reset(r.s) != r.isValid) || (r.isValid && (!macIntf.hasKey(r.addr))))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void MacIntfSuite::testSize00()
{
    bool ok = (sizeof(MacIntf) == 8);
    CPPUNIT_ASSERT(ok);
}


void MacIntfSuite::testToString00()
{
    bool ok = true;
    MacIntf macIntf;
    char s0[MacIntf::StrLength + 1];
    char s1[MacIntf::StrLength + 1];
    for (unsigned int i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        macIntf.reset(r.addr, r.intf);
        if (r.isValid &&
            (r.delim0 || r.delim1) &&
            ((Str::compareKI(macIntf.toString(s0, r.delim0, r.delim1), r.s) != 0) ||
            (Str::compareKI(macIntf.toSTRING(s1, r.delim0, r.delim1), r.s) != 0)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void MacIntfSuite::testVec00()
{
    bool ok = true;
    MacIntf macIntf;
    MacIntfVec vec0;
    String intf;
    StringVec intfs;
    for (unsigned int i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        if (r.isValid)
        {
            macIntf.reset(r.addr, r.intf);
            unsigned int foundAt = vec0.numItems();
            if ((!vec0.add(macIntf)) || (vec0.findIndex(macIntf) != foundAt))
            {
                ok = false;
                break;
            }
            intf = macIntf;
            intfs.add(intf);
        }
    }
    CPPUNIT_ASSERT(ok);

    MacIntfVec vec1(vec0);
    ok = (vec1 == vec0);
    CPPUNIT_ASSERT(ok);
    vec1.reset();
    ok = (vec1 != vec0) && (vec1.numItems() == 0);
    CPPUNIT_ASSERT(ok);

    ok = vec1.reset(intfs) && (vec1 == vec0);
    CPPUNIT_ASSERT(ok);
    intf = "not-an-intf";
    intfs.add(intf);
    ok = (!vec1.reset(intfs)) && (vec1 == vec0);
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
