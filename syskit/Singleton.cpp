/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <string.h>

#include "syskit-pch.h"
#include "syskit/CriSection.hpp"
#include "syskit/Foundation.hpp"
#include "syskit/Singleton.hpp"
#include "syskit/Vec.hpp"
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(syskit)


//!
//! Construct instance with given identifier. This per-process singleton can be located
//! during its lifetime using the getSingleton() methods. The number of per-process
//! singletons is assumed to be relatively small, and use of linear search in this class
//! should be sufficient.
//!
Singleton::Singleton(const char* id, unsigned int initialRefCount):
RefCounted(initialRefCount)
{
    id_ = syskit::strdup(id);

    // Singleton construction must be through getSingleton(). The singleton vector
    // is already locked at this point.
    const Foundation& r = Foundation::instance();
    Vec* vec = r.singletonVec();
    vec->add(this);
}


Singleton::~Singleton()
{
    CriSection* cs;
    const Foundation& r = Foundation::instance();
    Vec* vec = r.singletonVec(cs);
    {
        CriSection::Lock lock(*cs);
        vec->rm(this, false /*maintainOrder*/);
    }

    delete[] id_;
}


//!
//! Locate given singleton. Return the per-process singleton. If non-existent, return zero.
//!
Singleton* Singleton::getSingleton(const char* id)
{
    CriSection* cs;
    const Foundation& r = Foundation::instance();
    Vec* vec = r.singletonVec(cs);
    CriSection::Lock lock(*cs);

    size_t foundIndex;
    bool found = vec->find(id, diff, foundIndex);
    Singleton* singleton = found? static_cast<Singleton*>(vec->peek(foundIndex)): 0;
    return singleton;
}


//!
//! Locate given singleton. If non-existent, create one. Return the per-process singleton.
//!
Singleton* Singleton::getSingleton(const char* id, create_t create, unsigned int initialRefCount, void* createArg)
{
    CriSection* cs;
    const Foundation& r = Foundation::instance();
    Vec* vec = r.singletonVec(cs);
    CriSection::Lock lock(*cs);

    size_t foundIndex;
    bool found = vec->find(id, diff, foundIndex);
    Singleton* singleton = found? static_cast<Singleton*>(vec->peek(foundIndex)): create(id, initialRefCount, createArg);
    return singleton;
}


//
// Compare opaque items looking for matching singleton. Treat first item as a singleton
// identifier. Treat second item as a pointer to a Singleton instance. Return non-zero if
// the items differ.
//
int Singleton::diff(const void* item0, const void* item1)
{
    const char* k0 = static_cast<const char*>(item0);
    const char* k1 = static_cast<const Singleton*>(item1)->id_;
    return strcmp(k0, k1);
}

END_NAMESPACE1
