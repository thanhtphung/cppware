/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_SINGLETON_HPP
#define SYSKIT_SINGLETON_HPP

#include "syskit/RefCounted.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! per-process singleton
class Singleton: public virtual RefCounted
{

public:
    typedef Singleton* (*create_t)(const char* id, unsigned int initialRefCount, void* arg);

    const char* id() const;

    static Singleton* getSingleton(const char* id);
    static Singleton* getSingleton(const char* id, create_t create, unsigned int initialRefCount, void* createArg);

protected:
    Singleton(const char* id, unsigned int initialRefCount = 0U);
    virtual ~Singleton();

private:
    const char* id_;

    Singleton(const Singleton&); //prohibit usage
    const Singleton& operator =(const Singleton&); //prohibit usage

    static int diff(const void*, const void*);

};

inline const char* Singleton::id() const
{
    return id_;
}

END_NAMESPACE1

#endif
