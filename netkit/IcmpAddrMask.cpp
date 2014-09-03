/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <string.h>

#include "netkit-pch.h"
#include "netkit/IcmpAddrMask.hpp"

BEGIN_NAMESPACE1(netkit)


IcmpAddrMask::IcmpAddrMask(bool isReply):
RoIcmpPkt(msg_, HdrLength + BodyLength, false /*makeCopy*/)
{
    memset(msg_, 0, sizeof(msg_));
    hdr_.type = isReply? (unsigned char)(AddrMaskReply): (unsigned char)(AddrMaskReq);
}


IcmpAddrMask::IcmpAddrMask(const IcmpAddrMask& icmpAddrMask):
RoIcmpPkt(msg_, HdrLength + BodyLength, false /*makeCopy*/)
{
    memcpy(msg_, icmpAddrMask.msg_, sizeof(msg_));
}


IcmpAddrMask::~IcmpAddrMask()
{
}


const IcmpAddrMask& IcmpAddrMask::operator =(const IcmpAddrMask& icmpAddrMask)
{
    if (this != &icmpAddrMask)
    {
        memcpy(msg_, icmpAddrMask.msg_, sizeof(msg_));
    }

    return *this;
}

END_NAMESPACE1
