/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_UDP_CLIENT_WIN_HPP
#define NETKIT_UDP_CLIENT_WIN_HPP

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifdef htonl
#undef htonl
#endif
#ifdef htonll
#undef htonll
#endif
#ifdef htons
#undef htons
#endif
#ifdef ntohl
#undef ntohl
#endif
#ifdef ntohll
#undef ntohll
#endif
#ifdef ntohs
#undef ntohs
#endif
#include <winsock2.h>
#include "syskit/macros.h"

BEGIN_NAMESPACE1(netkit)

class Paddr;


//! udp socket
class UdpClient
{

public:
    static const unsigned int ETERNITY;

    UdpClient();
    UdpClient(int ibufSize, int obufSize);
    UdpClient(unsigned int addr, unsigned short port, Paddr& myAddr);

    operator SOCKET() const;
    static void operator delete(void* p, size_t size);
    static void operator delete(void* p, void* buf);
    static void* operator new(size_t size);
    static void* operator new(size_t size, void* buf);

    SOCKET soc() const;
    bool bind(unsigned int addr, unsigned short port = 0U);
    bool getMyAddr(Paddr& addr) const;

    virtual ~UdpClient();
    virtual bool connect(const Paddr& paddr);
    virtual bool isOk() const;
    virtual int rcv(unsigned char* msg, size_t maxLength, Paddr& src, unsigned int timeoutInMsecs = ETERNITY);
    virtual int rcv(unsigned char* msg, size_t maxLength, unsigned int timeoutInMsecs = ETERNITY);
    virtual bool snd(const unsigned char* msg, size_t length, const Paddr& dst, unsigned int timeoutInMsecs = 0U);

    static UdpClient* createNull();

protected:
    UdpClient(SOCKET soc, WSAEVENT ev);

private:
    SOCKET soc_;
    WSAEVENT ev_;

    UdpClient(const UdpClient&); //prohibit usage
    UdpClient& operator=(const UdpClient&); //prohibit usage

    void construct();

};

END_NAMESPACE1

#include "syskit/BufPool.hpp"

BEGIN_NAMESPACE1(netkit)

inline UdpClient::operator SOCKET() const
{
    return soc_;
}

inline void UdpClient::operator delete(void* p, size_t size)
{
    syskit::BufPool::freeBuf(p, size);
}

inline void UdpClient::operator delete(void* /*p*/, void* /*buf*/)
{
}

inline void* UdpClient::operator new(size_t size)
{
    void* buf = syskit::BufPool::allocateBuf(size);
    return buf;
}

inline void* UdpClient::operator new(size_t /*size*/, void* buf)
{
    return buf;
}

inline SOCKET UdpClient::soc() const
{
    return soc_;
}

END_NAMESPACE1

#endif
