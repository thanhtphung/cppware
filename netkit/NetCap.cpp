/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>

#include "syskit/Process.hpp"
#include "syskit/Thread.hpp"

#include "netkit-pch.h"
#include <pcap.h>
#include "netkit/CapDevice.hpp"
#include "netkit/CapFile.hpp"
#include "netkit/IpCap.hpp"
#include "netkit/NetCap.hpp"
#include "netkit/RoEtherPkt.hpp"
#include "netkit/RoIpPkt.hpp"

#if _DEBUG
#pragma comment(lib,"packetd")
#pragma comment(lib,"wpcapd")
#else
#pragma comment(lib,"packet")
#pragma comment(lib,"wpcap")
#endif

using namespace syskit;

BEGIN_NAMESPACE1(netkit)


//!
//! Construct a live network capture for given device.
//! Use default configuration.
//!
NetCap::NetCap(const CapDevice* device):
statSs_(),
config_()
{
    // Open capturing device.
    // Start capturing.
    construct(noOp, 0);
    runLive(device);
}


//!
//! Construct a live network capture for given device and configuration.
//!
NetCap::NetCap(const CapDevice* device, const CapConfig& config):
statSs_(),
config_(config)
{
    // Open capturing device.
    // Start capturing.
    construct(noOp, 0);
    runLive(device);
}


//!
//! Construct an offline network capture from given file.
//! Use default configuration.
//!
NetCap::NetCap(const CapFile* file):
statSs_(),
config_()
{
    // Open capturing device.
    // Start capturing.
    construct(0, 0);
    runOffline(file);
}


//!
//! Construct an offline network capture from given file and configuration.
//!
NetCap::NetCap(const CapFile* file, const CapConfig& config):
statSs_(),
config_(config)
{
    // Open capturing device.
    // Start capturing.
    construct(0, 0);
    runOffline(file);
}


NetCap::~NetCap()
{
    stopPcap();
    delete device_;
    delete[] pcapErr_;
}


bool NetCap::openDevice()
{
    // Assume failure.
    bool ok = false;

    // Open capturing device.
    const char* name = device_->name();
    int capLength = config_.capLength();
    int loopTimeoutInMsecs = config_.loopTimeout();
    int promiscuous = config_.bePromiscuous()? 1: 0;
    pcap_ = pcap_open_live(name, capLength, promiscuous, loopTimeoutInMsecs, pcapErr_);
    if (pcap_ == 0)
    {
        return ok;
    }

    // Some sanity check.
    if (pcap_datalink(pcap_) != DLT_EN10MB)
    {
        pcap_close(pcap_);
        pcap_ = 0;
        return ok;
    }

    // Return true if successful.
    ok = true;
    return ok;
}


//!
//! Repair the instance (if broken). Return true if instance was repaired. An
//! instance can be broken if some NIC was reconfigured. The repair, if any,
//! is performed by reinitializing pcap and restarting the dedicated capturer
//! thread.
//!
bool NetCap::repair(bool onlyIfBroken)
{
    // Not live?
    bool repaired = false;
    if (device_ == 0)
    {
        return repaired;
    }

    // No important change?
    const CapDevice* cur = CapDevice::find(device_->macAddr());
    if (onlyIfBroken && ((cur == 0) || (cur->ifIndex() == ifIndex_)))
    {
        return repaired;
    }

    // Refresh and restart the capturer thread.
    *device_ = *cur;
    ifIndex_ = device_->ifIndex();
    stopPcap();
    if (openDevice())
    {
        startPcap();
    }

    repaired = true;
    return repaired;
}


bool NetCap::setFilter()
{
    // Must capture either incoming packets, outgoing packets, or both.
    bool ok = false;
    pcap_direction_t newDir;
    if (!config_.capIcPkts())
    {
        if (!config_.capOgPkts())
        {
            memcpy(pcapErr_, "illogical configuration", sizeof("illogical configuration"));
            return ok;
        }
        newDir = PCAP_D_OUT;
    }
    else if (config_.capOgPkts())
    {
        newDir = PCAP_D_INOUT;
    }
    else
    {
        newDir = PCAP_D_IN;
    }

    // Update filter. Assume we won't be dealing with broadcast
    // addresses in filters, so don't worry about the net mask
    // parameter.
    const char* filter = config_.filter();
    size_t size = strlen(filter) + 1;
    char* newFilter = new char[size];
    memcpy(newFilter, filter, size);
    struct bpf_program prog;
    if (pcap_compile(pcap_, &prog, newFilter, 1 /*optimize*/, 0 /*netmask*/) == 0)
    {
        if ((pcap_setfilter(pcap_, &prog) == 0) && (pcap_setdirection(pcap_, newDir) == 0))
        {
            ok = true;
        }
        pcap_freecode(&prog);
    }

    // Clarify failure.
    if (!ok)
    {
        const char* err = pcap_geterr(pcap_);
        size_t length = strlen(err);
        memcpy(pcapErr_, err, length + 1);
    }

    // Return true if successful.
    delete[] newFilter;
    return ok;
}


void NetCap::construct(pktCb_t cb, void* arg)
{
    // Available stats.
    bytesCaptured_ = 0ULL;
    bytesDropped_ = 0ULL;
    lastCapTime_ = 0ULL;
    pktsCaptured_ = 0ULL;

    agent_ = 0;
    cb_ = cb;
    cbArg_ = arg;
    pcap_ = 0;
    pcapErr_ = new char[PCAP_ERRBUF_SIZE];
    pcapErr_[0] = 0;
}


