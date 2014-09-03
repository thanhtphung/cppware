#include <string.h>

#include "netkit-pch.h"
#include "netkit/RoEtherPkt.hpp"

BEGIN_NAMESPACE1(netkit)

RoEtherPkt::hdr_t RoEtherPkt::badPkt_ =
{
    {0xbaU, 0xdbU, 0xadU, 0xbaU, 0xdbU, 0xadU},
    {0xdeU, 0xadU, 0xdeU, 0xadU, 0xdeU, 0xadU},
    0xbeefU
};


RoEtherPkt::RoEtherPkt(const RoEtherPkt& etherPkt)
{
    copyMade_ = 0;
    raw_ = 0;
    // hdrLength_ initialized in reset()
    // rawLength_ initialized in reset()
    reset(etherPkt.raw_, etherPkt.rawLength_, etherPkt.copyMade_ != 0);
}


RoEtherPkt::RoEtherPkt(const unsigned char* raw, size_t rawLength, bool makeCopy)
{
    copyMade_ = 0;
    raw_ = 0;
    // hdrLength_ initialized in reset()
    // rawLength_ initialized in reset()
    reset(raw, rawLength, makeCopy);
}


RoEtherPkt::~RoEtherPkt()
{
    // Destroy private copy.
    if (copyMade_)
    {
        delete[] raw_;
    }
}


//!
//! Reset instance with given raw packet. Return true if successful.
//! Return false otherwise (given raw packet is invalid).
//!
bool RoEtherPkt::reset(const unsigned char* raw, size_t rawLength, bool makeCopy)
{
    // Destroy private copy.
    if (copyMade_)
    {
        delete[] raw_;
    }

    // Use dummy data when given data is invalid.
    raw_ = raw;
    unsigned int type;
    if ((raw == 0) || (rawLength < HdrLength) || (((type = RoEtherPkt::type()) == Vlan) && (rawLength < VlanHdrLength)))
    {
        copyMade_ = 0;
        hdrLength_ = sizeof(badPkt_);
        rawLength_ = sizeof(badPkt_);
        pkt_ = &badPkt_;
        bool ok = false;
        return ok;
    }

    copyMade_ = makeCopy? 1: 0;
    hdrLength_ = static_cast<unsigned char>((type == Vlan)? VlanHdrLength: HdrLength);
    rawLength_ = static_cast<unsigned int>(rawLength);
    if (makeCopy)
    {
        unsigned char* p = new unsigned char[rawLength_];
        memcpy(p, raw, rawLength_);
        raw_ = p;
    }

    bool ok = true;
    return ok;
}

END_NAMESPACE1
