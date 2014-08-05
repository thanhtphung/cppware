/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "appkit/DelimitedTxt.hpp"
#include "appkit/U32.hpp"
#include "appkit/U8.hpp"
#include "syskit/macros.h"

#include "netkit-pch.h"
#include "netkit/IpAddr.hpp"
#include "netkit/Subnet.hpp"

using namespace appkit;

BEGIN_NAMESPACE1(netkit)

const char* const Subnet::CLAS_E[] =
{
    STRINGIFY(A),
    STRINGIFY(B),
    STRINGIFY(C),
    "N/A", //X
    0
};

// Network class.
// Indexed by first octet.
const unsigned char Subnet::clas_[] =
{
    X, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, //0x00-0x0f
    A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, //0x10-0x1f
    A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, //0x20-0x2f
    A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, //0x30-0x3f
    A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, //0x40-0x4f
    A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, //0x50-0x5f
    A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, //0x60-0x6f
    A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, //0x70-0x7f
    B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, //0x80-0x8f
    B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, //0x90-0x9f
    B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, //0xa0-0xaf
    B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, //0xb0-0xbf
    C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, //0xc0-0xcf
    C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, //0xd0-0xdf
    X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, //0xe0-0xef
    X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X  //0xf0-0xff
};

// Default network mask.
// Indexed by class.
const unsigned int Subnet::defaultMask_[] =
{
    0xff000000U, //A
    0xffff0000U, //B
    0xffffff00U  //C
};

// Array of all valid masks.
const unsigned int Subnet::validMask_[] =
{
    0xff000000U, //X, A
    0xff800000U, //X, A
    0xffc00000U, //X, A
    0xffe00000U, //X, A
    0xfff00000U, //X, A
    0xfff80000U, //X, A
    0xfffc0000U, //X, A
    0xfffe0000U, //X, A
    0xffff0000U, //X, A, B
    0xffff8000U, //X, A, B
    0xffffc000U, //X, A, B
    0xffffe000U, //X, A, B
    0xfffff000U, //X, A, B
    0xfffff800U, //X, A, B
    0xfffffc00U, //X, A, B
    0xfffffe00U, //X, A, B
    0xffffff00U, //X, A, B, C
    0xffffff80U, //X, A, B, C
    0xffffffc0U, //X, A, B, C
    0xffffffe0U, //X, A, B, C
    0xfffffff0U, //X, A, B, C
    0xfffffff8U, //X, A, B, C
    0xfffffffcU, //X, A, B, C
    0xfffffffeU, //X, A, B, C
    0xffffffffU, //X, A, B, C
    0xfe000000U, //X
    0xfc000000U, //X
    0xf8000000U, //X
    0xf0000000U, //X
    0xe0000000U, //X
    0xc0000000U, //X
    0x80000000U  //X
};


//!
//! Clone and split into splitCount subnets. splitCount must be a power of two not
//! less than two. Return the split subnets which must be destroyed using the delete[]
//! operator when done. Return zero in case of failure (invalid splitCount or subnet
//! is too small and cannot be split into that many subnets).
//!
Subnet* Subnet::clone(size_t splitCount) const
{
    unsigned int newMask;
    if ((mask_ == 0xffffffffU) || (splitCount < 2) || ((newMask = computeMask(splitCount)) == 0))
    {
        Subnet* cloned = 0;
        return cloned;
    }

    Subnet* cloned = new Subnet[splitCount];
    unsigned int delta = ~newMask + 1;
    unsigned int newAddr = addr_;
    for (size_t i = 0; i < splitCount; ++i, newAddr += delta)
    {
        cloned[i].addr_ = newAddr;
        cloned[i].mask_ = newMask;
    }

    return cloned;
}


