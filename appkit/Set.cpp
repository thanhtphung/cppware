/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/Set.hpp"

using namespace syskit;

BEGIN_NAMESPACE1(appkit)


//!
//! Construct a duplicate instance of the given set.
//!
Set::Set(const Set& set):
Growable(set)
{
    type_ = set.type_;
}


//!
//! Construct an empty set with initial capacity of capacity
//! ranges. The set grows by doubling. If given capacity is zero,
//! then an initial capacity of one will be used instead. type
//! is a free-format short ASCII string. It is intended to be used to
//! categorize different types of set for troubleshooting. A shallow
//! copy of type is held in the instance.
//!
Set::Set(const char* type, unsigned int capacity):
Growable(capacity, -1 /*growBy*/)
{
    type_ = type;
}


//!
//! Destruct set.
//!
Set::~Set()
{
}


//!
//! Assignment operator.
//!
const Set& Set::operator =(const Set& set)
{

    // Type is set once when constructed and is not affected by the assignment operator.
    Growable::operator =(set);
    return *this;
}


//!
//! Manage growth. A Set instance grows by doubling and cannot be
//! customized. That is, the growth factor is negative and cannot be
//! changed. Return true if given growth factor is negative. Return
//! false otherwise.
//!
bool Set::setGrowth(int growBy)
{
    return (growBy < 0);
}

END_NAMESPACE1
