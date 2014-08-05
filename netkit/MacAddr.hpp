/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_MAC_ADDR_HPP
#define NETKIT_MAC_ADDR_HPP

#include "syskit/macros.h"

DECLARE_CLASS1(appkit, String)

BEGIN_NAMESPACE1(netkit)


//! media access control address
class MacAddr
    //!
    //! A class representing a MAC (Media Access Control) address.
    //!
{

public:
    enum
    {
        RawLength = 6, //bytes
        StrLength = 17 //XX:XX:XX:XX:XX:XX
    };

    MacAddr();
    MacAddr(const MacAddr& addr);
    MacAddr(const char* addr, char delim = 0);
    MacAddr(const unsigned char addr[RawLength]);
    MacAddr(double rawAddr);
    MacAddr(unsigned long long addr);

    operator appkit::String() const;
    operator const unsigned char*() const;
    operator unsigned long long() const;
    bool operator !=(const MacAddr& addr) const;
    bool operator !=(const unsigned char addr[RawLength]) const;
    bool operator <(const MacAddr& addr) const;
    bool operator <=(const MacAddr& addr) const;
    bool operator >(const MacAddr& addr) const;
    bool operator >=(const MacAddr& addr) const;
    bool operator ==(const MacAddr& addr) const;
    bool operator ==(const unsigned char addr[RawLength]) const;
    const MacAddr& operator =(const MacAddr& addr);
    const MacAddr& operator =(const unsigned char addr[RawLength]);
    const MacAddr& operator =(unsigned long long addr);
    static void operator delete(void* p, size_t size);
    static void operator delete(void* p, void* buf);
    static void* operator new(size_t size);
    static void* operator new(size_t size, void* buf);

    appkit::String toSTRING(char delim = ':') const;
    appkit::String toString(char delim = ':') const;
    bool isBroadcast() const;
    bool isVmware() const;
    char* toSTRING(char s[StrLength + 1], char delim) const;
    char* toString(char s[StrLength + 1], char delim) const;
    const unsigned char* asRawBytes() const;
    double raw() const;
    unsigned int toXDIGITS(char s[StrLength], char delim) const;
    unsigned int toXdigits(char s[StrLength], char delim) const;
    unsigned long long asU64() const;
    unsigned long long toN64() const;

    bool reset(const char* addr, char delim = 0);
    void reset();

    static MacAddr fromN64(unsigned long long n64);
    static int compareP(const void* item0, const void* item1);

private:
    typedef union
    {
        unsigned char addr[RawLength + 2]; //must be first to allow some code to initialize the union w/ unsigned bytes
        unsigned long long addr64;
    } bcast_t;

    union
    {
        double d64_;
        unsigned char addr_[RawLength + 2];
        unsigned long long addr64_;
    };

    static const bcast_t bcast_;

};

END_NAMESPACE1

#include "appkit/String.hpp"
#include "syskit/BufPool.hpp"
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(netkit)

inline MacAddr::MacAddr()
{
    d64_ = 0.0;
}

inline MacAddr::MacAddr(const MacAddr& addr)
{
    d64_ = addr.d64_;
}

inline MacAddr::MacAddr(const char* s, char delim)
{
    d64_ = 0.0;
    reset(s, delim);
}

inline MacAddr::MacAddr(const unsigned char addr[RawLength])
{
    addr_[0] = addr[0];
    addr_[1] = addr[1];
    addr_[2] = addr[2];
    addr_[3] = addr[3];
    addr_[4] = addr[4];
    addr_[5] = addr[5];
    addr_[6] = 0;
    addr_[7] = 0;
}

//! Construct instance using given raw address.
//! Given raw address must have been obtained from raw().
inline MacAddr::MacAddr(double rawAddr)
{
    d64_ = rawAddr;
}

inline MacAddr::MacAddr(unsigned long long addr)
{
    addr64_ = addr;
}

//! Return string form. Use uppercase hex digits. Use a hyphen to
//! delimit individual bytes. For example, "00-23-AE-6A-DA-57".
inline MacAddr::operator appkit::String() const
{
    char s[StrLength];
    size_t n = toXDIGITS(s, '-' /*delim*/);
    return appkit::String(s, n);
}

//! Return raw form (array of 6 bytes).
inline MacAddr::operator const unsigned char*() const
{
    return addr_;
}

//! Return raw form as an unsigned 64-bit number.
inline MacAddr::operator unsigned long long() const
{
    return addr64_;
}

inline bool MacAddr::operator !=(const MacAddr& addr) const
{
    bool ne = (addr64_ != addr.addr64_);
    return ne;
}

inline bool MacAddr::operator !=(const unsigned char addr[RawLength]) const
{
    bool ne = (memcmp(addr_, addr, RawLength) != 0);
    return ne;
}

