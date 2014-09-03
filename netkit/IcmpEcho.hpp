/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_ICMP_ECHO_HPP
#define NETKIT_ICMP_ECHO_HPP

#include "netkit/RoIcmpPkt.hpp"
#include "netkit/net.hpp"

BEGIN_NAMESPACE1(netkit)


//! icmp echo/echo reply
class IcmpEcho: private RoIcmpPkt
    //!
    //! A class representing an ICMP Echo or Echo Reply message.
    //! RFC: 792.
    //!
{

public:
    IcmpEcho(bool isReply, const char* body);
    IcmpEcho(bool isReply = false, const unsigned char* body = 0, size_t bodyLength = 0);
    IcmpEcho(const IcmpEcho& icmpEcho);
    ~IcmpEcho();
    const IcmpEcho& operator =(const IcmpEcho& icmpEcho);

    // From RoIcmpPkt.
    using RoIcmpPkt::body;
    using RoIcmpPkt::bodyLength;
    using RoIcmpPkt::checksum;
    using RoIcmpPkt::code;
    using RoIcmpPkt::id;
    using RoIcmpPkt::raw;
    using RoIcmpPkt::rawLength;
    using RoIcmpPkt::seq;
    using RoIcmpPkt::type;

    const char* bodyAsString() const;
    void setBody(const char* body);
    void setBody(const unsigned char* body, size_t bodyLength);
    void setChecksum();
    void setCode(unsigned char code);
    void setId(unsigned short id);
    void setSeq(unsigned short seq);
    void setType(bool isReply);

private:
    union
    {
        const unsigned short* u16_;
        hdr_t* hdr_;
        unsigned char* msg_;
    };

    void construct(bool, const unsigned char*, size_t);

};

END_NAMESPACE1

#include "appkit/U16.hpp"

BEGIN_NAMESPACE1(netkit)

inline const char* IcmpEcho::bodyAsString() const
{
    const char* s = reinterpret_cast<const char*>(body());
    return s;
}

inline void IcmpEcho::setBody(const char* body)
{
    setBody(reinterpret_cast<const unsigned char*>(body), strlen(body) + 1);
}

//! Compute the checksum of the packet,
//! then update the checksum part w/ computed value.
inline void IcmpEcho::setChecksum()
{
    hdr_->checksum = 0;
    unsigned int numItems = ((unsigned int)(rawLength()) + 1) >> 1;
    unsigned short checksum = appkit::U16::checksum(u16_, numItems);
    hdr_->checksum = checksum;
}

inline void IcmpEcho::setCode(unsigned char code)
{
    hdr_->code = code;
}

//! Update the identifier in this packet.
inline void IcmpEcho::setId(unsigned short id)
{
    hdr_->u16[0] = htons(id);
}

//! Update the sequence number in this packet.
inline void IcmpEcho::setSeq(unsigned short seq)
{
    hdr_->u16[1] = htons(seq);
}

inline void IcmpEcho::setType(bool isReply)
{
    hdr_->type = isReply? (unsigned char)(EchoReply): (unsigned char)(Echo);
}

END_NAMESPACE1

#endif
