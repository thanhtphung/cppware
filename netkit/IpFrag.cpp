/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "netkit-pch.h"
#include "netkit/IpCap.hpp"
#include "netkit/IpFrag.hpp"
#include "netkit/RoIpPkt.hpp"


BEGIN_NAMESPACE1(netkit)


//!
//! Make deep copy of given IPv4 partial packet (a first fragment with more bit
//! or a subsequent fragment). Link constructed instance to next. Limit deep copy
//! range (copy only bytes residing at position zero through maxByteOffset).
//!
IpFrag::IpFrag(const IpCap& frag, IpFrag* next, unsigned int maxByteOffset)
{
    next_ = next;

    const RoIpPkt& ipPkt = frag.ipPkt();
    byteOffset_ = ipPkt.fragment() * 8;
    rawLength_ = static_cast<unsigned int>(ipPkt.bodyLength());

    // Limit deep copy range.
    unsigned int deepBytes;
    if (byteOffset_ <= maxByteOffset)
    {
        unsigned int hiByteOffset = byteOffset_ + rawLength_;
        deepBytes = (hiByteOffset <= maxByteOffset)? rawLength_: maxByteOffset - byteOffset_ + 1;
    }
    else
    {
        deepBytes = 0;
    }

    // Make deep copy.
    unsigned char* p = new unsigned char[deepBytes];
    memcpy(p, ipPkt.body(), deepBytes);
    raw_ = p;
}


IpFrag::~IpFrag()
{
    delete[] raw_;
}

END_NAMESPACE1
