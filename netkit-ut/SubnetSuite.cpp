#include "appkit/String.hpp"
#include "netkit/IpAddr.hpp"
#include "netkit/IpAddrSet.hpp"
#include "netkit/Subnet.hpp"

#include "netkit-ut-pch.h"
#include "SubnetSuite.hpp"

using namespace appkit;
using namespace netkit;

typedef struct
{
    const char* id;
    const char* cidrBitMask;
    const char* classfulBitMask;
    const char* range;
    const char* str0;
    const char* str1;
    const char* str2;
    size_t size;
    unsigned int addr;
    unsigned int broadcast;
    unsigned int clas;
    unsigned int mask;
    unsigned int wildcard;
} sample_t;

const sample_t SAMPLE[] =
{
    {
        "10.0.0.0",
        "nnnnnnnn.hhhhhhhh.hhhhhhhh.hhhhhhhh",
        "0nnnnnnn.hhhhhhhh.hhhhhhhh.hhhhhhhh",
        "10.0.0.1-10.255.255.254",
        "10.0.0.0",
        "10.0.0.0/8",
        "10.0.0.0/255.0.0.0",
        0xfffffeUL,
        0x0a000000UL,
        0x0affffffUL,
        Subnet::A,
        0xff000000UL,
        0x00ffffffUL
    },
    {
        "10.255.255.255",
        "nnnnnnnn.hhhhhhhh.hhhhhhhh.hhhhhhhh",
        "0nnnnnnn.hhhhhhhh.hhhhhhhh.hhhhhhhh",
        "10.0.0.1-10.255.255.254",
        "10.0.0.0",
        "10.0.0.0/8",
        "10.0.0.0/255.0.0.0",
        0xfffffeUL,
        0x0a000000UL,
        0x0affffffUL,
        Subnet::A,
        0xff000000UL,
        0x00ffffffUL
    },
    {
        "172.16.0.0",
        "nnnnnnnn.nnnnnnnn.hhhhhhhh.hhhhhhhh",
        "10nnnnnn.nnnnnnnn.hhhhhhhh.hhhhhhhh",
        "172.16.0.1-172.16.255.254",
        "172.16.0.0",
        "172.16.0.0/16",
        "172.16.0.0/255.255.0.0",
        0x00fffeUL,
        0xac100000UL,
        0xac10ffffUL,
        Subnet::B,
        0xffff0000UL,
        0x0000ffffUL
    },
    {
        "172.16.1.2",
        "nnnnnnnn.nnnnnnnn.hhhhhhhh.hhhhhhhh",
        "10nnnnnn.nnnnnnnn.hhhhhhhh.hhhhhhhh",
        "172.16.0.1-172.16.255.254",
        "172.16.0.0",
        "172.16.0.0/16",
        "172.16.0.0/255.255.0.0",
        0x00fffeUL,
        0xac100000UL,
        0xac10ffffUL,
        Subnet::B,
        0xffff0000UL,
        0x0000ffffUL
    },
    {
        "192.168.0.0",
        "nnnnnnnn.nnnnnnnn.nnnnnnnn.hhhhhhhh",
        "110nnnnn.nnnnnnnn.nnnnnnnn.hhhhhhhh",
        "192.168.0.1-192.168.0.254",
        "192.168.0.0",
        "192.168.0.0/24",
        "192.168.0.0/255.255.255.0",
        0x0000feUL,
        0xc0a80000UL,
        0xc0a800ffUL,
        Subnet::C,
        0xffffff00UL,
        0x000000ffUL
    },
    {
        "192.168.0.1",
        "nnnnnnnn.nnnnnnnn.nnnnnnnn.hhhhhhhh",
        "110nnnnn.nnnnnnnn.nnnnnnnn.hhhhhhhh",
        "192.168.0.1-192.168.0.254",
        "192.168.0.0",
        "192.168.0.0/24",
        "192.168.0.0/255.255.255.0",
        0x0000feUL,
        0xc0a80000UL,
        0xc0a800ffUL,
        Subnet::C,
        0xffffff00UL,
        0x000000ffUL
    }
};

const size_t NUM_SAMPLES = sizeof(SAMPLE) / sizeof(SAMPLE[0]);

static bool validate(const Subnet& subnet, const sample_t& r)
{
    char cidrBitMask[Subnet::BitMaskStrLength + 1];
    char classfulBitMask[Subnet::BitMaskStrLength + 1];
    subnet.formBitMask(cidrBitMask, true /*useCidrMode*/);
    subnet.formBitMask(classfulBitMask, false /*useCidrMode*/);

    unsigned int loAddr;
    unsigned int hiAddr;
    subnet.getRange(loAddr, hiAddr);
    IpAddrSet range(r.range);

    char str0[Subnet::StrLength + 1];
    char str1[Subnet::StrLength + 1];
    char str2[Subnet::StrLength + 1];
    char* s0 = subnet.toStr(str0, 0);
    char* s1 = subnet.toStr(str1, 1);
    char* s2 = subnet.toStr(str2, 2);

    bool ok = subnet.isOk() &&
        (strcmp(cidrBitMask, r.cidrBitMask) == 0) &&
        (strcmp(classfulBitMask, r.classfulBitMask) == 0) &&
        (subnet.addr() == r.addr) &&
        (subnet.broadcastAddr() == r.broadcast) &&
        (subnet.clas() == r.clas) &&
        (subnet.mask() == r.mask) &&
        (subnet.size() == r.size) &&
        (subnet.wildcardMask() == r.wildcard) &&
        ((range.loAddrAt(0) == loAddr) && (range.hiAddrAt(0) == hiAddr)) &&
        ((s0 == str0) && (strcmp(s0, r.str0) == 0)) &&
        ((s1 == str1) && (strcmp(s1, r.str1) == 0)) &&
        ((s2 == str2) && (strcmp(s2, r.str2) == 0));

    return ok;
}


SubnetSuite::SubnetSuite()
{
}


SubnetSuite::~SubnetSuite()
{
}


void SubnetSuite::testCtor00()
{
    bool ok = true;
    char s[Subnet::StrLength + 1];
    for (unsigned int i = 0; i < NUM_SAMPLES; ++i)
    {

        const sample_t& r = SAMPLE[i];
        Subnet subnet(r.id);
        if (!validate(subnet, r))
        {
            ok = false;
            break;
        }

        subnet = Subnet(r.addr, r.mask, false /*skipNormalization*/);
        subnet.toStr(s, Subnet::CidrForm);
        if (String(s) != r.str1)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}
