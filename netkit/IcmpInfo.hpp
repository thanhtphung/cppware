/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_ICMP_INFO_HPP
#define NETKIT_ICMP_INFO_HPP

#include "netkit/RoIcmpPkt.hpp"

BEGIN_NAMESPACE1(netkit)


//! icmp information request/reply
class IcmpInfo: private RoIcmpPkt
    //!
    //! A class representing an ICMP Information Request or Information Reply message.
    //! RFC: 792.
    //!
{

public:
    IcmpInfo(bool isReply = false);
    IcmpInfo(const IcmpInfo& icmpInfo);
    ~IcmpInfo();
    const IcmpInfo& operator =(const IcmpInfo& icmpInfo);

    // From RoIcmpPkt.
    using RoIcmpPkt::checksum;
    using RoIcmpPkt::code;
    using RoIcmpPkt::id;
    using RoIcmpPkt::raw;
    using RoIcmpPkt::rawLength;
    using RoIcmpPkt::seq;
    using RoIcmpPkt::type;

    void setChecksum();
    void setCode(unsigned char code);
    void setId(unsigned short id);
    void setSeq(unsigned short seq);
    void setType(bool isReply);

private:
    union
    {
        hdr_t hdr_;
        unsigned char msg_[HdrLength];
        unsigned short u16_[1];
    };

};

END_NAMESPACE1

#include "appkit/U16.hpp"

BEGIN_NAMESPACE1(netkit)

//! Compute the checksum of the packet,
//! then update the checksum part w/ computed value.
inline void IcmpInfo::setChecksum()
{
    hdr_.checksum = 0;
    unsigned int numItems = HdrLength >> 1;
    unsigned short checksum = appkit::U16::checksum(u16_, numItems);
    hdr_.checksum = checksum;
}

inline void IcmpInfo::setCode(unsigned char code)
{
    hdr_.code = code;
}

//! Update the identifier in this packet.
inline void IcmpInfo::setId(unsigned short id)
{
    hdr_.u16[0] = htons(id);
}

//! Update the sequence number in this packet.
inline void IcmpInfo::setSeq(unsigned short seq)
{
    hdr_.u16[1] = htons(seq);
}

inline void IcmpInfo::setType(bool isReply)
{
    hdr_.type = isReply? (unsigned char)(InfoReply): (unsigned char)(InfoReq);
}

END_NAMESPACE1

#endif
