/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "appkit/S32.hpp"
#include "appkit/String.hpp"
#include "appkit/U16.hpp"
#include "syskit/sys.hpp"

#include "netkit-pch.h"
#include "netkit/IpAddr.hpp"
#include "netkit/Paddr.hpp"
#include "netkit/Winsock.hpp"

using namespace appkit;

const char DOT = '.';

BEGIN_NAMESPACE1(netkit)


Paddr::Paddr(const String* s, char delim, const Paddr& defaultV)
{
    if ((s == 0) || s->empty())
    {
        d64_ = defaultV.d64_;
    }
    else
    {
        d64_ = 0.0;
        reset(s->ascii(), delim, false /*portIsOptional*/);
    }
}


//!
//! Return new instance constructed from given network-order unsigned 64-bit number.
//!
Paddr Paddr::fromN64(unsigned long long n64)
{
    paddr_t u = {ntohll(n64)};
    u.u32[Addr] = ntohl(u.u32[Addr]);
    u.u16[Port] = ntohs(u.u16[Port]);
    return u.paddr;
}


bool Paddr::getAddrInfo(unsigned int& addr, unsigned short& port, const char* s, char delim, bool portIsOptional)
{

    // With the dotted notation, if delim is zero, any reasonable delimiter is acceptable.
    // With a hostname, the delimiter needs to be more specific and the default colon seems
    // like the best fit to avoid ambiguity. For example, both "xxx" and "xxx-1" can be
    // valid hostnames.
    if (delim == 0)
    {
        delim = ':';
    }

    bool ok;
    unsigned int addr32;
    unsigned int port32;
    const char* delimP = strrchr(s, delim);
    if (delimP == 0)
    {
        if (!portIsOptional)
        {
            ok = false;
            return ok; //no delimiter, but a port number is required
        }
        port32 = 0;
        ok = Winsock::getAddrInfo(addr32, s);
    }
    else
    {
        const char* p0 = delimP + 1;
        char* p1;
        port32 = strtoul(p0, &p1, 0);
        if ((!S32::isDigit(*p0)) || (p1 == p0) || (*p1 != 0) || (port32 > 65535UL))
        {
            ok = false;
            return ok; //invalid port number
        }
        ok = Winsock::getAddrInfo(addr32, s, delimP - s);
    }

    if (ok)
    {
        addr = addr32;
        port = static_cast<unsigned short>(port32);
    }

    return ok;
}


//!
//! Reset instance with given port address (e.g., "172.16.16.1:8080"). If
//! portIsOptional is set, then assume port is zero (e.g., "172.16.16.1"
//! would be the same as "172.16.16.1:0"). If delim is non-zero, given port
//! address must use that delimiter to delimit the address and the port parts.
//! If delim is zero, any delimiter is acceptable. Return true if s specifies
//! a valid port address. Use of a hostname instead of the dotted notation is
//! allowed. See IpAddr for details.
//!
bool Paddr::reset(const char* s, char delim, bool portIsOptional)
{

    // Sanity check.
    if (s == 0)
    {
        bool ok = false;
        return ok;
    }

    // String s must start with "a.b.c." (e.g., "172.16.16.").
    char* p;
    unsigned int abcde[5];
    for (size_t i = 0; i < 3; ++i, s = p + 1)
    {
        abcde[i] = strtoul(s, &p, 0);
        if ((!S32::isDigit(*s)) || (p == s) || (*p != DOT) || (abcde[i] > 255UL))
        {
            bool ok = (i == 0) && getAddrInfo(u32_[Addr], u16_[Port], s, delim, portIsOptional);
            return ok;
        }
    }

    // String s must start with "a.b.c.d" (e.g., "172.16.16.1").
    abcde[3] = strtoul(s, &p, 0);
    if ((!S32::isDigit(*s)) || (p == s) || (abcde[3] > 255UL))
    {
        bool ok = false;
        return ok;
    }

    bool delimIsValid = (delim == 0)? (S32::isPunct(*p)): (*p == delim);
    if (delimIsValid)
    {
        s = p + 1;
        abcde[4] = strtoul(s, &p, 0);
        if ((!S32::isDigit(*s)) || (p == s) || (*p != 0) || (abcde[4] > 65535UL))
        {
            bool ok = false;
            return ok;
        }
    }
    else
    {
        if ((!portIsOptional) || (*p != 0))
        {
            bool ok = false;
            return ok;
        }
        abcde[4] = 0;
    }

    // Return true to indicate s specifies a valid port address.
    u32_[Addr] = (abcde[0] << 24) | (abcde[1] << 16) | (abcde[2] << 8) | abcde[3];
    u16_[Port] = static_cast<unsigned short>(abcde[4]);
    bool ok = true;
    return ok;
}


//!
//! Compare two Paddr instances given their addresses. Return a negative value
//! if *item0 < *item1. Return 0 if *item0 == *item1. Return a positive value if
//! *item0 > *item1.
//!
int Paddr::compareP(const void* item0, const void* item1)
{
    const Paddr& k0 = *static_cast<const Paddr*>(item0);
    const Paddr& k1 = *static_cast<const Paddr*>(item1);

    int rc;
    if (k0.u32_[Addr] < k1.u32_[Addr])
    {
        rc = -1;
    }
    else if (k0.u32_[Addr] > k1.u32_[Addr])
    {
        rc = 1;
    }
    else if (k0.u16_[Port] < k1.u16_[Port])
    {
        rc = -1;
    }
    else
    {
        rc = (k0.u16_[Port] > k1.u16_[Port])? 1: 0;
    }

    return rc;
}


unsigned int Paddr::toDigits(char s[StrLength], char delim) const
{
    char* p = s;
    IpAddr ipAddr(u32_[Addr]);
    p += ipAddr.toDigits(p);
    *p++ = delim;
    p += U16::toDigits(u16_[Port], p);

    size_t length = p - s;
    return static_cast<unsigned int>(length);
}


//!
//! Convert to a network-order unsigned 64-bit number.
//!
unsigned long long Paddr::toN64() const
{
    paddr_t u = {paddr_};
    u.u32[Addr] = htonl(u.u32[Addr]);
    u.u16[Port] = htons(u.u16[Port]);
    return htonll(u.paddr);
}

END_NAMESPACE1
