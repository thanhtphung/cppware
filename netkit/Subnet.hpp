/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_SUBNET_HPP
#define NETKIT_SUBNET_HPP

#include "syskit/macros.h"

BEGIN_NAMESPACE1(netkit)


//! IPv4 subnet
class Subnet
{

public:
    enum clas_e
    {
        A = 0, //leading bits: 0,   classful first octet: 1-126
        B,     //leading bits: 10,  classful first octet: 128-191
        C,     //leading bits: 110, classful first octet: 192-223
        X      //undefined or not applicable
    };
    static const char* const CLAS_E[];

    enum format_e
    {
        AddrOnly = 0, //a.b.c.d
        CidrForm,     //a.b.c.d/n
        MaskTail      //a.b.c.d/a.b.c.d
    };

    enum
    {
        BitMaskStrLength = 35, //xxxxxxxx.xxxxxxxx.xxxxxxxx.xxxxxxxx
        StrLength = 32         //aaa.bbb.ccc.ddd/nn or aaa.bbb.ccc.ddd/aaa.bbb.ccc.ddd
    };

    Subnet(const Subnet& subnet);
    Subnet(const char* id, bool skipNormalization = false);
    Subnet(unsigned int addr = 0, unsigned int mask = 0, bool skipNormalization = true);
    const Subnet& operator =(const Subnet& subnet);

    bool isOk(bool useCidrMode = true) const;
    bool reset(const char* id, bool skipNormalization = false);
    char* formBitMask(char bitMask[BitMaskStrLength + 1], bool useCidrMode = true, unsigned int netMask = 0U) const;
    char* toStr(char s[StrLength + 1], unsigned int /*format_e*/ format = CidrForm) const;
    unsigned int addr() const;
    unsigned int broadcastAddr() const;
    unsigned int hiAddr() const;
    unsigned int loAddr() const;
    unsigned int mask() const;
    unsigned int size() const;
    unsigned int wildcardMask() const;
    unsigned int /*clas_e*/ clas() const;
    void getRange(unsigned int& loAddr, unsigned int& hiAddr) const;
    void normalizeAddr();
    void reset(unsigned int addr = 0, unsigned int mask = 0, bool skipNormalization = true);

    Subnet* clone(size_t splitCount) const;
    Subnet* clone1(size_t splitCount, size_t index = 0) const;

    static unsigned int defaultMask(unsigned int /*clas_e*/ clas);
    static unsigned int /*clas_e*/ classify(unsigned int addr);

private:
    unsigned int addr_;
    unsigned int mask_;

    static const unsigned char clas_[];
    static const unsigned int defaultMask_[];
    static const unsigned int validMask_[];

    unsigned int computeMask(size_t) const;

};

inline Subnet::Subnet(const Subnet& subnet)
{
    addr_ = subnet.addr_;
    mask_ = subnet.mask_;
}

//! Construct instance from given address and mask. Also normalize the address
//! part by ignoring the host bits. For example, "192.168.3.1/24" is normalized
//! as "192.168.3.0/24". The following formats are allowed: "a.b.c.d/n",
//! "a.b.c.d/a.b.c.d", and "a.b.c.d". For the "a.b.c.d" form, the mask is
//! derived from its network class assuming classful subnetting. For example,
//! "172.16.0.0" is equivalent to "172.16.0.0/16" because the address is in the
//! B class. Use skipNormalization to skip normalizing the address part. Use
//! isOk() to see if instance is valid.
inline Subnet::Subnet(const char* id, bool skipNormalization)
{
    addr_ = 0;
    mask_ = 0;
    reset(id, skipNormalization);
}

//! Construct instance from given address and mask.
//! Use isOk() to see if instance is valid.
inline Subnet::Subnet(unsigned int addr, unsigned int mask, bool skipNormalization)
{
    reset(addr, mask, skipNormalization);
}

inline const Subnet& Subnet::operator =(const Subnet& subnet)
{
    addr_ = subnet.addr_;
    mask_ = subnet.mask_;
    return *this;
}

//! Return the subnet address.
inline unsigned int Subnet::addr() const
{
    return addr_;
}

//! Return the subnet broadcast address.
inline unsigned int Subnet::broadcastAddr() const
{
    unsigned int addr = addr_ | ~mask_;
    return addr;
}

//! Return the high host address in the subnet. For a 32-bit mask, the subnet
//! address is the range. For a 31-bit mask, the range overlaps with the subnet
//! address and its broadcast address (see RFC 3021). For other masks, the range
//! covers all addresses between the subnet address and its broadcast address.
inline unsigned int Subnet::hiAddr() const
{
    unsigned int hi = (mask_ != 0xffffffffU)?
        ((mask_ != 0xfffffffeU)? ((addr_ | ~mask_) - 1): (addr_ | ~mask_)):
        (addr_);
    return hi;
}

//! Return the low host address in the subnet. For a 32-bit mask, the subnet
//! address is the range. For a 31-bit mask, the range overlaps with the subnet
//! address and its broadcast address (see RFC 3021). For other masks, the range
//! covers all addresses between the subnet address and its broadcast address.
inline unsigned int Subnet::loAddr() const
{
    unsigned int lo = (mask_ < 0xfffffffeU)? (addr_ + 1): (addr_);
    return lo;
}

//! Return the subnet mask.
inline unsigned int Subnet::mask() const
{
    return mask_;
}

//! Return the subnet size (hosts per subnet). For a 32-bit mask, the subnet
//! size is one. For a 31-bit mask, the subnet size is two (see RFC 3021). For
//! other masks, the subnet size is the number of addresses between the subnet
//! address and its broadcast address.
inline unsigned int Subnet::size() const
{
    unsigned int maxHosts = (mask_ != 0xffffffffU)?
        ((mask_ != 0xfffffffeU)? ((~mask_) - 1): (2)):
        (1);
    return maxHosts;
}

//! Return the wildcard mask (aka inverse mask).
inline unsigned int Subnet::wildcardMask() const
{
    return ~mask_;
}

//! Return the default subnet mask for given network class. The default subnet mask
//! is all ones if given network class is not A, B, nor C.
inline unsigned int Subnet::defaultMask(unsigned int /*clas_e*/ clas)
{
    unsigned int mask = ((clas >= A) && (clas <= C))? defaultMask_[clas]: 0xffffffffU;
    return mask;
}

//! Return the network class (A, B, C, or X). X is used to denote an undefined or
//! not-applicable class. A class A network can have up to 16,777,214 hosts, a class
//! B network can have up to 65,534 hosts, and a class C network can have up to 254
//! hosts.
inline unsigned int /*clas_e*/ Subnet::clas() const
{
    size_t i = (addr_ >> 24);
    return clas_[i];
}

//! Classify given address. Return its network class (A, B, C, or X). X is used to
//! denote an undefined or not-applicable class. A class A network can have up to
//! 16,777,214 hosts, a class B network can have up to 65,534 hosts, and a class C
//! network can have up to 254 hosts.
inline unsigned int /*clas_e*/ Subnet::classify(unsigned int addr)
{
    size_t i = (addr >> 24);
    return clas_[i];
}

//! Normalize the address part by ignoring the host bits.
//! For example, "192.168.3.1/24" is normalized as "192.168.3.0/24".
inline void Subnet::normalizeAddr()
{
    addr_ &= mask_;
}

END_NAMESPACE1

#endif
