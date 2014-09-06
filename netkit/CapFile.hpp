/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_CAP_FILE_HPP
#define NETKIT_CAP_FILE_HPP

#include "syskit/macros.h"

struct pcap;

BEGIN_NAMESPACE1(netkit)


//! network capture offline dump
class CapFile
{

public:
    CapFile(const char* path);
    ~CapFile();

    bool isOk() const;
    const char* lastErr() const;
    struct pcap* handle() const;

private:
    char* pcapErr_;
    struct pcap* pcap_;

    CapFile(const CapFile&); //prohibit usage
    const CapFile& operator =(const CapFile&); //prohibit usage

};

//! Return true if instance was successfully constructed.
//! Use lastErr() to obtain failure details.
inline bool CapFile::isOk() const
{
    bool ok = (pcap_ != 0);
    return ok;
}

//! Return the last error as a null-terminated string.
inline const char* CapFile::lastErr() const
{
    return pcapErr_;
}

inline struct pcap* CapFile::handle() const
{
    return pcap_;
}

END_NAMESPACE1

#endif
