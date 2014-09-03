/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "netkit-pch.h"
#include "netkit/IpCap.hpp"
#include "netkit/RoEtherPkt.hpp"
#include "netkit/RoIpPkt.hpp"


BEGIN_NAMESPACE1(netkit)


IpCap::IpCap(const RoEtherPkt& etherPkt, const RoIpPkt& ipPkt, unsigned int ifIndex, unsigned long long capTime)
{
    etherRaw_ = 0;

    capTime_ = capTime;
    etherPkt_ = &etherPkt;
    ifIndex_ = ifIndex;
    ipPkt_ = &ipPkt;
}


//!
//! Take over ownership of given raw packet.
//!
IpCap::IpCap(unsigned char* etherRaw, size_t etherRawLength, unsigned int ifIndex, unsigned long long capTime)
{
    etherRaw_ = etherRaw;

    capTime_ = capTime;
    bool makeCopy = false;
    etherPkt_ = new RoEtherPkt(etherRaw_, etherRawLength, makeCopy);
    ifIndex_ = ifIndex;
    ipPkt_ = new RoIpPkt(*etherPkt_, makeCopy);
}


IpCap::~IpCap()
{
    if (etherRaw_ != 0)
    {
        delete ipPkt_;
        delete etherPkt_;
        delete[] etherRaw_;
    }
}

END_NAMESPACE1
