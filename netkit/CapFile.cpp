/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>

#include "netkit-pch.h"
#include <pcap.h>
#include "netkit/CapFile.hpp"

#if _DEBUG
#pragma comment(lib,"packetd")
#pragma comment(lib,"wpcapd")
#else
#pragma comment(lib,"packet")
#pragma comment(lib,"wpcap")
#endif

BEGIN_NAMESPACE1(netkit)


CapFile::CapFile(const char* path)
{
    pcapErr_ = new char[PCAP_ERRBUF_SIZE];
    pcapErr_[0] = 0;
    pcap_ = pcap_open_offline(path, pcapErr_);
}


CapFile::~CapFile()
{
    if (pcap_ != 0)
    {
        pcap_close(pcap_);
    }

    delete[] pcapErr_;
}

END_NAMESPACE1
