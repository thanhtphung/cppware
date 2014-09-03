#include <string.h>

#include "netkit-pch.h"
#include "netkit/RoIpPkt.hpp"
#include "netkit/RoUdpPkt.hpp"

BEGIN_NAMESPACE1(netkit)

RoUdpPkt::hdr_t RoUdpPkt::badPkt_ =
{
    0xdeadU, 0xbeefU, 0xfeebU, 0xdaedU
};


RoUdpPkt::RoUdpPkt(const RoIpPkt& ipPkt, bool makeCopy)
{
    copyMade_ = false;
    rawLength_ = 0;
    raw_ = 0;
    reset(ipPkt, makeCopy);
}


RoUdpPkt::RoUdpPkt(const RoUdpPkt& udpPkt)
{
    copyMade_ = false;
    rawLength_ = 0;
    raw_ = 0;
    reset(udpPkt.raw_, udpPkt.rawLength_, udpPkt.copyMade_);
}


RoUdpPkt::RoUdpPkt(const unsigned char* raw, size_t rawLength, bool makeCopy)
{
    copyMade_ = false;
    rawLength_ = 0;
    raw_ = 0;
    reset(raw, rawLength, makeCopy);
}


RoUdpPkt::~RoUdpPkt()
{
    // Destroy private copy.
    if (copyMade_)
    {
        delete[] raw_;
    }
}


const RoUdpPkt& RoUdpPkt::operator =(const RoUdpPkt& udpPkt)
{
    if (this != &udpPkt)
    {
        reset(udpPkt.raw_, udpPkt.rawLength_, udpPkt.copyMade_);
    }

    return *this;
}


//!
//! Reset instance with given IP packet. Return true if successful. Return
//! false otherwise (given IP packet cannot be interpreted as a UDP packet).
//!
bool RoUdpPkt::reset(const RoIpPkt& ipPkt, bool makeCopy)
{
    const unsigned char* raw;
    size_t rawLength;
    if (ipPkt.proto() == RoIpPkt::Udp)
    {
        raw = ipPkt.body(rawLength);
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
bool RoUdpPkt::reset(const unsigned char* raw, size_t rawLength, bool makeCopy)
{
    // Destroy private copy.
    if (copyMade_)
    {
        delete[] raw_;
    }

    // Use dummy data when given data is invalid.
    if ((raw == 0) || (rawLength < HdrLength))
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

END_NAMESPACE1
