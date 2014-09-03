#include <string.h>

#include "netkit-pch.h"
#include "netkit/RoIcmpPkt.hpp"
#include "netkit/RoIpPkt.hpp"

const size_t MIN_LENGTH[netkit::RoIcmpPkt::HiType + 1] =
{
    netkit::RoIcmpPkt::HdrLength,                                   //Echo
    netkit::RoIcmpPkt::HdrLength,
    netkit::RoIcmpPkt::HdrLength,
    netkit::RoIcmpPkt::HdrLength + netkit::RoIpPkt::MinHdrLength, //DstUnreachable
    netkit::RoIcmpPkt::HdrLength + netkit::RoIpPkt::MinHdrLength, //SrcQuench
    netkit::RoIcmpPkt::HdrLength + netkit::RoIpPkt::MinHdrLength, //Redirect
    netkit::RoIcmpPkt::HdrLength,
    netkit::RoIcmpPkt::HdrLength,
    netkit::RoIcmpPkt::HdrLength,                                  //EchoReply
    netkit::RoIcmpPkt::HdrLength,
    netkit::RoIcmpPkt::HdrLength,
    netkit::RoIcmpPkt::HdrLength + netkit::RoIpPkt::MinHdrLength, //TimeExceed
    netkit::RoIcmpPkt::HdrLength + netkit::RoIpPkt::MinHdrLength, //ParmProb
    netkit::RoIcmpPkt::HdrLength + 12,                             //Timestamp
    netkit::RoIcmpPkt::HdrLength + 12,                             //TimestampReply
    netkit::RoIcmpPkt::HdrLength,                                  //InfoReq
    netkit::RoIcmpPkt::HdrLength,                                  //InfoReply
    netkit::RoIcmpPkt::HdrLength + 4,                              //MaskReq
    netkit::RoIcmpPkt::HdrLength + 4                               //MaskReply
};

BEGIN_NAMESPACE1(netkit)

RoIcmpPkt::hdr_t RoIcmpPkt::badPkt_ =
{
    0xdeU, 0xadU, 0xbeefU, {{0xfeU, 0xebU, 0xdaU, 0xedU}}
};


RoIcmpPkt::RoIcmpPkt(const RoIcmpPkt& icmpPkt)
{
    copyMade_ = false;
    rawLength_ = 0;
    raw_ = 0;
    reset(icmpPkt.raw_, icmpPkt.rawLength_, icmpPkt.copyMade_);
}


RoIcmpPkt::RoIcmpPkt(const RoIpPkt& ipPkt, bool makeCopy)
{
    copyMade_ = false;
    rawLength_ = 0;
    raw_ = 0;
    reset(ipPkt, makeCopy);
}


RoIcmpPkt::RoIcmpPkt(const unsigned char* raw, size_t rawLength, bool makeCopy)
{
    copyMade_ = false;
    rawLength_ = 0;
    raw_ = 0;
    reset(raw, rawLength, makeCopy);
}


RoIcmpPkt::~RoIcmpPkt()
{
    // Destroy private copy.
    if (copyMade_)
    {
        delete[] raw_;
    }
}


const RoIcmpPkt& RoIcmpPkt::operator =(const RoIcmpPkt& icmpPkt)
{
    if (this != &icmpPkt)
    {
        reset(icmpPkt.raw_, icmpPkt.rawLength_, icmpPkt.copyMade_);
    }

    return *this;
}


//!
//! Reset instance with given raw packet. Return true if successful.
//! Return false otherwise (given raw packet is invalid).
//!
bool RoIcmpPkt::reset(const unsigned char* raw, size_t rawLength, bool makeCopy)
{
    // Destroy private copy.
    if (copyMade_)
    {
        delete[] raw_;
    }

    // Use dummy data when given data is invalid.
    if ((raw == 0) || (rawLength < HdrLength) || ((raw[0] <= HiType) && (rawLength < MIN_LENGTH[raw[0]])))
    {
        copyMade_ = false;
        rawLength_ = sizeof(badPkt_);
        pkt_ = &badPkt_;
        bool ok = false;
        return ok;
    }

    copyMade_ = makeCopy;
    rawLength_ = rawLength;
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


//!
//! Allocate a buffer of given size in bytes. If given size is odd,
//! allocate an extra trailing byte and initialize the extra trailing
//! byte with zero.
//!
unsigned char* RoIcmpPkt::newBuf(size_t bufSize)
{
    unsigned char* buf;
    if (bufSize & 1)
    {
        buf = new unsigned char[bufSize + 1];
        buf[bufSize] = 0;
    }
    else
    {
        buf = new unsigned char[bufSize];
    }

    return buf;
}


void RoIcmpPkt::reset(const RoIpPkt& ipPkt, bool makeCopy)
{
    const unsigned char* raw;
    size_t rawLength;
    if (ipPkt.proto() == RoIpPkt::Icmp)
    {
        raw = ipPkt.body(rawLength);
    }
    else
    {
        raw = 0;
        rawLength = 0;
    }

    reset(raw, rawLength, makeCopy);
}

END_NAMESPACE1
