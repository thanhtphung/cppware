/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_ICMP_TIMESTAMP_HPP
#define NETKIT_ICMP_TIMESTAMP_HPP

#include "netkit/RoIcmpPkt.hpp"

BEGIN_NAMESPACE1(netkit)


//! icmp timestamp/timestamp reply
class IcmpTimestamp: private RoIcmpPkt
    //!
    //! A class representing an ICMP Timestamp or Timestamp Reply message.
    //! RFC: 792.
    //!
{

public:
    IcmpTimestamp(bool isReply = false);
    IcmpTimestamp(const IcmpTimestamp& icmpTimestamp);
    ~IcmpTimestamp();
    const IcmpTimestamp& operator =(const IcmpTimestamp& icmpTimestamp);

    // From RoIcmpPkt.
    using RoIcmpPkt::checksum;
    using RoIcmpPkt::code;
    using RoIcmpPkt::id;
    using RoIcmpPkt::oStamp;
    using RoIcmpPkt::rStamp;
    using RoIcmpPkt::raw;
    using RoIcmpPkt::rawLength;
    using RoIcmpPkt::seq;
    using RoIcmpPkt::tStamp;
    using RoIcmpPkt::type;

    void setChecksum();
    void setCode(unsigned char code);
    void setId(unsigned short id);
    void setOStamp(unsigned int oStamp);
    void setRStamp(unsigned int rStamp);
    void setSeq(unsigned short seq);
    void setTStamp(unsigned int tStamp);
    void setType(bool isReply);

private:
    enum
    {
        BodyLength = 12
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
inline void IcmpTimestamp::setChecksum()
{
    hdr_.checksum = 0;
    unsigned int numItems = (HdrLength + BodyLength) >> 1;
    unsigned short checksum = appkit::U16::checksum(u16_, numItems);
    hdr_.checksum = checksum;
}

inline void IcmpTimestamp::setCode(unsigned char code)
{
    hdr_.code = code;
}

//! Update the identifier in this packet.
inline void IcmpTimestamp::setId(unsigned short id)
{
    hdr_.u16[0] = htons(id);
}

inline void IcmpTimestamp::setOStamp(unsigned int oStamp)
{
    hdr_.u32[1] = htonl(oStamp);
}

inline void IcmpTimestamp::setRStamp(unsigned int rStamp)
{
    hdr_.u32[2] = htonl(rStamp);
}

//! Update the sequence number in this packet.
inline void IcmpTimestamp::setSeq(unsigned short seq)
{
    hdr_.u16[1] = htons(seq);
}

inline void IcmpTimestamp::setTStamp(unsigned int tStamp)
{
    hdr_.u32[3] = htonl(tStamp);
}

inline void IcmpTimestamp::setType(bool isReply)
{
    hdr_.type = isReply? (unsigned char)(TimestampReply): (unsigned char)(Timestamp);
}

END_NAMESPACE1

#endif
