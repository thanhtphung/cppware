/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/CriSection.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//!
//! Construct a lock for given critical section. Wait forever if necessary.
//! Given critical section is locked after the lock construction and remains
//! locked until the lock is destructed.
//!
CriSection::Lock::Lock(CriSection& criSection):
criSection_(criSection)
{
    criSection_.lock();
}


CriSection::Lock::~Lock()
{
    criSection_.unlock();
}

END_NAMESPACE1
