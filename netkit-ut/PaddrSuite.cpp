#include "appkit/String.hpp"
#include "appkit/StringVec.hpp"
#include "netkit/IpDevice.hpp"
#include "netkit/Paddr.hpp"
#include "netkit/PaddrVec.hpp"
#include "netkit/net.hpp"

#include "netkit-ut-pch.h"
#include "PaddrSuite.hpp"

using namespace appkit;
using namespace netkit;

typedef struct
{
    const char* s;
    bool isValid;
    char delim;
    unsigned long addr;
    unsigned short port;
    unsigned long long n64;
} sample_t;

const sample_t SAMPLE[] =
{
    {"0.0.0.0:0", true, ':', 0x00000000UL, 0, 0x0000000000000000ULL},
    {"1.2.3.4:6", true, ':', 0x01020304UL, 6, 0x0102030400060000ULL},
    {"1.2.3.4:5", true, ':', 0x01020304UL, 5, 0x0102030400050000ULL},
    {"11.22.33.44.55", true, '.', 0x0b16212cUL, 55, 0x0b16212c00370000ULL},
    {"10.2.3.4.5", true, 0, 0x0a020304UL, 5, 0x0a02030400050000ULL},
    {"9.2.3.4.5", true, 0, 0x09020304UL, 5, 0x0902030400050000ULL},
    {0, false, 0, 0x00000000UL, 0, 0x0000000000000000ULL},
    {"1.2.3.4", false, 0, 0x00000000UL, 0, 0x0000000000000000ULL},
    {"1.2.333.4:5", false, ':', 0x00000000UL, 0, 0x0000000000000000ULL},
    {"1.2.x.4:5", false, ':', 0x00000000UL, 0, 0x0000000000000000ULL},
    {"1.2.3.x:5", false, ':', 0x00000000UL, 0, 0x0000000000000000ULL},
    {"1.2.3.4:x", false, ':', 0x00000000UL, 0, 0x0000000000000000ULL},
    {"1.2.3.4:5", false, '.', 0x00000000UL, 0, 0x0000000000000000ULL}
};
const size_t NUM_SAMPLES = sizeof(SAMPLE) / sizeof(SAMPLE[0]);


PaddrSuite::PaddrSuite()
{
}


PaddrSuite::~PaddrSuite()
{
}


void PaddrSuite::testCompare00()
{
    char delim = 0;
    Paddr paddr0("1.2.3.4.210", delim);
    Paddr paddr1("1.2.3.4.123", delim);
    Paddr paddr2("1.23.32.4.123", delim);
    Paddr paddr3(paddr2);
    bool ok = (Paddr::compareP(&paddr0, &paddr1) > 0) && (Paddr::compareP(&paddr0, &paddr2) < 0);
    CPPUNIT_ASSERT(ok);
    ok = (Paddr::compareP(&paddr1, &paddr0) < 0) && (Paddr::compareP(&paddr2, &paddr0) > 0);
    CPPUNIT_ASSERT(ok);
    ok = (Paddr::compareP(&paddr0, &paddr0) == 0) && (Paddr::compareP(&paddr2, &paddr3) == 0);
    CPPUNIT_ASSERT(ok);
}


