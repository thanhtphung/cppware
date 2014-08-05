/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/Bool.hpp"
#include "appkit/D64.hpp"
#include "appkit/F32.hpp"
#include "appkit/S32.hpp"
#include "appkit/StringPair.hpp"
#include "appkit/U32.hpp"
#include "appkit/U64.hpp"
#include "appkit/U16.hpp"

BEGIN_NAMESPACE1(appkit)


//!
//! Construct name-value pair with given name and an empty value.
//!
StringPair::StringPair(const String& k):
k_(k),
v_()
{
}


//!
//! Construct name-value pair with given name and value.
//!
StringPair::StringPair(const String& k, const String& v):
k_(k),
v_(v)
{
}


//!
//! Construct a duplicate instance of the given name-value pair.
//!
StringPair::StringPair(const StringPair& kv):
k_(kv.k_),
v_(kv.v_)
{
}


StringPair::~StringPair()
{
}


const StringPair& StringPair::operator =(const StringPair& kv)
{

    // Prevent self assignment.
    if (this != &kv)
    {
        k_ = kv.k_;
        v_ = kv.v_;
    }

    // Return reference to self.
    return *this;
}


//!
//! Return the value part.
//!
bool StringPair::vAsBool() const
{
    Bool b(v_);
    return b;
}


//!
//! Return the value part.
//!
double StringPair::vAsD64() const
{
    D64 d64(v_);
    return d64;
}


//!
//! Return the value part.
//!
float StringPair::vAsF32() const
{
    F32 f32(v_);
    return f32;
}


//!
//! Return the value part.
//!
int StringPair::vAsS32() const
{
    S32 s32(v_);
    return s32;
}


//!
//! Return the value part.
//!
unsigned int StringPair::vAsU32() const
{
    U32 u32(v_);
    return u32;
}


//!
//! Return the value part.
//!
unsigned long long StringPair::vAsU64() const
{
    U64 u64(v_);
    return u64;
}


//!
//! Return the value part.
//!
unsigned short StringPair::vAsU16() const
{
    U16 u16(v_);
    return u16;
}


void StringPair::addV(const String& v, char delim)
{
    v_ += delim;
    v_ += v;
}


//!
//! Update the value part.
//!
void StringPair::setV(bool v)
{
    Bool b(v);
    v_ = b.toString();
}


//!
//! Update the value part.
//!
void StringPair::setV(double v)
{
    D64 d64(v);
    v_ = d64.toString();
}


//!
//! Update the value part.
//!
void StringPair::setV(float v)
{
    F32 f32(v);
    v_ = f32.toString();
}


//!
//! Update the value part.
//!
void StringPair::setV(int v)
{
    S32 s32(v);
    v_ = s32.toString();
}


//!
//! Update the value part.
//!
void StringPair::setV(unsigned int v)
{
    U32 u32(v);
    v_ = u32.toString();
}


//!
//! Update the value part.
//!
void StringPair::setV(unsigned long long v)
{
    U64 u64(v);
    v_ = u64.toString();
}

END_NAMESPACE1
