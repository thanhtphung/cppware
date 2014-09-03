/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_IP_FRAG_HPP
#define NETKIT_IP_FRAG_HPP

#include <new>
#include "syskit/macros.h"

BEGIN_NAMESPACE1(netkit)

class IpCap;


//! IPv4 packet fragment
class IpFrag
{

public:
    IpFrag(const IpCap& frag, IpFrag* next, unsigned int maxByteOffset);
    ~IpFrag();
    static void operator delete(void* p, size_t size);
    static void operator delete(void* p, void* buf);
    static void* operator new(size_t size);
    static void* operator new(size_t size, void* buf);

    const IpFrag* next() const;
    const unsigned char* raw() const;
    unsigned int byteOffset() const;
    unsigned int offset() const;
    unsigned int rawLength() const;
    void setNext(IpFrag* next);

private:
    IpFrag* next_;
    const unsigned char* raw_;
    unsigned int byteOffset_;
    unsigned int rawLength_;

    IpFrag(const IpFrag&); //prohibit usage
    const IpFrag& operator =(const IpFrag&); //prohibit usage

};

END_NAMESPACE1

#include "syskit/BufPool.hpp"

BEGIN_NAMESPACE1(netkit)

inline void IpFrag::operator delete(void* p, size_t size)
{
    syskit::BufPool::freeBuf(p, size);
}

inline void IpFrag::operator delete(void* /*p*/, void* /*buf*/)
{
}

inline void* IpFrag::operator new(size_t size)
{
    void* buf = syskit::BufPool::allocateBuf(size);
    return buf;
}

inline void* IpFrag::operator new(size_t /*size*/, void* buf)
{
    return buf;
}

//! Return the next fragment in the linked list, if any.
//! Return zero if none.
inline const IpFrag* IpFrag::next() const
{
    return next_;
}

//! Return the raw fragment contents.
inline const unsigned char* IpFrag::raw() const
{
    return raw_;
}

//! Return the byte position where this fragment resides.
inline unsigned int IpFrag::byteOffset() const
{
    return byteOffset_;
}

//! Return the byte size of this fragment.
inline unsigned int IpFrag::rawLength() const
{
    return rawLength_;
}

//! Return the position in 8-byte units where this fragment resides.
//! Zero is first byte, one is 8th byte, two is 16th byte, and so on.
inline unsigned int IpFrag::offset() const
{
    unsigned int n = byteOffset_ >> 3;
    return n;
}

//! Link with given fragment.
inline void IpFrag::setNext(IpFrag* next)
{
    next_ = next;
}

END_NAMESPACE1

#endif
