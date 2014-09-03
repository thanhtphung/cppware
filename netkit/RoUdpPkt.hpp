/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_RO_UDP_PKT_HPP
#define NETKIT_RO_UDP_PKT_HPP

#include "netkit/net.hpp"
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(netkit)

class RoIpPkt;


//! read-only UDP packet
class RoUdpPkt
    //!
    //! A class representing a read-only UDP packet.
    //!
{

public:
#if _WIN32
#pragma pack(push,1)
#endif
    typedef struct hdr_s //RFC: 768
    {
        unsigned short srcPort;
        unsigned short dstPort;
        unsigned short length;
        unsigned short checksum;
    } hdr_t;
#if _WIN32
#pragma pack(pop)
#endif

    enum
    {
        HdrLength = sizeof(hdr_t)
    };

    RoUdpPkt(const RoIpPkt& ipPkt, bool makeCopy = false);
    RoUdpPkt(const RoUdpPkt& udpPkt);
    RoUdpPkt(const unsigned char* raw, size_t rawLength, bool makeCopy = false);
    ~RoUdpPkt();
    const RoUdpPkt& operator =(const RoUdpPkt& udpPkt);

    // Raw contents.
    bool isOk() const;
    bool reset(const RoIpPkt& ipPkt, bool makeCopy = false);
    bool reset(const unsigned char* raw, size_t rawLength, bool makeCopy = false);
    const unsigned char* raw() const;
    const unsigned char* raw(size_t& rawLength) const;
    size_t rawLength() const;

    // Header.
    unsigned short checksum() const;
    unsigned short dstPort() const;
    unsigned short length() const;
    unsigned short srcPort() const;

    // Body.
    const unsigned char* body() const;
    const unsigned char* body(size_t& bodyLength) const;
    size_t bodyLength() const;

private:
    bool copyMade_;
    unsigned int rawLength_;
    union
    {
        const hdr_t* pkt_;
        const unsigned char* raw_;
    };

    static hdr_t badPkt_;

};

//! Return true if instance was constructed successfully.
inline bool RoUdpPkt::isOk() const
{
    return (pkt_ != &badPkt_);
}

//! Return the body part. Use bodyLength() to obtain its length in bytes.
inline const unsigned char* RoUdpPkt::body() const
{
    return (raw_ + HdrLength);
}

//! Return the body part. Also return its length in bytes in <i>bodyLength</i>.
inline const unsigned char* RoUdpPkt::body(size_t& bodyLength) const
{
    bodyLength = rawLength_ - HdrLength;
    return (raw_ + HdrLength);
}

//! Return the raw packet. Use rawLength() to obtain its length in bytes.
inline const unsigned char* RoUdpPkt::raw() const
{
    return raw_;
}

//! Return the raw packet. Also return its length in bytes in <i>rawLength</i>.
inline const unsigned char* RoUdpPkt::raw(size_t& rawLength) const
{
    rawLength = rawLength_;
    return raw_;
}

//! Return the length (in bytes) of the body part.
inline size_t RoUdpPkt::bodyLength() const
{
    return (rawLength_ - HdrLength);
}

//! Return the length (in bytes) of the raw packet.
inline size_t RoUdpPkt::rawLength() const
{
    return rawLength_;
}

inline unsigned short RoUdpPkt::checksum() const
{
    return ntohs(pkt_->checksum);
}

//! Return the destination port in this packet.
inline unsigned short RoUdpPkt::dstPort() const
{
    return ntohs(pkt_->dstPort);
}

inline unsigned short RoUdpPkt::length() const
{
    return ntohs(pkt_->length);
}

//! Return the source port in this packet.
inline unsigned short RoUdpPkt::srcPort() const
{
    return ntohs(pkt_->srcPort);
}

END_NAMESPACE1

#endif
