/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_STRING_PAIR_HPP
#define APPKIT_STRING_PAIR_HPP

#include "appkit/String.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)


//! key-value string pair
class StringPair
{

public:
    StringPair(const String& k = String());
    StringPair(const String& k, const String& v);
    StringPair(const StringPair& kv);
    ~StringPair();

    bool operator !=(const StringPair& kv) const;
    bool operator ==(const StringPair& kv) const;
    const StringPair& operator =(const StringPair& kv);
    static void operator delete(void* p, size_t size);
    static void operator delete(void* p, void* buf);
    static void* operator new(size_t size);
    static void* operator new(size_t size, void* buf);

    String& v();
    const String& k() const;
    const String& n() const;
    const String& v() const;
    void addV(const String& v, char delim);
    void reset(const String& k = String());
    void reset(const String& k, const String& v);
    void setV(const String& v);

    // Variants supporting primitive value types.
    bool vAsBool() const;
    const char* vAsAscii() const;
    double vAsD64() const;
    float vAsF32() const;
    int vAsS32() const;
    unsigned int vAsU32() const;
    unsigned long long vAsU64() const;
    unsigned short vAsU16() const;
    void setV(bool v);
    void setV(const char* v);
    void setV(double v);
    void setV(float v);
    void setV(int v);
    void setV(unsigned int v);
    void setV(unsigned long long v);
    void setV(unsigned short v);

private:
    String k_;
    String v_;

};

END_NAMESPACE1

#include "syskit/BufPool.hpp"

BEGIN_NAMESPACE1(appkit)

inline bool StringPair::operator !=(const StringPair& kv) const
{
    return (k_ != kv.k_) || (v_ != kv.v_);
}

inline bool StringPair::operator ==(const StringPair& kv) const
{
    return (k_ == kv.k_) && (v_ == kv.v_);
}

inline void StringPair::operator delete(void* p, size_t size)
{
    syskit::BufPool::freeBuf(p, size);
}

inline void StringPair::operator delete(void* /*p*/, void* /*buf*/)
{
}

inline void* StringPair::operator new(size_t size)
{
    void* buf = syskit::BufPool::allocateBuf(size);
    return buf;
}

inline void* StringPair::operator new(size_t /*size*/, void* buf)
{
    return buf;
}

//! Return the value part.
inline String& StringPair::v()
{
    return v_;
}

//! Return the key (aka name) part.
inline const String& StringPair::k() const
{
    return k_;
}

//! Return the name (aka key) part.
inline const String& StringPair::n() const
{
    return k_;
}

//! Return the value part.
inline const String& StringPair::v() const
{
    return v_;
}

inline const char* StringPair::vAsAscii() const
{
    return v_.ascii();
}

inline void StringPair::reset(const String& k)
{
    k_ = k;
    v_.reset();
}

inline void StringPair::reset(const String& k, const String& v)
{
    k_ = k;
    v_ = v;
}

//! Update the value part.
inline void StringPair::setV(const String& v)
{
    v_ = v;
}

//! Update the value part.
inline void StringPair::setV(const char* v)
{
    v_ = v;
}

//! Update the value part.
inline void StringPair::setV(unsigned short v)
{
    unsigned int u32 = v;
    setV(u32);
}

END_NAMESPACE1

#endif
