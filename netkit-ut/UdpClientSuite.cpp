#include "netkit/Paddr.hpp"
#include "netkit/S32SocOpt.hpp"
#include "netkit/UdpClient.hpp"

#include "netkit-ut-pch.h"
#include "UdpClientSuite.hpp"

using namespace netkit;

BEGIN_NAMESPACE

class BadClient: public netkit::UdpClient
{
public:
    BadClient();
    virtual ~BadClient();
private:
    BadClient(const BadClient&); //prohibit usage
    const BadClient& operator =(const BadClient&); //prohibit usage
};

BadClient::BadClient():
UdpClient(INVALID_SOCKET, WSA_INVALID_EVENT)
{
}

BadClient::~BadClient()
{
}

END_NAMESPACE


UdpClientSuite::UdpClientSuite()
{
}


UdpClientSuite::~UdpClientSuite()
{
}


void UdpClientSuite::testCtor00()
{
    UdpClient client;
    bool ok = client.isOk();
    CPPUNIT_ASSERT(ok);

    unsigned int addr = INADDR_ANY;
    unsigned short port = 0;
    ok = client.bind(addr, port) && (!client.bind(addr, port));
    CPPUNIT_ASSERT(ok);

    Paddr paddr;
    ok = client.getMyAddr(paddr) && (paddr.port() != 0);
    CPPUNIT_ASSERT(ok);

    int ibufSize = 0;
    int obufSize = 0;
    S32SocOpt opt(client.soc());
    ok = opt.getV(S32SocOpt::N_RCVBUF, ibufSize) && opt.getV(S32SocOpt::N_SNDBUF, obufSize);
    CPPUNIT_ASSERT(ok);
    ok = (ibufSize > 0) && (obufSize > 0);
    CPPUNIT_ASSERT(ok);
}


void UdpClientSuite::testCtor01()
{
    int ibufSize = 1234;
    int obufSize = 1235;
    UdpClient client(ibufSize, obufSize);
    bool ok = client.isOk();
    CPPUNIT_ASSERT(ok);

    ibufSize = 0;
    obufSize = 0;
    S32SocOpt opt(client.soc());
    opt.getV(S32SocOpt::N_RCVBUF, ibufSize);
    opt.getV(S32SocOpt::N_SNDBUF, obufSize);
    ok = (ibufSize == 1234) && (obufSize == 1235);
    CPPUNIT_ASSERT(ok);

    // Must bind before receiving for the checks to be meaningful.
    unsigned int addr = INADDR_ANY;
    unsigned short port = 0;
    client.bind(addr, port);

    Paddr src;
    unsigned char msg[1];
    size_t maxLength = sizeof(msg);
    unsigned int timeoutInMsecs = 0;
    int bytesRcvd = client.rcv(msg, maxLength, timeoutInMsecs);
    ok = (bytesRcvd < 0);
    CPPUNIT_ASSERT(ok);
    bytesRcvd = client.rcv(msg, maxLength, src, timeoutInMsecs);
    ok = (bytesRcvd < 0);
    CPPUNIT_ASSERT(ok);

    timeoutInMsecs = 10;
    bytesRcvd = client.rcv(msg, maxLength, timeoutInMsecs);
    ok = (bytesRcvd < 0);
    CPPUNIT_ASSERT(ok);
    bytesRcvd = client.rcv(msg, maxLength, src, timeoutInMsecs);
    ok = (bytesRcvd < 0);
    CPPUNIT_ASSERT(ok);
}


void UdpClientSuite::testCtor02()
{
    unsigned int addr = INADDR_ANY;
    unsigned short port = 0;
    Paddr paddr0;
    UdpClient client0(addr, port, paddr0);
    bool ok = (paddr0.port() != 0);
    CPPUNIT_ASSERT(ok);

    Paddr paddr1;
    ok = client0.getMyAddr(paddr1) && (paddr1 == paddr0);
    CPPUNIT_ASSERT(ok);

    port = paddr0.port();
    UdpClient client1(addr, port, paddr0);
    ok = (paddr0.port() == 0);
    CPPUNIT_ASSERT(ok);
}


void UdpClientSuite::testCtor03()
{
    UdpClient* client = new BadClient;
    Paddr paddr;
    bool ok = (!client->bind(INADDR_LOOPBACK, 0U)) && (!client->getMyAddr(paddr));
    CPPUNIT_ASSERT(ok);
    delete client;
}


void UdpClientSuite::testNull00()
{
    UdpClient* nullClient = UdpClient::createNull();
    bool ok = nullClient->isOk() && (nullClient->soc() == INVALID_SOCKET);
    CPPUNIT_ASSERT(ok);

    Paddr dst;
    ok = nullClient->connect(dst);
    CPPUNIT_ASSERT(ok);

    unsigned char msg[] = {0};
    size_t length = 1;
    ok = nullClient->snd(msg, length, dst);
    CPPUNIT_ASSERT(ok);
    ok = (nullClient->rcv(msg, length) < 0);
    CPPUNIT_ASSERT(ok);
    Paddr src;
    ok = (nullClient->rcv(msg, length, src) < 0);
    CPPUNIT_ASSERT(ok);

    delete nullClient;
}


void UdpClientSuite::testSnd00()
{
    UdpClient client;
    unsigned int addr = INADDR_LOOPBACK;
    unsigned short port = 0;
    client.bind(addr, port);
    Paddr paddr;
    client.getMyAddr(paddr);

    unsigned char* msg = 0;
    unsigned int timeoutInMsecs = 0;
    size_t length = 0;
    bool ok = client.snd(msg, length, paddr, timeoutInMsecs);
    CPPUNIT_ASSERT(ok);

    Paddr src;
    size_t maxLength = 0;
    timeoutInMsecs = 123;
    int bytesRcvd = client.rcv(msg, maxLength, src, timeoutInMsecs);
    ok = (bytesRcvd == 0) && (src == paddr);
    CPPUNIT_ASSERT(ok);
}
