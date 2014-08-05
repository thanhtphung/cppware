/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_MAC_INTF_HPP
#define NETKIT_MAC_INTF_HPP

#include "netkit/MacAddr.hpp"
#include "syskit/sys.hpp"

DECLARE_CLASS1(appkit, String)

BEGIN_NAMESPACE1(netkit)


//! mac-interface key-value pair
class MacIntf
{

public:
    enum
    {
        StrLength = 23 //XX-XX-XX-XX-XX-XX:XXXXX
    };

    MacIntf();
    MacIntf(const MacIntf& macIntf);
    MacIntf(const char* s);
    MacIntf(const unsigned char* mac, unsigned short intf);
    MacIntf(double rawMacIntf);
    MacIntf(unsigned long long macIntf);

    operator appkit::String() const;
    operator unsigned long long() const;
    bool operator !=(const MacIntf& macIntf) const;
    bool operator <(const MacIntf& macIntf) const;
    bool operator <=(const MacIntf& macIntf) const;
    bool operator >(const MacIntf& macIntf) const;
    bool operator >=(const MacIntf& macIntf) const;
    bool operator ==(const MacIntf& macIntf) const;
    const MacIntf& operator =(const MacIntf& macIntf);
    const MacIntf& operator =(unsigned long long macIntf);
    static void operator delete(void* p, size_t size);
    static void operator delete(void* p, void* buf);
    static void* operator new(size_t size);
    static void* operator new(size_t size, void* buf);

    appkit::String toSTRING(char delim0, char delim1) const;
    appkit::String toString(char delim0, char delim1) const;
    bool hasKey(const unsigned char* mac) const;
    char* toSTRING(char s[StrLength + 1], char delim0, char delim1) const;
    char* toString(char s[StrLength + 1], char delim0, char delim1) const;
    const unsigned char* mac() const;
    double raw() const;
    unsigned int intf() const;
    unsigned long long asU64() const;
    unsigned long long toN64() const;

    bool reset(const char* s);
    void setIntf(unsigned short intf);
    void setMac(const unsigned char* mac);
    void reset();
    void reset(const unsigned char* mac, unsigned short intf);

    static MacIntf fromN64(unsigned long long n64);
    static int compareP(const void* item0, const void* item1);

private:
    enum
    {
        Intf = 3
    };

    typedef union
    {
        unsigned long long macIntf; //must be first to allow some code to initialize the union w/ a 64-bit number
        unsigned short intf[4];
    } macIntf_t;

    union
    {
        double d64_;
        unsigned char mac_[MacAddr::RawLength + 2];
        unsigned short intf_[4];
        unsigned long long macIntf_;
    };

};

END_NAMESPACE1

#include "appkit/String.hpp"
#include "syskit/BufPool.hpp"
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(netkit)

inline MacIntf::MacIntf()
{
    d64_ = 0.0;
}

inline MacIntf::MacIntf(const MacIntf& macIntf)
{
    d64_ = macIntf.d64_;
}

inline MacIntf::MacIntf(const char* s)
{
    d64_ = 0.0;
    reset(s);
}

inline MacIntf::MacIntf(const unsigned char* mac, unsigned short intf)
{
    mac_[0] = mac[0];
    mac_[1] = mac[1];
    mac_[2] = mac[2];
    mac_[3] = mac[3];
    mac_[4] = mac[4];
    mac_[5] = mac[5];
    intf_[Intf] = intf;
}

//! Construct instance using given raw mac-interface.
//! Given raw mac-interface must have been obtained from raw().
inline MacIntf::MacIntf(double rawMacIntf)
{
    d64_ = rawMacIntf;
}

inline MacIntf::MacIntf(unsigned long long macIntf)
{
    macIntf_ = macIntf;
}

//! Return string form. Use uppercase hex digits. Use hyphens and a colon
//! for delimiters. For example, "00-23-AE-6A-DA-57:12345".
inline MacIntf::operator appkit::String() const
{
    char s[StrLength + 1];
    return toSTRING(s, '-', ':');
}

//! Return raw form as an unsigned 64-bit number.
inline MacIntf::operator unsigned long long() const
{
    return macIntf_;
}

inline bool MacIntf::operator !=(const MacIntf& macIntf) const
{
    bool ne = (macIntf_ != macIntf.macIntf_);
    return ne;
}

