/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "appkit/StringVec.hpp"
#include "syskit/macros.h"

#include "netkit-pch.h"
#include "netkit/MacAddrVec.hpp"

using namespace appkit;
using namespace syskit;

BEGIN_NAMESPACE1(netkit)


MacAddrVec::MacAddrVec(const MacAddrVec& vec):
U64Vec(vec)
{
}


//!
//! Construct an empty vector with initial capacity of capacity items.
//! The vector does not grow if growBy is zero, exponentially grows by
//! doubling if growBy is negative, and grows by growBy items otherwise.
//!
MacAddrVec::MacAddrVec(unsigned int capacity, int growBy):
U64Vec(capacity, growBy)
{
}


MacAddrVec::~MacAddrVec()
{
}


void MacAddrVec::toH64()
{
    MacAddr macAddr;
    for (size_t i = 0, numItems = MacAddrVec::numItems(); i < numItems; ++i)
    {
        const item_t& item = peek(i);
        macAddr = MacAddr::fromN64(item.asU64());
        U64Vec::setItem(i, macAddr.asU64());
    }
}


void MacAddrVec::toN64()
{
    for (size_t i = 0, numItems = MacAddrVec::numItems(); i < numItems; ++i)
    {
        const item_t& item = peek(i);
        unsigned long long n64 = item.toN64();
        U64Vec::setItem(i, n64);
    }
}


//!
//! Reset the vector with given item strings.
//! Return true if successful.
//!
bool MacAddrVec::reset(const StringVec& vec)
{
    int growBy = 0;
    unsigned int capacity = vec.numItems();
    U64Vec tmp(capacity, growBy);

    item_t item;
    for (size_t i = 0, numItems = vec.numItems(); i < numItems; tmp.add(item), ++i)
    {
        if (!item.reset(vec.peek(i).ascii()))
        {
            bool ok = false;
            return ok;
        }
    }

    U64Vec::operator =(tmp);
    bool ok = true;
    return ok;
}


//!
//! Sort vector. Order is ascending if reverseOrder is false and is descending
//! if reverseOrder is true.
//!
void MacAddrVec::sort(bool reverseOrder)
{
    toN64();
    U64Vec::sort(reverseOrder);
    toH64();
}

END_NAMESPACE1
