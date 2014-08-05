/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_U16_HPP
#define APPKIT_U16_HPP

#include "appkit/String.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)


//! unsigned 16-bit number
class U16
    //!
    //! A class representing a 16-bit unsigned number. It also serves as a namespace
    //! for miscellaneous utilities using unsigned short as key arguments.
    //!
{

public:
    enum
    {
        MaxDigits = 5, //"0"-"65535"
        NumXdigits = 4 //"0000"-"ffff"
    };

    typedef unsigned short item_t;

    U16(const String& s);
    U16(const String& s, size_t* bytesUsed);
    U16(const String* s, item_t defaultV = 0);
    U16(const U16& item);
    U16(const char* s);
    U16(const char* s, size_t length);
    U16(const char* s, size_t length, size_t* bytesUsed);
    U16(const char* s, size_t* bytesUsed);
    U16(item_t v = 0);

    operator String() const;
    operator item_t() const;
    const U16& operator =(const U16& item);
    const U16& operator =(item_t v);

    String toString() const;
    String toXDIGITS() const;
    String toXdigits() const;
    item_t asWord() const;

    static bool isValid(const char* s);
    static bool isValid(const char* s, item_t validMin, item_t validMax);
    static bool isValid(const char* s, size_t length);
    static unsigned int numDigits(item_t item);
    static unsigned int toDigits(item_t item, char digit[MaxDigits]);
    static void toXDIGITS(item_t item, char xdigit[NumXdigits]);
    static void toXdigits(item_t item, char xdigit[NumXdigits]);

    static item_t checksum(const item_t* item, size_t numItems);
    static item_t checksum(const item_t* itemA, size_t numItemAs, const item_t* itemB, size_t numItemBs);

    static void bswap(const item_t* meti, size_t numMetis, item_t* item);
    static void bswap(item_t* item, size_t numItems);

private:
    item_t v_;

};

END_NAMESPACE1

#include "appkit/U32.hpp"
#include "appkit/U8.hpp"

BEGIN_NAMESPACE1(appkit)

//! Construct instance from given valid string.
inline U16::U16(const String& s)
{
    unsigned int u32 = U32(s);
    v_ = (u32 <= 0xffffU)? static_cast<item_t>(u32): 0;
}

//! Construct instance from given string. Also return the number of bytes
//! from given string utilized in the conversion process (zero indicates
//! given string could not be interpreted as an unsigned 16-bit integer).
inline U16::U16(const String& s, size_t* bytesUsed)
{
    unsigned int u32 = U32(s, bytesUsed);
    v_ = (u32 <= 0xffffU)? static_cast<item_t>(u32): 0;
}

inline U16::U16(const U16& item)
{
    v_ = item.v_;
}

//! Construct instance from given valid string.
inline U16::U16(const char* s)
{
    unsigned int u32 = U32(s);
    v_ = (u32 <= 0xffffU)? static_cast<item_t>(u32): 0;
}

//! Construct instance from given valid string (length bytes starting at s).
inline U16::U16(const char* s, size_t length)
{
    unsigned int u32 = U32(s, length);
    v_ = (u32 <= 0xffffU)? static_cast<item_t>(u32): 0;
}

//! Construct instance from given string (length bytes starting at s).
//! Also return the number of bytes from given string utilized in the
//! conversion process (zero indicates given string could not be
//! interpreted as an unsigned 16-bit integer).
inline U16::U16(const char* s, size_t length, size_t* bytesUsed)
{
    unsigned int u32 = U32(s, length, bytesUsed);
    v_ = (u32 <= 0xffffU)? static_cast<item_t>(u32): 0;
}

//! Construct instance from given string. Also return the number of bytes
//! from given string utilized in the conversion process (zero indicates
//! given string could not be interpreted as an unsigned 16-bit integer).
inline U16::U16(const char* s, size_t* bytesUsed)
{
    unsigned int u32 = U32(s, bytesUsed);
    v_ = (u32 <= 0xffffU)? static_cast<item_t>(u32): 0;
}