//!
//! Clone and split into splitCount subnets. splitCount must be a power of two not
//! less than two. Return the split subnet at given index which must be destroyed
//! using the delete[] operator when done. The first split subnet has index zero.
//! Return zero in case of failure (invalid splitCount or subnet is too small and
//! cannot be split into that many subnets). Result is incorrect if given index is
//! invalid (not less than splitCount).
//!
Subnet* Subnet::clone1(size_t splitCount, size_t index) const
{
    unsigned int newMask;
    if ((mask_ == 0xffffffffU) || (splitCount < 2) || ((newMask = computeMask(splitCount)) == 0))
    {
        Subnet* cloned = 0;
        return cloned;
    }

    Subnet* cloned = new Subnet[1];
    unsigned int newAddr = addr_;
    if (index > 0)
    {
        unsigned int delta = ~newMask + 1;
        newAddr += delta * static_cast<unsigned int>(index);
    }
    cloned[0].addr_ = newAddr;
    cloned[0].mask_ = newMask;
    return cloned;
}


//!
//! Return true if instance is valid. Network bits must be non-zero and contiguous.
//! In classful subnetting, a valid subnet must be classifiable as either class A,
//! class B, or class C.
//!
bool Subnet::isOk(bool useCidrMode) const
{
    bool ok = false;
    unsigned int clas = Subnet::clas();
    if (clas == X)
    {
        if (!useCidrMode)
        {
            return ok;
        }
    }
    else if (useCidrMode)
    {
        clas = X;
    }

    static const size_t s_minI[4] = {0  /*A*/, 8  /*B*/, 16 /*C*/, 0  /*X*/};
    static const size_t s_maxI[4] = {24 /*A*/, 24 /*B*/, 24 /*C*/, 31 /*X*/};
    size_t maxI = s_maxI[clas];
    for (size_t i = s_minI[clas]; i <= maxI; ++i)
    {
        if (mask_ == validMask_[i])
        {
            ok = true;
            break;
        }
    }

    return ok;
}


//!
//! Reset instance with given address and mask. Also normalize the address
//! part by ignoring the host bits. For example, "192.168.3.1/24" is normalized
//! as "192.168.3.0/24". The following formats are allowed: "a.b.c.d/n",
//! "a.b.c.d/a.b.c.d", and "a.b.c.d". For the "a.b.c.d" form, the mask is
//! derived from its network class assuming classful subnetting. For example,
//! "172.16.0.0" is equivalent to "172.16.0.0/16" because the address is in the
//! B class. Use skipNormalization to skip normalizing the address part. Return
//! true if successful.
//!
bool Subnet::reset(const char* id, bool skipNormalization)
{
    bool makeCopy = false;
    char delim = '/';
    DelimitedTxt txt(id, strlen(id), makeCopy, delim);
    IpAddr addr;
    String part0;
    String part1;
    if (txt.next(part0, true /*doTrimLine*/) && txt.next(part1, false /*doTrimLine*/))
    {
        bool ok = addr.reset(part0.ascii());
        if (ok)
        {

            size_t bytesUsed;
            U32 n(part1, &bytesUsed);
            if (S32::isDigit(*part1.ascii()) && (bytesUsed == part1.length()) && (n > 0) && (n <= 32)) //"a.b.c.d/n" form?
            {
                mask_ = 0xffffffffU << (32 - n);
                addr_ = skipNormalization? addr.asU32(): (addr.asU32() & mask_);
                return ok;
            }

            IpAddr mask;
            ok = mask.reset(part1.ascii());
            if (ok) //"a.b.c.d/a.b.c.d" form?
            {
                mask_ = mask.asU32();
                addr_ = skipNormalization? addr.asU32(): (addr.asU32() & mask_);
                return ok;
            }
        }
        return ok;
    }

    bool ok = addr.reset(id);
    if (ok)
    {
        unsigned int clas = classify(addr.asU32());
        addr_ = addr.asU32();
        mask_ = defaultMask(clas);
        if (!skipNormalization)
        {
            addr_ &= mask_;
        }
    }

    return ok;
}


