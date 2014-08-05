/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/Thread.hpp"
#include "syskit/sys.hpp"

#include "netkit-pch.h"
#include "netkit/Paddr.hpp"
#include "netkit/UdpClient.hpp"

#pragma comment(lib,"ws2_32")

using namespace netkit;
using namespace syskit;

BEGIN_NAMESPACE

class NoOpClient: public UdpClient
{
public:
    NoOpClient();
    virtual ~NoOpClient();
    virtual bool connect(const Paddr& paddr);
    virtual bool isOk() const;
    virtual bool snd(const unsigned char* msg, size_t length, const Paddr& dst, unsigned int timeoutInMsecs = 0U);
    virtual int rcv(unsigned char* msg, size_t maxLength, Paddr& src, unsigned int timeoutInMsecs = ETERNITY);
    virtual int rcv(unsigned char* msg, size_t maxLength, unsigned int timeoutInMsecs = ETERNITY);
private:
    NoOpClient(const NoOpClient&); //prohibit usage
    const NoOpClient& operator =(const NoOpClient&); //prohibit usage
};

NoOpClient::NoOpClient():
UdpClient(INVALID_SOCKET, WSA_INVALID_EVENT)
{
}

NoOpClient::~NoOpClient()
{
}

bool NoOpClient::connect(const Paddr& /*paddr*/)
{
    bool ok = true;
    return ok;
}

bool NoOpClient::isOk() const
{
    bool ok = true;
    return ok;
}

bool NoOpClient::snd(const unsigned char* /*msg*/, size_t /*length*/, const Paddr& /*dst*/, unsigned int /*timeoutInMsecs*/)
{
    bool ok = true;
    return ok;
}

int NoOpClient::rcv(unsigned char* /*msg*/, size_t /*maxLength*/, Paddr& /*src*/, unsigned int /*timeoutInMsecs*/)
{
    int bytesReceived = -1;
    return bytesReceived;
}

int NoOpClient::rcv(unsigned char* /*msg*/, size_t /*maxLength*/, unsigned int /*timeoutInMsecs*/)
{
    int bytesReceived = -1;
    return bytesReceived;
}

END_NAMESPACE

BEGIN_NAMESPACE1(netkit)


//!
//! Construct a UDP socket using given system resources. Take over ownership of
//! those resources and release them at destruction.
//!
UdpClient::UdpClient(SOCKET soc, WSAEVENT ev)
{
    soc_ = soc;
    ev_ = ev;
}


UdpClient::~UdpClient()
{
    if (ev_ != WSA_INVALID_EVENT)
    {
        WSACloseEvent(ev_);
    }
    if (soc_ != INVALID_SOCKET)
    {
        closesocket(soc_);
    }
}


//!
//! Create a minimal UDP client. Its use results in mostly no-ops. Returned client must
//! be deleted using the delete operator when done.
//!
UdpClient* UdpClient::createNull()
{
    UdpClient* client = new NoOpClient;
    return client;
}


//!
//! Connect to given port address (i.e., restrict incoming data sources and
//! allow incoming data from given port address only). Return true if successful.
//!
bool UdpClient::connect(const Paddr& paddr)
{
    struct sockaddr_in saddr = {0};
    saddr.sin_addr.s_addr = htonl(paddr.addr());
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(paddr.port());

    bool connected = (WSAConnect(soc_, reinterpret_cast<struct sockaddr*>(&saddr), sizeof(saddr), 0, 0, 0, 0) == 0);
    return connected;
}


