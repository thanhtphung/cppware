/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>

#include "appkit/Str.hpp"
#include "appkit/U16.hpp"
#include "appkit/U32.hpp"
#include "appkit/U8.hpp"
#include "syskit/Foundation.hpp"
#include "syskit/sys.hpp"

#include "netkit-pch.h"

#include <pcap.h>
#ifdef strdup
#undef strdup
#endif

#include "netkit/CapDevice.hpp"
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

//
// Snapshot from <netioapi.h> to support GetIfEntry2() at run-time on vista and later.
//

//
// Medium the Ndis Driver is running on (OID_GEN_MEDIA_SUPPORTED/ OID_GEN_MEDIA_IN_USE).
//
typedef enum _NDIS_MEDIUM
{
    NdisMedium802_3,
    NdisMedium802_5,
    NdisMediumFddi,
    NdisMediumWan,
    NdisMediumLocalTalk,
    NdisMediumDix,              // defined for convenience, not a real medium
    NdisMediumArcnetRaw,
    NdisMediumArcnet878_2,
    NdisMediumAtm,
    NdisMediumWirelessWan,
    NdisMediumIrda,
    NdisMediumBpc,
    NdisMediumCoWan,
    NdisMedium1394,
    NdisMediumInfiniBand,
#if ((NTDDI_VERSION >= NTDDI_VISTA) || NDIS_SUPPORT_NDIS6)
    NdisMediumTunnel,
    NdisMediumNative802_11,
    NdisMediumLoopback,
#endif // (NTDDI_VERSION >= NTDDI_VISTA)

#if (NTDDI_VERSION >= NTDDI_WIN7)
    NdisMediumWiMAX,
    NdisMediumIP,
#endif

    NdisMediumMax               // Not a real medium, defined as an upper-bound
} NDIS_MEDIUM, *PNDIS_MEDIUM;

//
// Physical Medium Type definitions. Used with OID_GEN_PHYSICAL_MEDIUM.
//
typedef enum _NDIS_PHYSICAL_MEDIUM
{
    NdisPhysicalMediumUnspecified,
    NdisPhysicalMediumWirelessLan,
    NdisPhysicalMediumCableModem,
    NdisPhysicalMediumPhoneLine,
    NdisPhysicalMediumPowerLine,
    NdisPhysicalMediumDSL,      // includes ADSL and UADSL (G.Lite)
    NdisPhysicalMediumFibreChannel,
    NdisPhysicalMedium1394,
    NdisPhysicalMediumWirelessWan,
    NdisPhysicalMediumNative802_11,
    NdisPhysicalMediumBluetooth,
    NdisPhysicalMediumInfiniband,
    NdisPhysicalMediumWiMax,
    NdisPhysicalMediumUWB,
    NdisPhysicalMedium802_3,
    NdisPhysicalMedium802_5,
    NdisPhysicalMediumIrda,
    NdisPhysicalMediumWiredWAN,
    NdisPhysicalMediumWiredCoWan,
    NdisPhysicalMediumOther,
    NdisPhysicalMediumMax       // Not a real physical type, defined as an upper-bound
} NDIS_PHYSICAL_MEDIUM, *PNDIS_PHYSICAL_MEDIUM;

