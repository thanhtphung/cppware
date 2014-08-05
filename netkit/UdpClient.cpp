/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/sys.hpp"

#include "netkit-pch.h"
#include "netkit/Paddr.hpp"
#include "netkit/S32SocOpt.hpp"
#include "netkit/UdpClient.hpp"

using namespace syskit;

BEGIN_NAMESPACE1(netkit)

const unsigned int UdpClient::ETERNITY = 0xffffffffU;


//!
//! Construct a default UDP socket. The construction can fail due to lack of
//! system resources. Use isOk() to determine if the socket construction is
//! successful.
//!
UdpClient::UdpClient()
{
    construct();
}


//!
//! Construct a UDP socket. Adjust the socket buffers as specified by ibufSize
//! (receiving buffer size in bytes) and obufSize (sending buffer size in bytes).
//! Ignore failures, if any, while sizing the buffers. The construction can fail
//! due to lack of system resources. Use isOk() to determine if the socket
//! construction is successful.
//!
UdpClient::UdpClient(int ibufSize, int obufSize)
{
    construct();

    // Adjust socket buffers.
    // Don't care if fail.
    S32SocOpt opt(soc_);
    int ibuf;
    int obuf;
    if (opt.getV(S32SocOpt::N_RCVBUF, ibuf) && opt.getV(S32SocOpt::N_SNDBUF, obuf))
    {
        opt.setV(S32SocOpt::N_RCVBUF, ibufSize);
        opt.setV(S32SocOpt::N_SNDBUF, obufSize);
    }
}


//!
//! Construct a UDP socket with given binding. Also return the effective binding in
//! myAddr. The construction can fail due to lack of system resources. Use isOk()
//! to determine if the socket construction is successful.
//!
UdpClient::UdpClient(unsigned int addr, unsigned short port, Paddr& myAddr)
{
    construct();
    bool ok = bind(addr, port) && getMyAddr(myAddr);
    if (!ok)
    {
        myAddr.reset();
    }
}


//!
//! Bind socket to given port address. Return true if successful.
//!
bool UdpClient::bind(unsigned int addr, unsigned short port)
{
    struct sockaddr_in socAddr;
    socAddr.sin_family = AF_INET;
    socAddr.sin_addr.s_addr = htonl(addr);
    socAddr.sin_port = htons(port);
    return (soc_ != INVALID_SOCKET) && (::bind(soc_, (struct sockaddr*)(&socAddr), sizeof(socAddr)) == 0);
}


//!
//! Get socket binding. Return true if successful.
//!
bool UdpClient::getMyAddr(Paddr& paddr) const
{
    bool ok;
    struct sockaddr_in socAddr;
    socklen_t socAddrLen = sizeof(socAddr);
    if ((soc_ != INVALID_SOCKET) &&
        (getsockname(soc_, (struct sockaddr*)(&socAddr), &socAddrLen) == 0) &&
        (socAddrLen == sizeof(socAddr)) &&
        (socAddr.sin_family == AF_INET))
    {
        paddr.setAddr(ntohl(socAddr.sin_addr.s_addr));
        paddr.setPort(ntohs(socAddr.sin_port));
        ok = true;
    }
    else
    {
        ok = false;
    }

    return ok;
}


//!
//! Return true if instance was successfully constructed.
//!
bool UdpClient::isOk() const
{
    return soc_ != INVALID_SOCKET;
}

END_NAMESPACE1