inline bool MacIntf::operator <(const MacIntf& macIntf) const
{
    int rc = memcmp(mac_, macIntf.mac_, MacAddr::RawLength);
    bool lt = (rc < 0) || ((rc == 0) && (intf_[Intf] < macIntf.intf_[Intf]));
    return lt;
}

inline bool MacIntf::operator <=(const MacIntf& macIntf) const
{
    int rc = memcmp(mac_, macIntf.mac_, MacAddr::RawLength);
    bool gt = (rc > 0) || ((rc == 0) && (intf_[Intf] > macIntf.intf_[Intf]));
    return (!gt);
}

inline bool MacIntf::operator >(const MacIntf& macIntf) const
{
    int rc = memcmp(mac_, macIntf.mac_, MacAddr::RawLength);
    bool gt = (rc > 0) || ((rc == 0) && (intf_[Intf] > macIntf.intf_[Intf]));
    return gt;
}

inline bool MacIntf::operator >=(const MacIntf& macIntf) const
{
    int rc = memcmp(mac_, macIntf.mac_, MacAddr::RawLength);
    bool lt = (rc < 0) || ((rc == 0) && (intf_[Intf] < macIntf.intf_[Intf]));
    return (!lt);
}

inline bool MacIntf::operator ==(const MacIntf& macIntf) const
{
    bool eq = (macIntf_ == macIntf.macIntf_);
    return eq;
}

inline const MacIntf& MacIntf::operator =(const MacIntf& macIntf)
{
    macIntf_ = macIntf.macIntf_;
    return *this;
}

inline const MacIntf& MacIntf::operator =(unsigned long long macIntf)
{
    macIntf_ = macIntf;
    return *this;
}

inline void MacIntf::operator delete(void* p, size_t size)
{
    syskit::BufPool::freeBuf(p, size);
}

inline void MacIntf::operator delete(void* /*p*/, void* /*buf*/)
{
}

inline void* MacIntf::operator new(size_t size)
{
    void* buf = syskit::BufPool::allocateBuf(size);
    return buf;
}

inline void* MacIntf::operator new(size_t /*size*/, void* buf)
{
    return buf;
}

inline bool MacIntf::hasKey(const unsigned char* mac) const
{
    bool eq = (memcmp(mac_, mac, MacAddr::RawLength) == 0);
    return eq;
}

inline const unsigned char* MacIntf::mac() const
{
    return mac_;
}

//! Return string form. Use uppercase hex digits. Use given delimiters.
//! For example, "00-23-AE-6A-DA-57:12345".
inline appkit::String MacIntf::toSTRING(char delim0, char delim1) const
{
    char s[StrLength + 1];
    return toSTRING(s, delim0, delim1);
}

//! Return string form. Use lowercase hex digits. Use given delimiters.
//! For example, "00:23:ae:6a:da:57.12345".
inline appkit::String MacIntf::toString(char delim0, char delim1) const
{
    char s[StrLength + 1];
    return toString(s, delim0, delim1);
}

//! Return meaningless raw mac-interface.
//! It's meant to be used for constructing an identical MacIntf instance.
inline double MacIntf::raw() const
{
    return d64_;
}

inline unsigned int MacIntf::intf() const
{
    return intf_[Intf];
}

//! Return raw form as an unsigned 64-bit number.
inline unsigned long long MacIntf::asU64() const
{
    return macIntf_;
}

//! Convert to a network-order unsigned 64-bit number.
inline unsigned long long MacIntf::toN64() const
{
    macIntf_t u = {macIntf_};
    u.intf[Intf] = htons(u.intf[Intf]);
    return htonll(u.macIntf);
}

inline void MacIntf::setIntf(unsigned short intf)
{
    intf_[Intf] = intf;
}

inline void MacIntf::setMac(const unsigned char* mac)
{
    mac_[0] = mac[0];
    mac_[1] = mac[1];
    mac_[2] = mac[2];
    mac_[3] = mac[3];
    mac_[4] = mac[4];
    mac_[5] = mac[5];
}

inline void MacIntf::reset()
{
    d64_ = 0.0;
}

inline void MacIntf::reset(const unsigned char* mac, unsigned short intf)
{
    mac_[0] = mac[0];
    mac_[1] = mac[1];
    mac_[2] = mac[2];
    mac_[3] = mac[3];
    mac_[4] = mac[4];
    mac_[5] = mac[5];
    intf_[Intf] = intf;
}

END_NAMESPACE1

#endif
