/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/U8.hpp"

BEGIN_NAMESPACE1(appkit)


U8::U8(const String* s, item_t defaultV)
{
    if ((s == 0) || s->empty())
    {
        v_ = defaultV;
    }
    else
    {
        size_t bytesUsed;
        unsigned int u32 = U32(s->ascii(), &bytesUsed);
        v_ = ((bytesUsed > 0) && (u32 <= 0xffU))? static_cast<item_t>(u32): defaultV;
    }
}


//!
//! Return true if given string (length characters starting at s) holds an unsigned
//! 8-bit number and nothing else.
//!
bool U8::isValid(const char* s, size_t length)
{
    const size_t MAX_LENGTH = 127;
    if ((length <= 0) || (length > MAX_LENGTH))
    {
        return false;
    }

    char* pEnd;
    int c = s[length - 1];
    if ((c == 0) || S32::isSpace(c))
    {
        unsigned int u32 = std::strtoul(s, &pEnd, 0);
        return ((pEnd == (s + length - 1)) && (u32 <= 0xffU));
    }

    // Use stack for temporary null-terminated string copy.
    char buf[MAX_LENGTH + 1];
    memcpy(buf, s, length);
    buf[length] = 0;
    unsigned int u32 = std::strtoul(buf, &pEnd, 0);
    return ((pEnd == (buf + length)) && (u32 <= 0xffU));
}


//!
//! Convert number to decimal digits.
//! Return number of decimal digits in the number.
//! Example:
//!\code
//!   U8::toDigits(124U, digit)=3 --digit[0]='1' digit[1]='2' digit[2]='4'
//!   U8::toDigits(63U, digit)=2  --digit[0]='6' digit[1]='3'
//!   U8::toDigits(0U, digit)=1   --digit[0]='0'
//!\endcode
//!
unsigned int U8::toDigits(item_t item, char digit[MaxDigits])
{

    // 0-9
    unsigned int rc;
    if (item < 10)
    {
        digit[0] = '0' + item;
        rc = 1;
    }

    // 10-99
    else if (item < 100)
    {
        div_t result = std::div(item, 10);
        digit[0] = static_cast<char>('0' + result.quot);
        digit[1] = static_cast<char>('0' + result.rem);
        rc = 2;
    }

    // 100-255
    else
    {
        div_t result = std::div(item, 100);
        digit[0] = static_cast<char>('0' + result.quot);
        digit[1] = static_cast<char>('0' + (result.rem / 10));
        digit[2] = '0' + (item % 10);
        rc = 3;
    }

    return rc;
}


void U8::binarize(unsigned char* u8, const char* xdigit, size_t numXdigits)
{
    for (size_t i = 0; i < numXdigits; i += 2)
    {
        char hiXdigit = xdigit[i];
        char loXdigit = xdigit[i + 1];
        *u8++ = (syskit::NIBBLE[hiXdigit - '0'] << 4U) | syskit::NIBBLE[loXdigit - '0'];
    }
}

END_NAMESPACE1
