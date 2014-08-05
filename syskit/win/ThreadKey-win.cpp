/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "syskit-pch.h"
#include "syskit/ThreadKey.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)

const threadKey_t ThreadKey::INVALID_KEY = TLS_OUT_OF_INDEXES;


ThreadKey::ThreadKey()
{
    key_ = TlsAlloc();
}


ThreadKey::~ThreadKey()
{
    TlsFree(key_);
}


//!
//! Associate given value to this key for the calling thread.
//!
void ThreadKey::setValue(void* v)
{
    TlsSetValue(key_, v);
}


//!
//! Return the associated value for the calling thread.
//! Return zero if key is unassociated.
//!
void* ThreadKey::value() const
{
    void* v = TlsGetValue(key_);
    return v;
}

END_NAMESPACE1