typedef struct _MIB_IF_ROW2
{
    //
    // Key structure.  Sorted by preference.
    //
    NET_LUID InterfaceLuid;
    NET_IFINDEX InterfaceIndex;

    //
    // Read-Only fields.
    //
    GUID InterfaceGuid;
    WCHAR Alias[IF_MAX_STRING_SIZE + 1];
    WCHAR Description[IF_MAX_STRING_SIZE + 1];
    ULONG PhysicalAddressLength;
    UCHAR PhysicalAddress[IF_MAX_PHYS_ADDRESS_LENGTH];
    UCHAR PermanentPhysicalAddress[IF_MAX_PHYS_ADDRESS_LENGTH];

    ULONG Mtu;
    IFTYPE Type;                // Interface Type.
    TUNNEL_TYPE TunnelType;     // Tunnel Type, if Type = IF_TUNNEL.
    NDIS_MEDIUM MediaType;
    NDIS_PHYSICAL_MEDIUM PhysicalMediumType;
    NET_IF_ACCESS_TYPE AccessType;
    NET_IF_DIRECTION_TYPE DirectionType;
    struct
    {
        BOOLEAN HardwareInterface : 1;
        BOOLEAN FilterInterface : 1;
        BOOLEAN ConnectorPresent : 1;
        BOOLEAN NotAuthenticated : 1;
        BOOLEAN NotMediaConnected : 1;
        BOOLEAN Paused : 1;
        BOOLEAN LowPower : 1;
        BOOLEAN EndPointInterface : 1;
    } InterfaceAndOperStatusFlags;

    IF_OPER_STATUS OperStatus;
    NET_IF_ADMIN_STATUS AdminStatus;
    NET_IF_MEDIA_CONNECT_STATE MediaConnectState;
    NET_IF_NETWORK_GUID NetworkGuid;
    NET_IF_CONNECTION_TYPE ConnectionType;

    //
    // Statistics.
    //
    ULONG64 TransmitLinkSpeed;
    ULONG64 ReceiveLinkSpeed;

    ULONG64 InOctets;
    ULONG64 InUcastPkts;
    ULONG64 InNUcastPkts;
    ULONG64 InDiscards;
    ULONG64 InErrors;
    ULONG64 InUnknownProtos;
    ULONG64 InUcastOctets;
    ULONG64 InMulticastOctets;
    ULONG64 InBroadcastOctets;
    ULONG64 OutOctets;
    ULONG64 OutUcastPkts;
    ULONG64 OutNUcastPkts;
    ULONG64 OutDiscards;
    ULONG64 OutErrors;
    ULONG64 OutUcastOctets;
    ULONG64 OutMulticastOctets;
    ULONG64 OutBroadcastOctets;
    ULONG64 OutQLen;
} MIB_IF_ROW2, *PMIB_IF_ROW2;

typedef struct _MIB_IF_TABLE2
{
    ULONG NumEntries;
    MIB_IF_ROW2 Table[ANY_SIZE];
} MIB_IF_TABLE2, *PMIB_IF_TABLE2;


//
// Mimic <NtDDNdis.h> from Microsoft SDK and <Packet32.h> from winpcap
// to get MAC addresses from the NICs.
//
const unsigned int OID_802_3_CURRENT_ADDRESS = 0x01010102UL;

typedef struct
{
    unsigned int oid;
    unsigned int length;
    unsigned char data[1];
} pktReq_t;

extern "C" HANDLE* PacketOpenAdapter(const char* adapterName);
extern "C" unsigned char PacketRequest(void* adapter, unsigned char doSet, void* req);
extern "C" void PacketCloseAdapter(void* adapter);

using namespace appkit;

BEGIN_NAMESPACE1(netkit)

// Public winpcap uses "\\Device\\NPF_" as device name prefix.
// cppgallery winpcap uses "\\Device\\NETCAP_" as device name prefix.
const char* s_devicePrefix = "\\Device\\NETCAP_"; //or "\\Device\\NPF_"


//
// Retrieve MAC addresses matching capture devices given by pcap.
// Returned array is to be destroyed when done.
//
static MacAddr* getIfMacs(const pcap_if_t* pcapIf)
{
    unsigned int count = 0;
    for (const pcap_if_t* p = pcapIf; p != 0; count++, p = p->next);

    MacAddr* macs = new MacAddr[count];
    const pcap_if_t* p = pcapIf;
    for (unsigned int i = 0; i < count; ++i, p = p->next)
    {
        HANDLE* a = PacketOpenAdapter(p->name);
        if (a != 0)
        {
            if (*a != INVALID_HANDLE_VALUE)
            {
                char buf[offsetof(pktReq_t, data) + MacAddr::RawLength];
                pktReq_t* req = reinterpret_cast<pktReq_t*>(buf);
                req->oid = OID_802_3_CURRENT_ADDRESS;
                req->length = MacAddr::RawLength;
                memset(req->data, 0, MacAddr::RawLength);
                unsigned char doSet = 0;
                unsigned char ok = PacketRequest(a, doSet, req);
                if (ok)
                {
                    macs[i] = req->data;
                }
            }
            PacketCloseAdapter(a);
        }
    }

    return macs;
}


