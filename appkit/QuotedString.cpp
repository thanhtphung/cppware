/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/Utf8.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/QuotedString.hpp"
#include "appkit/S32.hpp"
#include "appkit/U8.hpp"

using namespace syskit;

const char BACKSLASH = '\\';
const char DOUBLE_QUOTE = '"';
const char SINGLE_QUOTE = '\'';

inline const utf8_t* findFirst(const utf8_t* s, utf32_t c)
{
    return reinterpret_cast<const utf8_t*>(strchr(reinterpret_cast<const char*>(s), static_cast<char>(c)));
}

BEGIN_NAMESPACE1(appkit)


QuotedString::QuotedString(const QuotedString& quotedString):
quoted_(quotedString.quoted_)
{
}


//!
//! Enquote given string inside given quotes. If a character has a pre-defined simple escape
//! sequence, then its escape sequence will be used. For example: <abc'"123> --> <"abc\'\"123">.
//! The quote value is typically a single quote or a double quote. If it's zero, no surrounding
//! quotes will be added, but special characters will still be replaced with their known escape
//! sequences. To skip escape sequence usage in a single quoted string, set escapeSingleQuotedString
//! to false.
//!
QuotedString::QuotedString(const String& str, char quote, bool escapeSingleQuotedString):
quoted_(str)
{
    enquote(quoted_, quote, escapeSingleQuotedString);
}


QuotedString::~QuotedString()
{
}


const QuotedString& QuotedString::operator =(const QuotedString& quotedString)
{
    if (this != &quotedString)
    {
        quoted_ = quotedString.quoted_;
    }

    return *this;
}


//!
//! Treat instance as a C++ string literal and remove surrounding quotes (if any) in
//! addition to resolving escape sequences. For example, <"abc\'\"123"> --> <abc'"123>.
//! Return dequoted string. For a single quoted string, escape sequence resolution can
//! be skipped if unescapeSingleQuotedString is false.
//!
String QuotedString::dequote(bool unescapeSingleQuotedString) const
{
    String str(quoted_);
    dequote(str, unescapeSingleQuotedString);
    return str;
}


String QuotedString::escape(const String& native)
{

    // Look for pre-defined special characters with simple escape sequences.
    // Copy characters in bulk when possible.
    String result;
    Utf8 c;
    const utf8_t* p = native.raw();
    const utf8_t* p0 = 0;
    size_t numChars = 0;
    size_t numU8s = 0;
    for (size_t bytesDecoded;; p += bytesDecoded, ++numChars, numU8s += bytesDecoded)
    {

        bytesDecoded = c.decode(p);
        if (c == 0)
        {
            if (p0)
            {
                result.append(p0, numU8s, numChars);
            }
            break;
        }

        const char* esc;
        switch (c)
        {
        case '\?': esc = "\\\?"; break;
        case '\'': esc = "\\\'"; break;
        case '\"': esc = "\\\""; break;
        case '\\': esc = "\\\\"; break;
        case '\a': esc = "\\a"; break;
        case '\b': esc = "\\b"; break;
        case '\f': esc = "\\f"; break;
        case '\n': esc = "\\n"; break;
        case '\r': esc = "\\r"; break;
        case '\t': esc = "\\t"; break;
        case '\v': esc = "\\v"; break;
        default:
            esc = 0;
            if (p0 == 0)
            {
                p0 = p;
                numChars = 0;
                numU8s = 0;
            }
            break;
        }

        // Replace current character with simple escape sequence.
        if (esc)
        {
            if (p0 != 0)
            {
                result.append(p0, numU8s, numChars);
                p0 = 0;
            }
            result += esc[0];
            result += esc[1];
        }
    }

    return result;
}


