/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <string.h>
#include "syskit/Foundation.hpp"
#include "syskit/sys.hpp"

#include "netkit-pch.h"

#include <pcap.h>
#ifdef strdup
#undef strdup
#endif

#include "netkit/CapDevice.hpp"
#include "netkit/IpAddr.hpp"
#include "netkit/IpHlpApi.hpp"
#include "netkit/net.hpp"

#if _DEBUG
#pragma comment(lib,"packetd")
#pragma comment(lib,"wpcapd")
#else
#pragma comment(lib,"packet")
#pragma comment(lib,"wpcap")
#endif

using namespace syskit;

BEGIN_NAMESPACE1(netkit)

CapDevice::Instance::Count CapDevice::instanceRef_(initialize());
const char* const CapDevice::ADDR_CATEGORY_E[] =
{
    STRINGIFY(Broadcast),
    STRINGIFY(Far),
    STRINGIFY(Local),
    STRINGIFY(Loopback),
    STRINGIFY(Near),
    0
};
CapDevice* CapDevice::device_[MaxIndex + 1] = {0};
unsigned char CapDevice::numDevices_ = 0;


CapDevice::CapDevice(const CapDevice& device):
macAddr_(device.macAddr_)
{
    copyFrom(device);
}


const CapDevice& CapDevice::operator =(const CapDevice& device)
{
    if (this != &device)
    {
        delete[] netMask_;
        delete[] name_;
        delete[] ifDesc_;
        delete[] addr_;
        macAddr_ = device.macAddr_;
        copyFrom(device);
    }

    return *this;
}


CapDevice::~CapDevice()
{
    delete[] netMask_;
    delete[] name_;
    delete[] ifDesc_;
    delete[] addr_;
}


//
// Locate the device w/ given address.
// Look for it in the given array (numDevices in device).
// Return zero if not found.
//
CapDevice* CapDevice::findByMac(CapDevice* device[MaxIndex + 1], unsigned char numDevices, const unsigned char* macAddr)
{
    initialize();

    CapDevice* found = 0;
    CapDevice* const* p = device;
    const CapDevice* const* pEnd = p + numDevices;
    for (; p < pEnd; ++p)
    {
        CapDevice* d = *p;
        if (d->macAddr_ == macAddr)
        {
            found = d;
            break;
        }
    }

    return found;
}


//!
//! Return true if given address is near (part of a connected subnet).
//!
bool CapDevice::addrIsNear(unsigned int addr) const
{
    bool isNear = false;
    unsigned int numAddrs = numAddrs_;
    for (unsigned int i = 0; i < numAddrs; ++i)
    {
        if (addr == addr_[i])
        {
            return isNear;
        }
    }

    for (unsigned int i = 0; i < numAddrs; ++i)
    {
        unsigned int netMask = netMask_[i];
        if ((addr & netMask) == (addr_[i] & netMask))
        {
            isNear = true;
            break;
        }
    }

    return isNear;
}


//
// Renew device with current characteristics if necessary
// (if some changes occurred). Return true if modified.
//
bool CapDevice::renew(const CapDevice& cur)
{
    // In the singleton, CapDevice instances can be added but cannot be removed.
    // Obsolete ones are marked old. The index_ value does not change.
    bool modified;
    if ((isOld_ != 0) ||
        (ifIndex_ != cur.ifIndex_) ||
        (numAddrs_ != cur.numAddrs_) ||
        (memcmp(addr_, cur.addr_, numAddrs_ * sizeof(*addr_)) != 0) ||
        (memcmp(netMask_, cur.netMask_, numAddrs_ * sizeof(*netMask_)) != 0) ||
        (wcscmp(ifDesc_, cur.ifDesc_) != 0) ||
        (strcmp(name_, cur.name_) != 0))
    {
        unsigned char index = index_;
        *this = cur;
        index_ = index;
        modified = true;
    }
    else
    {
        modified = false;
    }

    return modified;
}


//!
//! Return any device (loopback is okay if nothing else is available).
//! Return zero if none.
//!
const CapDevice* CapDevice::any()
{
    initialize();

    // More than one available.
    // The non-loopback one is preferred.
    const CapDevice* found;
    if (numDevices_ > 1)
    {
        found = device_[0]->isLoopback()? device_[1]: device_[0];
    }

    // Only one available.
    else if (numDevices_ > 0)
    {
        found = device_[0];
    }

    // None.
    else
    {
        found = 0;
    }

    return found;
}


//!
//! Locate the device w/ given key. Given key can be a MAC address (e.g.,
//! "00:23:AE:6A:DA:57"), an IPv4 address (e.g., "192.168.60.166"), or a
//! device name (e.g., "\\Device\\NPF_{136B6AAF-4F98-4831-9C77-CB1AC36F1EA6}").
//! Return zero if not found.
//!
const CapDevice* CapDevice::find(const char* key)
{
    initialize();

    // Is key a MAC address?
    MacAddr macAddr;
    bool ok = macAddr.reset(key);
    if (ok)
    {
        const CapDevice* found = find(macAddr.asRawBytes());
        return found;
    }

    // Is key an IPv4 address?
    IpAddr ipAddr;
    ok = ipAddr.reset(key);
    if (ok)
    {
        const CapDevice* found = find(ipAddr.asU32());
        return found;
    }

    // Assume key is a device name.
    const CapDevice* found = 0;
    const CapDevice* const* p = device_;
    const CapDevice* const* pEnd = p + numDevices_;
    for (; p < pEnd; ++p)
    {
        const CapDevice* device = *p;
        if (strcmp(key, device->name_) == 0)
        {
            found = device;
            break;
        }
    }

    return found;
}


