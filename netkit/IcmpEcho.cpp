/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <string.h>

#include "netkit-pch.h"
#include "netkit/IcmpEcho.hpp"

BEGIN_NAMESPACE1(netkit)


IcmpEcho::IcmpEcho(bool isReply, const char* body):
RoIcmpPkt(0 /*raw*/, 0 /*rawLength*/, false /*makeCopy*/)
{
    size_t bodyLength = strlen(body) + 1;
    construct(isReply, reinterpret_cast<const unsigned char*>(body), bodyLength);
}


IcmpEcho::IcmpEcho(bool isReply, const unsigned char* body, size_t bodyLength):
RoIcmpPkt(0 /*raw*/, 0 /*rawLength*/, false /*makeCopy*/)
{
    construct(isReply, body, bodyLength);
}


IcmpEcho::IcmpEcho(const IcmpEcho& icmpEcho):
RoIcmpPkt(0 /*raw*/, 0 /*rawLength*/, false /*makeCopy*/)
{
    size_t rawLength;
    const unsigned char* raw = icmpEcho.raw(rawLength);
    msg_ = newBuf(rawLength);
    memcpy(msg_, raw, rawLength);

    reset(msg_, rawLength, false /*makeCopy*/);
}


IcmpEcho::~IcmpEcho()
{
    delete[] msg_;
}


const IcmpEcho& IcmpEcho::operator =(const IcmpEcho& icmpEcho)
{
    if (this != &icmpEcho)
    {

        // Reallocate buffer for different packet length.
        size_t rawLength;
        const unsigned char* raw = icmpEcho.raw(rawLength);
        if (RoIcmpPkt::rawLength() != rawLength)
        {
            delete[] msg_;
            msg_ = newBuf(rawLength);
            memcpy(msg_, raw, rawLength);
            reset(msg_, rawLength, false /*makeCopy*/);
        }
        else
        {
            memcpy(msg_, raw, rawLength);
        }
    }

    return *this;
}


void IcmpEcho::construct(bool isReply, const unsigned char* body, size_t bodyLength)
{
    size_t rawLength = sizeof(*hdr_) + bodyLength;
    msg_ = newBuf(rawLength);
    memset(hdr_, 0, sizeof(*hdr_));
    hdr_->type = isReply? static_cast<unsigned char>(EchoReply): static_cast<unsigned char>(Echo);
    memcpy(msg_ + sizeof(*hdr_), body, bodyLength);

    reset(msg_, rawLength, false /*makeCopy*/);
}


void IcmpEcho::setBody(const unsigned char* body, size_t bodyLength)
{
    // Reallocate buffer for different packet length.
    if (RoIcmpPkt::bodyLength() != bodyLength)
    {
        size_t rawLength = sizeof(*hdr_) + bodyLength;
        unsigned char* raw = newBuf(rawLength);
        memcpy(raw, msg_, sizeof(*hdr_));
        delete[] msg_;
        msg_ = raw;
        reset(raw, rawLength, false /*makeCopy*/);
    }

    // Update body part.
    memcpy(msg_ + sizeof(*hdr_), body, bodyLength);
}

END_NAMESPACE1