//!
//! Treat instance as a C++ string literal and remove surrounding quotes (if any) in
//! addition to resolving escape sequences. For example, <"abc\'\"123"> --> <abc'"123>.
//! Return true if instance was modified (due to surrounding quote removal and/or
//! escape sequence resolution). For a single quoted string, escape sequence resolution
//! can be skipped if unescapeSingleQuotedString is false.
//!
bool QuotedString::dequote(String& str, bool unescapeSingleQuotedString)
{

    // Empty string. No-op.
    if (str.s_->numChars() == 1)
    {
        bool modified = false;
        return modified;
    }

    // Quoted string.
    const utf8_t* esc0;
    const utf8_t* p = str.s_->raw();
    const utf8_t* pLast = p + str.s_->byteSize() - 2;
    if (((*p == SINGLE_QUOTE) || (*p == DOUBLE_QUOTE)) && (p < pLast) && (*pLast == *p))
    {

        // Empty quoted string.
        if (str.s_->numChars() == 3)
        {
            str.reset();
            bool modified = true;
            return modified;
        }

        // No escape sequences.
        // Remove surrounding quotes.
        bool singleQuoted = (*p == SINGLE_QUOTE);
        ++p;
        --pLast;
        esc0 = (singleQuoted && (!unescapeSingleQuotedString))? 0: findFirst(p, BACKSLASH);
        if (esc0 == 0)
        {
            String::S* s = new String::S(str.s_->initialCap());
            s->reset(p, str.s_->byteSize() - 3, str.s_->numChars() - 3);
            s->addNull();
            str.s_->rmRef();
            str.s_ = s;
            bool modified = true;
            return modified;
        }
    }

    // No surrounding quotes.
    // No escape sequences.
    else if ((esc0 = findFirst(p, BACKSLASH)) == 0)
    {
        bool modified = false;
        return modified;
    }

    // Resolve escape sequences.
    utf8_t* result = new utf8_t[str.s_->byteSize()];
    size_t n0 = esc0 - p;
    memcpy(result, p, n0);
    size_t n1 = unescape(result + n0, esc0, pLast);
    str.row();
    str.s_->convert8(result, n0 + n1);
    str.s_->addNullIfNone();
    delete[] result;
    bool modified = true;
    return modified;
}


//
// Parse and return the value of given octal escape sequence ([\\][0-7]{1,3}).
//
unsigned char QuotedString::unescapeO(const utf8_t* p1, const utf8_t* p2, size_t& bytesUsed)
{
    unsigned char esc = p1[1] - '0';
    const utf8_t* p = p1 + 2;
    if ((p <= p2) && (p[0] >= '0') && (p[0] <= '7'))
    {
        if ((p < p2) && (p[1] >= '0') && (p[1] <= '7') && (p[-1] <= '3'))
        {
            esc <<= 6;
            esc += ((p[0] - '0') << 3) + (p[1] - '0');
            bytesUsed = 4;
        }
        else
        {
            esc <<= 3;
            esc += p[0] - '0';
            bytesUsed = 3;
        }
    }

    else
    {
        bytesUsed = 2;
    }

    return esc;
}


//
// Parse and return the value of given hexadecimal escape sequence ([\\]x[A-Fa-f0-9]{1,2}).
//
unsigned char QuotedString::unescapeX(const utf8_t* p1, const utf8_t* p2, size_t& bytesUsed)
{
    unsigned char esc;
    const utf8_t* p = p1 + 2;
    if ((p <= p2) && S32::isXdigit(p[0]))
    {
        if ((p < p2) && S32::isXdigit(p[1]))
        {
            esc = U8::xdigitsToU8(p[0], p[1]);
            bytesUsed = 4;
        }
        else
        {
            esc = U8::xdigitToNibble(p[0]);
            bytesUsed = 3;
        }
    }

    else
    {
        esc = '?';
        bytesUsed = 2;
    }

    return esc;
}


