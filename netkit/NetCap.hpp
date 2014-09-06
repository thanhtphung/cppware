/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_NET_CAP_HPP
#define NETKIT_NET_CAP_HPP

#include "netkit/CapConfig.hpp"
#include "syskit/SpinSection.hpp"
#include "syskit/macros.h"

struct pcap;
struct pcap_pkthdr;

DECLARE_CLASS1(appkit, String)
DECLARE_CLASS1(syskit, Thread)

BEGIN_NAMESPACE1(netkit)

class CapDevice;
class CapFile;
class IpCap;
class RoEtherPkt;
class RoIpPkt;


//!
//! A class representing a capture of network traffic. The capture can
//! be live from a network interface card or offline from a pcap file.
//!
class NetCap
{

public:
    typedef void(*pktCb_t)(void* arg, const IpCap& cap);

    NetCap(const CapDevice* device);
    NetCap(const CapDevice* device, const CapConfig& config);
    NetCap(const CapFile* file);
    NetCap(const CapFile* file, const CapConfig& config);
    ~NetCap();

    bool isLive() const;
    bool isOk() const;
    const CapConfig& config() const;
    const CapDevice* device() const;
    const CapFile* file() const;
    const char* lastErr() const;
    const appkit::String& name() const;
    unsigned int ifIndex() const;
    unsigned int pseudoIf() const;
    unsigned long long lastCapTime() const;
    unsigned long long pktsCaptured() const;
    size_t tid() const;
    void setName(const appkit::String& name);

    bool repair(bool onlyIfBroken = true);
    void monitor(pktCb_t cb, void* arg);


    //! capture stats
    class Stat
        //!
        //! Available stats.
        //!
    {
    public:
        Stat(const NetCap& netCap);
        unsigned int pktsDropped() const;
        unsigned int pktsFiltered() const;
        unsigned long long bytesCaptured() const;
        unsigned long long bytesDropped() const;
        unsigned long long pktsCaptured() const;
        void reset(const NetCap& netCap);
    private:
        unsigned int pktsDropped_;
        unsigned int pktsFiltered_;
        unsigned long long bytesCaptured_;
        unsigned long long bytesDropped_;
        unsigned long long pktsCaptured_;
        Stat(const Stat&); //prohibit usage
        const Stat& operator =(const Stat&); //prohibit usage
    };

private:

    // Available stats.
    // statSs_ synchronizes updates to bytesCaptured_, bytesDropped_, and pktsCaptured_.
    mutable syskit::SpinSection statSs_;
    unsigned long long bytesCaptured_;
    unsigned long long bytesDropped_;
    unsigned long long lastCapTime_;
    unsigned long long pktsCaptured_;

    // Observer support.
    pktCb_t cb_;
    void* cbArg_;

    // Capturing device.
    CapConfig config_;
    CapDevice* device_;
    char* pcapErr_;
    const CapFile* file_;
    struct pcap* pcap_;
    syskit::Thread* agent_;
    unsigned int ifIndex_;
    unsigned int pseudoIf_;

    NetCap(const NetCap&); //prohibit usage
    const NetCap& operator =(const NetCap&); //prohibit usage

    bool adjustBufs();
    bool openDevice();
    bool setFilter();
    void construct(pktCb_t, void*);
    void runLive(const CapDevice*);
    void runOffline(const CapFile*);
    void startPcap();
    void stopPcap();

    static void noOp(void*, const IpCap&);
    static void onCapture(unsigned char*, const struct pcap_pkthdr*, const unsigned char*);
    static void onReplay(unsigned char*, const struct pcap_pkthdr*, const unsigned char*);
    static void* entryFunc(void*);

};

END_NAMESPACE1

#include "syskit/Thread.hpp"

BEGIN_NAMESPACE1(netkit)

//! Return true if this is a live capture.
inline bool NetCap::isLive() const
{
    bool live = (device_ != 0);
    return live;
}

//! Return true if instance was successfully constructed.
//! Use lastErr() to obtain failure details.
inline bool NetCap::isOk() const
{
    return (pcap_ != 0);
}

//! Return the capturing device configurable characteristics.
inline const CapConfig& NetCap::config() const
{
    return config_;
}

//! Return the associated live capturing device.
//! Return zero if none (offline capturing).
inline const CapDevice* NetCap::device() const
{
    return device_;
}

//! Return the associated offline capturing device.
//! Return zero if none (live capturing)
inline const CapFile* NetCap::file() const
{
    return file_;
}

//! Return the last error as a null-terminated string.
inline const char* NetCap::lastErr() const
{
    return pcapErr_;
}

//! Return the capture name.
inline const appkit::String& NetCap::name() const
{
    return config_.name();
}

//! Return the SNMP interface index.
inline unsigned int NetCap::ifIndex() const
{
    return ifIndex_;
}

//! Return a pseudo interface index. This number is formed using parts of the
//! capture device's MAC address and does not change. The SNMP interface index
//! on windows can change when the capture device is reconfigured.
inline unsigned int NetCap::pseudoIf() const
{
    return pseudoIf_;
}

//! Return the timestamp of the last captured packet.
inline unsigned long long NetCap::lastCapTime() const
{
    return lastCapTime_;
}

//! Return the number of packets captured by NetCap.
inline unsigned long long NetCap::pktsCaptured() const
{
    return pktsCaptured_;
}

//! Return the thread ID of the dedicated packet capturer thread.
inline size_t NetCap::tid() const
{
    size_t id = (agent_ != 0)? agent_->id(): syskit::Thread::INVALID_ID;
    return id;
}

//! Rename this capture.
inline void NetCap::setName(const appkit::String& name)
{
    config_.setName(name);
}

//! Construct instance w/ stats from given capture.
inline NetCap::Stat::Stat(const NetCap& netCap)
{
    reset(netCap);
}

//! Return the number of packets dropped by kernel.
inline unsigned int NetCap::Stat::pktsDropped() const
{
    return pktsDropped_;
}

//! Return the number of packets filtered by kernel.
//! Filtered packets are then captured by NetCap.
inline unsigned int NetCap::Stat::pktsFiltered() const
{
    return pktsFiltered_;
}

//! Return the number of bytes captured by NetCap.
inline unsigned long long NetCap::Stat::bytesCaptured() const
{
    return bytesCaptured_;
}

//! Return the number of bytes dropped by NetCap.
inline unsigned long long NetCap::Stat::bytesDropped() const
{
    return bytesDropped_;
}

//! Return the number of packets captured by NetCap.
inline unsigned long long NetCap::Stat::pktsCaptured() const
{
    return pktsCaptured_;
}

END_NAMESPACE1

#endif