//
// Secondary API to enumerate the interfaces when GetIfTable2() is unavailable.
//
static MIB_IFTABLE* getIfTbl()
{
    MIB_IFTABLE* ifTbl = 0;
    const IpHlpApi& api = IpHlpApi::instance();
    GetIfTable_t GetIfTable = api.GetIfTable();

    unsigned int errCode;
    do
    {
        BOOL sortTbl = FALSE;
        ULONG bufSize = 0;
        errCode = GetIfTable(ifTbl, &bufSize, sortTbl);
        if (errCode != ERROR_INSUFFICIENT_BUFFER)
        {
            break;
        }
        ifTbl = reinterpret_cast<MIB_IFTABLE*>(new unsigned char[bufSize]);
        errCode = GetIfTable(ifTbl, &bufSize, sortTbl);
        if (errCode == NO_ERROR)
        {
            break;
        }
        delete[] reinterpret_cast<unsigned char*>(ifTbl);
        ifTbl = 0;
    } while (errCode == ERROR_INSUFFICIENT_BUFFER);

    return ifTbl;
}


//
// Primary API to enumerate the interfaces.
// However, the GetIfTable2() function is unavailable before Vista.
//
static MIB_IF_TABLE2* getIfTbl2()
{
    MIB_IF_TABLE2* ifTbl = 0;
    const IpHlpApi& api = IpHlpApi::instance();
    GetIfTable2_t GetIfTable2 = api.GetIfTable2();
    if (GetIfTable2 == 0)
    {
        return ifTbl;
    }

    GetIfTable2(&ifTbl);
    return ifTbl;
}


//
// Convert from MIB_IF_ROW2.InterfaceGuid to pcap device name.
//
static String guidToDeviceName(const GUID& guid)
{
    String deviceName(s_devicePrefix);
    char tmp[38]; //example: "{CDC1CC3F-F32A-46F9-9418-3DCE81AD1F55}"
    tmp[0] = '{';
    U32::toXDIGITS(guid.Data1, tmp + 1);
    tmp[9] = '-';
    U16::toXDIGITS(guid.Data2, tmp + 10);
    tmp[14] = '-';
    U16::toXDIGITS(guid.Data3, tmp + 15);
    tmp[19] = '-';
    U8::toXDIGITS(guid.Data4[0], tmp + 20);
    U8::toXDIGITS(guid.Data4[1], tmp + 22);
    tmp[24] = '-';
    U8::toXDIGITS(guid.Data4[2], tmp + 25);
    U8::toXDIGITS(guid.Data4[3], tmp + 27);
    U8::toXDIGITS(guid.Data4[4], tmp + 29);
    U8::toXDIGITS(guid.Data4[5], tmp + 31);
    U8::toXDIGITS(guid.Data4[6], tmp + 33);
    U8::toXDIGITS(guid.Data4[7], tmp + 35);
    tmp[37] = '}';
    deviceName.append(tmp, 38);
    return deviceName;
}


static const MIB_IFROW* findIfRow(const pcap_if_t& /*pcapIf*/, const MacAddr& ifMac, const MIB_IFTABLE* ifTbl)
{
    const MIB_IFROW* found = 0;
    for (size_t row = 0, numRows = ifTbl->dwNumEntries; row<numRows; ++row)
    {
        const MIB_IFROW* ifRow = ifTbl->table + row;
        if ((ifRow->dwPhysAddrLen == MacAddr::RawLength) && (ifMac == ifRow->bPhysAddr))
        {
            if ((found == 0) || (found->dwDescrLen > ifRow->dwDescrLen))
            {
                found = ifRow;
            }
        }
    }

    return found;
}


static const pcap_if_t* findPcapIf(const String& name, const pcap_if_t* pcapIf)
{
    const pcap_if_t* found = 0;
    for (const pcap_if_t* p = pcapIf; p != 0; p = p->next)
    {
        if (Str::compareKI(name.ascii(), p->name) == 0)
        {
            found = p;
            break;
        }
    }

    return found;
}


