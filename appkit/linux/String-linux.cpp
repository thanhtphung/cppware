/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "appkit/String.hpp"
#include "syskit/sys.hpp"

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
        size_t numWchars = wcslen(s) + 1;
        s_->shrink(reinterpret_cast<const utf32_t*>(s), numWchars);
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
        s_->shrink(reinterpret_cast<const utf32_t*>(s), numWchars);
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
        w = reinterpret_cast<wchar_t*>(s_->expand());
    }

    return w;
}

END_NAMESPACE1
