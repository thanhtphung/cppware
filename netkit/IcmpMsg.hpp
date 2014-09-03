/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_ICMP_MSG_HPP
#define NETKIT_ICMP_MSG_HPP

#include "netkit/RoIcmpPkt.hpp"
#include "netkit/RoIpPkt.hpp"

BEGIN_NAMESPACE1(netkit)

class IcmpHdr;
class IpHdr;


class IcmpMsg: public RoIpPkt, private RoIcmpPkt
{

public:
    IcmpMsg(const IcmpMsg& icmpMsg);
    IcmpMsg(const IpHdr& ipHdr, const IcmpHdr& icmpHdr, const unsigned char* body = 0, size_t bodyLength = 0);
    ~IcmpMsg();
    const IcmpMsg& operator =(const IcmpMsg& icmpMsg);

    // From RoIpPkt.
    using RoIpPkt::raw;
    using RoIpPkt::rawLength;

    void reset(const IpHdr& ipHdr, const IcmpHdr& icmpHdr, const unsigned char* body = 0, size_t bodyLength = 0);

private:
    union
    {
        RoIpPkt::hdr_t* ip_;
        const unsigned short* u16_;
        unsigned char* msg_;
    };

    void setSums(size_t, size_t);

};

END_NAMESPACE1

#endif
