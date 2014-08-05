/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_WINSOCK_HPP
#define NETKIT_WINSOCK_HPP

#include <sys/types.h>
#include "syskit/macros.h"

BEGIN_NAMESPACE1(netkit)


//! winsock API
class Winsock
{

public:
    Winsock();
    ~Winsock();

    bool isOk() const;

    static bool getAddrInfo(unsigned int& addr, const char* nodeName);
    static bool getAddrInfo(unsigned int& addr, const char* nodeName, size_t length);
    static const Winsock& instance();

private:
    bool ok_;

    static const Winsock* instance_;

    static bool getAddrInfo(unsigned int& addr, const wchar_t*);

};

inline bool Winsock::isOk() const
{
    return ok_;
}

inline const Winsock& Winsock::instance()
{
    return *instance_;
}

END_NAMESPACE1

#include "netkit/win/link-with-netkit.h"
#endif
