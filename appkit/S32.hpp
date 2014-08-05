/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_S32_HPP
#define APPKIT_S32_HPP

#include "appkit/String.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)


//! signed 32-bit number
class S32
    //!
    //! A class representing a 32-bit signed number. It also serves as a namespace
    //! for miscellaneous utilities using int as key arguments. A traditional
    //! no-locale ctype.h is provided. The traditional "int isxxx(int c)" signature
    //! is replaced with the "bool S32::isXxx(int c)" signature, but the behavior
    //! is identical. The modern c++ ctype.h is no longer efficient due to locale
    //! support, and these replacements should be used if locale support is not
    //! needed.
    //!
{

public:
    typedef int item_t;

    S32(const String& s);
    S32(const String& s, size_t* bytesUsed);
    S32(const String* s, item_t defaultV = 0);
    S32(const S32& item);
    S32(const char* s);
    S32(const char* s, size_t length);
    S32(const char* s, size_t length, size_t* bytesUsed);
    S32(const char* s, size_t* bytesUsed);
    S32(item_t v = 0);

    operator String() const;
    operator item_t() const;
    const S32& operator =(const S32& item);
    const S32& operator =(item_t v);

    String toString() const;
    item_t asS32() const;

    static bool isAlnum(item_t c);
    static bool isAlpha(item_t c);
    static bool isAscii(item_t c);
    static bool isCntrl(item_t c);
    static bool isDigit(item_t c);
    static bool isGraph(item_t c);
    static bool isLower(item_t c);
    static bool isPrint(item_t c);
    static bool isPunct(item_t c);
    static bool isSpace(item_t c);
    static bool isUpper(item_t c);
    static bool isXdigit(item_t c);

    static bool isValid(const char* s);
    static bool isValid(const char* s, item_t validMin, item_t validMax);
    static bool isValid(const char* s, size_t length);
    static item_t toLower(item_t c);
    static item_t toUpper(item_t c);

private:
    enum
    {
        BlankM = 0x01,
        CntrlM = 0x02,
        DigitM = 0x04,
        LowerM = 0x08,
        PunctM = 0x10,
        SpaceM = 0x20,
        UpperM = 0x40,
        XdigitM = 0x80,

        AlphaM = (LowerM | UpperM),
        AlnumM = (DigitM | LowerM | UpperM),
        GraphM = (DigitM | LowerM | PunctM | UpperM),
        PrintM = (BlankM | DigitM | LowerM | PunctM | UpperM)
    };

    item_t v_;

    static const char lower_[];
    static const char upper_[];
    static const unsigned char* const mask_;

    static item_t toS32(const char*, size_t, size_t*);

};

END_NAMESPACE1

#include <cstdlib>

BEGIN_NAMESPACE1(appkit)

//! Construct instance from given valid string.
inline S32::S32(const String& s)
{
    v_ = std::strtol(s.ascii(), 0, 0);
}

//! Construct instance from given string. Also return the number of bytes
//! from given string utilized in the conversion process (zero indicates
//! given string could not be interpreted as a signed 32-bit integer).
inline S32::S32(const String& s, size_t* bytesUsed)
{
    char* pEnd;
    const char* p = s.ascii();
    v_ = std::strtol(p, &pEnd, 0);
    *bytesUsed = pEnd - p;
}

inline S32::S32(const S32& item)
{
    v_ = item.v_;
}

//! Construct instance from given valid string.
inline S32::S32(const char* s)
{
    v_ = std::strtol(s, 0, 0);
}

//! Construct instance from given valid string (length bytes starting at s).
inline S32::S32(const char* s, size_t length)
{
    size_t bytesUsed;
    v_ = toS32(s, length, &bytesUsed);
}

//! Construct instance from given string (length bytes starting at s).
//! Also return the number of bytes from given string utilized in the
//! conversion process (zero indicates given string could not be
//! interpreted as a signed 32-bit integer).
inline S32::S32(const char* s, size_t length, size_t* bytesUsed)
{
    v_ = toS32(s, length, bytesUsed);
}

//! Construct instance from given string. Also return the number of bytes
//! from given string utilized in the conversion process (zero indicates
//! given string could not be interpreted as a signed 32-bit integer).
inline S32::S32(const char* s, size_t* bytesUsed)
{
    char* pEnd;
    v_ = std::strtol(s, &pEnd, 0);
    *bytesUsed = pEnd - s;
}

inline S32::S32(item_t v)
{
    v_ = v;
}

//! Return instance as a decimal string.
inline S32::operator String() const
{
    return toString();
}

//! Return instance as a signed 32-bit number.
inline S32::operator item_t() const
{
    return v_;
}

inline const S32& S32::operator =(const S32& item)
{
    v_ = item.v_;
    return *this;
}

//! Reset instance with given signed 32-bit number.
inline const S32& S32::operator =(item_t v)
{
    v_ = v;
    return *this;
}

//! Return instance as a signed 32-bit number.
inline S32::item_t S32::asS32() const
{
    return v_;
}

inline bool S32::isAlnum(item_t c)
{
    return (mask_[c] & AlnumM) != 0;
}

inline bool S32::isAlpha(item_t c)
{
    return (mask_[c] & AlphaM) != 0;
}

inline bool S32::isAscii(item_t c)
{
    return static_cast<unsigned int>(c) < 0x80U;
}

inline bool S32::isCntrl(item_t c)
{
    return (mask_[c] & CntrlM) != 0;
}

inline bool S32::isDigit(item_t c)
{
    return (mask_[c] & DigitM) != 0;
}

inline bool S32::isGraph(item_t c)
{
    return (mask_[c] & GraphM) != 0;
}

inline bool S32::isLower(item_t c)
{
    return (mask_[c] & LowerM) != 0;
}

inline bool S32::isPrint(item_t c)
{
    return (mask_[c] & PrintM) != 0;
}

inline bool S32::isPunct(item_t c)
{
    return (mask_[c] & PunctM) != 0;
}

inline bool S32::isSpace(item_t c)
{
    return (mask_[c] & SpaceM) != 0;
}

inline bool S32::isUpper(item_t c)
{
    return (mask_[c] & UpperM) != 0;
}

//! Return true if given string holds a signed 32-bit number and nothing else.
inline bool S32::isValid(const char* s)
{
    char* pEnd;
    std::strtol(s, &pEnd, 0);
    return ((pEnd != s) && (*pEnd == 0));
}

//! Return true if given string holds a signed 32-bit number and nothing else.
inline bool S32::isValid(const char* s, item_t validMin, item_t validMax)
{
    char* pEnd;
    item_t v = std::strtol(s, &pEnd, 0);
    return ((pEnd != s) && (*pEnd == 0) && (v >= validMin) && (v <= validMax));
}

inline bool S32::isXdigit(item_t c)
{
    return (mask_[c] & XdigitM) != 0;
}

inline S32::item_t S32::toLower(item_t c)
{
    return (static_cast<unsigned int>(c) < 0x80U)? lower_[c]: c;
}

inline S32::item_t S32::toUpper(item_t c)
{
    return (static_cast<unsigned int>(c) < 0x80U)? upper_[c]: c;
}

END_NAMESPACE1

#endif
