/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "appkit/StringVec.hpp"
#include "syskit/macros.h"

#include "netkit-pch.h"
#include "netkit/MacIntfVec.hpp"

using namespace appkit;

BEGIN_NAMESPACE1(netkit)


MacIntfVec::MacIntfVec(const MacIntfVec& vec):
U64Vec(vec)
{
}


//!
//! Construct an empty vector with initial capacity of capacity items.
//! The vector does not grow if growBy is zero, exponentially grows by
//! doubling if growBy is negative, and grows by growBy items otherwise.
//!
MacIntfVec::MacIntfVec(unsigned int capacity, int growBy):
U64Vec(capacity, growBy)
{
}


MacIntfVec::~MacIntfVec()
{
}


void MacIntfVec::toH64()
{
    MacIntf macIntf;
    for (size_t i = 0, numItems = MacIntfVec::numItems(); i < numItems; ++i)
    {
        const item_t& item = peek(i);
        macIntf = MacIntf::fromN64(item.asU64());
        U64Vec::setItem(i, macIntf.asU64());
    }
}


void MacIntfVec::toN64()
{
    for (size_t i = 0, numItems = MacIntfVec::numItems(); i < numItems; ++i)
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
bool MacIntfVec::reset(const StringVec& vec)
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
void MacIntfVec::sort(bool reverseOrder)
{
    toN64();
    U64Vec::sort(reverseOrder);
    toH64();
}

END_NAMESPACE1
