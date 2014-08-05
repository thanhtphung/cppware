/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_PADDR_HPP
#define NETKIT_PADDR_HPP

#include "syskit/macros.h"

DECLARE_CLASS1(appkit, String)

BEGIN_NAMESPACE1(netkit)


//! IPv4 port address
class Paddr
    //!
    //! A class representing an IPv4 port address.
    //!
{

public:
    enum
    {
        KeyLength = 12,
        StrLength = 21 //xxx.xxx.xxx.xxx:xxxxx
    };

    Paddr(const Paddr& paddr);
    Paddr(const appkit::String* s, char delim = ':', const Paddr& defaultV = Paddr(0.0));
    Paddr(const char* s, char delim = ':');
    Paddr(double rawAddr);
    Paddr(unsigned int addr, unsigned short port);
    Paddr(unsigned long long paddr = 0ULL);

    operator appkit::String() const;
    operator unsigned long long() const;
    bool operator !=(const Paddr& paddr) const;
    bool operator <(const Paddr& paddr) const;
    bool operator <=(const Paddr& paddr) const;
    bool operator >(const Paddr& paddr) const;
    bool operator >=(const Paddr& paddr) const;
    bool operator ==(const Paddr& paddr) const;
    const Paddr& operator =(const Paddr& paddr);
    const Paddr& operator =(unsigned long long paddr);

    appkit::String toString(char delim = ':') const;
    bool reset(const char* s, char delim = ':', bool portIsOptional = false);
    char* toString(char s[StrLength + 1], char delim) const;
    double raw() const;
    unsigned int addr() const;
    unsigned int toDigits(char s[StrLength], char delim) const;
    unsigned long long asU64() const;
    unsigned long long toN64() const;
    unsigned short port() const;
    void reset();
    void reset(unsigned int addr, unsigned short port);
    void setAddr(unsigned int addr);
    void setPort(unsigned short port);

    static Paddr fromN64(unsigned long long n64);
    static int compareP(const void* item0, const void* item1);

private:
    enum
    {
        Addr = 0,
        Pad = 3,
        Port = 2
    };

    typedef union
    {
        unsigned long long paddr; //must be first to allow some code to initialize the union w/ a 64-bit number
        unsigned int u32[2];
        unsigned short u16[4];
    } paddr_t;

    union
    {
        double d64_;
        unsigned int u32_[2];
        unsigned long long paddr_;
        unsigned short u16_[4];
    };

    static bool getAddrInfo(unsigned int&, unsigned short&, const char*, char, bool);

};

END_NAMESPACE1

#include "appkit/String.hpp"

BEGIN_NAMESPACE1(netkit)

inline Paddr::Paddr(const Paddr& paddr)
{
    d64_ = paddr.d64_;
}

//! Construct instance from string (e.g., s="172.16.16.1:8080", delim=':').
//! An invalid string results in IP address zero and port zero ("0.0.0.0:0").
//! Use reset() for string format validation. Use of a hostname instead of
//! the dotted notation is allowed. See IpAddr for details.
inline Paddr::Paddr(const char* s, char delim)
{
    d64_ = 0.0;
    reset(s, delim, false /*portIsOptional*/);
}

//! Construct instance using given raw address.
//! Given raw address must have been obtained from raw().
inline Paddr::Paddr(double rawAddr)
{
    d64_ = rawAddr;
}

inline Paddr::Paddr(unsigned int addr, unsigned short port)
{
    u32_[Addr] = addr;
    u16_[Port] = port;
    u16_[Pad] = 0;
}

inline Paddr::Paddr(unsigned long long paddr)
{
    paddr_ = paddr;
}

//! Return string form.
//! For example, port 8080 at address 0xac101001UL -> "172.16.16.1:8080".
inline Paddr::operator appkit::String() const
{
    char s[StrLength];
    size_t n = toDigits(s, ':' /*delim*/);
    return appkit::String(s, n);
}

inline Paddr::operator unsigned long long() const
{
    return paddr_;
}

inline bool Paddr::operator !=(const Paddr& paddr) const
{
    bool ne = (paddr_ != paddr.paddr_);
    return ne;
}

//! Return true if this address is less than given address.
inline bool Paddr::operator <(const Paddr& paddr) const
{
    bool lt = (u32_[Addr] < paddr.u32_[Addr]) || ((u32_[Addr] == paddr.u32_[Addr]) && (u16_[Port] < paddr.u16_[Port]));
    return lt;
}

//! Return true if this address is not greater than given address.
inline bool Paddr::operator <=(const Paddr& paddr) const
{
    bool gt = (u32_[Addr] > paddr.u32_[Addr]) || ((u32_[Addr] == paddr.u32_[Addr]) && (u16_[Port] > paddr.u16_[Port]));
    return (!gt);
}

//! Return true if this address is greater than given address.
inline bool Paddr::operator >(const Paddr& paddr) const
{
    bool gt = (u32_[Addr] > paddr.u32_[Addr]) || ((u32_[Addr] == paddr.u32_[Addr]) && (u16_[Port] > paddr.u16_[Port]));
    return gt;
}

//! Return true if this address is not less than given address.
inline bool Paddr::operator >=(const Paddr& paddr) const
{
    bool lt = (u32_[Addr] < paddr.u32_[Addr]) || ((u32_[Addr] == paddr.u32_[Addr]) && (u16_[Port] < paddr.u16_[Port]));
    return (!lt);
}

inline bool Paddr::operator ==(const Paddr& paddr) const
{
    bool eq = (paddr_ == paddr.paddr_);
    return eq;
}

inline const Paddr& Paddr::operator =(const Paddr& paddr)
{
    d64_ = paddr.d64_;
    return *this;
}

//! Return string form. Use given delimiter to delimit address and port number.
//! For example, the string form of port 8080 at address 0xac101001UL using a
//! colon as the delimiter is: "172.16.16.1:8080".
inline appkit::String Paddr::toString(char delim) const
{
    char s[StrLength];
    size_t n = toDigits(s, delim);
    return appkit::String(s, n);
}

//! Convert instance to string s. Use given delimiter to delimit
//! address and port number. Return string s. For example, the
//! string form of port 8080 at address 0xac101001UL using a colon as
//! the delimiter is: "172.16.16.1:8080".
inline char* Paddr::toString(char s[StrLength + 1], char delim) const
{
    size_t n = toDigits(s, delim);
    s[n] = 0;
    return s;
}

//! Return meaningless raw address.
//! It's meant to be used for constructing an identical Paddr instance.
inline double Paddr::raw() const
{
    return d64_;
}

inline unsigned int Paddr::addr() const
{
    return u32_[Addr];
}

//! Return raw form as an unsigned 64-bit number.
inline unsigned long long Paddr::asU64() const
{
    return paddr_;
}

inline unsigned short Paddr::port() const
{
    return u16_[Port];
}

inline void Paddr::reset()
{
    d64_ = 0.0;
}

inline void Paddr::reset(unsigned int addr, unsigned short port)
{
    u32_[Addr] = addr;
    u16_[Port] = port;
}

inline void Paddr::setAddr(unsigned int addr)
{
    u32_[Addr] = addr;
}

inline void Paddr::setPort(unsigned short port)
{
    u16_[Port] = port;
}

END_NAMESPACE1

#endif
