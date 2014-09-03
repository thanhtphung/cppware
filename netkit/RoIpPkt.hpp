/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_RO_IP_PKT_HPP
#define NETKIT_RO_IP_PKT_HPP

#include <new>
#include "netkit/net.hpp"

BEGIN_NAMESPACE1(netkit)

class RoEtherPkt;


//! read-only IPv4 packet
class RoIpPkt
    //!
    //! A class representing a read-only IP packet.
    //!
{

public:
#if _WIN32
#pragma pack(push,1)
#endif
    typedef struct hdr_s //RFC: 791
    {
        unsigned char vihl;
        unsigned char tos;
        unsigned short length;
        unsigned short id;
        unsigned short flagsFrag;
        unsigned char ttl;
        unsigned char proto;
        unsigned short headsum;
        unsigned int src;
        unsigned int dst;
    } hdr_t;
#if _WIN32
#pragma pack(pop)
#endif

    enum
    {
        MinHdrLength = sizeof(hdr_t)
    };

    enum proto_e
    {
        Icmp = 0x01U,
        Tcp = 0x06U,
        Udp = 0x11U
    };

    RoIpPkt(const RoEtherPkt& etherPkt, bool makeCopy = false);
    RoIpPkt(const RoIpPkt& ipPkt);
    RoIpPkt(const unsigned char* raw, size_t rawLength, bool makeCopy = false);
    ~RoIpPkt();
    const RoIpPkt& operator =(const RoIpPkt& ipPkt);
    static void operator delete(void* p, size_t size);
    static void operator delete(void* p, void* buf);
    static void* operator new(size_t size);
    static void* operator new(size_t size, void* buf);

    // Raw contents.
    bool isOk() const;
    bool reset(const RoEtherPkt& etherPkt, bool makeCopy = false);
    bool reset(const unsigned char* raw, size_t rawLength, bool makeCopy = false);
    const unsigned char* raw() const;
    const unsigned char* raw(size_t& length) const;
    size_t rawLength() const;

    // Header.
    bool dfFlag() const;
    bool isPartial() const;
    bool mfFlag() const;
    bool reservedFlag() const;
    const hdr_t* rawHdr() const;
    unsigned char flags() const;
    unsigned char ihl() const;
    unsigned char tos() const;
    unsigned char ttl() const;
    unsigned char version() const;
    unsigned char /*proto_e*/ proto() const;
    unsigned int dst() const;
    unsigned int src() const;
    unsigned short fragment() const;
    unsigned short headsum() const;
    unsigned short id() const;
    unsigned short length() const;
    size_t hdrLength() const;
    void setRawHdr(const hdr_t* hdr);

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
        hdr_t* hdr_;
    };

    static hdr_t badPkt_;

};

END_NAMESPACE1

#include "syskit/BufPool.hpp"

BEGIN_NAMESPACE1(netkit)

const unsigned char IP_IHL_M = 0x0fU;
const unsigned char IP_VERSION_M = 0xf0U;
const unsigned short IP_DF_M = 0x4000U;
const unsigned short IP_FLAGS_M = 0xe000U;
const unsigned short IP_FRAG_M = 0x1fffU;
const unsigned short IP_MF_M = 0x2000U;
const unsigned short IP_RESERVED_M = 0x8000U;

inline const RoIpPkt& RoIpPkt::operator =(const RoIpPkt& ipPkt)
{
    if (this != &ipPkt) reset(ipPkt.raw_, ipPkt.rawLength_, ipPkt.copyMade_);
    return *this;
}

inline void RoIpPkt::operator delete(void* p, size_t size)
{
    syskit::BufPool::freeBuf(p, size);
}

inline void RoIpPkt::operator delete(void* /*p*/, void* /*buf*/)
{
}

inline void* RoIpPkt::operator new(size_t size)
{
    void* buf = syskit::BufPool::allocateBuf(size);
    return buf;
}

inline void* RoIpPkt::operator new(size_t /*size*/, void* buf)
{
    return buf;
}

//! Return true if the dontFragment flag is set.
inline bool RoIpPkt::dfFlag() const
{
    return ((ntohs(pkt_->flagsFrag) & IP_DF_M) != 0);
}

//! Return true if instance was constructed successfully.
inline bool RoIpPkt::isOk() const
{
    return (pkt_ != &badPkt_);
}

//! Return true if packet is incomplete
//! (is a first fragment with more bit or is a subsequent fragment).
inline bool RoIpPkt::isPartial() const
{
    return ((ntohs(pkt_->flagsFrag) & (IP_MF_M | IP_FRAG_M)) != 0);
}

//! Return true if the moreFragments flag is set.
inline bool RoIpPkt::mfFlag() const
{
    return ((ntohs(pkt_->flagsFrag) & IP_MF_M) != 0);
}

inline bool RoIpPkt::reservedFlag() const
{
    return ((ntohs(pkt_->flagsFrag) & IP_RESERVED_M) != 0);
}

inline const RoIpPkt::hdr_t* RoIpPkt::rawHdr() const
{
    return pkt_;
}

inline const unsigned char* RoIpPkt::body() const
{
    size_t hdrLen = ((pkt_->vihl & IP_IHL_M) << 2U);
    const unsigned char* p = raw_ + hdrLen;
    return p;
}

//! Return the raw packet. Use rawLength() to obtain its length in bytes.
inline const unsigned char* RoIpPkt::raw() const
{
    return raw_;
}

//! Return the raw packet. Also return its length in bytes in <i>rawLength</i>.
inline const unsigned char* RoIpPkt::raw(size_t& rawLength) const
{
    rawLength = rawLength_;
    return raw_;
}

//! Return the header length (in bytes).
inline size_t RoIpPkt::hdrLength() const
{
    size_t hdrLen = ((pkt_->vihl & IP_IHL_M) << 2U);
    return hdrLen;
}

//! Return the length (in bytes) of the raw packet.
inline size_t RoIpPkt::rawLength() const
{
    return rawLength_;
}

//! Return the composite flags field.
inline unsigned char RoIpPkt::flags() const
{
    unsigned short flagsFrag = ntohs(pkt_->flagsFrag);
    return (unsigned char)(flagsFrag >> 13U);
}

//! Return the internet header length in 32-bit words.
//! Use hdrLength() for header length in bytes.
inline unsigned char RoIpPkt::ihl() const
{
    return pkt_->vihl & IP_IHL_M;
}

inline unsigned char RoIpPkt::tos() const
{
    return pkt_->tos;
}

inline unsigned char RoIpPkt::ttl() const
{
    return pkt_->ttl;
}

inline unsigned char RoIpPkt::version() const
{
    return (pkt_->vihl >> 4U);
}

inline unsigned char /*proto_e*/ RoIpPkt::proto() const
{
    return pkt_->proto;
}

inline unsigned int RoIpPkt::dst() const
{
    return ntohl(pkt_->dst);
}

inline unsigned int RoIpPkt::src() const
{
    return ntohl(pkt_->src);
}

inline unsigned short RoIpPkt::headsum() const
{
    return ntohs(pkt_->headsum);
}

inline unsigned short RoIpPkt::fragment() const
{
    unsigned short flagsFrag = ntohs(pkt_->flagsFrag);
    return (flagsFrag & IP_FRAG_M);
}

inline unsigned short RoIpPkt::id() const
{
    return ntohs(pkt_->id);
}

inline unsigned short RoIpPkt::length() const
{
    return ntohs(pkt_->length);
}

inline void RoIpPkt::setRawHdr(const hdr_t* hdr)
{
    memcpy(hdr_, hdr, sizeof(*hdr_));
}

END_NAMESPACE1

#endif
