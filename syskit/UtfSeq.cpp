/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/UtfSeq.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//!
//! Construct a duplicate instance of the given sequence.
//!
UtfSeq::UtfSeq(const UtfSeq& seq):
Growable(seq)
{
    byteSize_ = seq.byteSize_;
    numChars_ = seq.numChars_;
}


//!
//! Construct a sequence with given characteristics. Sequence exponentially
//! grows by doubling. If given capacity is zero, then an initial capacity of
//! one will be used instead.
//!
UtfSeq::UtfSeq(unsigned int capacity, size_t byteSize, size_t numChars):
Growable(capacity, -1 /*growBy*/)
{
    setLength(byteSize, numChars);
}


UtfSeq::~UtfSeq()
{
}


//!
//! Grow until the sequence can handle given capacity.
//! Return true.
//!
bool UtfSeq::growTo(unsigned int minCap)
{
    unsigned int newCap = nextCap(minCap);
    resize(newCap);
    return true;
}


//!
//! Manage growth. A UTFx sequence exponentially grows by doubling and
//! cannot be customized. That is, the growth factor is negative and
//! cannot be changed. Return true if given growth factor is negative.
//! Return false otherwise.
//!
bool UtfSeq::setGrowth(int growBy)
{
    return (growBy < 0);
}

END_NAMESPACE1
