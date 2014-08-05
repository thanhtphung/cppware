/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_STRING_LINUX_HPP
#define APPKIT_STRING_LINUX_HPP
#ifndef APPKIT_STRING_HPP
#error "unsupported usage"
#endif

BEGIN_NAMESPACE1(appkit)


//! Reset instance with given null-terminated wide-character string.
inline const String& String::operator =(const wchar_t* s)
{
    row();
    size_t numWchars = wcslen(s) + 1;
    s_->shrink(reinterpret_cast<const syskit::utf32_t*>(s), numWchars);
    return *this;
}

//! Reset instance with given wide-character string (numWchars wide-characters
//! starting at s).
inline void String::reset(const wchar_t* s, size_t numWchars)
{
    row();
    s_->shrink(reinterpret_cast<const syskit::utf32_t*>(s), numWchars);
    s_->addNullIfNone();
}

//! Return the character residing at given index.
inline wchar_t String::peek(size_t index) const
{
    return static_cast<wchar_t>((*s_)[index]);
}

END_NAMESPACE1

#endif
