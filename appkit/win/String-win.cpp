/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/Utf8.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/String.hpp"

using namespace syskit;

BEGIN_NAMESPACE1(appkit)


//!
//! Construct an instance initialized with given null-terminated wide-character string.
//!
String::String(const wchar_t* s)
{
    if (*s != 0)
    {
        s_ = new S;
        size_t numWchars = 0; //TBD
        size_t numU16s = wcslen(s) + 1;
        s_->reset(reinterpret_cast<const utf16_t*>(s), numU16s, numWchars);
    }
    else
    {
        s_ = emptyStringRef();
    }
}


//!
//! Construct an instance initialized with given wide-character string
//! (numWchars wide-characters starting at s).
//!
String::String(const wchar_t* s, size_t numWchars)
{
    if (numWchars > 0)
    {
        s_ = new S;
        size_t numU16s = 0; //TBD
        s_->reset(reinterpret_cast<const utf16_t*>(s), numU16s, numWchars);
        s_->addNullIfNone();
    }
    else
    {
        s_ = emptyStringRef();
    }
}


//!
//! Return string as an array of null-terminated wide characters.
//!
String::W String::widen() const
{
    wchar_t* w;
    if (s_->isAscii())
    {
        w = new wchar_t[s_->byteSize()];
        const utf8_t* p8 = s_->raw();
        const utf8_t* p8End = p8 + s_->byteSize();
        for (wchar_t* p = w; p8 < p8End; *p++ = *p8++);
    }
    else
    {
        unsigned int capacity = s_->byteSize() << 1;
        Utf16Seq seq16(capacity);
        seq16.reset(s_->raw(), s_->byteSize(), s_->numChars());
        w = reinterpret_cast<wchar_t*>(seq16.detachRaw());
    }

    return w;
}

END_NAMESPACE1
