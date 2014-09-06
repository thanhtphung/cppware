/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_IP_DEVICE_HPP
#define NETKIT_IP_DEVICE_HPP

#include "netkit/MacAddr.hpp"
#include "syskit/RefCounted.hpp"
#include "syskit/macros.h"

struct _IP_ADAPTER_ADDRESSES_XP;

BEGIN_NAMESPACE1(netkit)


//! available operational IP device
class IpDevice
    //!
    //! A class representing an available IP device. Each device is known to be operational at
    //! construction time. Each device has a name in addition to an index. Each is associated
    //! with one MAC address in addition to one or more IP address/net mask pairs. An arbitrary
    //! maximum of (MaxIndex+1) devices is supported. An arbitrary maximum of (MaxAddrIndex+1)
    //! IP address/net mask pairs per device is supported.
    //!
{

public:
    enum addrCategory_e
    {
        Broadcast = 0,
        Far,
        Local,
        Loopback,
        Near
    };
    static const char* const ADDR_CATEGORY_E[];

    enum
    {
        LoopbackNet = 127,
        MaxAddrIndex = 255,
        MaxIndex = 31,
        NumAddrCategories = Near + 1
    };

    IpDevice(const IpDevice& device);
    ~IpDevice();

    const IpDevice& operator =(const IpDevice& device);

    const MacAddr& macAddr() const;
    bool addrIsNear(unsigned int addr) const;
    bool isLoopback() const;
    bool isVmware() const;
    bool isOld() const;
    const char* name() const;
    const wchar_t* ifDesc() const;
    unsigned char index() const;
    unsigned int addr(size_t index = 0) const;
    unsigned int netMask(size_t index = 0) const;
    unsigned int numAddrs() const;

    static const IpDevice* any();
    static const IpDevice* find(const char* key);
    static const IpDevice* find(const unsigned char macAddr[MacAddr::RawLength]);
    static const IpDevice* find(unsigned int addr);
    static const IpDevice* instance(unsigned char index = 0);
    static unsigned char numDevices();
    static unsigned char refresh();

    static unsigned int /*addrCategory_e*/ categorize(unsigned int addr);

private:

    class Instance: public syskit::RefCounted
    {
    public:
        Instance();
    protected:
        virtual ~Instance();
    private:
        Instance(const Instance&); //prohibit usage
        const Instance& operator =(const Instance&); //prohibit usage
    };

    MacAddr macAddr_;
    const char* name_;
    const unsigned int* addr_;
    const unsigned int* netMask_;
    const wchar_t* ifDesc_;
    unsigned char index_;
    unsigned char isOld_;
    unsigned int numAddrs_;

    static Instance::Count instanceRef_;

    static IpDevice* device_[MaxIndex + 1];
    static unsigned char numDevices_;

    IpDevice(const struct _IP_ADAPTER_ADDRESSES_XP&, unsigned char, unsigned int, const unsigned int*, const unsigned int*);

    bool renew(const IpDevice&);
    void age();
    void copyFrom(const IpDevice&);

    static IpDevice* findByMac(IpDevice*[MaxIndex + 1], unsigned char, const unsigned char*);
    static const Instance& initialize();
    static unsigned char mkDevice(IpDevice*[MaxIndex + 1], const struct _IP_ADAPTER_ADDRESSES_XP*);

};

//! Return true if this is a loopback device.
inline bool IpDevice::isLoopback() const
{
    return ((numAddrs_ > 0) && ((addr_[0] >> 24) == LoopbackNet));
}

//! Return true if device is old.
//! An old device must be renewed before it can function again.
inline bool IpDevice::isOld() const
{
    return (isOld_ != 0);
}

//! Return true if this device has a VMware-assigned address.
inline bool IpDevice::isVmware() const
{
    bool vmware = macAddr_.isVmware();
    return vmware;
}

//! Locate the device w/ given address.
//! Return zero if not found.
inline const IpDevice* IpDevice::find(const unsigned char macAddr[MacAddr::RawLength])
{
    const IpDevice* found = findByMac(device_, numDevices_, macAddr);
    return found;
}

//! Return the singleton w/ given index. Behavior is unpredictable if
//! given index is invalid (greater than MaxIndex). Method should not
//! be used during static data construction/destruction.
inline const IpDevice* IpDevice::instance(unsigned char index)
{
    return device_[index];
}

//! Return the device MAC address.
inline const MacAddr& IpDevice::macAddr() const
{
    return macAddr_;
}

//! Return the device name.
inline const char* IpDevice::name() const
{
    return name_;
}

//! Return the interface description.
inline const wchar_t* IpDevice::ifDesc() const
{
    return ifDesc_;
}

//! Return the device index (0..numDevices()-1).
inline unsigned char IpDevice::index() const
{
    return index_;
}

//! Return the number of available devices.
//! Method should not be used during static data construction/destruction.
inline unsigned char IpDevice::numDevices()
{
    return numDevices_;
}

//! Return the device address. If the device has more than one
//! address, return the address with given index (0..numAddrs()-1).
inline unsigned int IpDevice::addr(size_t index) const
{
    return addr_[index];
}

//! Return the device net mask. If the device has more than one
//! address, return the net mask with given index (0..numAddrs()-1).
inline unsigned int IpDevice::netMask(size_t index) const
{
    return netMask_[index];
}

//! Return the number of addresses this device is associated with.
inline unsigned int IpDevice::numAddrs() const
{
    return numAddrs_;
}

inline void IpDevice::age()
{
    isOld_ = 1;
}

END_NAMESPACE1

#endif
