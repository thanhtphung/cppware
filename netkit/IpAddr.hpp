/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_IP_ADDR_HPP
#define NETKIT_IP_ADDR_HPP

#include "syskit/macros.h"

DECLARE_CLASS1(appkit, String)

BEGIN_NAMESPACE1(netkit)


//! IPv4 address
class IpAddr
    //!
    //! A class representing an IPv4 address.
    //!
{

public:
    enum
    {
        StrLength = 15 //xxx.xxx.xxx.xxx
    };

    IpAddr(const IpAddr& addr);
    IpAddr(const char* addr);
    IpAddr(unsigned int addr = 0U);

    operator appkit::String() const;
    operator unsigned int() const;
    const IpAddr& operator =(const IpAddr& addr);
    const IpAddr& operator =(unsigned int addr);

    appkit::String toString() const;
    char* toString(char s[StrLength + 1]) const;
    unsigned int asU32() const;
    unsigned int toDigits(char s[StrLength]) const;

    bool reset(const char* addr);

    static bool toU8(unsigned char addr[4], const char* s);

private:
    unsigned int addr_;

};

END_NAMESPACE1

#include "appkit/String.hpp"

BEGIN_NAMESPACE1(netkit)

inline IpAddr::IpAddr(const IpAddr& addr)
{
    addr_ = addr.addr_;
}

//! Construct instance from given address.
//! Use zeroes ("0.0.0.0") if given address is invalid.
inline IpAddr::IpAddr(const char* addr)
{
    addr_ = 0;
    reset(addr);
}

inline IpAddr::IpAddr(unsigned int addr)
{
    addr_ = addr;
}

//! Return string form.
//! For example, 0xac101001UL -> "172.16.16.1".
inline IpAddr::operator appkit::String() const
{
    char s[StrLength];
    size_t n = toDigits(s);
    return appkit::String(s, n);
}

inline IpAddr::operator unsigned int() const
{
    return addr_;
}

inline const IpAddr& IpAddr::operator =(const IpAddr& addr)
{
    addr_ = addr.addr_;
    return *this;
}

inline const IpAddr& IpAddr::operator =(unsigned int addr)
{
    addr_ = addr;
    return *this;
}

//! Convert instance to null-terminated string s and return s.
//! For example, 0xac101001UL -> "172.16.16.1".
inline char* IpAddr::toString(char s[StrLength + 1]) const
{
    size_t n = toDigits(s);
    s[n] = 0;
    return s;
}

//! Return string form.
//! For example, 0xac101001UL -> "172.16.16.1".
inline appkit::String IpAddr::toString() const
{
    char s[StrLength];
    size_t n = toDigits(s);
    return appkit::String(s, n);
}

inline unsigned int IpAddr::asU32() const
{
    return addr_;
}

END_NAMESPACE1

#endif
