#include "appkit/String.hpp"
#include "appkit/U32.hpp"
#include "netkit/IpAddrSet.hpp"

#include "netkit-ut-pch.h"
#include "IpAddrSetSuite.hpp"

using namespace appkit;
using namespace netkit;
using namespace syskit;


IpAddrSetSuite::IpAddrSetSuite()
{
}


IpAddrSetSuite::~IpAddrSetSuite()
{
}


//
// Add random addresses to empty set.
//
void IpAddrSetSuite::testAdd00()
{
    IpAddrSet set;

    String addr("192.168.0.");
    unsigned int addr32 = 0xc0a80000U;
    U32 u32;
    const char* s = "aRandomStringUsedForIpAddrSetPopulation!!!";
    bool ok = true;
    for (const char* p = s; *p; ++p)
    {
        u32 = *p;
        addr.truncate(10);
        addr += u32.toString();
        set.add(addr.ascii());
        if ((!set.contains(addr.ascii())) || (!set.contains(addr32 | u32)))
        {
            ok = false;
            break;
        }
        validateSet(set);
    }
    CPPUNIT_ASSERT(ok);
}


//
// Add random addresses to non-empty set.
//
void IpAddrSetSuite::testAdd01()
{
    IpAddrSet set;
    for (unsigned int addr = 0xac101001U; addr <= 0xac101063U; set.add(addr), addr += 2);

    String addr("172.16.16.");
    unsigned int addr32 = 0xac101000U;
    U32 u32;
    const char* s = "aRandomStringUsedForIpAddrSetPopulation!!!";
    bool ok = true;
    for (const char* p = s; *p; ++p)
    {
        u32 = *p;
        addr.truncate(10);
        addr += u32.toString();
        set.add(addr.ascii());
        if ((!set.contains(addr.ascii())) || (!set.contains(addr32 | u32)))
        {
            ok = false;
            break;
        }
        validateSet(set);
    }
    CPPUNIT_ASSERT(ok);
}


//
// Add even addresses to set of odd addresses.
//
void IpAddrSetSuite::testAdd02()
{
    IpAddrSet set;
    for (unsigned int addr = 0xac101001U; addr <= 0xac101063U; set.add(addr), addr += 2);

    // Add even addresses to set of odd addresses.
    String addr("172.16.16.");
    U32 u32;
    bool ok = true;
    for (unsigned int i = 2; i <= 100; i += 2)
    {
        u32 = i;
        addr.truncate(10);
        addr += u32.toString();
        if ((!set.add(addr.ascii())) || (!set.contains(addr.ascii())))
        {
            ok = false;
            break;
        }
        validateSet(set);
    }
    CPPUNIT_ASSERT(ok);

    ok = (set.numAddrs() == 100) && (set.numRanges() == 1);
    CPPUNIT_ASSERT(ok);
}


//
// Add overlapped addresses.
//
void IpAddrSetSuite::testAdd03()
{
    IpAddrSet set;
    for (unsigned int addr = 0xac101001U; addr <= 0xac101063U; set.add(addr), addr += 2);

    IpAddrSet set1(set);
    bool ok = set1.add("172.16.16.1-172.16.16.100");
    CPPUNIT_ASSERT(ok);
    validateSet(set1);
    ok = (set1.numAddrs() == 100) && (set1.numRanges() == 1);
    CPPUNIT_ASSERT(ok);

    IpAddrSet set2("172.16.16.1-172.16.16.100");
    ok = set.add(set2);
    CPPUNIT_ASSERT(ok);
    ok = (set == set1);
    CPPUNIT_ASSERT(ok);
}


//
// Add invalid addresses and invalid address ranges.
//
void IpAddrSetSuite::testAdd05()
{
    IpAddrSet tmp;
    bool ok = (!tmp.add("1.2.3")) &&
        (!tmp.add("1.2.333.4")) &&
        (!tmp.add("1.2.3.4.5")) &&
        (!tmp.add("1.2.3.4-1.2.3")) &&
        (!tmp.add("1.2.3.4-1.2.333.4")) &&
        (!tmp.add("1.2.3.4-1.2.3.4.5")) &&
        (!tmp.add("172.16.16.200-172.16.16.100"));
    CPPUNIT_ASSERT(ok);
}