bool UdpClient::snd(const unsigned char* msg, size_t length, const Paddr& dst, unsigned int timeoutInMsecs)
{
    struct sockaddr_in socAddr;
    socAddr.sin_family = AF_INET;
    socAddr.sin_addr.s_addr = htonl(dst.addr());
    socAddr.sin_port = htons(dst.port());

    // Try send.
    WSABUF wsaBuf;
    wsaBuf.buf = (char*)(msg);
    wsaBuf.len = (unsigned int)(length);
    DWORD sent = 0;
    int rc = WSASendTo(soc_, &wsaBuf, 1, &sent, 0, (const struct sockaddr*)(&socAddr), sizeof(socAddr), 0, 0);
    bool ok = ((rc == 0) && (sent == length));
    if (ok)
    {
        return ok;
    }

    // Non-blocking.
    ok = false;
    if (timeoutInMsecs == 0)
    {
        return ok;
    }

    // Wait and try again.
    unsigned int lastError = WSAGetLastError();
    if ((lastError == WSAEWOULDBLOCK) && (WSAEventSelect(soc_, ev_, FD_CLOSE | FD_WRITE) == 0))
    {
        bool alertable = true;
        unsigned int startTime = GetTickCount();
        unsigned int waitTime = timeoutInMsecs;
        for (WSANETWORKEVENTS occurred;;)
        {
            DWORD waitRc = WSAWaitForMultipleEvents(1, &ev_, 0 /*fWaitAll*/, waitTime, alertable);
            if ((waitRc == WSA_WAIT_EVENT_0) && (WSAEnumNetworkEvents(soc_, ev_, &occurred) == 0))
            {
                if ((occurred.lNetworkEvents & FD_WRITE) != 0)
                {
                    rc = WSASendTo(soc_, &wsaBuf, 1, &sent, 0, (const struct sockaddr*)(&socAddr), sizeof(socAddr), 0, 0);
                    ok = ((rc == 0) && (sent == length));
                }
            }
            else if ((waitRc == WSA_WAIT_IO_COMPLETION) && alertable && (!Thread::isTerminating()))
            {
                unsigned int delta = GetTickCount() - startTime;
                waitTime = (delta >= timeoutInMsecs)? ((alertable = false), 0): (timeoutInMsecs - delta);
                continue;
            }
            break;
        }
        WSAEventSelect(soc_, ev_, 0);
    }

    return ok;
}


int UdpClient::rcv(unsigned char* msg, size_t maxLength, Paddr& src, unsigned int timeoutInMsecs)
{

    // Try receive.
    WSABUF wsaBuf;
    wsaBuf.buf = (char*)(msg);
    wsaBuf.len = (unsigned int)(maxLength);
    DWORD received = 0;
    DWORD flags = 0;
    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);
    int rc = WSARecvFrom(soc_, &wsaBuf, 1 /*dwBufferCount*/, &received, &flags, (struct sockaddr*)(&addr), &addrLen, 0, 0);
    if ((rc == 0) && (addrLen == sizeof(addr)) && (addr.sin_family == AF_INET))
    {
        rc = received;
    }

    // Non-blocking.
    else if (timeoutInMsecs == 0)
    {
        rc = -1;
    }

    // Wait and try again.
    else
    {
        rc = -1;
        unsigned int lastError = WSAGetLastError();
        if ((lastError == WSAEWOULDBLOCK) && (WSAEventSelect(soc_, ev_, FD_CLOSE | FD_READ) == 0))
        {
            bool alertable = true;
            unsigned int startTime = GetTickCount();
            unsigned int waitTime = timeoutInMsecs;
            for (WSANETWORKEVENTS occurred;;)
            {
                DWORD waitRc = WSAWaitForMultipleEvents(1, &ev_, 0 /*fWaitAll*/, waitTime, alertable);
                if ((waitRc == WSA_WAIT_EVENT_0) && (WSAEnumNetworkEvents(soc_, ev_, &occurred) == 0))
                {
                    if ((occurred.lNetworkEvents & FD_READ) != 0)
                    {
                        rc = WSARecvFrom(soc_, &wsaBuf, 1 /*dwBufferCount*/, &received, &flags, (struct sockaddr*)(&addr), &addrLen, 0, 0);
                        rc = ((rc == 0) && (addrLen == sizeof(addr)) && (addr.sin_family == AF_INET))? received: -1;
                    }
                    else if (occurred.lNetworkEvents == 0)
                    {
                        continue; //does occur in wxapps, not sure why --ttp
                    }
                }
                else if ((waitRc == WSA_WAIT_IO_COMPLETION) && alertable && (!Thread::isTerminating()))
                {
                    unsigned int delta = GetTickCount() - startTime;
                    waitTime = (delta >= timeoutInMsecs)? ((alertable = false), 0): (timeoutInMsecs - delta);
                    continue;
                }
                break;
            }
            WSAEventSelect(soc_, ev_, 0);
        }
    }

    if (rc >= 0)
    {
        src.setAddr(ntohl(addr.sin_addr.s_addr));
        src.setPort(ntohs(addr.sin_port));
    }

    return rc;
}


