#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/poll.h>
#include "netkit/Paddr.hpp"
#include "netkit/UdpClient.hpp"
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(netkit)


UdpClient::~UdpClient()
{
    if (soc_ != INVALID_SOCKET)
    {
        close(soc_);
    }
}


bool UdpClient::snd(const unsigned char* msg, size_t length, const Paddr& dst, unsigned int timeoutInMsecs)
{
    struct sockaddr_in saddr = {0};
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(dst.addr());
    saddr.sin_port = htons(dst.port());

    ssize_t rc = ::sendto(soc_, msg, length, 0 /*flags*/, (const struct sockaddr*)(&saddr), sizeof(saddr));
    bool ok = (rc >= 0) && (static_cast<size_t>(rc) == length);
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
    if (errno == EWOULDBLOCK)
    {
        struct pollfd pfd;
        pfd.fd = soc_;
        pfd.events = POLLOUT; //POLLERR|POLLHUP|POLLNVAL already implied
        pfd.revents = 0;
        int timeout = (timeoutInMsecs >= 0x7fffffffUL)? -1: (int)(timeoutInMsecs);
        int rc = poll(&pfd, 1, timeout);
        if ((rc > 0) || ((pfd.revents & POLLIN) != 0))
        {
        }
    }

#if 0
    unsigned long lastError = WSAGetLastError();
    if ((lastError == WSAEWOULDBLOCK) && (WSAEventSelect(soc_, ev_, FD_CLOSE | FD_WRITE) == 0))
    {
        bool alertable = true;
        unsigned long startTime = GetTickCount();
        unsigned long waitTime = timeoutInMsecs;
        for (WSANETWORKEVENTS occurred;;)
        {
            unsigned long waitRc = WSAWaitForMultipleEvents(1, &ev_, 0 /*fWaitAll*/, waitTime, alertable);
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
                unsigned long delta = GetTickCount() - startTime;
                waitTime = (delta >= timeoutInMsecs)? ((alertable = false), 0): (timeoutInMsecs - delta);
                continue;
            }
            break;
        }
        WSAEventSelect(soc_, ev_, 0);
    }
#endif

    return ok;
}


int UdpClient::rcv(unsigned char* msg, size_t maxLength, Paddr& src, unsigned int timeoutInMsecs)
{

    // Timed out?
    if (timeoutInMsecs > 0)
    {
        struct pollfd pfd;
        pfd.fd = soc_;
        pfd.events = POLLIN; //POLLERR|POLLHUP|POLLNVAL already implied
        pfd.revents = 0;
        int timeout = (timeoutInMsecs >= 0x7fffffffUL)? -1: (int)(timeoutInMsecs);
        int rc = poll(&pfd, 1, timeout);
        if ((rc <= 0) || ((pfd.revents & POLLIN) == 0))
        {
            return 0;
        }
    }

    // Perform non-blocking receive.
    // Return number of received bytes (zero if timed out and negative for other failures).
    struct sockaddr_in saddr;
    socklen_t socAddrLen = sizeof(saddr);
    int rc = recvfrom(soc_, msg, maxLength, 0 /*flags*/, (struct sockaddr*)(&saddr), &socAddrLen);
    if (rc > 0)
    {
        if ((socAddrLen == sizeof(saddr)) && (saddr.sin_family == AF_INET))
        {
            src.setAddr(ntohl(saddr.sin_addr.s_addr));
            src.setPort(ntohs(saddr.sin_port));
        }
        else
        {
            rc = -1;
        }
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

    // Timed out?
    if (timeoutInMsecs > 0)
    {
        struct pollfd pfd;
        pfd.fd = soc_;
        pfd.events = POLLIN; //POLLERR|POLLHUP|POLLNVAL already implied
        pfd.revents = 0;
        int timeout = (timeoutInMsecs >= 0x7fffffffUL)? -1: (int)(timeoutInMsecs);
        int rc = poll(&pfd, 1, timeout);
        if ((rc <= 0) || ((pfd.revents & POLLIN) == 0))
        {
            return 0;
        }
    }

    // Perform non-blocking receive.
    // Return number of received bytes (zero if timed out and negative for other failures).
    int rc = recv(soc_, msg, maxLength, 0 /*flags*/);
    return rc;
}


void UdpClient::construct()
{
    SOCKET soc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (soc != INVALID_SOCKET)
    {
        int flags = fcntl(soc, F_GETFL);
        if ((flags < 0) || (fcntl(soc, F_SETFL, flags | O_NONBLOCK) < 0))
        {
            close(soc);
            soc = INVALID_SOCKET;
        }
    }

    soc_ = soc;
}

END_NAMESPACE1