unsigned int QuotedString::unescape(utf8_t* result, const utf8_t* p1, const utf8_t* p2)
{
    Utf8 esc8;
    utf8_t* dst = result;
    size_t bytesUsed;
    for (const utf8_t* p = p1; p <= p2;)
    {

        // Not an escape sequence.
        if (p[0] != BACKSLASH)
        {
            *dst++ = *p++;
            continue;
        }

        // Ignore trailing backslash.
        if (p == p2)
        {
            break;
        }

        // Simple escape sequence.
        utf8_t esc;
        switch (p[1])
        {
        case '?':  esc = '\?'; break;
        case '\'': esc = '\''; break;
        case '\"': esc = '\"'; break;
        case '\\': esc = '\\'; break;
        case 'a':  esc = '\a'; break;
        case 'b':  esc = '\b'; break;
        case 'f':  esc = '\f'; break;
        case 'n':  esc = '\n'; break;
        case 'r':  esc = '\r'; break;
        case 't':  esc = '\t'; break;
        case 'v':  esc = '\v'; break;

            // Octal escape sequence ([\\][0-7]{1,3}).
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
            esc8 = unescapeO(p, p2, bytesUsed);
            dst += esc8.encode(dst);
            p += bytesUsed;
            continue;

            // Hexadecimal escape sequence ([\\]x[A-Fa-f0-9]{1,2}).
        case 'x':
            esc8 = unescapeX(p, p2, bytesUsed);
            dst += esc8.encode(dst);
            p += bytesUsed;
            continue;

            // Universal name ([\\]u([A-Fa-f0-9]{4,4})|([A-Fa-f0-9]{8,8})).
        case 'u':
            esc8 = unescapeU(p, p2, bytesUsed);
            dst += esc8.encode(dst);
            p += bytesUsed;
            continue;

            // Invalid escape sequence.
        default:
            esc = '?';
            break;
        }

        // Invalid/simple escape sequence.
        *dst++ = esc;
        p += 2;
    }

    unsigned int byteCount = static_cast<unsigned int>(dst - result);
    return byteCount;
}


//
// Parse and return the value of given universal name escape sequence
// ([\\]u([A-Fa-f0-9]{4,4})|([A-Fa-f0-9]{8,8})).
//
utf32_t QuotedString::unescapeU(const utf8_t* p1, const utf8_t* p2, size_t& bytesUsed)
{
    utf32_t esc;
    const utf8_t* p = p1 + 2;
    if ((p + 3) <= p2)
    {
        if (S32::isXdigit(p[0]) && S32::isXdigit(p[1]) && S32::isXdigit(p[2]) && S32::isXdigit(p[3]))
        {
            if ((p + 7) <= p2)
            {

                // 10-byte escape sequence.
                if (S32::isXdigit(p[4]) && S32::isXdigit(p[5]) && S32::isXdigit(p[6]) && S32::isXdigit(p[7]))
                {
                    esc = (U8::xdigitsToU8(p[0], p[1]) << 24U) +
                        (U8::xdigitsToU8(p[2], p[3]) << 16U) +
                        (U8::xdigitsToU8(p[4], p[5]) << 8U) +
                        U8::xdigitsToU8(p[6], p[7]);
                    bytesUsed = 10;
                    return esc;
                }
            }

            // 6-byte escape sequence.
            esc = (U8::xdigitsToU8(p[0], p[1]) << 8U) + U8::xdigitsToU8(p[2], p[3]);
            bytesUsed = 6;
            return esc;
        }
    }

    // Invalid escape sequence.
    esc = '?';
    bytesUsed = 2;
    return esc;
}


//!
//! Enquote given string inside given quotes. If a character has a pre-defined simple escape
//! sequence, then its escape sequence will be used. For example: <abc'"123> --> <"abc\'\"123">.
//! The quote value is typically a single quote or a double quote. If it's zero, no surrounding
//! quotes will be added, but special characters will still be replaced with their known escape
//! sequences. To skip escape sequence usage in a single quoted string, set escapeSingleQuotedString
//! to false.
//!
void QuotedString::enquote(String& str, char quote, bool escapeSingleQuotedString)
{
    if (str.empty())
    {
        str.append(quote? 2: 0, quote);
        return;
    }

    if ((quote == SINGLE_QUOTE) && (!escapeSingleQuotedString))
    {
        String quoted(1, quote);
        quoted += str;
        quoted += quote;
        str = quoted;
        return;
    }

    String s(quote? 1: 0, quote);
    String quoted(s);
    quoted += escape(str);
    quoted += s;
    str = quoted;
}

END_NAMESPACE1
