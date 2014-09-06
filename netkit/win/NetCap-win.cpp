#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>

#include "netkit-pch.h"
#include <pcap.h>
#include "netkit/NetCap.hpp"


//!
//! Stub. Unavailable before wpdpack-4.1.1. Exposed in wpdpack-4.1.1, but not
//! functional (err="Setting direction is not implemented on this platform").
//!
extern "C" int pcap_setdirection(pcap_t* /*p*/, pcap_direction_t /*d*/)
{
    return 0;
}


BEGIN_NAMESPACE1(netkit)


bool NetCap::adjustBufs()
{
    int bufSize = config_.bufSize();
    bool ok = (pcap_setbuff(pcap_, bufSize) == 0);
    return ok;
}

END_NAMESPACE1