inline bool MacAddr::operator <(const MacAddr& addr) const
{
    bool lt = (memcmp(addr_, addr.addr_, RawLength) < 0);
    return lt;
}

inline bool MacAddr::operator <=(const MacAddr& addr) const
{
    bool le = (memcmp(addr_, addr.addr_, RawLength) <= 0);
    return le;
}

inline bool MacAddr::operator >(const MacAddr& addr) const
{
    bool gt = (memcmp(addr_, addr.addr_, RawLength) > 0);
    return gt;
}

inline bool MacAddr::operator >=(const MacAddr& addr) const
{
    bool ge = (memcmp(addr_, addr.addr_, RawLength) >= 0);
    return ge;
}

inline bool MacAddr::operator ==(const MacAddr& addr) const
{
    bool eq = (addr64_ == addr.addr64_);
    return eq;
}

inline bool MacAddr::operator ==(const unsigned char addr[RawLength]) const
{
    bool eq = (memcmp(addr_, addr, RawLength) == 0);
    return eq;
}

inline const MacAddr& MacAddr::operator =(const MacAddr& addr)
{
    addr64_ = addr.addr64_;
    return *this;
}

inline const MacAddr& MacAddr::operator =(const unsigned char addr[RawLength])
{
    addr_[0] = addr[0];
    addr_[1] = addr[1];
    addr_[2] = addr[2];
    addr_[3] = addr[3];
    addr_[4] = addr[4];
    addr_[5] = addr[5];
    addr_[6] = 0;
    addr_[7] = 0;
    return *this;
}

inline const MacAddr& MacAddr::operator =(unsigned long long addr)
{
    addr64_ = addr;
    return *this;
}

inline void MacAddr::operator delete(void* p, size_t size)
{
    syskit::BufPool::freeBuf(p, size);
}

inline void MacAddr::operator delete(void* /*p*/, void* /*buf*/)
{
}

inline void* MacAddr::operator new(size_t size)
{
    void* buf = syskit::BufPool::allocateBuf(size);
    return buf;
}

inline void* MacAddr::operator new(size_t /*size*/, void* buf)
{
    return buf;
}

//! Return string form. Use uppercase hex digits. Use given delimiter (zero for none)
//! to delimit individual bytes. Example of a hyphen-delimited string form of a MAC
//! address: "00-0D-56-A7-10-EC".
inline appkit::String MacAddr::toSTRING(char delim) const
{
    char s[StrLength];
    size_t n = toXDIGITS(s, delim);
    return appkit::String(s, n);
}

//! Return string form. Use lowercase hex digits. Use given delimiter (zero for none)
//! to delimit individual bytes. Example of a colon-delimited string form of a MAC
//! address: "00:0d:56:a7:10:ec".
inline appkit::String MacAddr::toString(char delim) const
{
    char s[StrLength];
    size_t n = toXdigits(s, delim);
    return appkit::String(s, n);
}

//! Return true if this is the broadcast MAC address.
inline bool MacAddr::isBroadcast() const
{
    bool isBcast = (addr64_ == bcast_.addr64);
    return isBcast;
}

//! Convert instance to a null-terminated string in provided buffer. Use uppercase
//! hex digits. Use given delimiter (zero for none) to delimit individual bytes.
//! Return the updated buffer. Example of a hyphen-delimited string form of a
//! MAC address: "00-0D-56-A7-10-EC".
inline char* MacAddr::toSTRING(char s[StrLength + 1], char delim) const
{
    size_t n = toXDIGITS(s, delim);
    s[n] = 0;
    return s;
}

//! Convert instance to a null-terminated string in provided buffer. Use lowercase
//! hex digits. Use given delimiter (zero for none) to delimit individual bytes.
//! Return the updated buffer. Example of a colon-delimited string form of a
//! MAC address: "00:0d:56:a7:10:ec".
inline char* MacAddr::toString(char s[StrLength + 1], char delim) const
{
    size_t n = toXdigits(s, delim);
    s[n] = 0;
    return s;
}

//! Return raw form (array of 6 bytes).
inline const unsigned char* MacAddr::asRawBytes() const
{
    return addr_;
}

//! Return meaningless raw address.
//! It's meant to be used for constructing an identical MacAddr instance.
inline double MacAddr::raw() const
{
    return d64_;
}

//! Return raw form as an unsigned 64-bit number.
inline unsigned long long MacAddr::asU64() const
{
    return addr64_;
}

//! Convert to a network-order unsigned 64-bit number.
inline unsigned long long MacAddr::toN64() const
{
    return syskit::htonll__(addr64_); //TODO: why doesn't htonll() compile?
}

//! Reset instance with "00-00-00-00-00-00".
inline void MacAddr::reset()
{
    d64_ = 0.0;
}

END_NAMESPACE1

#endif
