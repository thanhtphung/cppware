/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_RO_TCP_PKT_HPP
#define NETKIT_RO_TCP_PKT_HPP

#include "netkit/net.hpp"
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(netkit)

class RoIpPkt;


//! read-only TCP packet
class RoTcpPkt
    //!
    //! A class representing a read-only TCP packet.
    //!
{

public:
#if _WIN32
#pragma pack(push,1)
#endif
    typedef struct hdr_s //RFCs: 793, 1072, 1323
    {
        unsigned short srcPort;
        unsigned short dstPort;
        unsigned int seq;
        unsigned int ack;
        unsigned short flagsEtc;
        unsigned short win;
        unsigned short checksum;
        unsigned short urp;
    } hdr_t;
#if _WIN32
#pragma pack(pop)
#endif

    enum
    {
        MinHdrLength = sizeof(hdr_t)
    };

    RoTcpPkt(const RoTcpPkt& tcpPkt);
    RoTcpPkt(const RoIpPkt& ipPkt, bool makeCopy = false);
    RoTcpPkt(const unsigned char* raw, size_t rawLength, bool makeCopy = false);
    ~RoTcpPkt();
    const RoTcpPkt& operator =(const RoTcpPkt& tcpPkt);

    // Raw contents.
    bool isOk() const;
    bool reset(const RoIpPkt& ipPkt, bool makeCopy = false);
    bool reset(const unsigned char* raw, size_t rawLength, bool makeCopy = false);
    const unsigned char* raw() const;
    const unsigned char* raw(size_t& rawLength) const;
    unsigned int rawLength() const;

    // Header.
    bool ackFlag() const;
    bool finFlag() const;
    bool pshFlag() const;
    bool rstFlag() const;
    bool synFlag() const;
    bool urgFlag() const;
    unsigned char flags() const;
    unsigned char off() const;
    unsigned char reserved() const;
    unsigned int ack() const;
    unsigned int seq() const;
    unsigned short checksum() const;
    unsigned short dstPort() const;
    unsigned short srcPort() const;
    unsigned short urp() const;
    unsigned short win() const;

    // Body.
    bool findTsOpt(unsigned int& val, unsigned int& ecr) const;
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

const unsigned short TCP_ACK_M = 0x0010U;
const unsigned short TCP_FIN_M = 0x0001U;
const unsigned short TCP_FLAGS_M = 0x003fU;
const unsigned short TCP_OFF_M = 0xf000U;
const unsigned short TCP_PSH_M = 0x0008U;
const unsigned short TCP_RESERVED_M = 0x0fc0U;
const unsigned short TCP_RST_M = 0x0004U;
const unsigned short TCP_SYN_M = 0x0002U;
const unsigned short TCP_URG_M = 0x0020U;

inline bool RoTcpPkt::ackFlag() const
{
    return ((ntohs(pkt_->flagsEtc) & TCP_ACK_M) != 0);
}

inline bool RoTcpPkt::finFlag() const
{
    return ((ntohs(pkt_->flagsEtc) & TCP_FIN_M) != 0);
}

//! Return true if instance was constructed successfully.
inline bool RoTcpPkt::isOk() const
{
    return (pkt_ != &badPkt_);
}

inline bool RoTcpPkt::pshFlag() const
{
    return ((ntohs(pkt_->flagsEtc) & TCP_PSH_M) != 0);
}

inline bool RoTcpPkt::rstFlag() const
{
    return ((ntohs(pkt_->flagsEtc) & TCP_RST_M) != 0);
}

inline bool RoTcpPkt::synFlag() const
{
    return ((ntohs(pkt_->flagsEtc) & TCP_SYN_M) != 0);
}

inline bool RoTcpPkt::urgFlag() const
{
    return ((ntohs(pkt_->flagsEtc) & TCP_URG_M) != 0);
}

//! Return the body part. Use bodyLength() to obtain its length in bytes.
inline const unsigned char* RoTcpPkt::body() const
{
    return (raw_ + (ntohs(pkt_->flagsEtc) >> 10U));
}

//! Return the body part. Also return its length in bytes in <i>bodyLength</i>.
inline const unsigned char* RoTcpPkt::body(size_t& bodyLength) const
{
    size_t hdrLength = ntohs(pkt_->flagsEtc) >> 10U;
    bodyLength = rawLength_ - hdrLength;
    return (raw_ + hdrLength);
}

//! Return the raw packet. Use rawLength() to obtain its length in bytes.
inline const unsigned char* RoTcpPkt::raw() const
{
    return raw_;
}

//! Return the raw packet. Also return its length in bytes in <i>rawLength</i>.
inline const unsigned char* RoTcpPkt::raw(size_t& rawLength) const
{
    rawLength = rawLength_;
    return raw_;
}

//! Return the length (in bytes) of the body part.
inline size_t RoTcpPkt::bodyLength() const
{
    return (rawLength_ - (ntohs(pkt_->flagsEtc) >> 10U));
}

inline unsigned char RoTcpPkt::flags() const
{
    return (unsigned char)(ntohs(pkt_->flagsEtc) & TCP_FLAGS_M);
}

inline unsigned char RoTcpPkt::off() const
{
    return (unsigned char)(ntohs(pkt_->flagsEtc) >> 12U);
}

inline unsigned char RoTcpPkt::reserved() const
{
    return (unsigned char)((ntohs(pkt_->flagsEtc) & TCP_RESERVED_M) >> 6U);
}

inline unsigned int RoTcpPkt::ack() const
{
    return ntohl(pkt_->ack);
}

//! Return the length (in bytes) of the raw packet.
inline unsigned int RoTcpPkt::rawLength() const
{
    return rawLength_;
}

inline unsigned int RoTcpPkt::seq() const
{
    return ntohl(pkt_->seq);
}

inline unsigned short RoTcpPkt::checksum() const
{
    return ntohs(pkt_->checksum);
}

//! Return the destination port in this packet.
inline unsigned short RoTcpPkt::dstPort() const
{
    return ntohs(pkt_->dstPort);
}

//! Return the source port in this packet.
inline unsigned short RoTcpPkt::srcPort() const
{
    return ntohs(pkt_->srcPort);
}

inline unsigned short RoTcpPkt::urp() const
{
    return ntohs(pkt_->urp);
}

inline unsigned short RoTcpPkt::win() const
{
    return ntohs(pkt_->win);
}

END_NAMESPACE1

#endif
