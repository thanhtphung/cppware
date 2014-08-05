/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "appkit/S32.hpp"
#include "appkit/U8.hpp"
#include "syskit/macros.h"

#include "netkit-pch.h"
#include "netkit/IpAddr.hpp"
#include "netkit/Winsock.hpp"

using namespace appkit;

const char DOT = '.';

BEGIN_NAMESPACE1(netkit)


//!
//! Reset instance with given address. Return true if successful. That is, if s
//! results in a valid dotted IP address. For example, "172.16.16.1". A hostname
//! is valid if it can be translated to an IPv4 address. An empty string results
//! in a registered address of the local computer, and "localhost" results in the
//! loopback address. Instance is not updated if unsuccessful.
//!
bool IpAddr::reset(const char* s)
{
    // Sanity check.
    if (s == 0)
    {
        bool ok = false;
        return ok;
    }

    // Assume string s has format "a.b.c.d" (e.g., "172.16.16.1"), but give a potential
    // hostname a chance.
    char* p;
    unsigned int abcd[4];
    static const char s_delim[4] = {DOT, DOT, DOT, 0};
    for (size_t i = 0; i < 4; ++i, s = p + 1)
    {
        abcd[i] = strtoul(s, &p, 0);
        if ((!S32::isDigit(*s)) || (p == s) || (*p != s_delim[i]) || (abcd[i] > 255U))
        {
            bool ok = (i == 0) && Winsock::getAddrInfo(addr_, s);
            return ok;
        }
    }

    // Return true to indicate s specifies a valid dotted IP address.
    addr_ = (abcd[0] << 24) | (abcd[1] << 16) | (abcd[2] << 8) | abcd[3];
    bool ok = true;
    return ok;
}


//!
//! Convert string s to addr. Return true if s results in a valid dotted IP address.
//! For example, s="172.16.16.1" -> addr[]={0xacU,0x10U,0x10U,0x01U}. A hostname is
//! valid if it can be translated to an IPv4 address. An empty string results in a
//! registered address of the local computer, and "localhost" results in the loopback
//! address.
bool IpAddr::toU8(unsigned char addr[4], const char* s)
{
    // Sanity check.
    if (s == 0)
    {
        bool ok = false;
        return ok;
    }

    // Assume string s has format "a.b.c.d" (e.g., "172.16.16.1"), but give a potential
    // hostname a chance.
    char* p;
    unsigned int abcd[4];
    static const char s_delim[4] = {DOT, DOT, DOT, 0};
    for (size_t i = 0; i < 4; ++i, s = p + 1)
    {
        abcd[i] = strtoul(s, &p, 0);
        if ((!S32::isDigit(*s)) || (p == s) || (*p != s_delim[i]) || (abcd[i] > 255))
        {
            union
            {
                unsigned char addr8[4];
                unsigned int addr32;
            };
            bool ok = (i == 0) && Winsock::getAddrInfo(addr32, s);
            if (ok)
            {
                addr[0] = addr8[0];
                addr[1] = addr8[1];
                addr[2] = addr8[2];
                addr[3] = addr8[3];
            }
            return ok;
        }
    }

    // Return true to indicate s specifies a valid dotted IP address.
    addr[0] = (unsigned char)(abcd[0]);
    addr[1] = (unsigned char)(abcd[1]);
    addr[2] = (unsigned char)(abcd[2]);
    addr[3] = (unsigned char)(abcd[3]);
    bool ok = true;
    return ok;
}


//!
//! Convert to string s and return its length. Resulting string
//! s is not null-terminated. For example, 0xac101001UL -> "172.16.16.1".
//!
unsigned int IpAddr::toDigits(char s[StrLength]) const
{
    char* p = s;
    p += U8::toDigits((unsigned char)(addr_ >> 24), p);
    *p++ = DOT;
    p += U8::toDigits((unsigned char)(addr_ >> 16), p);
    *p++ = DOT;
    p += U8::toDigits((unsigned char)(addr_ >> 8), p);
    *p++ = DOT;
    p += U8::toDigits((unsigned char)(addr_), p);

    size_t length = p - s;
    return static_cast<unsigned int>(length);
}

END_NAMESPACE1
