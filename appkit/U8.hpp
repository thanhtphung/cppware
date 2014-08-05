/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_U8_HPP
#define APPKIT_U8_HPP

#include <string.h>
#include "appkit/String.hpp"
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(appkit)


//! unsigned 8-bit number
class U8
    //!
    //! A class representing an 8-bit unsigned number. It also serves as a namespace
    //! for miscellaneous utilities using unsigned char as key arguments.
    //!
{

public:
    enum
    {
        MaxDigits = 3, //"0"-"255"
        NumXdigits = 2 //"00"-"ff"
    };

    typedef unsigned char item_t;

    U8(const String& s);
    U8(const String& s, size_t* bytesUsed);
    U8(const String* s, item_t defaultV = 0);
    U8(const U8& item);
    U8(const char* s);
    U8(const char* s, size_t length);
    U8(const char* s, size_t length, size_t* bytesUsed);
    U8(const char* s, size_t* bytesUsed);
    U8(item_t v = 0);

    operator String() const;
    operator item_t() const;
    const U8& operator =(const U8& item);
    const U8& operator =(item_t v);

    String toString() const;
    String toXDIGITS() const;
    String toXdigits() const;
    item_t asU8() const;

    static bool isValid(const char* s);
    static bool isValid(const char* s, item_t validMin, item_t validMax);
    static bool isValid(const char* s, size_t length);
    static unsigned int numDigits(item_t item);
    static unsigned int toDigits(item_t item, char digit[MaxDigits]);
    static void toXDIGITS(item_t item, char xdigit[NumXdigits]);
    static void toXdigits(item_t item, char xdigit[NumXdigits]);

    static unsigned char xdigitToNibble(char xdigit);
    static unsigned char xdigitsToU8(char hiXdigit, char loXdigit);
    static void binarize(unsigned char* u8, const char* xdigit, size_t numXdigits);

private:
    item_t v_;

};

END_NAMESPACE1

#include "appkit/U32.hpp"

BEGIN_NAMESPACE1(appkit)

//! Construct instance from given valid string.
inline U8::U8(const String& s)
{
    unsigned int u32 = U32(s);
    v_ = (u32 <= 0xffU)? static_cast<item_t>(u32): 0;
}

//! Construct instance from given string. Also return the number of bytes
//! from given string utilized in the conversion process (zero indicates
//! given string could not be interpreted as an 8-bit integer).
inline U8::U8(const String& s, size_t* bytesUsed)
{
    unsigned int u32 = U32(s, bytesUsed);
    v_ = (u32 <= 0xffU)? static_cast<item_t>(u32): 0;
}

inline U8::U8(const U8& item)
{
    v_ = item.v_;
}

//! Construct instance from given valid string.
inline U8::U8(const char* s)
{
    unsigned int u32 = U32(s);
    v_ = (u32 <= 0xffU)? static_cast<item_t>(u32): 0;
}

//! Construct instance from given valid string (length bytes starting at s).
inline U8::U8(const char* s, size_t length)
{
    unsigned int u32 = U32(s, length);
    v_ = (u32 <= 0xffU)? static_cast<item_t>(u32): 0;
}

//! Construct instance from given string (length bytes starting at s).
//! Also return the number of bytes from given string utilized in the
//! conversion process (zero indicates given string could not be
//! interpreted as an 8-bit integer).
inline U8::U8(const char* s, size_t length, size_t* bytesUsed)
{
    unsigned int u32 = U32(s, length, bytesUsed);
    v_ = (u32 <= 0xffU)? static_cast<item_t>(u32): 0;
}

inline U8::U8(item_t v)
{
    v_ = v;
}

//! Return instance as a decimal string.
inline U8::operator String() const
{
    char digit[MaxDigits];
    size_t n = U32::toDigits(v_, digit);
    return String(digit, n);
}

//! Return instance as an unsigned 8-bit number.
inline U8::operator item_t() const
{
    return v_;
}

inline const U8& U8::operator =(const U8& item)
{
    v_ = item.v_;
    return *this;
}

//! Reset instance with given unsigned 8-bit number.
inline const U8& U8::operator =(item_t v)
{
    v_ = v;
    return *this;
}

//! Return instance as a decimal string.
inline String U8::toString() const
{
    char digit[MaxDigits];
    size_t n = U32::toDigits(v_, digit);
    return String(digit, n);
}

//! Convert to uppercase hex string. Return resulting string.
inline String U8::toXDIGITS() const
{
    char xdigit[NumXdigits];
    xdigit[0] = syskit::XDIGIT[v_ >> 4U];
    xdigit[1] = syskit::XDIGIT[v_ & 0x0fU];
    return String(xdigit, NumXdigits);
}

//! Convert to lowercase hex string. Return resulting string.
inline String U8::toXdigits() const
{
    char xdigit[NumXdigits];
    xdigit[0] = syskit::xdigit[v_ >> 4U];
    xdigit[1] = syskit::xdigit[v_ & 0x0fU];
    return String(xdigit, NumXdigits);
}

//! Return instance as an unsigned 8-bit number.
inline U8::item_t U8::asU8() const
{
    return v_;
}

//! Return true if given string holds an unsigned 8-bit number and nothing else.
inline bool U8::isValid(const char* s)
{
    size_t bytesUsed;
    unsigned int u32 = U32(s, &bytesUsed);
    return ((bytesUsed > 0) && (s[bytesUsed] == 0) && (u32 <= 0xffU));
}

//! Return true if given string holds an unsigned 8-bit number and nothing else.
inline bool U8::isValid(const char* s, item_t validMin, item_t validMax)
{
    size_t bytesUsed;
    unsigned int u32 = U32(s, &bytesUsed);
    return ((bytesUsed > 0) && (s[bytesUsed] == 0) && (u32 >= validMin) && (u32 <= validMax));
}

//! Return the ASCII column width of given value if displayed as an
//! unsigned decimal number (%u). For example, 0..9 requires one
//! character, 10..99 requires two, and 100..999 requires three, etc.
inline unsigned int U8::numDigits(item_t item)
{
    return U32::numDigits(item);
}

//! Convert 8-bit number to uppercase hex digits.
//! For example, item=0xabU --> xdigit[0]='A' xdigit[1]='B'.
inline void U8::toXDIGITS(item_t item, char xdigit[NumXdigits])
{
    xdigit[0] = syskit::XDIGIT[item >> 4U];
    xdigit[1] = syskit::XDIGIT[item & 0x0fU];
}

//! Convert 8-bit number to lowercase hex digits.
//! For example, item=0xabU --> xdigit[0]='a' xdigit[1]='b'.
inline void U8::toXdigits(item_t item, char xdigit[NumXdigits])
{
    xdigit[0] = syskit::xdigit[item >> 4U];
    xdigit[1] = syskit::xdigit[item & 0x0fU];
}

//! Convert hex digit to nibble. For example, xdigitToNibble('a')
//! returns 0x0aU, and xdigitToNibble('C') returns 0x0cU.
inline unsigned char U8::xdigitToNibble(char xdigit)
{
    return syskit::NIBBLE[xdigit - '0'];
}

//! Convert hex digits to byte. For example, xdigitsToU8('a','b')
//! returns 0xabU, and xdigitsToU8('C','D') returns 0xcdU.
inline unsigned char U8::xdigitsToU8(char hiXdigit, char loXdigit)
{
    return (syskit::NIBBLE[hiXdigit - '0'] << 4U) | syskit::NIBBLE[loXdigit - '0'];
}

END_NAMESPACE1

#endif
