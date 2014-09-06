/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_CAP_DEVICE_HPP
#define NETKIT_CAP_DEVICE_HPP

#include "netkit/MacAddr.hpp"
#include "syskit/RefCounted.hpp"
#include "syskit/macros.h"

struct _MIB_IFROW;
struct _MIB_IFTABLE;
struct _MIB_IF_ROW2;
struct _MIB_IF_TABLE2;
struct pcap_addr;
struct pcap_if;

BEGIN_NAMESPACE1(netkit)


//! available network capture device
class CapDevice
    //!
    //! A class representing an available network capture device. Each device has a name in
    //! addition to an index. Each is associated with one MAC address in addition to zero or
    //! more IP address/net mask pairs. An arbitrary maximum of (MaxIndex+1) capture devices
    //! is supported. An arbitrary maximum of (MaxAddrIndex+1) IP address/net mask pairs per
    //! capture device is supported.
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

    CapDevice(const CapDevice& device);
    ~CapDevice();

    const CapDevice& operator =(const CapDevice& device);

    const MacAddr& macAddr() const;
    bool addrIsNear(unsigned int addr) const;
    bool isLoopback() const;
    bool isVmware() const;
    bool isOld() const;
    const char* name() const;
    const wchar_t* ifDesc() const;
    unsigned char index() const;
    unsigned int addr(size_t index = 0) const;
    unsigned int ifIndex() const;
    unsigned int ifType() const;
    unsigned int netMask(size_t index = 0) const;
    unsigned int numAddrs() const;
    unsigned long long ifSpeed() const;

    static const CapDevice* any();
    static const CapDevice* find(const char* key);
    static const CapDevice* find(const unsigned char macAddr[MacAddr::RawLength]);
    static const CapDevice* find(unsigned int addr);
    static const CapDevice* instance(unsigned char index = 0);
    static unsigned char numDevices();
    static unsigned char refresh();

    static unsigned int /*addrCategory_e*/ categorize(unsigned int addr);


    //! device stats
    class Stat
        //!
        //! Available stats.
        //!
    {
    public:
        Stat(const CapDevice& device);
        unsigned long long inBytes() const;
        unsigned long long inPkts() const;
        unsigned long long outBytes() const;
        unsigned long long outPkts() const;
        void reset(const CapDevice& device);
    private:
        unsigned long long inBytes_;
        unsigned long long inPkts_;
        unsigned long long outBytes_;
        unsigned long long outPkts_;
        Stat(const Stat&); //prohibit usage
        const Stat& operator =(const Stat&); //prohibit usage
        friend CapDevice;
    };

private:
    typedef bool (CapDevice::*getStat_t)(Stat&) const;

    class Instance: public syskit::RefCounted
    {
    public:
        Instance();
    protected:
        virtual ~Instance();
        Instance(const Instance&); //prohibit usage
        const Instance& operator =(const Instance&); //prohibit usage
    };

    MacAddr macAddr_;
    const char* name_;
    const unsigned int* addr_;
    const unsigned int* netMask_;
    const wchar_t* ifDesc_;
    getStat_t getStat_;
    unsigned char index_;
    unsigned char isOld_;
    unsigned int ifIndex_;
    unsigned int ifType_;
    unsigned int numAddrs_;
    unsigned long long ifSpeed_;

    static Instance::Count instanceRef_;

    static CapDevice* device_[MaxIndex + 1];
    static unsigned char numDevices_;

    CapDevice(const char*, const struct _MIB_IFROW&, unsigned char, unsigned int, const unsigned int*, const unsigned int*);
    CapDevice(const char*, const struct _MIB_IF_ROW2&, unsigned char, unsigned int, const unsigned int*, const unsigned int*);

    bool getStat(Stat&) const;
    bool getIfStat(Stat&) const;
    bool getIfStat2(Stat&) const;
    bool renew(const CapDevice&);
    void age();
    void copyFrom(const CapDevice&);

    static CapDevice* findByMac(CapDevice*[MaxIndex + 1], unsigned char, const unsigned char*);
    static const Instance& initialize();
    static unsigned char mkDevice(CapDevice*[MaxIndex + 1]);
    static unsigned char mkDevice(CapDevice*[MaxIndex + 1], const struct pcap_if*, const MacAddr*, const struct _MIB_IFTABLE*);
    static unsigned char mkDevice(CapDevice*[MaxIndex + 1], const struct pcap_if*, const struct _MIB_IF_TABLE2*);
    static unsigned int normalizeAddrs(const struct pcap_addr*, unsigned int[MaxAddrIndex + 1], unsigned int[MaxAddrIndex + 1]);

};

