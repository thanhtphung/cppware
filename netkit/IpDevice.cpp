/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <string.h>
#include "syskit/Foundation.hpp"
#include "syskit/sys.hpp"

#include "netkit-pch.h"
#include "netkit/IpAddr.hpp"
#include "netkit/IpDevice.hpp"
#include "netkit/IpHlpApi.hpp"

using namespace syskit;

const unsigned int LINK_LOCAL_LO = 0xa9fe0000U; //169.254.0.0
const unsigned int LINK_LOCAL_HI = 0xa9feffffU; //169.254.255.255

BEGIN_NAMESPACE1(netkit)

IpDevice::Instance::Count IpDevice::instanceRef_(initialize());

const char* const IpDevice::ADDR_CATEGORY_E[] =
{
    STRINGIFY(Broadcast),
    STRINGIFY(Far),
    STRINGIFY(Local),
    STRINGIFY(Loopback),
    STRINGIFY(Near),
    0
};

IpDevice* IpDevice::device_[MaxIndex + 1] = {0};
unsigned char IpDevice::numDevices_ = 0;


IpDevice::IpDevice(const IpDevice& ipDevice):
macAddr_(ipDevice.macAddr_)
{
    copyFrom(ipDevice);
}


const IpDevice& IpDevice::operator =(const IpDevice& ipDevice)
{
    if (this != &ipDevice)
    {
        delete[] netMask_;
        delete[] name_;
        delete[] ifDesc_;
        delete[] addr_;
        macAddr_ = ipDevice.macAddr_;
        copyFrom(ipDevice);
    }

    return *this;
}


IpDevice::~IpDevice()
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
IpDevice* IpDevice::findByMac(IpDevice* device[MaxIndex + 1], unsigned char numDevices, const unsigned char* macAddr)
{
    initialize();

    IpDevice* found = 0;
    IpDevice* const* p = device;
    const IpDevice* const* pEnd = p + numDevices;
    for (; p < pEnd; ++p)
    {
        IpDevice* d = *p;
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
bool IpDevice::addrIsNear(unsigned int addr) const
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
bool IpDevice::renew(const IpDevice& cur)
{

    // In the singleton, IpDevice instances can be added but cannot be removed.
    // Obsolete ones are marked old. The index_ value does not change.
    bool modified;
    if ((isOld_ != 0) ||
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
const IpDevice* IpDevice::any()
{
    initialize();

    // More than one available.
    // The non-loopback one is preferred.
    // The link-local one is less preferred.
    const IpDevice* found;
    if (numDevices_ > 1)
    {
        found = device_[0];
        const IpDevice* const* p = device_;
        const IpDevice* const* pEnd = p + numDevices_;
        for (; p < pEnd; ++p)
        {
            const IpDevice* device = *p;
            if (device->numAddrs_ > 0)
            {
                unsigned int addr = device->addr_[0];
                if (((addr >> 24) != LoopbackNet) && ((addr < LINK_LOCAL_LO) || (addr > LINK_LOCAL_HI)))
                {
                    found = device;
                    break;
                }
            }
        }
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
//! device name (e.g., "{136B6AAF-4F98-4831-9C77-CB1AC36F1EA6}"). Return
//! zero if not found.
//!
const IpDevice* IpDevice::find(const char* key)
{
    initialize();

    // Is key a MAC address?
    MacAddr macAddr;
    bool ok = macAddr.reset(key);
    if (ok)
    {
        const IpDevice* found = find(macAddr.asRawBytes());
        return found;
    }

    // Is key an IPv4 address?
    IpAddr ipAddr;
    ok = ipAddr.reset(key);
    if (ok)
    {
        const IpDevice* found = find(ipAddr.asU32());
        return found;
    }

    // Assume key is a device name.
    const IpDevice* found = 0;
    const IpDevice* const* p = device_;
    const IpDevice* const* pEnd = p + numDevices_;
    for (; p < pEnd; ++p)
    {
        const IpDevice* device = *p;
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
const IpDevice* IpDevice::find(unsigned int addr)
{
    initialize();

    const IpDevice* found = 0;
    const IpDevice* const* p = device_;
    const IpDevice* const* pEnd = p + numDevices_;
    for (; p < pEnd; ++p)
    {
        const IpDevice* device = *p;
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
unsigned int /*IpDevice::addrCategory_e*/ IpDevice::categorize(unsigned int addr)
{

    // Loopback?
    // Broadcast?
    initialize();
    if ((addr >> 24) == LoopbackNet)
    {
        return ((addr == 0x7f000000U) || (addr == 0x7fffffffU))? Broadcast: Loopback;
    }

    const IpDevice* const* p = device_;
    const IpDevice* const* pEnd = p + numDevices_;
    unsigned int category = Far;
    for (; p < pEnd; ++p)
    {
        const IpDevice* device = *p;
        unsigned int numAddrs = device->numAddrs_;
        for (unsigned int i = 0; i < numAddrs; ++i)
        {

            // Local? (home address of one of the IP devices)
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


void IpDevice::copyFrom(const IpDevice& ipDevice)
{
    index_ = ipDevice.index_;
    isOld_ = ipDevice.isOld_;
    numAddrs_ = ipDevice.numAddrs_;

    unsigned int* p = new unsigned int[numAddrs_];
    memcpy(p, ipDevice.addr_, sizeof(*p) * numAddrs_);
    addr_ = p;

    ifDesc_ = syskit::strdup(ipDevice.ifDesc_);
    name_ = syskit::strdup(ipDevice.name_);

    p = new unsigned int[numAddrs_];
    memcpy(p, ipDevice.netMask_, sizeof(*p) * numAddrs_);
    netMask_ = p;
}


const IpDevice::Instance& IpDevice::initialize()
{
    static const Instance* s_instance = new Instance;
    return *s_instance;
}


IpDevice::Instance::~Instance()
{
    IpDevice** p = device_ + numDevices_ - 1;
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