void PaddrSuite::testCtor00()
{
    Paddr paddr0(0x12345678UL, 0x9876U);
    bool ok = (paddr0.addr() == 0x12345678UL) &&
        (paddr0.port() == 0x9876U) &&
        (paddr0.toString() == "18.52.86.120:39030") &&
        (paddr0.toN64() == 0x1234567898760000ULL);
    CPPUNIT_ASSERT(ok);

    Paddr paddr1(Paddr::fromN64(0x1234567898760000ULL));
    ok = (paddr1 == paddr0);
    CPPUNIT_ASSERT(ok);

    double rawAddr = paddr0.raw();
    paddr0 = Paddr(rawAddr);
    ok = (paddr0 == paddr1);
    CPPUNIT_ASSERT(ok);

    paddr1 = Paddr(0.0);
    String* s0 = 0;
    paddr0 = Paddr(s0);
    ok = (paddr0 == paddr1);
    CPPUNIT_ASSERT(ok);

    String s1("1.2.3.4-5");
    char delim = '-';
    paddr0 = Paddr(&s1, delim);
    ok = (paddr0.toString(delim) == s1);
    CPPUNIT_ASSERT(ok);

    char host[255 + 1];
    host[0] = 0;
    gethostname(host, sizeof(host));
    ok = paddr0.reset(host, ':' /*delim*/, true /*portIsOptional*/) && (IpDevice::find(paddr0.addr()) != 0) && (paddr0.port() == 0);
    CPPUNIT_ASSERT(ok);
    ok = paddr1.reset(":9") && (IpDevice::find(paddr1.addr()) != 0) && (paddr1.port() == 9);
    CPPUNIT_ASSERT(ok);
    ok = paddr0.reset("localhost:1638") && (paddr0.addr() == INADDR_LOOPBACK) && (paddr0.port() == 1638);
    CPPUNIT_ASSERT(ok);
    paddr1 = paddr0;
    ok = (!paddr1.reset("localhost@ 999", '@' /*delim*/)) && (paddr1 == paddr0);
    CPPUNIT_ASSERT(ok);
    ok = (!paddr1.reset("localhost", 0 /*delim*/, false /*portIsOptional*/)) && (paddr1 == paddr0);
    CPPUNIT_ASSERT(ok);
}


void PaddrSuite::testReset00()
{
    bool ok = true;
    Paddr paddr;
    for (unsigned long i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        if ((paddr.reset(r.s, r.delim) != r.isValid) ||
            (r.isValid && ((paddr != Paddr(r.addr, r.port)) || (paddr.toN64() != r.n64) || (Paddr::fromN64(r.n64) != paddr))))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    bool portIsOptional = true;
    ok = paddr.reset("1.2.3.4", ':', portIsOptional) && (paddr == Paddr(0x01020304UL, 0U));
    CPPUNIT_ASSERT(ok);

    paddr.reset();
    ok = (paddr == Paddr());
    CPPUNIT_ASSERT(ok);
}


void PaddrSuite::testToString00()
{
    Paddr paddr(0x0c22384eUL, 9098U);
    char s0[Paddr::StrLength + 1] = "";
    bool ok = (paddr.toString(s0, '.') == s0) && (strcmp(s0, "12.34.56.78.9098") == 0);
    CPPUNIT_ASSERT(ok);

    String s1 = paddr;
    ok = (s1 == "12.34.56.78:9098");
    CPPUNIT_ASSERT(ok);
}


void PaddrSuite::testToString01()
{
    Paddr paddr(0x01020304UL, 5U);
    bool ok = (paddr.toString() == "1.2.3.4:5");
    CPPUNIT_ASSERT(ok);
}


void PaddrSuite::testVec00()
{
    bool ok = true;
    Paddr paddr;
    PaddrVec vec0;
    String addr;
    StringVec addrs;
    for (unsigned long i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        if (r.isValid)
        {
            paddr.reset(r.addr, r.port);
            unsigned long foundAt = vec0.numItems();
            if ((!vec0.add(paddr)) || (vec0.findIndex(paddr) != foundAt))
            {
                ok = false;
                break;
            }
            addr = paddr;
            addrs.add(addr);
        }
    }
    CPPUNIT_ASSERT(ok);

    PaddrVec vec1(vec0);
    ok = (vec1 == vec0);
    CPPUNIT_ASSERT(ok);
    vec1.reset();
    ok = (vec1 != vec0) && (vec1.numItems() == 0);
    CPPUNIT_ASSERT(ok);

    ok = vec1.reset(addrs) && (vec1 == vec0);
    CPPUNIT_ASSERT(ok);
    addr = "not-a-paddr";
    addrs.add(addr);
    ok = (!vec1.reset(addrs)) && (vec1 == vec0);
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
