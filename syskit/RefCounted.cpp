/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/RefCounted.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//!
//! Construct a reference-counted instance with given initial reference count.
//! A reference-counted instance is destroyed when its reference count reaches
//! zero via a rmRef() method.
//!
RefCounted::RefCounted(unsigned int initialRefCount):
count_(initialRefCount)
{
}


RefCounted::~RefCounted()
{
}


//!
//! Clone instance. Return cloned instance. The default implementation is a no-op
//! returning zero to indicate cloning is unsupported.
//!
RefCounted* RefCounted::clone() const
{
    return 0;
}


//!
//! Decrement reference count. Destroy instance if its reference count reaches
//! zero after decrementing. Return true if instance is destroyed because of
//! this decrement operation.
//!
bool RefCounted::rmRef() const
{
    bool destroyed;
    if (--count_ == 0U)
    {
        destroy();
        destroyed = true;
    }
    else
    {
        destroyed = false;
    }

    return destroyed;
}


//!
//! Decrement reference count by given delta. Destroy instance if its reference
//! count reaches zero after decrementing. Return true if instance is destroyed
//! because of this decrement operation.
//!
bool RefCounted::rmRef(unsigned int delta) const
{
    bool destroyed;
    unsigned int oldRefCount;
    count_.decrementBy(delta, oldRefCount);
    if (oldRefCount == delta)
    {
        destroy();
        destroyed = true;
    }
    else
    {
        destroyed = false;
    }

    return destroyed;
}


//!
//! Destroy instance. The default implementation destroys the instance via the
//! delete operator.
//!
void RefCounted::destroy() const
{
    delete this;
}


//!
//! Construct a count against the given reference-counted instance.
//! Increment reference count at construction, and decrement reference
//! count at destruction.
//!
RefCounted::Count::Count(const RefCounted& refCounted):
refCounted_(refCounted)
{
    refCounted_.addRef();
}


//!
//! Construct a count against the given reference-counted instance.
//! Skip incrementing reference count at construction, but do still
//! decrement reference count at destruction.
//!
RefCounted::Count::Count(const RefCounted& refCounted, bool skipAddRef):
refCounted_(refCounted)
{
    if (!skipAddRef)
    {
        refCounted_.addRef();
    }
}


RefCounted::Count::~Count()
{
    refCounted_.rmRef();
}

END_NAMESPACE1