void IpAddrSetSuite::testChk00()
{
    bool ok = (!IpAddrSet::isValid("192.135.178/0"));
    CPPUNIT_ASSERT(ok);

    ok = (!IpAddrSet::isValid("172.16.16.0//24"));
    CPPUNIT_ASSERT(ok);

    ok = (!IpAddrSet::isValid("172.16.17.0/25,172.16.16.0/"));
    CPPUNIT_ASSERT(ok);

    ok = IpAddrSet::isValid("172.16.16.0/24");
    CPPUNIT_ASSERT(ok);
}


//
// Try the contains() methods.
//
void IpAddrSetSuite::testCmp00()
{
    IpAddrSet set0;
    IpAddrSet set1;
    for (unsigned int addr = 0xac101001U; addr <= 0xac101063U; set1.add(addr), addr += 2);
    IpAddrSet set2;
    for (unsigned int addr = 0xac101002U; addr <= 0xac101064U; set2.add(addr), addr += 2);

    IpAddrSet tmp;
    tmp.add("172.16.16.1");
    tmp.add("172.16.16.9");

    bool ok = set1.contains(tmp) && tmp.contains(set0) && (!set2.contains(tmp)) && (!tmp.contains(set1));
    CPPUNIT_ASSERT(ok);
}


//
// Try the overlaps() methods.
//
void IpAddrSetSuite::testCmp01()
{
    IpAddrSet set1;
    for (unsigned int addr = 0xac101001U; addr <= 0xac101063U; set1.add(addr), addr += 2);
    IpAddrSet set2;
    for (unsigned int addr = 0xac101002U; addr <= 0xac101064U; set2.add(addr), addr += 2);

    IpAddrSet tmp;
    tmp.add("172.16.16.1");
    tmp.add("172.16.16.9");

    bool ok = set1.overlaps(tmp) && tmp.overlaps(set1) && (!set2.overlaps(tmp)) && (!tmp.overlaps(set2));
    CPPUNIT_ASSERT(ok);
}


//
// Default constructor.
//
void IpAddrSetSuite::testCtor00()
{
    IpAddrSet set;
    bool ok = set.canGrow() &&
        (set.growthFactor() == -1) &&
        (set.numAddrs() == 0) &&
        (set.numRanges() == 0) &&
        (set.capacity() == IpAddrSet::DefaultCap);
    CPPUNIT_ASSERT(ok);
}


//
// Construct with initial members.
//
void IpAddrSetSuite::testCtor01()
{
    IpAddrSet set0("192.168.0.1,192.168.0.1,192.168.0.10-192.168.0.99,192.168.0.50,192.168.0.5");
    bool ok = (set0.numAddrs() == 92) && (set0.numRanges() == 3);
    CPPUNIT_ASSERT(ok);
    validateSet(set0);

    IpAddrSet set1("192.168.0.1;192.168.0.1;192.168.0.10-192.168.0.99;192.168.0.50;192.168.0.5", ';');
    ok = (set1.numAddrs() == 92) && (set1.numRanges() == 3);
    CPPUNIT_ASSERT(ok);
    validateSet(set1);

    IpAddrSet set2("192.168.0.1 192.168.0.1 192.168.0.10-192.168.0.99 192.168.0.50 192.168.0.5", ' ');
    ok = (set2.numAddrs() == 92) && (set2.numRanges() == 3);
    CPPUNIT_ASSERT(ok);
    validateSet(set2);

    IpAddrSet set3("192.168.0.1, 192.168.0.1, 192.168.0.10-192.168.0.99, 192.168.0.50, 192.168.0.5 ", ',');
    ok = (set3.numAddrs() == 92) && (set3.numRanges() == 3);
    CPPUNIT_ASSERT(ok);
    validateSet(set3);
}


//
// Construct with empty string.
//
void IpAddrSetSuite::testCtor02()
{
    IpAddrSet set0;
    String s(set0.toString());
    IpAddrSet set(s.ascii());
    bool ok = (set == set0) && (set0 == set);
    CPPUNIT_ASSERT(ok);
}


