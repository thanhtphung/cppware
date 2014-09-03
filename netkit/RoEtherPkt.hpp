/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_RO_ETHER_PKT_HPP
#define NETKIT_RO_ETHER_PKT_HPP

#include <new>
#include "netkit/net.hpp"

BEGIN_NAMESPACE1(netkit)


//! read-only ethernet packet
class RoEtherPkt
    //!
    //! A class representing a read-only ethernet packet.
    //!
{

public:
    enum
    {
        AddrLength = 6
    };

    enum type_e
    {
        Arp = 0x0806U,
        Ip = 0x0800U,
        Vlan = 0x8100U //802.1Q virtual lan
    };

#if _WIN32
#pragma pack(push,1)
#endif

    typedef struct hdr_s
    {
        unsigned char dst[AddrLength];
        unsigned char src[AddrLength];
        unsigned short /*type_e*/ type;
    } hdr_t;

    typedef struct vlanHdr_s
    {
        unsigned char etherDst[AddrLength];
        unsigned char etherSrc[AddrLength];
        unsigned short /*type_e*/ etherType;
        unsigned short id;
        unsigned short type;
    } vlanHdr_t;

#if _WIN32
#pragma pack(pop)
#endif

    enum
    {
        HdrLength = sizeof(hdr_t),
        VlanHdrLength = sizeof(vlanHdr_t)
    };

    RoEtherPkt(const RoEtherPkt& etherPkt);
    RoEtherPkt(const unsigned char* raw, size_t rawLength, bool makeCopy = false);
    ~RoEtherPkt();
    const RoEtherPkt& operator =(const RoEtherPkt& etherPkt);
    static void operator delete(void* p, size_t size);
    static void operator delete(void* p, void* buf);
    static void* operator new(size_t size);
    static void* operator new(size_t size, void* buf);

    // Raw contents.
    bool isOk() const;
    bool reset(const unsigned char* raw, size_t rawLength, bool makeCopy = false);
    const unsigned char* raw() const;
    const unsigned char* raw(size_t& rawLength) const;
    size_t rawLength() const;

    // Header.
    const hdr_t* rawHdr() const;
    const unsigned char* dst() const;
    const unsigned char* src() const;
    const vlanHdr_t* rawVlanHdr() const;
    size_t hdrLength() const;
    unsigned short vlanId() const;
    unsigned short /*type_e*/ type() const;
    unsigned short /*type_e*/ vlanType() const;
    void setRawHdr(const hdr_t* hdr);
    void setRawVlanHdr(const vlanHdr_t* vlanHdr);

    // Body.
    const unsigned char* body() const;
    const unsigned char* body(size_t& bodyLength) const;
    size_t bodyLength() const;

private:
    unsigned char copyMade_;
    unsigned char hdrLength_;
    unsigned int rawLength_;
    union
    {
        const hdr_t* pkt_;
        const unsigned char* raw_;
        const vlanHdr_t* vlanPkt_;
        hdr_t* hdr_;
        vlanHdr_t* vlanHdr_;
    };

    static hdr_t badPkt_;

};

END_NAMESPACE1

#include "syskit/BufPool.hpp"

BEGIN_NAMESPACE1(netkit)

inline const RoEtherPkt& RoEtherPkt::operator =(const RoEtherPkt& etherPkt)
{
    if (this != &etherPkt) reset(etherPkt.raw_, etherPkt.rawLength_, etherPkt.copyMade_ != 0);
    return *this;
}

inline void RoEtherPkt::operator delete(void* p, size_t size)
{
    syskit::BufPool::freeBuf(p, size);
}

inline void RoEtherPkt::operator delete(void* /*p*/, void* /*buf*/)
{
}

inline void* RoEtherPkt::operator new(size_t size)
{
    void* buf = syskit::BufPool::allocateBuf(size);
    return buf;
}

inline void* RoEtherPkt::operator new(size_t /*size*/, void* buf)
{
    return buf;
}

//! Return true if instance was constructed successfully.
inline bool RoEtherPkt::isOk() const
{
    return (pkt_ != &badPkt_);
}

inline const RoEtherPkt::hdr_t* RoEtherPkt::rawHdr() const
{
    return pkt_;
}

inline const RoEtherPkt::vlanHdr_t* RoEtherPkt::rawVlanHdr() const
{
    return vlanPkt_;
}

//! Return the body part. Use bodyLength() to obtain its length in bytes.
inline const unsigned char* RoEtherPkt::body() const
{
    return (raw_ + hdrLength_);
}

//! Return the body part. Also return its length in bytes in <i>bodyLength</i>.
inline const unsigned char* RoEtherPkt::body(size_t& bodyLength) const
{
    bodyLength = rawLength_ - hdrLength_;
    return (raw_ + hdrLength_);
}

//! Return the destination address in this packet.
inline const unsigned char* RoEtherPkt::dst() const
{
    return pkt_->dst;
}

//! Return the raw packet. Use rawLength() to obtain its length in bytes.
inline const unsigned char* RoEtherPkt::raw() const
{
    return raw_;
}

//! Return the raw packet. Also return its length in bytes in <i>rawLength</i>.
inline const unsigned char* RoEtherPkt::raw(size_t& rawLength) const
{
    rawLength = rawLength_;
    return raw_;
}

//! Return the source address in this packet.
inline const unsigned char* RoEtherPkt::src() const
{
    return pkt_->src;
}

//! Return the length (in bytes) of the body part.
inline size_t RoEtherPkt::bodyLength() const
{
    return (rawLength_ - hdrLength_);
}

//! Return the header length in bytes.
inline size_t RoEtherPkt::hdrLength() const
{
    return hdrLength_;
}

//! Return the length (in bytes) of the raw packet.
inline size_t RoEtherPkt::rawLength() const
{
    return rawLength_;
}

inline unsigned short RoEtherPkt::vlanId() const
{
    unsigned short id = (hdrLength_ == VlanHdrLength)? ntohs(vlanPkt_->id): 0;
    return id;
}

inline unsigned short /*type_e*/ RoEtherPkt::type() const
{
    return ntohs(pkt_->type);
}

inline unsigned short /*type_e*/ RoEtherPkt::vlanType() const
{
    return ntohs(vlanPkt_->type);
}

inline void RoEtherPkt::setRawHdr(const hdr_t* hdr)
{
    memcpy(hdr_, hdr, sizeof(*hdr_));
}

inline void RoEtherPkt::setRawVlanHdr(const vlanHdr_t* vlanHdr)
{
    memcpy(vlanHdr_, vlanHdr, sizeof(*vlanHdr_));
}

END_NAMESPACE1

#endif