//!
//! Locate the device w/ given address.
//! Return zero if not found.
//!
const CapDevice* CapDevice::find(unsigned int addr)
{
    initialize();

    const CapDevice* found = 0;
    const CapDevice* const* p = device_;
    const CapDevice* const* pEnd = p + numDevices_;
    for (; p < pEnd; ++p)
    {
        const CapDevice* device = *p;
        for (unsigned int i = 0; i < device->numAddrs_; ++i)
        {
            if (addr == device->addr_[i])
            {
                found = device;
                p = pEnd - 1; //terminate outer loop
                break;
            }
        }
    }

    return found;
}


//!
//! Categorize given address.
//!
unsigned int /*CapDevice::addrCategory_e*/ CapDevice::categorize(unsigned int addr)
{
    // Loopback?
    // Broadcast?
    initialize();
    if ((addr >> 24) == LoopbackNet)
    {
        return ((addr == 0x7f000000U) || (addr == 0x7fffffffU))? Broadcast: Loopback;
    }

    const CapDevice* const* p = device_;
    const CapDevice* const* pEnd = p + numDevices_;
    unsigned int category = Far;
    for (; p < pEnd; ++p)
    {
        const CapDevice* device = *p;
        unsigned int numAddrs = device->numAddrs_;
        for (unsigned int i = 0; i < numAddrs; ++i)
        {

            // Local? (home address of one of the capturing devices)
            if (addr == device->addr_[i])
            {
                return Local;
            }

            // Broadcast? (broadcast address for a connected subnet)
            unsigned int netMask = device->netMask_[i];
            unsigned int broadcast0 = device->addr_[i] & netMask;
            unsigned int broadcast1 = broadcast0 | ~netMask;
            if ((addr == broadcast0) || (addr == broadcast1))
            {
                return Broadcast;
            }

            // Near? (part of a connected subnet)
            // Only if it's not a local address nor a broadcast address.
            if ((category != Near) && (addr & netMask) == broadcast0)
            {
                category = Near;
            }
        }
    }

    return category;
}


void CapDevice::copyFrom(const CapDevice& device)
{
    ifIndex_ = device.ifIndex_;
    ifSpeed_ = device.ifSpeed_;
    ifType_ = device.ifType_;
    index_ = device.index_;
    isOld_ = device.isOld_;
    numAddrs_ = device.numAddrs_;

    unsigned int* p = new unsigned int[numAddrs_];
    memcpy(p, device.addr_, sizeof(*p) * numAddrs_);
    addr_ = p;

    ifDesc_ = syskit::strdup(device.ifDesc_);
    name_ = syskit::strdup(device.name_);

    p = new unsigned int[numAddrs_];
    memcpy(p, device.netMask_, sizeof(*p) * numAddrs_);
    netMask_ = p;
}


const CapDevice::Instance& CapDevice::initialize()
{
    static const Instance* s_instance = new Instance;
    return *s_instance;
}


//!
//! Changes to capture devices likely have occurred. Refresh singleton.
//! Return the number of changes that were effective.
//!
unsigned char CapDevice::refresh()
{
    CapDevice* curDevice[MaxIndex + 1];
    unsigned char numCurDevices = mkDevice(curDevice);

    // Find old devices.
    unsigned char numChanges = 0;
    for (unsigned char i = 0; i < numDevices_; ++i)
    {
        CapDevice* p = device_[i];
        CapDevice* found = findByMac(curDevice, numCurDevices, p->macAddr_);
        if (found == 0)
        {
            p->age();
            ++numChanges;
        }
    }

    // Find new/renewed devices.
    for (unsigned char i = 0; i < numCurDevices; ++i)
    {
        CapDevice* p = curDevice[i];
        CapDevice* found = findByMac(device_, numDevices_, p->macAddr_);
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

    for (CapDevice* const* p = curDevice + numCurDevices - 1; p >= curDevice; delete *p--);
    return numChanges;
}


unsigned int CapDevice::normalizeAddrs(const struct pcap_addr* pcapAddr,
    unsigned int addr[MaxAddrIndex + 1],
    unsigned int mask[MaxAddrIndex + 1])
{
    unsigned int numAddrs = 0;
    for (const pcap_addr_t* p = pcapAddr; p != 0; p = p->next)
    {
        if (p->addr->sa_family == AF_INET)
        {
            const struct sockaddr_in* socAddr = reinterpret_cast<const struct sockaddr_in*>(p->addr);
            addr[numAddrs] = ntohl(socAddr->sin_addr.s_addr);
            socAddr = reinterpret_cast<const struct sockaddr_in*>(p->netmask);
            mask[numAddrs] = (socAddr != 0)? ntohl(socAddr->sin_addr.s_addr): addr[numAddrs];
            if (numAddrs++ == CapDevice::MaxAddrIndex)
            {
                break;
            }
        }
    }

    return numAddrs;
}


CapDevice::Instance::~Instance()
{
    CapDevice** p = device_ + numDevices_ - 1;
    numDevices_ = 0;
    for (; p >= device_; --p)
    {
        delete *p;
        *p = 0;
    }

    IpHlpApi::instance().rmRef();
    Foundation::instance().rmRef();
}

END_NAMESPACE1
