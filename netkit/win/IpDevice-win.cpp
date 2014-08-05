/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/Foundation.hpp"
#include "syskit/sys.hpp"

#include "netkit-pch.h"
#include "netkit/IpDevice.hpp"
#include "netkit/IpHlpApi.hpp"
#include "netkit/MacAddr.hpp"

using namespace syskit;

const unsigned char LOOPBACK_MAC_ADDR[] = {0U, 0U, 0U, 0U, 0U, 0U};

BEGIN_NAMESPACE1(netkit)


static IP_ADAPTER_ADDRESSES_XP* getAdapterAddresses()
{
    const IpHlpApi& api = IpHlpApi::instance();
    GetAdaptersAddresses_t GetAdaptersAddresses = api.GetAdaptersAddresses();

    IP_ADAPTER_ADDRESSES_XP* adapter = 0;
    unsigned int flags = GAA_FLAG_INCLUDE_PREFIX |
        GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_FRIENDLY_NAME | GAA_FLAG_SKIP_MULTICAST;
    unsigned int family = AF_INET;

    // GetAdaptersAddresses() uses a significant amount of network resources,
    // so try invoking it once only with a reasonably-sized buffer to start.
    unsigned long bufSize = 16384;
    adapter = reinterpret_cast<IP_ADAPTER_ADDRESSES_XP*>(new unsigned char[bufSize]);
    unsigned int errCode = GetAdaptersAddresses(family, flags, 0, adapter, &bufSize);
    if (errCode == ERROR_SUCCESS)
    {
        return adapter;
    }

    for (;;)
    {
        delete[] reinterpret_cast<unsigned char*>(adapter);
        adapter = 0;
        if (errCode == ERROR_BUFFER_OVERFLOW)
        {
            adapter = reinterpret_cast<IP_ADAPTER_ADDRESSES_XP*>(new unsigned char[bufSize]);
            errCode = GetAdaptersAddresses(family, flags, 0, adapter, &bufSize);
            if (errCode == ERROR_SUCCESS)
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    return adapter;
}


IpDevice::IpDevice(const struct _IP_ADAPTER_ADDRESSES_XP& adapter,
    unsigned char index,
    unsigned int numAddrs,
    const unsigned int* addr,
    const unsigned int* netMask):
    macAddr_((adapter.PhysicalAddressLength == 0)? LOOPBACK_MAC_ADDR: adapter.PhysicalAddress)
{
    index_ = index;
    isOld_ = 0;
    numAddrs_ = numAddrs;

    unsigned int* p = new unsigned int[numAddrs_];
    memcpy(p, addr, sizeof(*p) * numAddrs_);
    addr_ = p;

    ifDesc_ = syskit::strdup(adapter.Description);
    name_ = syskit::strdup(adapter.AdapterName);

    p = new unsigned int[numAddrs_];
    memcpy(p, netMask, sizeof(*p) * numAddrs_);
    netMask_ = p;
}


unsigned char IpDevice::mkDevice(IpDevice* device[MaxIndex + 1], const struct _IP_ADAPTER_ADDRESSES_XP* adapter)
{
    unsigned char numDevices = 0;
    for (const IP_ADAPTER_ADDRESSES_XP* p0 = adapter; p0 != 0; p0 = p0->Next)
    {

        unsigned int numAddrs = 0;
        unsigned int addr[MaxAddrIndex + 1];
        unsigned int mask[MaxAddrIndex + 1];
        const IP_ADAPTER_PREFIX* p1 = p0->FirstPrefix;
        for (const IP_ADAPTER_UNICAST_ADDRESS* p2 = p0->FirstUnicastAddress; (p1 != 0) && (p2 != 0); p1 = p1->Next, p2 = p2->Next)
        {
            const struct sockaddr_in* socAddr = reinterpret_cast<const struct sockaddr_in*>(p2->Address.lpSockaddr);
            if (p2->Address.iSockaddrLength == sizeof(*socAddr))
            {
                addr[numAddrs] = ntohl(socAddr->sin_addr.s_addr);
                mask[numAddrs] = 0xffffffffUL << (32 - p1->PrefixLength);
                if (numAddrs++ == MaxAddrIndex)
                {
                    break;
                }
            }
        }

        if ((numAddrs > 0) &&
            ((p0->PhysicalAddressLength == MacAddr::RawLength) || ((p0->PhysicalAddressLength == 0) && (addr[0] == INADDR_LOOPBACK))) &&
            (p0->OperStatus == IfOperStatusUp))
        {
            device[numDevices] = new IpDevice(*p0, numDevices, numAddrs, addr, mask);
            if (numDevices++ == MaxIndex)
            {
                break;
            }
        }
    }

    return numDevices;
}


//!
//! Changes to IP devices likely have occurred. Refresh singleton.
//! Return the number of changes that were effective.
//!
unsigned char IpDevice::refresh()
{
    const IP_ADAPTER_ADDRESSES_XP* adapter = getAdapterAddresses();
    IpDevice* curDevice[MaxIndex + 1];
    unsigned char numCurDevices = mkDevice(curDevice, adapter);

    // Find old devices.
    unsigned char numChanges = 0;
    for (unsigned char i = 0; i < numDevices_; ++i)
    {
        IpDevice* p = device_[i];
        IpDevice* found = findByMac(curDevice, numCurDevices, p->macAddr_);
        if (found == 0)
        {
            p->age();
            ++numChanges;
        }
    }

    // Find new/renewed devices.
    for (unsigned char i = 0; i < numCurDevices; ++i)
    {
        IpDevice* p = curDevice[i];
        IpDevice* found = findByMac(device_, numDevices_, p->macAddr_);
        if (found == 0)
        {
            if (numDevices_ <= MaxIndex)
            {
                device_[numDevices_] = p;
                ++numDevices_;
                curDevice[i] = 0;
                ++numChanges;
            }
        }
        else if (found->renew(*p))
        {
            ++numChanges;
        }
    }

    for (IpDevice* const* p = curDevice + numCurDevices - 1; p >= curDevice; delete *p--);
    delete[] reinterpret_cast<const unsigned char*>(adapter);
    return numChanges;
}


IpDevice::Instance::Instance()
{
    Foundation::instance().addRef();
    IpHlpApi::instance().addRef();

    const IP_ADAPTER_ADDRESSES_XP* adapter = getAdapterAddresses();
    numDevices_ = mkDevice(device_, adapter);
    delete[] reinterpret_cast<const unsigned char*>(adapter);
}

END_NAMESPACE1