//!
//! Form bit mask string. Use '0' and '1' to show leading bits in classful
//! subnetting. Use 'n' to show network bits. Use 's' to show subnet bits.
//! Use 'h' to show host bits. For example, "110nnnnn.nnnnnnnn.nnnnnnnn.ssshhhhh"
//! is the bit mask string for "192.168.3.96/255.255.255.224" in classful
//! subnetting. The same subnet would have the following bit mask string
//! in classless subnetting: "nnnnnnnn.nnnnnnnn.nnnnnnnn.ssshhhhh".
//!
char* Subnet::formBitMask(char bitMask[BitMaskStrLength + 1], bool useCidrMode, unsigned int netMask) const
{

    // Use 'n' for network bit and 's' for subnet bit and 'h' for host bit.
    char* s = bitMask;
    unsigned int bitNum = 32;
    unsigned int clas = Subnet::clas();
    unsigned int nMask = (useCidrMode && (netMask != 0))? netMask: Subnet::defaultMask(clas);
    for (unsigned int m = 0x80000000U; m; m >>= 1)
    {
        if ((mask_ & m) == 0)
        {
            *s++ = 'h';
        }
        else if ((nMask & m) == 0)
        {
            *s++ = 's';
        }
        else
        {
            *s++ = 'n';
        }
        if ((--bitNum & 7) == 0)
        {
            *s++ = '.';
        }
    }
    *--s = 0;

    // Show leading bits identifying the network class.
    if (!useCidrMode)
    {
        switch (clas)
        {
        case A:
            bitMask[0] = '0';
            break;
        case B:
            bitMask[0] = '1';
            bitMask[1] = '0';
            break;
        case C:
            bitMask[0] = '1';
            bitMask[1] = '1';
            bitMask[2] = '0';
            break;
        default: //X
            break;
        }
    }

    return bitMask;
}


char* Subnet::toStr(char s[StrLength + 1], unsigned int /*format_e*/ format) const
{

    // Start with AddrOnly.
    IpAddr ipAddr(addr_);
    size_t addrLength = ipAddr.toDigits(s);

    // Use CIDR format.
    if (format == CidrForm)
    {
        s[addrLength] = '/';
        unsigned int prefixLength = 0;
        for (unsigned int m = mask_; m > 0; ++prefixLength, m <<= 1);
        size_t n = U8::toDigits(static_cast<unsigned char>(prefixLength), s + addrLength + 1);
        s[addrLength + 1 + n] = 0;
    }

    // Use mask as a suffix.
    else if (format == MaskTail)
    {
        s[addrLength] = '/';
        ipAddr = mask_;
        ipAddr.toString(s + addrLength + 1);
    }

    // No suffix.
    else
    {
        s[addrLength] = 0;
    }

    return s;
}


//
// Compute the resulting mask if splitting into splitCount subnets. splitCount
// must be a power of two not less than two. Return zero in case of failure
// (invalid splitCount or subnet is too small and cannot be split into that many
// subnets).
//
unsigned int Subnet::computeMask(size_t splitCount) const
{
    size_t numSubnets = 1;
    unsigned int mask = mask_;
    for (unsigned int m = ~((~mask) >> 1);; m >>= 1)
    {
        mask |= m;
        numSubnets <<= 1;
        if (numSubnets == splitCount)
        {
            break;
        }
        if ((numSubnets > splitCount) || (mask == 0xffffffffU))
        {
            mask = 0;
            break;
        }
    }

    return mask;
}


//!
//! Return the host address range for this subnet. For a 32-bit mask, the subnet
//! address is the range. For a 31-bit mask, the range overlaps with the subnet
//! address and its broadcast address (see RFC 3021). For other masks, the range
//! covers all addresses between the subnet address and its broadcast address.
//!
void Subnet::getRange(unsigned int& loAddr, unsigned int& hiAddr) const
{
    loAddr = addr_;
    if (mask_ != 0xffffffffU)
    {
        hiAddr = (addr_ | ~mask_);
        if (mask_ != 0xfffffffeU)
        {
            ++loAddr;
            --hiAddr;
        }
    }
    else
    {
        hiAddr = addr_;
    }
}


//!
//! Reset instance with given address and mask.
//! Use isOk() to see if instance is valid.
//!
void Subnet::reset(unsigned int addr, unsigned int mask, bool skipNormalization)
{
    addr_ = addr;
    mask_ = mask;
    if (!skipNormalization)
    {
        addr_ &= mask_;
    }
}

END_NAMESPACE1
