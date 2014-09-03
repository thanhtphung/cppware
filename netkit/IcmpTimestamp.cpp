/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <string.h>

#include "netkit-pch.h"
#include "netkit/IcmpTimestamp.hpp"

BEGIN_NAMESPACE1(netkit)


IcmpTimestamp::IcmpTimestamp(bool isReply):
RoIcmpPkt(msg_, HdrLength + BodyLength, false /*makeCopy*/)
{
    memset(msg_, 0, sizeof(msg_));
    hdr_.type = isReply? (unsigned char)(TimestampReply): (unsigned char)(Timestamp);
}


IcmpTimestamp::IcmpTimestamp(const IcmpTimestamp& icmpTimestamp):
RoIcmpPkt(msg_, HdrLength + BodyLength, false /*makeCopy*/)
{
    memcpy(msg_, icmpTimestamp.msg_, sizeof(msg_));
}


IcmpTimestamp::~IcmpTimestamp()
{
}


const IcmpTimestamp& IcmpTimestamp::operator =(const IcmpTimestamp& icmpTimestamp)
{
    if (this != &icmpTimestamp)
    {
        memcpy(msg_, icmpTimestamp.msg_, sizeof(msg_));
    }

    return *this;
}

END_NAMESPACE1
