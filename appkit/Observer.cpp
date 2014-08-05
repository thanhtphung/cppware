/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/sys.hpp"

#include "appkit-pch.h"
#include "appkit/Observer.hpp"

using namespace syskit;

BEGIN_NAMESPACE1(appkit)


//!
//! Construct observer.
//!
Observer::Observer(const char* name, unsigned int initialRefCount):
RefCounted(initialRefCount),
updateCount_(0U)
{
    name_ = syskit::strdup(name);
}


Observer::~Observer()
{
    delete[] name_;
}


bool Observer::operator !=(const Observer& observer) const
{
    bool ne = (this != &observer);
    return ne;
}


bool Observer::operator ==(const Observer& observer) const
{
    bool eq = (this == &observer);
    return eq;
}

END_NAMESPACE1
