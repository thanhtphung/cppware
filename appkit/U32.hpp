/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_U32_HPP
#define APPKIT_U32_HPP

#include "appkit/String.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)


//! unsigned 32-bit number
class U32
    //!
    //! A class representing a 32-bit unsigned number. It also serves as a namespace
    //! for miscellaneous utilities using unsigned int as key arguments. Comparison
    //! function naming hints:
    //!\code
    //!     K: compare keys
    //!    KR: compare keys, reverse the sense of comparison
    //!     P: compare keys given pointers
    //!    PR: compare keys given pointers, reverse the sense of comparison
    //!\endcode
    //!
{

public:
    enum
    {
        MaxDigits = 10, //"0"-"4294967295"
        NumXdigits = 8 //"00000000"-"ffffffff"
    };

    typedef unsigned int item_t;

    U32(const String& s);
    U32(const String& s, size_t* bytesUsed);
    U32(const String* s, item_t defaultV = 0);
    U32(const U32& item);
    U32(const char* s);
    U32(const char* s, size_t length);
    U32(const char* s, size_t length, size_t* bytesUsed);
    U32(const char* s, size_t* bytesUsed);
    U32(item_t v = 0);

    operator String() const;
    operator item_t() const;
    const U32& operator =(const U32& item);
    const U32& operator =(item_t v);

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

    static int compareK(const void* item0, const void* item1);
    static int compareKR(const void* item0, const void* item1);
    static int compareP(const void* item0, const void* item1);
    static int comparePR(const void* item0, const void* item1);
    static unsigned int hashK(const void* item, size_t numBuckets);
    static unsigned int hashP(const void* item, size_t numBuckets);

    static void bswap(const item_t* meti, size_t numMetis, item_t* item);
    static void bswap(item_t* item, size_t numItems);

private:
    item_t v_;

    static item_t toU32(const char*, size_t, size_t*);

};

END_NAMESPACE1

#include "appkit/S32.hpp"

BEGIN_NAMESPACE1(appkit)

//! Construct instance from given valid string.
inline U32::U32(const String& s)
{
    v_ = std::strtoul(s.ascii(), 0, 0);
}

//! Construct instance from given string. Also return the number of bytes
//! from given string utilized in the conversion process (zero indicates
//! given string could not be interpreted as an unsigned 32-bit integer).
inline U32::U32(const String& s, size_t* bytesUsed)
{
    char* pEnd;
    const char* p = s.ascii();
    v_ = std::strtoul(p, &pEnd, 0);
    *bytesUsed = pEnd - p;
}

inline U32::U32(const U32& item)
{
    v_ = item.v_;
}

//! Construct instance from given valid string.
inline U32::U32(const char* s)
{
    v_ = std::strtoul(s, 0, 0);
}

//! Construct instance from given valid string (length bytes starting at s).
inline U32::U32(const char* s, size_t length)
{
    size_t bytesUsed;
    v_ = toU32(s, length, &bytesUsed);
}

//! Construct instance from given string (length bytes starting at s).
//! Also return the number of bytes from given string utilized in the
//! conversion process (zero indicates given string could not be
//! interpreted as an unsigned 32-bit integer).
inline U32::U32(const char* s, size_t length, size_t* bytesUsed)
{
    v_ = toU32(s, length, bytesUsed);
}

//! Construct instance from given string. Also return the number of bytes
//! from given string utilized in the conversion process (zero indicates
//! given string could not be interpreted as an unsigned 32-bit integer).
inline U32::U32(const char* s, size_t* bytesUsed)
{
    char* pEnd;
    v_ = std::strtoul(s, &pEnd, 0);
    *bytesUsed = pEnd - s;
}

inline U32::U32(item_t v)
{
    v_ = v;
}

//! Return instance as a decimal string.
inline U32::operator String() const
{
    char digit[MaxDigits];
    size_t n = toDigits(v_, digit);
    return String(digit, n);
}

//! Return the unsigned int value.
inline U32::operator item_t() const
{
    return v_;
}

inline const U32& U32::operator =(const U32& item)
{
    v_ = item.v_;
    return *this;
}

//! Reset instance with given unsigned 32-bit number.
inline const U32& U32::operator =(item_t v)
{
    v_ = v;
    return *this;
}

//! Return instance as a decimal string.
inline String U32::toString() const
{
    char digit[MaxDigits];
    size_t n = toDigits(v_, digit);
    return String(digit, n);
}

//! Convert to uppercase hex string. Return resulting string.
inline String U32::toXDIGITS() const
{
    char xdigit[NumXdigits];
    toXDIGITS(v_, xdigit);
    return String(xdigit, NumXdigits);
}

//! Convert to lowercase hex string. Return resulting string.
inline String U32::toXdigits() const
{
    char xdigit[NumXdigits];
    toXdigits(v_, xdigit);
    return String(xdigit, NumXdigits);
}

//! Return the unsigned int value.
inline U32::item_t U32::asWord() const
{
    return v_;
}

//! Return true if given string holds an unsigned 32-bit number and nothing else.
inline bool U32::isValid(const char* s)
{
    char* pEnd;
    std::strtoul(s, &pEnd, 0);
    return ((pEnd != s) && (*pEnd == 0));
}

//! Return true if given string holds an unsigned 32-bit number and nothing else.
inline bool U32::isValid(const char* s, item_t validMin, item_t validMax)
{
    char* pEnd;
    item_t v = std::strtoul(s, &pEnd, 0);
    return ((pEnd != s) && (*pEnd == 0) && (v >= validMin) && (v <= validMax));
}

END_NAMESPACE1

#endif
