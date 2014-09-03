/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <string.h>

#include "netkit-pch.h"
#include "netkit/IcmpHdr.hpp"

BEGIN_NAMESPACE1(netkit)


IcmpHdr::IcmpHdr():
RoIcmpPkt((unsigned char*)(&hdr_), sizeof(hdr_), false /*makeCopy*/)
{
    memset(&hdr_, 0, sizeof(hdr_));
}


IcmpHdr::IcmpHdr(const IcmpHdr& icmpHdr):
RoIcmpPkt((unsigned char*)(&hdr_), sizeof(hdr_), false /*makeCopy*/)
{
    hdr_ = icmpHdr.hdr_;
}


IcmpHdr::IcmpHdr(const RoIcmpPkt& icmpPkt):
RoIcmpPkt((unsigned char*)(&hdr_), sizeof(hdr_), false /*makeCopy*/)
{
    memcpy(&hdr_, icmpPkt.raw(), sizeof(hdr_));
}


IcmpHdr::~IcmpHdr()
{
}


const IcmpHdr& IcmpHdr::operator =(const IcmpHdr& icmpHdr)
{
    if (this != &icmpHdr)
    {
        hdr_ = icmpHdr.hdr_;
    }

    return *this;
}

END_NAMESPACE1
