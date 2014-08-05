/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "appkit/S32.hpp"
#include "appkit/U8.hpp"
#include "syskit/sys.hpp"

#include "netkit-pch.h"
#include "netkit/MacAddr.hpp"

using namespace appkit;
using namespace syskit;

BEGIN_NAMESPACE1(netkit)

const MacAddr::bcast_t MacAddr::bcast_ = {{0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0x00U, 0x00U}};


//!
//! Return new instance constructed from given network-order unsigned 64-bit number.
//!
MacAddr MacAddr::fromN64(unsigned long long n64)
{
    unsigned long long u64 = ntohll(n64);
    return u64;
}


//!
//! Return true if this is a VMware-assigned address.
//!
bool MacAddr::isVmware() const
{

    // VMware uses these MAC prefixes: 00-05-69, 00-0c-29, 00-1c-14, and 00-50-56.
    bool vmware;
    if (addr_[0] == 0x00U)
    {
        vmware = ((addr_[1] == 0x05U) && (addr_[2] == 0x69U)) ||
            ((addr_[1] == 0x0cU) && (addr_[2] == 0x29U)) ||
            ((addr_[1] == 0x1cU) && (addr_[2] == 0x14U)) ||
            ((addr_[1] == 0x50U) && (addr_[2] == 0x56U));
    }
    else
    {
        vmware = false;
    }

    return vmware;
}


//!
//! Reset instance with given MAC address. If delim is non-zero, given MAC address
//! must use that delimiter to delimit individual bytes. If delim is zero, any
//! delimiter is acceptable. Return true if s specifies a valid MAC address. For
//! example, "00:23:AE:6A:DA:57" -> {0x00U, 0x23U, 0xaeU, 0x6aU, 0xdaU, 0x57U}.
//! Instance is not updated if unsuccessful.
//!
bool MacAddr::reset(const char* s, char delim)
{
    // Sanity check.
    bool ok = false;
    if (s == 0)
    {
        return ok;
    }

    // Determine proper delimiter.
    if (delim == 0)
    {
        if ((s[0] == 0) || (s[1] == 0) || (s[2] == 0)) return ok;
        if (S32::isXdigit(s[2]) || (!S32::isPunct(s[2])))
        {
            unsigned char addr[RawLength];
            for (unsigned int i = 0; i < 12; i += 2)
            {
                if ((!S32::isXdigit(s[i])) || (!S32::isXdigit(s[i + 1]))) return ok;
                addr[i >> 1] = U8::xdigitsToU8(s[i], s[i + 1]);
            }
            ok = (s[12] == 0);
            return ok? (memcpy(addr_, addr, RawLength), ok): (ok);
        }
        delim = s[2];
    }

    // Validate the first five bytes.
    for (unsigned int i = 0; i < 15; i += 3)
    {
        if ((!S32::isXdigit(s[i])) || (!S32::isXdigit(s[i + 1])) || (s[i + 2] != delim)) return ok;
    }

    // Validate the last byte.
    if ((!S32::isXdigit(s[15])) || (!S32::isXdigit(s[16])) || (s[17] != 0)) return ok;

    // Return result.
    addr_[0] = U8::xdigitsToU8(s[0], s[1]);
    addr_[1] = U8::xdigitsToU8(s[3], s[4]);
    addr_[2] = U8::xdigitsToU8(s[6], s[7]);
    addr_[3] = U8::xdigitsToU8(s[9], s[10]);
    addr_[4] = U8::xdigitsToU8(s[12], s[13]);
    addr_[5] = U8::xdigitsToU8(s[15], s[16]);
    ok = true;
    return ok;
}


//!
//! Compare two MacAddr instances given their addresses. Return a negative value
//! if *item0 < *item1. Return 0 if *item0 == *item1. Return a positive value if
//! *item0 > *item1.
//!
int MacAddr::compareP(const void* item0, const void* item1)
{
    const unsigned char* k0 = static_cast<const MacAddr*>(item0)->addr_;
    const unsigned char* k1 = static_cast<const MacAddr*>(item1)->addr_;
    int rc = memcmp(k0, k1, RawLength);
    return rc;
}


//!
//! Convert to string s and return its length. Resulting string s is not null-terminated.
//! For example, {0x00U, 0x23U, 0xaeU, 0x6aU, 0xdaU, 0x57U} --> "00:23:AE:6A:DA:57".
//!
unsigned int MacAddr::toXDIGITS(char s[StrLength], char delim) const
{

    // Convert the first five bytes.
    char* p0 = s;
    const unsigned char* p1 = addr_;
    for (const unsigned char* p1End = p1 + RawLength - 1; p1 < p1End; ++p1)
    {
        *p0++ = syskit::XDIGIT[*p1 >> 4U];
        *p0++ = syskit::XDIGIT[*p1 & 0x0fU];
        if (delim != 0)
        {
            *p0++ = delim;
        }
    }

    // Convert the last byte.
    *p0++ = syskit::XDIGIT[*p1 >> 4U];
    *p0++ = syskit::XDIGIT[*p1 & 0x0fU];
    size_t length = p0 - s;
    return static_cast<unsigned int>(length);
}


//!
//! Convert to string s and return its length. Resulting string s is not null-terminated.
//! For example, {0x00U, 0x23U, 0xaeU, 0x6aU, 0xdaU, 0x57U} --> "00:23:ae:6a:da:57".
//!
unsigned int MacAddr::toXdigits(char s[StrLength], char delim) const
{

    // Convert the first five bytes.
    char* p0 = s;
    const unsigned char* p1 = addr_;
    for (const unsigned char* p1End = p1 + RawLength - 1; p1 < p1End; ++p1)
    {
        *p0++ = syskit::xdigit[*p1 >> 4U];
        *p0++ = syskit::xdigit[*p1 & 0x0fU];
        if (delim != 0)
        {
            *p0++ = delim;
        }
    }

    // Convert the last byte.
    *p0++ = syskit::xdigit[*p1 >> 4U];
    *p0++ = syskit::xdigit[*p1 & 0x0fU];
    size_t length = p0 - s;
    return static_cast<unsigned int>(length);
}

END_NAMESPACE1
