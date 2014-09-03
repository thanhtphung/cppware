/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <string.h>

#include "netkit-pch.h"
#include "netkit/IpHdr.hpp"

BEGIN_NAMESPACE1(netkit)


IpHdr::IpHdr(const IpHdr& ipHdr):
RoIpPkt((unsigned char*)(&hdr_), sizeof(hdr_), false /*makeCopy*/)
{
    hdr_ = ipHdr.hdr_;
}


IpHdr::IpHdr(const RoIpPkt& ipPkt):
RoIpPkt((unsigned char*)(&hdr_), sizeof(hdr_), false /*makeCopy*/)
{
    memcpy(&hdr_, ipPkt.raw(), sizeof(hdr_));
}


IpHdr::IpHdr(unsigned char ihl, unsigned char ttl, unsigned char version):
RoIpPkt((unsigned char*)(&hdr_), sizeof(hdr_), false /*makeCopy*/)
{
    memset(&hdr_, 0, sizeof(hdr_));
    setIhl(ihl);
    setTtl(ttl);
    setVersion(version);
}


IpHdr::~IpHdr()
{
}


const IpHdr& IpHdr::operator =(const IpHdr& ipHdr)
{
    if (this != &ipHdr)
    {
        hdr_ = ipHdr.hdr_;
    }

    return *this;
}

END_NAMESPACE1