CapDevice::CapDevice(const char* name,
    const struct _MIB_IFROW& ifRow,
    unsigned char index,
    unsigned int numAddrs,
    const unsigned int* addr,
    const unsigned int* netMask):
    macAddr_(ifRow.bPhysAddr)
{
    getStat_ = &CapDevice::getIfStat;
    ifIndex_ = ifRow.dwIndex;
    ifSpeed_ = ifRow.dwSpeed;
    ifType_ = ifRow.dwType;
    index_ = index;
    isOld_ = 0;
    numAddrs_ = numAddrs;

    unsigned int* p = new unsigned int[numAddrs_];
    memcpy(p, addr, sizeof(*p) * numAddrs_);
    addr_ = p;

    wchar_t* w = new wchar_t[ifRow.dwDescrLen + 1];
    for (unsigned int i = 0; i < ifRow.dwDescrLen; ++i)
    {
        w[i] = ifRow.bDescr[i];
    }
    w[ifRow.dwDescrLen] = 0;
    ifDesc_ = w;
    name_ = syskit::strdup(name);

    p = new unsigned int[numAddrs_];
    memcpy(p, netMask, sizeof(*p) * numAddrs_);
    netMask_ = p;
}


CapDevice::CapDevice(const char* name,
    const struct _MIB_IF_ROW2& ifRow,
    unsigned char index,
    unsigned int numAddrs,
    const unsigned int* addr,
    const unsigned int* netMask):
    macAddr_(ifRow.PhysicalAddress)
{
    getStat_ = &CapDevice::getIfStat2;
    ifIndex_ = ifRow.InterfaceIndex;
    ifSpeed_ = ifRow.ReceiveLinkSpeed;
    ifType_ = ifRow.Type;
    index_ = index;
    isOld_ = 0;
    numAddrs_ = numAddrs;

    unsigned int* p = new unsigned int[numAddrs_];
    memcpy(p, addr, sizeof(*p) * numAddrs_);
    addr_ = p;

    ifDesc_ = syskit::strdup(ifRow.Description);
    name_ = syskit::strdup(name);

    p = new unsigned int[numAddrs_];
    memcpy(p, netMask, sizeof(*p) * numAddrs_);
    netMask_ = p;
}


bool CapDevice::getIfStat(Stat& stat) const
{
    const IpHlpApi& api = IpHlpApi::instance();
    GetIfEntry_t GetIfEntry = api.GetIfEntry();

    MIB_IFROW row = {0};
    row.dwIndex = ifIndex_;
    bool ok = (GetIfEntry(&row) == NO_ERROR);
    if (ok)
    {
        stat.inBytes_ = row.dwInOctets;
        stat.inPkts_ = static_cast<unsigned long long>(row.dwInUcastPkts) + row.dwInNUcastPkts;
        stat.outBytes_ = row.dwOutOctets;
        stat.outPkts_ = static_cast<unsigned long long>(row.dwOutUcastPkts) + row.dwOutNUcastPkts;
    }
    else
    {
        stat.inBytes_ = 0;
        stat.inPkts_ = 0;
        stat.outBytes_ = 0;
        stat.outPkts_ = 0;
    }

    return ok;
}


bool CapDevice::getIfStat2(Stat& stat) const
{
    const IpHlpApi& api = IpHlpApi::instance();
    GetIfEntry2_t GetIfEntry2 = api.GetIfEntry2();

    MIB_IF_ROW2 row = {0};
    row.InterfaceIndex = ifIndex_;
    bool ok = (GetIfEntry2(&row) == NO_ERROR);
    if (ok)
    {
        stat.inBytes_ = row.InOctets;
        stat.inPkts_ = row.InUcastPkts + row.InNUcastPkts;
        stat.outBytes_ = row.OutOctets;
        stat.outPkts_ = row.OutUcastPkts + row.OutNUcastPkts;
    }
    else
    {
        stat.inBytes_ = 0;
        stat.inPkts_ = 0;
        stat.outBytes_ = 0;
        stat.outPkts_ = 0;
    }

    return ok;
}