inline bool CapDevice::getStat(Stat& stat) const
{
    return (this->*getStat_)(stat);
}

//! Return true if this is a loopback device.
inline bool CapDevice::isLoopback() const
{
    return ((addr_[0] >> 24) == LoopbackNet);
}

//! Return true if device is old.
//! An old device must be renewed before it can function again.
inline bool CapDevice::isOld() const
{
    return (isOld_ != 0);
}

//! Return true if this device has a VMware-assigned address.
inline bool CapDevice::isVmware() const
{
    bool vmware = macAddr_.isVmware();
    return vmware;
}

//! Locate the device w/ given address.
//! Return zero if not found.
inline const CapDevice* CapDevice::find(const unsigned char macAddr[MacAddr::RawLength])
{
    const CapDevice* found = findByMac(device_, numDevices_, macAddr);
    return found;
}

//! Return the singleton w/ given index. Behavior is unpredictable if
//! given index is invalid (greater than MaxIndex). Method should not
//! be used during static data construction/destruction.
inline const CapDevice* CapDevice::instance(unsigned char index)
{
    return device_[index];
}

//! Return the device MAC address.
inline const MacAddr& CapDevice::macAddr() const
{
    return macAddr_;
}

//! Return the device name.
inline const char* CapDevice::name() const
{
    return name_;
}

//! Return the interface description.
inline const wchar_t* CapDevice::ifDesc() const
{
    return ifDesc_;
}

//! Return the device index (0..numDevices()-1).
inline unsigned char CapDevice::index() const
{
    return index_;
}

//! Return the number of available devices.
//! Method should not be used during static data construction/destruction.
inline unsigned char CapDevice::numDevices()
{
    return numDevices_;
}

//! Return the device address. If the device has more than one
//! address, return the address with given index (0..numAddrs()-1).
inline unsigned int CapDevice::addr(size_t index) const
{
    return addr_[index];
}

//! Return the SNMP interface index.
inline unsigned int CapDevice::ifIndex() const
{
    return ifIndex_;
}

//! Return the SNMP interface type.
inline unsigned int CapDevice::ifType() const
{
    return ifType_;
}

//! Return the device net mask. If the device has more than one
//! address, return the net mask with given index (0..numAddrs()-1).
inline unsigned int CapDevice::netMask(size_t index) const
{
    return netMask_[index];
}

//! Return the number of addresses this device is associated with.
inline unsigned int CapDevice::numAddrs() const
{
    return numAddrs_;
}

//! Return the interface speed in bits per second.
inline unsigned long long CapDevice::ifSpeed() const
{
    return ifSpeed_;
}

inline void CapDevice::age()
{
    isOld_ = 1;
}

inline CapDevice::Stat::Stat(const CapDevice& device)
{
    device.getStat(*this);
}

inline unsigned long long CapDevice::Stat::inBytes() const
{
    return inBytes_;
}

inline unsigned long long CapDevice::Stat::inPkts() const
{
    return inPkts_;
}

inline unsigned long long CapDevice::Stat::outBytes() const
{
    return outBytes_;
}

inline unsigned long long CapDevice::Stat::outPkts() const
{
    return outPkts_;
}

inline void CapDevice::Stat::reset(const CapDevice& device)
{
    device.getStat(*this);
}

END_NAMESPACE1

#include "netkit/win/link-with-netkit.h"
#endif