//
// Construct with non-empty string.
//
void IpAddrSetSuite::testCtor03()
{
    IpAddrSet set0;
    String addr("192.168.0.");
    U32 u32;
    const char* s = "aRandomStringUsedForIpAddrSetPopulation!!!";
    for (const char* p = s; *p; ++p)
    {
        u32 = *p;
        addr.truncate(10);
        addr += u32.toString();
        set0.add(addr.ascii());
    }

    String str(set0.toString());
    IpAddrSet set1(str.ascii());
    bool ok = (set1 == set0) && (set0 == set1);
    CPPUNIT_ASSERT(ok);

    str = set0.toString(";");
    IpAddrSet set2(str.ascii(), ';');
    ok = (set2 == set0) && (set0 == set2);
    CPPUNIT_ASSERT(ok);

    str = set0.toString(" ");
    IpAddrSet set3(str.ascii(), ' ');
    ok = (set3 == set0) && (set0 == set3);
    CPPUNIT_ASSERT(ok);
}


void IpAddrSetSuite::testCtor04()
{
    IpAddrSet set;
    for (unsigned int addr = 0xac101001U; addr <= 0xac101063U; set.add(addr), addr += 2);

    // Copy constructor.
    IpAddrSet set0(set);
    bool ok = (set0 == set);
    CPPUNIT_ASSERT(ok);

    // Zero capacity.
    unsigned int capacity = 0;
    IpAddrSet set1(capacity);
    ok = set1.canGrow() &&
        (set1.capacity() == 1) &&
        (set1.growthFactor() == -1) &&
        (set1.numAddrs() == 0) &&
        (set1.numRanges() == 0);
    CPPUNIT_ASSERT(ok);

    IpAddrSet set2("0.0.0.0-255.255.255.255");
    ok = ((set2.numAddrs() == 0x100000000ULL) && (set2.numRanges() == 1));
    CPPUNIT_ASSERT(ok);
}


//
// Try iterating from low to high.
//
void IpAddrSetSuite::testItor00()
{
    IpAddrSet set("172.16.16.1,172.16.16.10-172.16.16.99,172.16.16.102");
    IpAddrSet::Itor it(set, false /*makeCopy*/);

    unsigned int loAddr;
    unsigned int hiAddr;
    bool ok = (!it.curRange(loAddr, hiAddr));
    CPPUNIT_ASSERT(ok);

    ok = it.nextRange(loAddr, hiAddr) && (loAddr == 0xac101001U) && (hiAddr == 0xac101001U);
    CPPUNIT_ASSERT(ok);

    ok = it.nextRange(loAddr, hiAddr) && (loAddr == 0xac10100aU) && (hiAddr == 0xac101063U);
    CPPUNIT_ASSERT(ok);

    ok = it.nextRange(loAddr, hiAddr) && (loAddr == 0xac101066U) && (hiAddr == 0xac101066U);
    CPPUNIT_ASSERT(ok);

    ok = (!it.nextRange(loAddr, hiAddr));
    CPPUNIT_ASSERT(ok);

    ok = (!it.nextRange(loAddr, hiAddr));
    CPPUNIT_ASSERT(ok);

    loAddr = 0;
    hiAddr = 0;
    ok = it.curRange(loAddr, hiAddr) && (loAddr == 0xac101066U) && (hiAddr == 0xac101066U);
    CPPUNIT_ASSERT(ok);

    it.reset();
    ok = it.nextRange(loAddr, hiAddr) && (loAddr == 0xac101001U) && (hiAddr == 0xac101001U);
    CPPUNIT_ASSERT(ok);

    loAddr = 0;
    hiAddr = 0;
    ok = it.curRange(loAddr, hiAddr) && (loAddr == 0xac101001U) && (hiAddr == 0xac101001U);
    CPPUNIT_ASSERT(ok);
}


