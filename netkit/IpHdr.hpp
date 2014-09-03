/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_IP_HDR_HPP
#define NETKIT_IP_HDR_HPP

#include "netkit/RoIpPkt.hpp"
#include "netkit/net.hpp"

BEGIN_NAMESPACE1(netkit)


//! IPv4 packet header
class IpHdr: private RoIpPkt
    //!
    //! A class representing an IP packet header.
    //!
{

public:
    enum
    {
        DefaultIhl = 5,
        DefaultTtl = 123,
        DefaultVersion = 4
    };

    IpHdr(const IpHdr& ipHdr);
    IpHdr(const RoIpPkt& ipPkt);
    IpHdr(unsigned char ihl = DefaultIhl, unsigned char ttl = DefaultTtl, unsigned char version = DefaultVersion);
    ~IpHdr();
    const IpHdr& operator =(const IpHdr& ipHdr);

    // From RoIpPkt.
    using RoIpPkt::dfFlag;
    using RoIpPkt::dst;
    using RoIpPkt::flags;
    using RoIpPkt::fragment;
    using RoIpPkt::headsum;
    using RoIpPkt::hdrLength;
    using RoIpPkt::id;
    using RoIpPkt::ihl;
    using RoIpPkt::isPartial;
    using RoIpPkt::length;
    using RoIpPkt::mfFlag;
    using RoIpPkt::proto;
    using RoIpPkt::raw;
    using RoIpPkt::rawHdr;
    using RoIpPkt::rawLength;
    using RoIpPkt::reservedFlag;
    using RoIpPkt::setRawHdr;
    using RoIpPkt::src;
    using RoIpPkt::tos;
    using RoIpPkt::ttl;
    using RoIpPkt::version;

    void resetFrag();
    void setDfFlag(bool dfFlag);
    void setDst(unsigned int dst);
    void setFlags(unsigned char flags);
    void setFragment(unsigned short fragment);
    void setHeadsum(unsigned short headsum);
    void setId(unsigned short id);
    void setIhl(unsigned char ihl);
    void setLength(unsigned short length);
    void setMfFlag(bool mfFlag);
    void setProto(unsigned char /*RoIpPkt::proto_e*/ proto);
    void setReservedFlag(bool reservedFlag);
    void setSrc(unsigned int src);
    void setTos(unsigned char tos);
    void setTtl(unsigned char ttl);
    void setVersion(unsigned char version);

private:
    hdr_t hdr_;

};

//! Reset header for use in a whole packet (first fragment with no more bit).
inline void IpHdr::resetFrag()
{
    hdr_.flagsFrag &= htons(static_cast<unsigned short>(~(IP_MF_M | IP_FRAG_M)));
}

inline void IpHdr::setDfFlag(bool dfFlag)
{
    dfFlag?
        (hdr_.flagsFrag |= htons(IP_DF_M)):
        (hdr_.flagsFrag &= htons(static_cast<unsigned short>(~IP_DF_M)));
}

inline void IpHdr::setDst(unsigned int dst)
{
    hdr_.dst = htonl(dst);
}

//! Update instance with given composite flags. Given value is a
//! 3-bit value, and only the least significant 3 bits are used.
inline void IpHdr::setFlags(unsigned char flags)
{
    unsigned short flagsFrag = ntohs(hdr_.flagsFrag);
    flagsFrag &= ~IP_FLAGS_M;
    flagsFrag |= (static_cast<unsigned short>(flags) << 13U);
    hdr_.flagsFrag = htons(flagsFrag);
}

//! Update instance with given fragment offset. Given offset is a
//! 13-bit value, and only the least significant 13 bits are used.
inline void IpHdr::setFragment(unsigned short fragment)
{
    unsigned short flagsFrag = ntohs(hdr_.flagsFrag);
    flagsFrag &= ~IP_FRAG_M;
    flagsFrag |= (fragment & IP_FRAG_M);
    hdr_.flagsFrag = htons(flagsFrag);
}

inline void IpHdr::setHeadsum(unsigned short headsum)
{
    hdr_.headsum = htons(headsum);
}

inline void IpHdr::setId(unsigned short id)
{
    hdr_.id = htons(id);
}

//! Update instance with given internet header length. Given length
//! is a 4-bit value, and only the least significant 4 bits are used.
inline void IpHdr::setIhl(unsigned char ihl)
{
    hdr_.vihl &= ~IP_IHL_M;
    hdr_.vihl |= (ihl & IP_IHL_M);
}

inline void IpHdr::setLength(unsigned short length)
{
    hdr_.length = htons(length);
}

inline void IpHdr::setMfFlag(bool mfFlag)
{
    mfFlag?
        (hdr_.flagsFrag |= htons(IP_MF_M)):
        (hdr_.flagsFrag &= htons(static_cast<unsigned short>(~IP_MF_M)));
}

inline void IpHdr::setProto(unsigned char /*RoIpPkt::proto_e*/ proto)
{
    hdr_.proto = proto;
}

inline void IpHdr::setReservedFlag(bool reservedFlag)
{
    reservedFlag?
        (hdr_.flagsFrag |= htons(IP_RESERVED_M)):
        (hdr_.flagsFrag &= htons(static_cast<unsigned short>(~IP_RESERVED_M)));
}

inline void IpHdr::setSrc(unsigned int src)
{
    hdr_.src = htonl(src);
}

inline void IpHdr::setTos(unsigned char tos)
{
    hdr_.tos = tos;
}

inline void IpHdr::setTtl(unsigned char ttl)
{
    hdr_.ttl = ttl;
}

//! Update instance with given version. Given version is a 4-bit value,
//! and only the least significant 4 bits are used.
inline void IpHdr::setVersion(unsigned char version)
{
    hdr_.vihl &= ~IP_VERSION_M;
    hdr_.vihl |= (version << 4U);
}

END_NAMESPACE1

#endif
