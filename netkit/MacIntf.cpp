/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "appkit/S32.hpp"
#include "appkit/U16.hpp"
#include "syskit/sys.hpp"

#include "netkit-pch.h"
#include "netkit/MacIntf.hpp"

using namespace appkit;

BEGIN_NAMESPACE1(netkit)


//!
//! Return new instance constructed from given network-order unsigned 64-bit number.
//!
MacIntf MacIntf::fromN64(unsigned long long n64)
{
    macIntf_t u = {ntohll(n64)};
    u.intf[Intf] = ntohs(u.intf[Intf]);
    return u.macIntf;
}


bool MacIntf::reset(const char* s)
{

    // Assume string s has format "<mac><delim><intf>"
    // (e.g., "00.23.AE.6A.DA.57:123", "0023AE6ADA57.123").
    const char* mac0 = s;
    const char* intf0 = 0;
    size_t length = strlen(s);
    if (length > MacAddr::StrLength)
    {
        if (S32::isPunct(mac0[MacAddr::StrLength]))
        {
            intf0 = mac0 + MacAddr::StrLength + 1;
        }
        else if (S32::isPunct(mac0[12]))
        {
            intf0 = mac0 + 12 + 1;
        }
    }
    else if ((length > 12) && S32::isPunct(mac0[12]))
    {
        intf0 = mac0 + 12 + 1;
    }

    bool ok = false;
    if (intf0 != 0)
    {
        char* p;
        unsigned int intf = strtoul(intf0, &p, 0);
        if ((intf <= 0xffffU) && (*p == 0))
        {
            char macStr[MacAddr::StrLength + 1];
            length = intf0 - mac0 - 1;
            memcpy(macStr, s, length);
            macStr[length] = 0;
            MacAddr macAddr;
            ok = macAddr.reset(macStr);
            if (ok)
            {
                macIntf_ = macAddr.asU64();
                intf_[Intf] = static_cast<unsigned short>(intf);
            }
        }
    }

    return ok;
}


char* MacIntf::toSTRING(char s[StrLength + 1], char delim0, char delim1) const
{
    char* p = s;
    p += MacAddr(macIntf_).toXDIGITS(p, delim0);
    *p++ = delim1;
    p += U16::toDigits(intf_[Intf], p);
    *p = 0;

    return s;
}


char* MacIntf::toString(char s[StrLength + 1], char delim0, char delim1) const
{
    char* p = s;
    p += MacAddr(macIntf_).toXdigits(p, delim0);
    *p++ = delim1;
    p += U16::toDigits(intf_[Intf], p);
    *p = 0;

    return s;
}


//!
//! Compare two MacIntf instances given their addresses. Return a negative value
//! if *item0 < *item1. Return 0 if *item0 == *item1. Return a positive value if
//! *item0 > *item1.
//!
int MacIntf::compareP(const void* item0, const void* item1)
{
    const MacIntf& k0 = *static_cast<const MacIntf*>(item0);
    const MacIntf& k1 = *static_cast<const MacIntf*>(item1);

    int rc = memcmp(k0.mac_, k1.mac_, MacAddr::RawLength);
    if (rc == 0)
    {
        if (k0.intf_[Intf] < k1.intf_[Intf])
        {
            rc = -1;
        }
        else if (k0.intf_[Intf] > k1.intf_[Intf])
        {
            rc = 1;
        }
    }

    return rc;
}

END_NAMESPACE1
