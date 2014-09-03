/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_RO_ICMP_PKT_HPP
#define NETKIT_RO_ICMP_PKT_HPP

#include "netkit/net.hpp"

BEGIN_NAMESPACE1(netkit)

class RoIpPkt;


//! read-only icmp packet
class RoIcmpPkt
    //!
    //! A class representing a read-only ICMP packet.
    //!
{

public:
#if _WIN32
#pragma pack(push,1)
#endif
    typedef struct hdr_s //RFCs: 792, 950, 1812
    {
        unsigned char type;
        unsigned char code;
        unsigned short checksum;
        union
        {
            unsigned char u8[4];
            unsigned int u32[1];
            unsigned short u16[2];
        };
    } hdr_t;
#if _WIN32
#pragma pack(pop)
#endif

    enum exceedCode_e //TimeExceed code
    {
        InTransit = 0,
        Reassembly = 1
    };

    enum type_e
    {
        EchoReply = 0,
        DstUnreachable = 3,
        SrcQuench = 4,
        Redirect = 5,
        Echo = 8,
        TimeExceed = 11,
        ParmProb = 12,
        Timestamp = 13,
        TimestampReply = 14,
        InfoReq = 15,
        InfoReply = 16,
        AddrMaskReq = 17,
        AddrMaskReply = 18
    };

    enum unreachableCode_e //DstUnreachable code
    {
        Net = 0,
        Host = 1,
        Proto = 2,
        Port = 3,
        FragNeeded = 4,
        SrcRouteFailed = 5,
        NetUnknown = 6,
        HostUnknown = 7,
        HostIsolated = 8,
        NetAno = 9,
        HostAno = 10,
        NetTos = 11,
        HostTos = 12,
        PktFiltered = 13,
        PrecViolation = 14,
        PrecCutoff = 15
    };

    enum
    {
        HdrLength = sizeof(hdr_t),
        HiType = AddrMaskReply
    };

    RoIcmpPkt(const RoIcmpPkt& icmpPkt);
    RoIcmpPkt(const RoIpPkt& ipPkt, bool makeCopy = false);
    RoIcmpPkt(const unsigned char* raw, size_t rawLength, bool makeCopy = false);
    ~RoIcmpPkt();
    const RoIcmpPkt& operator =(const RoIcmpPkt& icmpPkt);

    // Raw contents.
    bool isOk() const;
    bool reset(const unsigned char* raw, size_t rawLength, bool makeCopy = false);
    const unsigned char* raw() const;
    const unsigned char* raw(size_t& rawLength) const;
    size_t rawLength() const;
    void reset(const RoIpPkt& ipPkt, bool makeCopy = false);

    // Header.
    unsigned char code() const;
    unsigned char ptr() const;
    unsigned char /*type_e*/ type() const;
    unsigned int addrMask() const;
    unsigned int gwAddr() const;
    unsigned int oStamp() const;
    unsigned int rStamp() const;
    unsigned int tStamp() const;
    unsigned short checksum() const;
    unsigned short id() const;
    unsigned short seq() const;

    // Raw body.
    const unsigned char* body() const;
    const unsigned char* body(size_t& bodyLength) const;
    size_t bodyLength() const;

protected:
    static unsigned char* newBuf(size_t bufSize);

private:
    bool copyMade_;
    size_t rawLength_;
    union
    {
        const hdr_t* pkt_;
        const unsigned char* raw_;
    };

    static hdr_t badPkt_;

};

//! Return true if instance was constructed successfully.
inline bool RoIcmpPkt::isOk() const
{
    return (pkt_ != &badPkt_);
}

//! Return the body part. Use bodyLength() to obtain its length in bytes.
inline const unsigned char* RoIcmpPkt::body() const
{
    return (raw_ + HdrLength);
}

//! Return the body part. Also return its length in bytes in <i>bodyLength</i>.
inline const unsigned char* RoIcmpPkt::body(size_t& bodyLength) const
{
    bodyLength = rawLength_ - HdrLength;
    return (raw_ + HdrLength);
}


//! Return the raw packet. Use rawLength() to obtain its length in bytes.
inline const unsigned char* RoIcmpPkt::raw() const
{
    return raw_;
}

//! Return the raw packet. Also return its length in bytes in <i>rawLength</i>.
inline const unsigned char* RoIcmpPkt::raw(size_t& rawLength) const
{
    rawLength = rawLength_;
    return raw_;
}

//! Return the length (in bytes) of the body part.
inline size_t RoIcmpPkt::bodyLength() const
{
    return (rawLength_ - HdrLength);
}

//! Return the length (in bytes) of the raw packet.
inline size_t RoIcmpPkt::rawLength() const
{
    return rawLength_;
}

//! Return the pointer value in this packet.
//! This method is valid for type ParmProb only.
inline unsigned char RoIcmpPkt::ptr() const
{
    return pkt_->u8[0];
}

inline unsigned char RoIcmpPkt::code() const
{
    return pkt_->code;
}

inline unsigned char /*type_e*/ RoIcmpPkt::type() const
{
    return pkt_->type;
}

//! Return the address mask in this packet.
//! This method is valid for types AddrMaskReq and AddrMaskReply only.
inline unsigned int RoIcmpPkt::addrMask() const
{
    return ntohl(pkt_->u32[1]);
}

//! Return the gateway internet address in this packet.
//! This method is valid for type Redirect only.
inline unsigned int RoIcmpPkt::gwAddr() const
{
    return ntohl(pkt_->u32[0]);
}

//! Return the originate timestamp in this packet.
//! This method is valid for types Timestamp and TimestampReply only.
inline unsigned int RoIcmpPkt::oStamp() const
{
    return ntohl(pkt_->u32[1]);
}

//! Return the receive timestamp in this packet.
//! This method is valid for types Timestamp and TimestampReply only.
inline unsigned int RoIcmpPkt::rStamp() const
{
    return ntohl(pkt_->u32[2]);
}

//! Return the transmit timestamp in this packet.
//! This method is valid for types Timestamp and TimestampReply only.
inline unsigned int RoIcmpPkt::tStamp() const
{
    return ntohl(pkt_->u32[3]);
}

inline unsigned short RoIcmpPkt::checksum() const
{
    return ntohs(pkt_->checksum);
}

//! Return the identifier in this packet. This method is valid for the following
//! types only: Echo, EchoReply, InfoReq, InfoReply, AddrMaskReq, AddrMaskReply,
//! Timestamp, TimestampReply.
inline unsigned short RoIcmpPkt::id() const
{
    return ntohs(pkt_->u16[0]);
}

//! Return the sequence number in this packet. This method is valid for the following
//! types only: Echo, EchoReply, InfoReq, InfoReply, AddrMaskReq, AddrMaskReply, Timestamp,
//! TimestampReply.
inline unsigned short RoIcmpPkt::seq() const
{
    return ntohs(pkt_->u16[1]);
}

END_NAMESPACE1

#endif