//!
//! Receive and save datagram in given msg buffer. Buffer can hold at most
//! maxLength bytes. Wait up to timeoutInMsecs msecs if necessary. Return
//! the received datagram size in bytes if successful. Return -1 if timed
//! out.
//!
int UdpClient::rcv(unsigned char* msg, size_t maxLength, unsigned int timeoutInMsecs)
{

    // Try receive.
    WSABUF wsaBuf;
    wsaBuf.buf = (char*)(msg);
    wsaBuf.len = (unsigned int)(maxLength);
    DWORD received = 0;
    DWORD flags = 0;
    int rc = WSARecv(soc_, &wsaBuf, 1 /*dwBufferCount*/, &received, &flags, 0, 0);
    if (rc == 0)
    {
        rc = received;
    }

    // Non-blocking.
    else if (timeoutInMsecs == 0)
    {
        rc = -1;
    }

    // Wait and try again.
    else
    {
        rc = -1;
        unsigned int lastError = WSAGetLastError();
        if ((lastError == WSAEWOULDBLOCK) && (WSAEventSelect(soc_, ev_, FD_CLOSE | FD_READ) == 0))
        {
            bool alertable = true;
            unsigned int startTime = GetTickCount();
            unsigned int waitTime = timeoutInMsecs;
            for (WSANETWORKEVENTS occurred;;)
            {
                DWORD waitRc = WSAWaitForMultipleEvents(1, &ev_, 0 /*fWaitAll*/, waitTime, alertable);
                if ((waitRc == WSA_WAIT_EVENT_0) && (WSAEnumNetworkEvents(soc_, ev_, &occurred) == 0))
                {
                    if ((occurred.lNetworkEvents & FD_READ) != 0)
                    {
                        rc = WSARecv(soc_, &wsaBuf, 1 /*dwBufferCount*/, &received, &flags, 0, 0);
                        rc = (rc == 0)? received: -1;
                    }
                    else if (occurred.lNetworkEvents == 0)
                    {
                        continue; //does occur in wxapps, not sure why --ttp
                    }
                }
                else if ((waitRc == WSA_WAIT_IO_COMPLETION) && alertable && (!Thread::isTerminating()))
                {
                    unsigned int delta = GetTickCount() - startTime;
                    waitTime = (delta >= timeoutInMsecs)? ((alertable = false), 0): (timeoutInMsecs - delta);
                    continue;
                }
                break;
            }
            WSAEventSelect(soc_, ev_, 0);
        }
    }

    return rc;
}


void UdpClient::construct()
{
    SOCKET soc = WSASocketW(AF_INET, SOCK_DGRAM, IPPROTO_UDP, 0, 0, WSA_FLAG_OVERLAPPED);
    WSAEVENT ev = WSACreateEvent();
    if ((soc != INVALID_SOCKET) && (ev != WSA_INVALID_EVENT))
    {
        unsigned long arg = 1;
        unsigned long bytesReturned;
        if (WSAIoctl(soc, FIONBIO, &arg, sizeof(arg), 0, 0, &bytesReturned, 0, 0) != 0)
        {
            WSACloseEvent(ev);
            ev = WSA_INVALID_EVENT;
            closesocket(soc);
            soc = INVALID_SOCKET;
        }
    }

    else
    {
        if (ev != WSA_INVALID_EVENT)
        {
            WSACloseEvent(ev);
            ev = WSA_INVALID_EVENT;
        }
        if (soc != INVALID_SOCKET)
        {
            closesocket(soc);
            soc = INVALID_SOCKET;
        }
    }

    ev_ = ev;
    soc_ = soc;
}

END_NAMESPACE1
