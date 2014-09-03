/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_IP_CAP_HPP
#define NETKIT_IP_CAP_HPP

#include "syskit/macros.h"

BEGIN_NAMESPACE1(netkit)

class RoEtherPkt;
class RoIpPkt;


//! IPv4 packet capture
class IpCap
    //!
    //! A class representing a capture of an IPv4 packet.
    //!
{

public:
    IpCap(const RoEtherPkt& etherPkt, const RoIpPkt& ipPkt, unsigned int ifIndex, unsigned long long capTime);
    IpCap(unsigned char* etherRaw, size_t etherRawLength, unsigned int ifIndex, unsigned long long capTime);
    ~IpCap();
    static void operator delete(void* p, size_t size);
    static void operator delete(void* p, void* buf);
    static void* operator new(size_t size);
    static void* operator new(size_t size, void* buf);

    const RoEtherPkt& etherPkt() const;
    const RoIpPkt& ipPkt() const;
    unsigned int ifIndex() const;
    unsigned long long capTime() const;

private:
    const RoEtherPkt* etherPkt_;
    const RoIpPkt* ipPkt_;
    unsigned char* etherRaw_;
    unsigned int ifIndex_;
    unsigned long long capTime_;

    IpCap(const IpCap&); //prohibit usage
    const IpCap& operator =(const IpCap&); //prohibit usage

};

END_NAMESPACE1

#include "syskit/BufPool.hpp"

BEGIN_NAMESPACE1(netkit)

inline void IpCap::operator delete(void* p, size_t size)
{
    syskit::BufPool::freeBuf(p, size);
}

inline void IpCap::operator delete(void* /*p*/, void* /*buf*/)
{
}

inline void* IpCap::operator new(size_t size)
{
    void* buf = syskit::BufPool::allocateBuf(size);
    return buf;
}

inline void* IpCap::operator new(size_t /*size*/, void* buf)
{
    return buf;
}

inline const RoEtherPkt& IpCap::etherPkt() const
{
    return *etherPkt_;
}

inline const RoIpPkt& IpCap::ipPkt() const
{
    return *ipPkt_;
}

inline unsigned int IpCap::ifIndex() const
{
    return ifIndex_;
}

inline unsigned long long IpCap::capTime() const
{
    return capTime_;
}

END_NAMESPACE1

#endif
