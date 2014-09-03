/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "appkit/U16.hpp"

#include "netkit-pch.h"
#include "netkit/IcmpHdr.hpp"
#include "netkit/IcmpMsg.hpp"
#include "netkit/IpHdr.hpp"

using namespace appkit;

BEGIN_NAMESPACE1(netkit)


IcmpMsg::IcmpMsg(const IpHdr& ipHdr, const IcmpHdr& icmpHdr, const unsigned char* body, size_t bodyLength):
RoIpPkt(0 /*raw*/, 0 /*rawLength*/, false /*makeCopy*/),
RoIcmpPkt(0 /*raw*/, 0 /*rawLength*/, false /*makeCopy*/)
{
    size_t ipLength;
    const unsigned char* ip = ipHdr.raw(ipLength);

    size_t icmpLength;
    const unsigned char* icmp = icmpHdr.raw(icmpLength);

    size_t msgLength = icmpLength + bodyLength;
    size_t rawLength = ipLength + msgLength;
    msg_ = new unsigned char[rawLength];
    memcpy(msg_, ip, ipLength);
    memcpy(msg_ + ipLength, icmp, icmpLength);
    memcpy(msg_ + ipLength + icmpLength, body, bodyLength);
    setSums(ipLength, msgLength);

    bool makeCopy = false;
    RoIpPkt::reset(msg_, rawLength, makeCopy);
    RoIcmpPkt::reset(msg_ + ipLength, msgLength, makeCopy);
}


IcmpMsg::~IcmpMsg()
{
    delete[] msg_;
}


void IcmpMsg::reset(const IpHdr& ipHdr, const IcmpHdr& icmpHdr, const unsigned char* body, size_t bodyLength)
{
    size_t ipLength;
    const unsigned char* ip = ipHdr.raw(ipLength);

    size_t icmpLength;
    const unsigned char* icmp = icmpHdr.raw(icmpLength);

    size_t msgLength = icmpLength + bodyLength;
    if ((ipLength != (size_t)(RoIcmpPkt::raw() - msg_)) || (bodyLength != RoIcmpPkt::bodyLength()))
    {
        delete[] msg_;
        size_t rawLength = ipLength + msgLength;
        msg_ = new unsigned char[rawLength];
        bool makeCopy = false;
        RoIpPkt::reset(msg_, rawLength, makeCopy);
        RoIcmpPkt::reset(msg_ + ipLength, msgLength, makeCopy);
    }

    memcpy(msg_, ip, ipLength);
    memcpy(msg_ + ipLength, icmp, icmpLength);
    memcpy(msg_ + ipLength + icmpLength, body, bodyLength);
    setSums(ipLength, msgLength);
}


void IcmpMsg::setSums(size_t ipLength, size_t msgLength)
{
    struct
    {
        unsigned int src;
        unsigned int dst;
        unsigned char zero;
        unsigned char proto;
        unsigned short length;
    } ph;

    // Form pseudo header required for ICMP checksum computation.
    ph.src = ip_->src;
    ph.dst = ip_->dst;
    ph.zero = 0;
    ph.proto = IPPROTO_ICMP;
    ph.length = htons((unsigned short)(msgLength));
    RoIcmpPkt::hdr_t* th = (RoIcmpPkt::hdr_t*)(msg_ + ipLength);

    // Compute and update ICMP checksum.
    th->checksum = 0;
    const unsigned short* ph16 = (const unsigned short*)(&ph);
    unsigned short checksum = U16::checksum(ph16, 6, (const unsigned short*)(th), ((unsigned int)(msgLength)+1) >> 1);
    th->checksum = checksum;

    // Compute and update IP header checksum.
    ip_->headsum = 0;
    unsigned short headsum = U16::checksum(u16_, (unsigned int)(ipLength) >> 1);
    ip_->headsum = headsum;
}

END_NAMESPACE1
