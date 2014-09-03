/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_ICMP_HDR_HPP
#define NETKIT_ICMP_HDR_HPP

#include "netkit/RoIcmpPkt.hpp"
#include "netkit/net.hpp"

BEGIN_NAMESPACE1(netkit)


//! ICMP packet header
class IcmpHdr: private RoIcmpPkt
    //!
    //! A class representing an ICMP packet header.
    //!
{

public:
    IcmpHdr();
    IcmpHdr(const IcmpHdr& icmpHdr);
    IcmpHdr(const RoIcmpPkt& icmpPkt);
    ~IcmpHdr();
    const IcmpHdr& operator =(const IcmpHdr& icmpHdr);

    // From RoIcmpPkt.
    using RoIcmpPkt::addrMask;
    using RoIcmpPkt::checksum;
    using RoIcmpPkt::code;
    using RoIcmpPkt::gwAddr;
    using RoIcmpPkt::id;
    using RoIcmpPkt::oStamp;
    using RoIcmpPkt::ptr;
    using RoIcmpPkt::rStamp;
    using RoIcmpPkt::raw;
    using RoIcmpPkt::rawLength;
    using RoIcmpPkt::seq;
    using RoIcmpPkt::tStamp;
    using RoIcmpPkt::type;

    void setAddrMask(unsigned int addrMask);
    void setCode(unsigned char code);
    void setGwAddr(unsigned int gwAddr);
    void setId(unsigned short id);
    void setOStamp(unsigned int oStamp);
    void setPtr(unsigned char ptr);
    void setRStamp(unsigned int rStamp);
    void setSeq(unsigned short seq);
    void setTStamp(unsigned int tStamp);
    void setType(unsigned char /*type_e*/ type);

private:
    hdr_t hdr_;

};

//! Update the address mask in this packet.
//! This method is valid for types MaskReq and MaskReply only.
inline void IcmpHdr::setAddrMask(unsigned int addrMask)
{
    hdr_.u32[1] = htonl(addrMask);
}

inline void IcmpHdr::setCode(unsigned char code)
{
    hdr_.code = code;
}

//! Update the gateway internet address in this packet.
//! This method is valid for type Redirect only.
inline void IcmpHdr::setGwAddr(unsigned int gwAddr)
{
    hdr_.u32[0] = htonl(gwAddr);
}

//! Update the identifier in this packet. This method is valid for the following
//! types only: Echo, EchoReply, InfoReq, InfoReply, MaskReq, MaskReply, Timestamp,
//! TimestampReply.
inline void IcmpHdr::setId(unsigned short id)
{
    hdr_.u16[0] = htons(id);
}

//! Update the originate timestamp in this packet.
//! This method is valid for types Timestamp and TimestampReply only.
inline void IcmpHdr::setOStamp(unsigned int oStamp)
{
    hdr_.u32[1] = htonl(oStamp);
}

//! Update the pointer value in this packet.
//! This method is valid for type ParmProb only.
inline void IcmpHdr::setPtr(unsigned char ptr)
{
    hdr_.u8[0] = ptr;
}

//! Update the receive timestamp in this packet.
//! This method is valid for types Timestamp and TimestampReply only.
inline void IcmpHdr::setRStamp(unsigned int rStamp)
{
    hdr_.u32[2] = htonl(rStamp);
}

//! Update the sequence number in this packet. This method is valid for the following
//! types only: Echo, EchoReply, InfoReq, InfoReply, MaskReq, MaskReply, Timestamp,
//! TimestampReply.
inline void IcmpHdr::setSeq(unsigned short seq)
{
    hdr_.u16[1] = htons(seq);
}

//! Update the transmit timestamp in this packet.
//! This method is valid for types Timestamp and TimestampReply only.
inline void IcmpHdr::setTStamp(unsigned int tStamp)
{
    hdr_.u32[3] = htonl(tStamp);
}

inline void IcmpHdr::setType(unsigned char /*type_e*/ type)
{
    hdr_.type = type;
}

END_NAMESPACE1

#endif
