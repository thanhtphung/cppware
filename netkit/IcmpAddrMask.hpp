/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_ICMP_ADDR_MASK_HPP
#define NETKIT_ICMP_ADDR_MASK_HPP

#include "netkit/RoIcmpPkt.hpp"

BEGIN_NAMESPACE1(netkit)


//! icmp address mask request/reply
class IcmpAddrMask: private RoIcmpPkt
    //!
    //! A class representing an ICMP Address Mask Request or Address Mask Reply message.
    //! RFC: 792.
    //!
{

public:
    IcmpAddrMask(bool isReply = false);
    IcmpAddrMask(const IcmpAddrMask& icmpAddrMask);
    ~IcmpAddrMask();
    const IcmpAddrMask& operator =(const IcmpAddrMask& icmpAddrMask);

    // From RoIcmpPkt.
    using RoIcmpPkt::addrMask;
    using RoIcmpPkt::checksum;
    using RoIcmpPkt::code;
    using RoIcmpPkt::id;
    using RoIcmpPkt::raw;
    using RoIcmpPkt::rawLength;
    using RoIcmpPkt::seq;
    using RoIcmpPkt::type;

    void setAddrMask(unsigned int addrMask);
    void setChecksum();
    void setCode(unsigned char code);
    void setId(unsigned short id);
    void setSeq(unsigned short seq);
    void setType(bool isReply);

private:
    enum
    {
        BodyLength = 4
    };

    union
    {
        hdr_t hdr_;
        unsigned char msg_[HdrLength + BodyLength];
        unsigned short u16_[1];
    };

};

END_NAMESPACE1

#include "appkit/U16.hpp"

BEGIN_NAMESPACE1(netkit)

//! Compute the checksum of the packet,
//! then update the checksum part w/ computed value.
inline void IcmpAddrMask::setChecksum()
{
    hdr_.checksum = 0;
    unsigned int numItems = (HdrLength + BodyLength) >> 1;
    unsigned short checksum = appkit::U16::checksum(u16_, numItems);
    hdr_.checksum = checksum;
}

inline void IcmpAddrMask::setCode(unsigned char code)
{
    hdr_.code = code;
}

//! Update the identifier in this packet.
inline void IcmpAddrMask::setId(unsigned short id)
{
    hdr_.u16[0] = htons(id);
}

//! Update the sequence number in this packet.
inline void IcmpAddrMask::setSeq(unsigned short seq)
{
    hdr_.u16[1] = htons(seq);
}

inline void IcmpAddrMask::setType(bool isReply)
{
    hdr_.type = isReply? (unsigned char)(AddrMaskReply): (unsigned char)(AddrMaskReq);
}

END_NAMESPACE1

#endif
