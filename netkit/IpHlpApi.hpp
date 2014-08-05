/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_IP_HLP_API_HPP
#define NETKIT_IP_HLP_API_HPP

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifdef htonl
#undef htonl
#endif
#ifdef htonll
#undef htonll
#endif
#ifdef htons
#undef htons
#endif
#ifdef ntohl
#undef ntohl
#endif
#ifdef ntohll
#undef ntohll
#endif
#ifdef ntohs
#undef ntohs
#endif
#include <winsock2.h>
#include <iphlpapi.h>
#include "syskit/RefCounted.hpp"
#include "syskit/macros.h"

struct _MIB_IF_ROW2;
struct _MIB_IF_TABLE2;

typedef DWORD(WINAPI *GetExtendedUdpTable_t)(PVOID, PDWORD, BOOL, ULONG, UDP_TABLE_CLASS, ULONG);
typedef DWORD(WINAPI *GetIfEntry_t)(PMIB_IFROW);
typedef DWORD(WINAPI *GetIfEntry2_t)(struct _MIB_IF_ROW2*);
typedef DWORD(WINAPI *GetIfTable_t)(PMIB_IFTABLE, PULONG, BOOL);
typedef DWORD(WINAPI *GetIfTable2_t)(struct _MIB_IF_TABLE2**);
typedef DWORD(WINAPI *NotifyAddrChange_t)(PHANDLE, LPOVERLAPPED);
typedef ULONG(WINAPI *GetAdaptersAddresses_t)(ULONG, ULONG, PVOID, PIP_ADAPTER_ADDRESSES, PULONG);
typedef VOID(WINAPI *FreeMibTable_t)(PVOID);

BEGIN_NAMESPACE1(netkit)


class IpHlpApi: public syskit::RefCounted
{

public:
    FreeMibTable_t FreeMibTable() const;
    GetAdaptersAddresses_t GetAdaptersAddresses() const;
    GetExtendedUdpTable_t GetExtendedUdpTable() const;
    GetIfEntry_t GetIfEntry() const;
    GetIfEntry2_t GetIfEntry2() const;
    GetIfTable_t GetIfTable() const;
    GetIfTable2_t GetIfTable2() const;
    NotifyAddrChange_t NotifyAddrChange() const;
    bool isOk() const;
    void bePrimitive(bool primitive = true);

    static IpHlpApi& instance();

protected:
    virtual ~IpHlpApi();

private:
    enum
    {
        FreeMibTableI = 0,
        GetAdaptersAddressesI,
        GetExtendedUdpTableI,
        GetIfEntryI,
        GetIfEntry2I,
        GetIfTableI,
        GetIfTable2I,
        NotifyAddrChangeI,

        NumProcs = NotifyAddrChangeI + 1
    };

    HMODULE lib_;
    bool primitive_;
    void* procAddr_[NumProcs];
    void* savedAddr_[NumProcs];

    static const IpHlpApi* instance_;

    IpHlpApi();
    IpHlpApi(const IpHlpApi&); //prohibit usage
    const IpHlpApi& operator =(const IpHlpApi&); //prohibit usage

};

inline bool IpHlpApi::isOk() const
{
    bool ok = (lib_ != 0);
    return ok;
}

inline GetAdaptersAddresses_t IpHlpApi::GetAdaptersAddresses() const
{
    return static_cast<GetAdaptersAddresses_t>(procAddr_[GetAdaptersAddressesI]);
}

inline FreeMibTable_t IpHlpApi::FreeMibTable() const
{
    return static_cast<FreeMibTable_t>(procAddr_[FreeMibTableI]);
}

inline GetExtendedUdpTable_t IpHlpApi::GetExtendedUdpTable() const
{
    return static_cast<GetExtendedUdpTable_t>(procAddr_[GetExtendedUdpTableI]);
}

inline GetIfEntry_t IpHlpApi::GetIfEntry() const
{
    return static_cast<GetIfEntry_t>(procAddr_[GetIfEntryI]);
}

inline GetIfEntry2_t IpHlpApi::GetIfEntry2() const
{
    return static_cast<GetIfEntry2_t>(procAddr_[GetIfEntry2I]);
}

inline GetIfTable_t IpHlpApi::GetIfTable() const
{
    return static_cast<GetIfTable_t>(procAddr_[GetIfTableI]);
}

inline GetIfTable2_t IpHlpApi::GetIfTable2() const
{
    return static_cast<GetIfTable2_t>(procAddr_[GetIfTable2I]);
}

inline NotifyAddrChange_t IpHlpApi::NotifyAddrChange() const
{
    return static_cast<NotifyAddrChange_t>(procAddr_[NotifyAddrChangeI]);
}

END_NAMESPACE1

#endif