//
// Try iterating from low to high.
//
void IpAddrSetSuite::testItor01()
{
    IpAddrSet set("192.168.0.1,192.168.0.10-192.168.0.99,192.168.0.102");
    IpAddrSet::Itor it;
    it.attach(set, false /*makeCopy*/);

    unsigned int addr;
    bool ok = (!it.curAddr(addr));
    CPPUNIT_ASSERT(ok);

    ok = it.nextAddr(addr) && (addr == 0xc0a80001U);
    CPPUNIT_ASSERT(ok);

    for (unsigned int i = 10; i <= 99; ++i)
    {
        if ((!it.nextAddr(addr)) || (addr != (0xc0a80000U + i)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = it.nextAddr(addr) && (addr == 0xc0a80066U);
    CPPUNIT_ASSERT(ok);

    ok = (!it.nextAddr(addr));
    CPPUNIT_ASSERT(ok);

    ok = (!it.nextAddr(addr));
    CPPUNIT_ASSERT(ok);

    addr = 0;
    ok = it.curAddr(addr) && (addr == 0xc0a80066U);
    CPPUNIT_ASSERT(ok);

    it.reset();
    ok = it.nextAddr(addr) && (addr == 0xc0a80001U);
    CPPUNIT_ASSERT(ok);

    addr = 0;
    ok = it.curAddr(addr) && (addr == 0xc0a80001U);
    CPPUNIT_ASSERT(ok);
}


//
// Try iterating from high to low.
//
void IpAddrSetSuite::testItor02()
{
    IpAddrSet set("172.16.16.1,172.16.16.10-172.16.16.99,172.16.16.102");
    IpAddrSet::Itor it(set, false /*makeCopy*/);

    unsigned int loAddr;
    unsigned int hiAddr;
    bool ok = it.prevRange(loAddr, hiAddr) && (loAddr == 0xac101066U) && (hiAddr == 0xac101066U);
    CPPUNIT_ASSERT(ok);

    ok = it.prevRange(loAddr, hiAddr) && (loAddr == 0xac10100aU) && (hiAddr == 0xac101063U);
    CPPUNIT_ASSERT(ok);

    ok = it.prevRange(loAddr, hiAddr) && (loAddr == 0xac101001U) && (hiAddr == 0xac101001U);
    CPPUNIT_ASSERT(ok);

    ok = (!it.prevRange(loAddr, hiAddr));
    CPPUNIT_ASSERT(ok);

    ok = (!it.prevRange(loAddr, hiAddr));
    CPPUNIT_ASSERT(ok);

    loAddr = 0;
    hiAddr = 0;
    ok = it.curRange(loAddr, hiAddr) && (loAddr == 0xac101001U) && (hiAddr == 0xac101001U);
    CPPUNIT_ASSERT(ok);

    it.reset();
    ok = it.prevRange(loAddr, hiAddr) && (loAddr == 0xac101066U) && (hiAddr == 0xac101066U);
    CPPUNIT_ASSERT(ok);

    loAddr = 0;
    hiAddr = 0;
    ok = it.curRange(loAddr, hiAddr) && (loAddr == 0xac101066U) && (hiAddr == 0xac101066U);
    CPPUNIT_ASSERT(ok);
}


//
// Try iterating from high to low.
//
void IpAddrSetSuite::testItor03()
{
    IpAddrSet set("192.168.0.1,192.168.0.10-192.168.0.99,192.168.0.102");
    IpAddrSet::Itor it(set, false /*makeCopy*/);

    unsigned int addr;
    bool ok = it.prevAddr(addr) && (addr == 0xc0a80066U);
    CPPUNIT_ASSERT(ok);

    for (unsigned int i = 99; i >= 10; --i)
    {
        if ((!it.prevAddr(addr)) || (addr != (0xc0a80000U + i)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = it.prevAddr(addr) && (addr == 0xc0a80001U);
    CPPUNIT_ASSERT(ok);

    ok = (!it.prevAddr(addr));
    CPPUNIT_ASSERT(ok);

    ok = (!it.prevAddr(addr));
    CPPUNIT_ASSERT(ok);

    addr = 0;
    ok = it.curAddr(addr) && (addr == 0xc0a80001U);
    CPPUNIT_ASSERT(ok);

    it.reset();
    ok = it.prevAddr(addr) && (addr == 0xc0a80066U);
    CPPUNIT_ASSERT(ok);

    addr = 0;
    ok = it.curAddr(addr) && (addr == 0xc0a80066U);
    CPPUNIT_ASSERT(ok);
}


void IpAddrSetSuite::testNew00()
{
    IpAddrSet* set0 = new IpAddrSet;
    bool ok = (set0 != 0);
    CPPUNIT_ASSERT(ok);
    delete set0;

    unsigned char buf[sizeof(*set0)];
    set0 = new(buf)IpAddrSet;
    ok = (reinterpret_cast<unsigned char*>(set0) == buf);
    CPPUNIT_ASSERT(ok);
    set0->IpAddrSet::~IpAddrSet();
}


//
// Try intersecting using an empty set. Resulting set remains empty.
//
void IpAddrSetSuite::testOp00()
{
    IpAddrSet set0;
    IpAddrSet set1;
    for (unsigned int addr = 0xac101001U; addr <= 0xac101063U; set1.add(addr), addr += 2);

    set0 &= set1;
    validateSet(set0);
    bool ok = (set0.numAddrs() == 0) && (set0.numRanges() == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Try intersecting using an empty set. Resulting set becomes empty.
//
void IpAddrSetSuite::testOp01()
{
    IpAddrSet set0;
    IpAddrSet set1;
    for (unsigned int addr = 0xac101001U; addr <= 0xac101063U; set1.add(addr), addr += 2);

    IpAddrSet tmp(set1);
    tmp &= set0;
    validateSet(tmp);
    bool ok = (tmp == set0) && (set0 == tmp);
    CPPUNIT_ASSERT(ok);
}


//
// Try intersecting using non-empty sets. Resulting set becomes empty.
//
void IpAddrSetSuite::testOp02()
{
    IpAddrSet set0;
    IpAddrSet set1;
    for (unsigned int addr = 0xac101001U; addr <= 0xac101063U; set1.add(addr), addr += 2);
    IpAddrSet set2;
    for (unsigned int addr = 0xac101002U; addr <= 0xac101064U; set2.add(addr), addr += 2);

    IpAddrSet tmp(set1);
    tmp &= set2;
    validateSet(tmp);
    bool ok = (tmp == set0) && (set0 == tmp);
    CPPUNIT_ASSERT(ok);
}


//
// Try intersecting using non-empty sets. Resulting set is non-empty.
//
void IpAddrSetSuite::testOp03()
{
    IpAddrSet set1;
    for (unsigned int addr = 0xac101001U; addr <= 0xac101063U; set1.add(addr), addr += 2);

    IpAddrSet tmp;
    tmp.add("72.16.16.1-172.16.16.100");

    tmp &= set1;
    validateSet(tmp);
    bool ok = (tmp == set1) && (set1 == tmp);
    CPPUNIT_ASSERT(ok);
}


//
// Assignment operator. No growth.
//
void IpAddrSetSuite::testOp04()
{
    IpAddrSet set1;
    for (unsigned int addr = 0xac101001U; addr <= 0xac101063U; set1.add(addr), addr += 2);

    IpAddrSet set(set1.capacity());
    set = set1;
    bool ok = (set == set1) && (set.capacity() == set1.capacity()) && (set.initialCap() == set1.capacity());
    CPPUNIT_ASSERT(ok);

    Set* set2 = set1.clone();
    ok = (set2->toString() == set1.toString());
    CPPUNIT_ASSERT(ok);
    ok = (set1.compare(*set2) == Set::Equal);
    CPPUNIT_ASSERT(ok);

    // Cannot downcast due to private/protected inheritance.
    ok = (dynamic_cast<IpAddrSet*>(set2) == 0);
    CPPUNIT_ASSERT(ok);
    delete set2;
}


//
// Assignment operator. Growth required.
//
void IpAddrSetSuite::testOp05()
{
    IpAddrSet set2;
    for (unsigned int addr = 0xac101002U; addr <= 0xac101064U; set2.add(addr), addr += 2);

    IpAddrSet set(8 /*capacity*/);
    set = set2;
    bool ok = (set == set2) && (set.capacity() == 64) && (set.initialCap() == 8);
    CPPUNIT_ASSERT(ok);
}


//
// Remove random addresses.
//
void IpAddrSetSuite::testRm00()
{
    IpAddrSet set;
    String addr;
    U32 u32;
    const char* s = "aRandomStringUsedForIpAddrSetPopulation!!!";
    for (const char* p = s; *p; ++p)
    {
        u32 = *p;
        addr = "192.168.0.";
        addr += u32.toString();
        set.add(addr.ascii());
        addr = "172.16.16.";
        addr += u32.toString();
        set.add(addr.ascii());
    }

    u32 = 'A';
    addr = "192.168.0.";
    addr += u32.toString();
    u32 = 'Z';
    addr += "-192.168.0.";
    addr += u32.toString();
    set.rm(addr.ascii());
    validateSet(set);

    u32 = 'a';
    addr = "192.168.0.";
    addr += u32.toString();
    u32 = 'z';
    addr += "-192.168.0.";
    addr += u32.toString();
    set.rm(addr.ascii());
    validateSet(set);

    u32 = '!';
    addr = "192.168.0.";
    addr += u32.toString();
    u32 = '!';
    addr += "-192.168.0.";
    addr += u32.toString();
    set.rm(addr.ascii());
    validateSet(set);

    IpAddrSet set1("172.16.16.0-172.16.16.9,172.16.16.210-172.16.16.255");
    set.rm(set1);
    validateSet(set);

    set.rm("172.16.16.10-172.16.16.19;172.16.16.123-172.16.16.145;172.16.16.189-172.16.16.209", ';');
    validateSet(set);

    set.rm("172.16.16.20-172.16.16.122 172.16.16.146-172.16.16.188", ' ');
    validateSet(set);

    bool ok = (set.numAddrs() == 0) && (set.numRanges() == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Start with one contiguous range. Remove even addresses to maximize
// the number of ranges. Remove odd addresses to collapse the set.
//
void IpAddrSetSuite::testRm01()
{

    // Start with one contiguous range 192.168.0.1-192.168.0.100.
    IpAddrSet set;
    set.add("192.168.0.1-192.168.0.100");

    // Remove even addresses.
    String addr("192.168.0.");
    U32 u32;
    bool ok = true;
    for (unsigned int i = 2; i <= 100; i += 2)
    {
        u32 = i;
        addr.truncate(10);
        addr += u32.toString();
        if ((!set.rm(addr.ascii())) || set.contains(addr.ascii()))
        {
            ok = false;
            break;
        }
        validateSet(set);
    }
    CPPUNIT_ASSERT(ok);

    ok = (set.numAddrs() == 50) && (set.numRanges() == 50);
    CPPUNIT_ASSERT(ok);

    // Remove odd addresses.
    for (unsigned int i = 1; i <= 99; i += 2)
    {
        u32 = i;
        addr.truncate(10);
        addr += u32.toString();
        if ((!set.rm(addr.ascii())) || set.contains(addr.ascii()))
        {
            ok = false;
            break;
        }
        validateSet(set);
    }
    CPPUNIT_ASSERT(ok);

    ok = (set.numAddrs() == 0) && (set.numRanges() == 0);
    CPPUNIT_ASSERT(ok);
}


void IpAddrSetSuite::testSize00()
{
    size_t size = sizeof(Set);
    bool ok = (size == (sizeof(Growable) + sizeof(void*))); //Win32:20 x64:28
    CPPUNIT_ASSERT(ok);

    size = sizeof(IpAddrSet);
    ok = (size == (sizeof(Set) + sizeof(void*) + 20)); //Win32:44 x64:56
    CPPUNIT_ASSERT(ok);
}


//
// Validate given set.
//
void IpAddrSetSuite::validateSet(const IpAddrSet& set)
{
    if (set.numAddrs() == 0)
    {
        bool ok = (set.numRanges() == 0);
        CPPUNIT_ASSERT(ok);
        return;
    }

    IpAddrSet::Itor it(set);
    unsigned int prevAddr;
    bool ok = it.nextAddr(prevAddr);
    CPPUNIT_ASSERT(ok);

    unsigned int addr;
    while (it.nextAddr(addr))
    {
        if (prevAddr >= addr)
        {
            ok = false;
            break;
        }
        prevAddr = addr;
    }
    CPPUNIT_ASSERT(ok);

    it.reset();
    unsigned int prevHiAddr;
    unsigned int prevLoAddr;
    ok = it.nextRange(prevLoAddr, prevHiAddr);
    CPPUNIT_ASSERT(ok);

    unsigned int hiAddr;
    unsigned int loAddr;
    while (it.nextRange(loAddr, hiAddr))
    {
        if ((loAddr > hiAddr) || (loAddr <= prevHiAddr))
        {
            ok = false;
            break;
        }
        prevLoAddr = loAddr;
        prevHiAddr = hiAddr;
    }
    CPPUNIT_ASSERT(ok);
}