inline U16::U16(item_t v)
{
    v_ = v;
}

//! Return instance as a decimal string.
inline U16::operator String() const
{
    char digit[MaxDigits];
    size_t n = U32::toDigits(v_, digit);
    return String(digit, n);
}

//! Return instance as an unsigned 16-bit number.
inline U16::operator item_t() const
{
    return v_;
}

inline const U16& U16::operator =(const U16& item)
{
    v_ = item.v_;
    return *this;
}

//! Reset instance with given unsigned 16-bit number.
inline const U16& U16::operator =(item_t v)
{
    v_ = v;
    return *this;
}

//! Return instance as a decimal string.
inline String U16::toString() const
{
    char digit[MaxDigits];
    size_t n = U32::toDigits(v_, digit);
    return String(digit, n);
}

//! Convert to uppercase hex string. Return resulting string.
inline String U16::toXDIGITS() const
{
    char xdigit[NumXdigits];
    U8::toXDIGITS(static_cast<unsigned char>(v_ >> 8), xdigit);
    U8::toXDIGITS(static_cast<unsigned char>(v_), xdigit + 2);
    return String(xdigit, NumXdigits);
}

//! Convert to lowercase hex string. Return resulting string.
inline String U16::toXdigits() const
{
    char xdigit[NumXdigits];
    U8::toXdigits(static_cast<unsigned char>(v_ >> 8), xdigit);
    U8::toXdigits(static_cast<unsigned char>(v_), xdigit + 2);
    return String(xdigit, NumXdigits);
}

//! Return instance as an unsigned 16-bit number.
inline U16::item_t U16::asWord() const
{
    return v_;
}

//! Return true if given string holds an unsigned 16-bit number and nothing else.
inline bool U16::isValid(const char* s)
{
    size_t bytesUsed;
    unsigned int u32 = U32(s, &bytesUsed);
    return ((bytesUsed > 0) && (s[bytesUsed] == 0) && (u32 <= 0xffffU));
}

//! Return true if given string holds an unsigned 16-bit number and nothing else.
inline bool U16::isValid(const char* s, item_t validMin, item_t validMax)
{
    size_t bytesUsed;
    unsigned int u32 = U32(s, &bytesUsed);
    return ((bytesUsed > 0) && (s[bytesUsed] == 0) && (u32 >= validMin) && (u32 <= validMax));
}

//! Convert number to decimal digits.
//! Return number of decimal digits in the number.
inline unsigned int U16::toDigits(item_t item, char digit[MaxDigits])
{
    return U32::toDigits(item, digit);
}

//! Return the ASCII column width of given value if displayed as an
//! unsigned decimal number (%u). For example, 0..9 requires one
//! character, 10..99 requires two, and 100..999 requires three, etc.
inline unsigned int U16::numDigits(item_t item)
{
    return U32::numDigits(item);
}

//! Convert 16-bit number to uppercase hex digits.
//! For example, item=0xa1b2U --> xdigit[0]='A' xdigit[1]='1' xdigit[2]='B' xdigit[3]='2'.
inline void U16::toXDIGITS(item_t item, char xdigit[NumXdigits])
{
    U8::toXDIGITS(static_cast<unsigned char>(item >> 8), xdigit);
    U8::toXDIGITS(static_cast<unsigned char>(item), xdigit + 2);
}

//! Convert 16-bit number to lowercase hex digits.
//! For example, item=0xa1b2U --> xdigit[0]='a' xdigit[1]='1' xdigit[2]='b' xdigit[3]='2'.
inline void U16::toXdigits(item_t item, char xdigit[NumXdigits])
{
    U8::toXdigits(static_cast<unsigned char>(item >> 8), xdigit);
    U8::toXdigits(static_cast<unsigned char>(item), xdigit + 2);
}

END_NAMESPACE1

#endif
