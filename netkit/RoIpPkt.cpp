#include <string.h>

#include "netkit-pch.h"
#include "netkit/RoEtherPkt.hpp"
#include "netkit/RoIpPkt.hpp"

BEGIN_NAMESPACE1(netkit)

RoIpPkt::hdr_t RoIpPkt::badPkt_ =
{
    0xdeU, 0xadU, 0xbeefU, 0xdeadU, 0xbeefU, 0xdeU, 0xadU, 0xbeefU, 0xdeadbeefUL, 0xdeadbeefUL
};


RoIpPkt::RoIpPkt(const RoEtherPkt& etherPkt, bool makeCopy)
{
    copyMade_ = false;
    rawLength_ = 0;
    raw_ = 0;
    reset(etherPkt, makeCopy);
}


RoIpPkt::RoIpPkt(const RoIpPkt& ipPkt)
{
    copyMade_ = false;
    rawLength_ = 0;
    raw_ = 0;
    reset(ipPkt.raw_, ipPkt.rawLength_, ipPkt.copyMade_);
}


RoIpPkt::RoIpPkt(const unsigned char* raw, size_t rawLength, bool makeCopy)
{
    copyMade_ = false;
    rawLength_ = 0;
    raw_ = 0;
    reset(raw, rawLength, makeCopy);
}


RoIpPkt::~RoIpPkt()
{
    // Destroy private copy.
    if (copyMade_)
    {
        delete[] raw_;
    }
}


//!
//! Reset instance with given ethernet packet. Return true if successful. Return
//! false otherwise (given ethernet packet cannot be interpreted as an IP packet).
//!
bool RoIpPkt::reset(const RoEtherPkt& etherPkt, bool makeCopy)
{
    const unsigned char* raw;
    size_t rawLength;
    unsigned int type = etherPkt.type();
    if ((type == RoEtherPkt::Ip) || ((type == RoEtherPkt::Vlan) && (etherPkt.vlanType() == RoEtherPkt::Ip)))
    {
        raw = etherPkt.body(rawLength);
    }
    else
    {
        raw = 0;
        rawLength = 0;
    }

    bool ok = reset(raw, rawLength, makeCopy);
    return ok;
}


//!
//! Reset instance with given raw packet. Return true if successful.
//! Return false otherwise (given raw packet is invalid).
//!
bool RoIpPkt::reset(const unsigned char* raw, size_t rawLength, bool makeCopy)
{
    // Destroy private copy.
    if (copyMade_)
    {
        delete[] raw_;
    }

    // Use dummy data when given data is invalid.
    if ((raw == 0) || (rawLength < MinHdrLength))
    {
        copyMade_ = false;
        rawLength_ = sizeof(badPkt_);
        pkt_ = &badPkt_;
        bool ok = false;
        return ok;
    }

    copyMade_ = makeCopy;
    rawLength_ = static_cast<unsigned int>(rawLength);
    if (makeCopy)
    {
        unsigned char* p = new unsigned char[rawLength_];
        memcpy(p, raw, rawLength_);
        raw_ = p;
    }
    else
    {
        raw_ = raw;
    }

    bool ok = true;
    return ok;
}


const unsigned char* RoIpPkt::body(size_t& bodyLength) const
{
    size_t hdrLen = (pkt_->vihl & IP_IHL_M) << 2U;
    size_t pktLen = ntohs(pkt_->length);
    bodyLength = (pktLen >= hdrLen)? (pktLen - hdrLen): (rawLength_ - hdrLen);
    const unsigned char* p = raw_ + hdrLen;
    return p;
}


size_t RoIpPkt::bodyLength() const
{
    size_t hdrLen = (pkt_->vihl & IP_IHL_M) << 2U;
    size_t pktLen = ntohs(pkt_->length);
    size_t bodyLen = (pktLen >= hdrLen)? (pktLen - hdrLen): (rawLength_ - hdrLen);
    return bodyLen;
}

END_NAMESPACE1
