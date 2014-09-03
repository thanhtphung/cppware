#include <string.h>

#include "netkit-pch.h"
#include "netkit/RoIpPkt.hpp"
#include "netkit/RoTcpPkt.hpp"

BEGIN_NAMESPACE1(netkit)

RoTcpPkt::hdr_t RoTcpPkt::badPkt_ =
{
    0xdeadU, 0xbeefU, 0xfeebdaedUL, 0xdeadbeefUL, 0xfeebU, 0xdaedU, 0xdeadU, 0xbeefU
};


RoTcpPkt::RoTcpPkt(const RoTcpPkt& tcpPkt)
{
    copyMade_ = false;
    rawLength_ = 0;
    raw_ = 0;
    reset(tcpPkt.raw_, tcpPkt.rawLength_, tcpPkt.copyMade_);
}


RoTcpPkt::RoTcpPkt(const RoIpPkt& ipPkt, bool makeCopy)
{
    copyMade_ = false;
    rawLength_ = 0;
    raw_ = 0;
    reset(ipPkt, makeCopy);
}


RoTcpPkt::RoTcpPkt(const unsigned char* raw, size_t rawLength, bool makeCopy)
{
    copyMade_ = false;
    rawLength_ = 0;
    raw_ = 0;
    reset(raw, rawLength, makeCopy);
}


RoTcpPkt::~RoTcpPkt()
{
    // Destroy private copy.
    if (copyMade_)
    {
        delete[] raw_;
    }
}


const RoTcpPkt& RoTcpPkt::operator =(const RoTcpPkt& tcpPkt)
{
    if (this != &tcpPkt)
    {
        reset(tcpPkt.raw_, tcpPkt.rawLength_, tcpPkt.copyMade_);
    }

    return *this;
}


//!
//! Return true if this packet contains a timestamps option.
//! Also retrieve the timestamps if the option exists.
//!
bool RoTcpPkt::findTsOpt(unsigned int& val, unsigned int& ecr) const
{
    enum
    {
        Eol = 0,
        NoOp = 1,
        Timestamps = 8
    };

    bool found = false;
    const unsigned char* p = raw_ + MinHdrLength;
    const unsigned char* pEnd = (raw_ + (ntohs(pkt_->flagsEtc) >> 10U));
    while (p < pEnd)
    {
        switch (p[0])
        {
        case Eol:
            p = pEnd; //terminate outer loop
            break;
        case NoOp:
            ++p;
            break;
        case Timestamps:
            if (((p + 9) < pEnd) && (p[1] == 10))
            {
                memcpy(&val, p + 2, 4);
                val = ntohl(val);
                memcpy(&ecr, p + 6, 4);
                ecr = ntohl(ecr);
                found = true;
                p = pEnd; //terminate outer loop
                break;
            }
            // no break
        default:
            (((p + 1) < pEnd) && (p[1] >= 2))? (p += p[1]): (p = pEnd);
            break;
        }
    }

    return found;
}


//!
//! Reset instance with given IP packet. Return true if successful. Return
//! false otherwise (given IP packet cannot be interpreted as a TCP packet).
//!
bool RoTcpPkt::reset(const RoIpPkt& ipPkt, bool makeCopy)
{
    const unsigned char* raw;
    size_t rawLength;
    if (ipPkt.proto() == RoIpPkt::Tcp)
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
bool RoTcpPkt::reset(const unsigned char* raw, size_t rawLength, bool makeCopy)
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

END_NAMESPACE1
