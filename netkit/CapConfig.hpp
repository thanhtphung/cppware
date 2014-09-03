/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_CAP_CONFIG_HPP
#define NETKIT_CAP_CONFIG_HPP

#include "appkit/String.hpp"
#include "syskit/macros.h"

DECLARE_CLASS1(appkit, CmdLine)

BEGIN_NAMESPACE1(netkit)


//! network capture configurable characteristics
class CapConfig
{

public:
    CapConfig();
    CapConfig(const CapConfig& config);
    CapConfig(const appkit::CmdLine& cmdLine);
    ~CapConfig();

    const CapConfig& operator =(const CapConfig& config);

    bool bePromiscuous() const;
    bool capIcPkts() const;
    bool capOgPkts() const;
    const char* filter() const;
    const appkit::String& name() const;
    appkit::String asString(bool includeFilter = true, bool includeName = true) const;
    int bufSize() const;
    int capLength() const;
    int loopTimeout() const;
    unsigned int agentStackSize() const;
    void reset();
    void reset(const appkit::CmdLine& cmdLine);
    void setName(const appkit::String& name);

private:
    bool bePromiscuous_;
    bool capIcPkts_;
    bool capOgPkts_;
    const char* filter_;
    int bufSize_;
    int capLength_;
    int loopTimeout_;
    unsigned int agentStackSize_;

    appkit::String name_;

    void copyFrom(const CapConfig&);

};

inline const CapConfig& CapConfig::operator =(const CapConfig& config)
{
    if (this != &config) copyFrom(config);
    return *this;
}

//! Return the capture name.
inline const appkit::String& CapConfig::name() const
{
    return name_;
}

//! Return true if capturing is to be done in promiscuous mode.
inline bool CapConfig::bePromiscuous() const
{
    return bePromiscuous_;
}

//! Return true if incoming packets are to be captured.
inline bool CapConfig::capIcPkts() const
{
    return capIcPkts_;
}

//! Return true if outgoing packets are to be captured.
inline bool CapConfig::capOgPkts() const
{
    return capOgPkts_;
}

//! Return the capturing filter. This filter controls the types
//! of packets filtered by the kernel. Filtered packets are then
//! captured by NetCap.
inline const char* CapConfig::filter() const
{
    return filter_;
}

//! Return the buffer size in bytes used in pcap to queue captured frames.
//! Frames can be dropped if captured frames are not processed timely filling
//! up the queue.
inline int CapConfig::bufSize() const
{
    return bufSize_;
}

//! Return the maximum capture length in bytes. Captured packets longer
//! than this limit are truncated when filtered by the kernel.
inline int CapConfig::capLength() const
{
    return capLength_;
}

inline int CapConfig::loopTimeout() const
{
    return loopTimeout_;
}

//! Return the desired agent stack size in bytes. Each NetCap instance comes
//! with an agent thread running callbacks presenting captured frames.
inline unsigned int CapConfig::agentStackSize() const
{
    return agentStackSize_;
}

//! Rename this capture.
inline void CapConfig::setName(const appkit::String& name)
{
    name_ = name;
}

END_NAMESPACE1

#include "netkit/win/link-with-netkit.h"
#endif