void NetCap::startPcap()
{
    // Apply filter.
    // Adjust socket buffer sizes.
    // Create capturer thread.
    bool ok = setFilter();
    if (ok)
    {
        adjustBufs();
        void* arg = this;
        unsigned int stackSizeInBytes = config_.agentStackSize();
        agent_ = new Thread(entryFunc, arg, stackSizeInBytes);
        ok = agent_->isOk();
        if (!ok)
        {
            delete agent_;
            agent_ = 0;
        }
    }

    // Give up if failed to apply filter or create capturer thread.
    if (!ok)
    {
        pcap_close(pcap_);
        pcap_ = 0;
    }
}


void NetCap::stopPcap()
{
    if (pcap_ != 0)
    {

        // Kill capturer thread.
        for (; agent_->state() == Thread::Idle; Thread::yield());
        pcap_breakloop(pcap_);
        agent_->kill();
        agent_->waitTilDone();
        delete agent_;
        agent_ = 0;

        // Close the capturing device.
        if (device_ != 0)
        {
            pcap_close(pcap_);
            pcap_ = 0;
        }
    }
}


//!
//! Monitor network capture for IPv4 frames. For each IPv4 frame, invoke
//! given callback with given opaque argument.
//!
void NetCap::monitor(pktCb_t cb, void* arg)
{
    cb_ = (cb == 0)? noOp: cb;
    cbArg_ = arg;

    if (file_ != 0)
    {
        for (; agent_->state() == Thread::Idle; Thread::yield());
        agent_->waitTilDone();
    }
}


void NetCap::noOp(void* /*arg*/, const IpCap& /*cap*/)
{
}


//
// Invoked by the dedicated capturer thread when a packet is captured.
// Notify monitor of captured packets via registered callback.
//
void NetCap::onCapture(unsigned char* arg, const struct pcap_pkthdr* hdr, const unsigned char* pkt)
{
    // Collect some minimal stats.
    const unsigned long long USECS_PER_SEC = 1000000ULL;
    unsigned long long capTime = hdr->ts.tv_sec * USECS_PER_SEC + hdr->ts.tv_usec;
    NetCap* netCap = reinterpret_cast<NetCap*>(arg);
    {
        SpinSection::Lock lock(netCap->statSs_);
        ++(netCap->pktsCaptured_);
        netCap->bytesCaptured_ += hdr->caplen;
        netCap->bytesDropped_ += (hdr->len - hdr->caplen);
        netCap->lastCapTime_ = capTime;
    }

    // Not enough bytes?
    // Ethernet header available?
    bool makeCopy = false;
    RoEtherPkt etherPkt(pkt, hdr->len, makeCopy);
    if (!etherPkt.isOk())
    {
        return;
    }

    // IPv4 only.
    RoIpPkt ipPkt(etherPkt, makeCopy);
    if ((!ipPkt.isOk()) || (ipPkt.version() != 4U))
    {
        return;
    }

    IpCap cap(etherPkt, ipPkt, netCap->ifIndex_, capTime);
    netCap->cb_(netCap->cbArg_, cap);
}


//
// Invoked by the dedicated capturer thread when a packet is replayed.
// Notify monitor of captured packets via registered callback.
//
void NetCap::onReplay(unsigned char* arg, const struct pcap_pkthdr* hdr, const unsigned char* pkt)
{
    // Mimic frame truncation in offline mode.
    const NetCap* netCap = reinterpret_cast<const NetCap*>(arg);
    unsigned int limit = netCap->config_.capLength();
    if (hdr->caplen > limit)
    {
        struct pcap_pkthdr h(*hdr);
        h.caplen = limit;
        onCapture(arg, &h, pkt);
    }
    else
    {
        onCapture(arg, hdr, pkt);
    }
}


void NetCap::runLive(const CapDevice* device)
{
    device_ = new CapDevice(*device);
    file_ = 0;
    ifIndex_ = device_->ifIndex();
    const unsigned char* macAddr = device->macAddr();
    pseudoIf_ = (macAddr[4] << 8UL) | macAddr[5];
    if (openDevice())
    {
        startPcap();
    }
}


void NetCap::runOffline(const CapFile* file)
{
    device_ = 0;
    file_ = file;
    ifIndex_ = 0;
    pseudoIf_ = 0;
    pcap_ = file_->handle();
    if (pcap_ != 0)
    {
        startPcap();
    }
}


//
// Entry point for the dedicated capturer thread. For live capturing, the
// thread runs immediately and packets are seen in callbacks as soon as they
// are monitored. For offline capturing, the thread waits until packets are
// monitored before running.
//
void* NetCap::entryFunc(void* arg)
{
    NetCap* netCap = static_cast<NetCap*>(arg);
    pcap_handler cb;
    if (netCap->file_ == 0)
    {
        cb = onCapture;
    }
    else
    {
        for (; netCap->cb_ == 0; Thread::yield());
        cb = onReplay;
    }

    int cnt = 0;
    int rc = pcap_loop(netCap->pcap_, cnt, cb, reinterpret_cast<unsigned char*>(netCap));
    if (rc == -1) //some error
    {
        const char* err = pcap_geterr(netCap->pcap_);
        size_t length = strlen(err);
        memcpy(netCap->pcapErr_, err, length + 1);
    }

    return 0;
}


//!
//! Reset instance w/ stats from given capture.
//!
void NetCap::Stat::reset(const NetCap& netCap)
{
    struct pcap_stat stat = {0};
    pcap_stats(netCap.pcap_, &stat);
    pktsDropped_ = stat.ps_drop;
    pktsFiltered_ = stat.ps_recv;

    SpinSection::Lock lock(netCap.statSs_);
    bytesCaptured_ = netCap.bytesCaptured_;
    bytesDropped_ = netCap.bytesDropped_;
    pktsCaptured_ = netCap.pktsCaptured_;
}

END_NAMESPACE1