//
// Use pcap to get available devices.
// Correlate results with MIB_IF_TABLE2 (preferred) or MIB_IFTABLE.
//
unsigned char CapDevice::mkDevice(CapDevice* device[MaxIndex + 1])
{
    // Get available capture devices from pcap, then query the devices for the MACs.
    // Public winpcap or cppgallery winpcap?
    pcap_if_t* pcapIf = 0;
    char pcapErr[PCAP_ERRBUF_SIZE];
    pcap_findalldevs(&pcapIf, pcapErr);
    MacAddr* ifMac = getIfMacs(pcapIf);
    if (pcapIf != 0)
    {
        if (Str::compareKIN("\\Device\\NETCAP_", pcapIf->name, sizeof("\\Device\\NETCAP_") - 1) != 0)
        {
            s_devicePrefix = "\\Device\\NPF_";
        }
    }

    unsigned char numDevices;
    MIB_IF_TABLE2* ifTbl2 = getIfTbl2();
    if (ifTbl2 == 0)
    {
        const MIB_IFTABLE* ifTbl = getIfTbl();
        numDevices = mkDevice(device, pcapIf, ifMac, ifTbl);
        delete[] reinterpret_cast<const unsigned char*>(ifTbl);
    }
    else
    {
        numDevices = mkDevice(device, pcapIf, ifTbl2);
        FreeMibTable_t FreeMibTable = IpHlpApi::instance().FreeMibTable();
        FreeMibTable(ifTbl2);
    }

    delete[] ifMac;
    pcap_freealldevs(pcapIf);
    return numDevices;
}


unsigned char CapDevice::mkDevice(CapDevice* device[MaxIndex + 1],
    const struct pcap_if* pcapIf,
    const MacAddr* ifMac,
    const struct _MIB_IFTABLE* ifTbl)
{
    MIB_IFROW fake = {0};
    unsigned char numDevices = 0;

    unsigned int i = 0;
    for (const pcap_if_t* p = pcapIf; p != 0; ++i, p = p->next)
    {
        const MacAddr& macAddr = ifMac[i];
        const MIB_IFROW* found = findIfRow(*p, macAddr, ifTbl);
        if (found == 0)
        {
            fake.dwPhysAddrLen = MacAddr::RawLength;
            memcpy(fake.bPhysAddr, macAddr.asRawBytes(), MacAddr::RawLength);
            fake.dwDescrLen = static_cast<unsigned int>(strlen(p->description));
            memcpy(fake.bDescr, p->description, fake.dwDescrLen + 1);
            found = &fake;
        }

        unsigned int addr[MaxAddrIndex + 1];
        unsigned int mask[MaxAddrIndex + 1];
        unsigned int numAddrs = normalizeAddrs(p->addresses, addr, mask);
        device[numDevices] = new CapDevice(p->name, *found, numDevices, numAddrs, addr, mask);
        if (numDevices++ == MaxIndex)
        {
            break;
        }
    }

    return numDevices;
}


unsigned char CapDevice::mkDevice(CapDevice* device[MaxIndex + 1], const struct pcap_if* pcapIf, const struct _MIB_IF_TABLE2* ifTbl)
{
    String name;
    unsigned char numDevices = 0;
    for (size_t row = 0, numRows = ifTbl->NumEntries; row < numRows; ++row)
    {
        const MIB_IF_ROW2& ifRow = ifTbl->Table[row];
        if (ifRow.PhysicalAddressLength != MacAddr::RawLength)
        {
            continue;
        }

        name = guidToDeviceName(ifRow.InterfaceGuid);
        const pcap_if_t* found = findPcapIf(name, pcapIf);
        if (found == 0)
        {
            continue;
        }

        unsigned int addr[MaxAddrIndex + 1];
        unsigned int mask[MaxAddrIndex + 1];
        unsigned int numAddrs = normalizeAddrs(found->addresses, addr, mask);
        device[numDevices] = new CapDevice(name.ascii(), ifRow, numDevices, numAddrs, addr, mask);
        if (numDevices++ == MaxIndex)
        {
            break;
        }
    }

    return numDevices;
}


CapDevice::Instance::Instance()
{
    Foundation::instance().addRef();
    IpHlpApi::instance().addRef();
    numDevices_ = mkDevice(device_);
}

END_NAMESPACE1
