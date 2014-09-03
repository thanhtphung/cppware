/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <string.h>

#include "netkit-pch.h"
#include "netkit/IcmpInfo.hpp"

BEGIN_NAMESPACE1(netkit)


IcmpInfo::IcmpInfo(bool isReply):
RoIcmpPkt(msg_, HdrLength, false /*makeCopy*/)
{
    memset(msg_, 0, sizeof(msg_));
    hdr_.type = isReply? (unsigned char)(InfoReply): (unsigned char)(InfoReq);
}


IcmpInfo::IcmpInfo(const IcmpInfo& icmpInfo):
RoIcmpPkt(msg_, HdrLength, false /*makeCopy*/)
{
    memcpy(msg_, icmpInfo.msg_, sizeof(msg_));
}


IcmpInfo::~IcmpInfo()
{
}


const IcmpInfo& IcmpInfo::operator =(const IcmpInfo& icmpInfo)
{
    if (this != &icmpInfo)
    {
        memcpy(msg_, icmpInfo.msg_, sizeof(msg_));
    }

    return *this;
}

END_NAMESPACE1
