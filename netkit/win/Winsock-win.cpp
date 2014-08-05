/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include "appkit/String.hpp"
#include "syskit/sys.hpp"

#include "netkit-pch.h"
#include "netkit/WinSock.hpp"

#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32")

using namespace appkit;
using namespace syskit;

BEGIN_NAMESPACE1(netkit)


Winsock::Winsock()
{
    ok_ = false;

    WSAData wsaData;
    int err = WSAStartup(MAKEWORD(2, 0), &wsaData);
    if (!err)
    {
        if ((LOBYTE(wsaData.wVersion) == 2) && (HIBYTE(wsaData.wVersion) == 0))
        {
            ok_ = true;
        }
        else
        {
            WSACleanup();
        }
    }
}


Winsock::~Winsock()
{
    if (ok_)
    {
        WSACleanup();
    }
}


//!
//! Retrieve the IPv4 address of given hostname. Return true if successful. A hostname
//! is valid if it can be translated to an IPv4 address. An empty string results in a
//! registered address of the local computer, and "localhost" results in the loopback
//! address.
//!
bool Winsock::getAddrInfo(unsigned int& addr, const char* nodeName)
{
    String w(nodeName);
    bool ok = getAddrInfo(addr, w.widen());
    return ok;
}


//!
//! Retrieve the IPv4 address of given hostname. Return true if successful. A hostname
//! is valid if it can be translated to an IPv4 address. An empty string results in a
//! registered address of the local computer, and "localhost" results in the loopback
//! address.
//!
bool Winsock::getAddrInfo(unsigned int& addr, const char* nodeName, size_t length)
{
    String w(nodeName, length);
    bool ok = getAddrInfo(addr, w.widen());
    return ok;
}


bool Winsock::getAddrInfo(unsigned int& addr, const wchar_t* nodeName)
{
    addrinfoW hints = {0};
    hints.ai_family = AF_INET; //IPv4 only
    addrinfoW* result = 0;
    bool ok = (GetAddrInfoW(nodeName, 0 /*serviceName*/, &hints, &result) == 0);

    if (ok)
    {
        const sockaddr_in* saddr = 0;
        for (const addrinfoW* p = result; p; p = p->ai_next)
        {
            if ((p->ai_family == AF_INET) && (p->ai_addrlen == sizeof(*saddr)))
            {
                saddr = reinterpret_cast<const sockaddr_in*>(p->ai_addr);
                addr = ntohl(saddr->sin_addr.s_addr);
                break;
            }
        }
        ok = (saddr != 0);
        FreeAddrInfoW(result);
    }

    return ok;
}

END_NAMESPACE1
