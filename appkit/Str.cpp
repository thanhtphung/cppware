/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/macros.h"
#include <string.h>

#include "appkit-pch.h"
#include "appkit/S32.hpp"
#include "appkit/Str.hpp"
#include "appkit/String.hpp"

// Ignore-case character map (0x41U=='A', 0x5aU=='Z', 0x61U=='a', 0x7aU=='z').
// Indexed by byte value. If i is not a capital letter, s_icCharMap[i]==i.
// Otherwise, s_icCharMap[i]==lowercase(i).
static const unsigned char s_icCharMap[] =
{
    0x00U, 0x01U, 0x02U, 0x03U, 0x04U, 0x05U, 0x06U, 0x07U, //0x00-0x07
    0x08U, 0x09U, 0x0aU, 0x0bU, 0x0cU, 0x0dU, 0x0eU, 0x0fU, //0x08-0x0f
    0x10U, 0x11U, 0x12U, 0x13U, 0x14U, 0x15U, 0x16U, 0x17U, //0x10-0x17
    0x18U, 0x19U, 0x1aU, 0x1bU, 0x1cU, 0x1dU, 0x1eU, 0x1fU, //0x18-0x1f
    0x20U, 0x21U, 0x22U, 0x23U, 0x24U, 0x25U, 0x26U, 0x27U, //0x20-0x27
    0x28U, 0x29U, 0x2aU, 0x2bU, 0x2cU, 0x2dU, 0x2eU, 0x2fU, //0x28-0x2f
    0x30U, 0x31U, 0x32U, 0x33U, 0x34U, 0x35U, 0x36U, 0x37U, //0x30-0x37
    0x38U, 0x39U, 0x3aU, 0x3bU, 0x3cU, 0x3dU, 0x3eU, 0x3fU, //0x38-0x3f
    0x40U, 0x61U, 0x62U, 0x63U, 0x64U, 0x65U, 0x66U, 0x67U, //0x40-0x47
    0x68U, 0x69U, 0x6aU, 0x6bU, 0x6cU, 0x6dU, 0x6eU, 0x6fU, //0x48-0x4f
    0x70U, 0x71U, 0x72U, 0x73U, 0x74U, 0x75U, 0x76U, 0x77U, //0x50-0x57
    0x78U, 0x79U, 0x7aU, 0x5bU, 0x5cU, 0x5dU, 0x5eU, 0x5fU, //0x58-0x5f
    0x60U, 0x61U, 0x62U, 0x63U, 0x64U, 0x65U, 0x66U, 0x67U, //0x60-0x67
    0x68U, 0x69U, 0x6aU, 0x6bU, 0x6cU, 0x6dU, 0x6eU, 0x6fU, //0x68-0x6f
    0x70U, 0x71U, 0x72U, 0x73U, 0x74U, 0x75U, 0x76U, 0x77U, //0x70-0x77
    0x78U, 0x79U, 0x7aU, 0x7bU, 0x7cU, 0x7dU, 0x7eU, 0x7fU, //0x78-0x7f
    0x80U, 0x81U, 0x82U, 0x83U, 0x84U, 0x85U, 0x86U, 0x87U, //0x80-0x87
    0x88U, 0x89U, 0x8aU, 0x8bU, 0x8cU, 0x8dU, 0x8eU, 0x8fU, //0x88-0x8f
    0x90U, 0x91U, 0x92U, 0x93U, 0x94U, 0x95U, 0x96U, 0x97U, //0x90-0x97
    0x98U, 0x99U, 0x9aU, 0x9bU, 0x9cU, 0x9dU, 0x9eU, 0x9fU, //0x98-0x9f
    0xa0U, 0xa1U, 0xa2U, 0xa3U, 0xa4U, 0xa5U, 0xa6U, 0xa7U, //0xa0-0xa7
    0xa8U, 0xa9U, 0xaaU, 0xabU, 0xacU, 0xadU, 0xaeU, 0xafU, //0xa8-0xaf
    0xb0U, 0xb1U, 0xb2U, 0xb3U, 0xb4U, 0xb5U, 0xb6U, 0xb7U, //0xb0-0xb7
    0xb8U, 0xb9U, 0xbaU, 0xbbU, 0xbcU, 0xbdU, 0xbeU, 0xbfU, //0xb8-0xbf
    0xc0U, 0xc1U, 0xc2U, 0xc3U, 0xc4U, 0xc5U, 0xc6U, 0xc7U, //0xc0-0xc7
    0xc8U, 0xc9U, 0xcaU, 0xcbU, 0xccU, 0xcdU, 0xceU, 0xcfU, //0xc8-0xcf
    0xd0U, 0xd1U, 0xd2U, 0xd3U, 0xd4U, 0xd5U, 0xd6U, 0xd7U, //0xd0-0xd7
    0xd8U, 0xd9U, 0xdaU, 0xdbU, 0xdcU, 0xddU, 0xdeU, 0xdfU, //0xd8-0xdf
    0xe0U, 0xe1U, 0xe2U, 0xe3U, 0xe4U, 0xe5U, 0xe6U, 0xe7U, //0xe0-0xe7
    0xe8U, 0xe9U, 0xeaU, 0xebU, 0xecU, 0xedU, 0xeeU, 0xefU, //0xe8-0xef
    0xf0U, 0xf1U, 0xf2U, 0xf3U, 0xf4U, 0xf5U, 0xf6U, 0xf7U, //0xf0-0xf7
    0xf8U, 0xf9U, 0xfaU, 0xfbU, 0xfcU, 0xfdU, 0xfeU, 0xffU  //0xf8-0xff
};

BEGIN_NAMESPACE1(appkit)


//!
//! Quick implementation for the case-insensitive strstr().
//!
const char* strcasestr(const char* haystack, const char* needle)
{
    const unsigned char* hp = 0;
    const unsigned char* np = 0;
    for (const unsigned char* p = reinterpret_cast<const unsigned char*>(haystack); *p != 0; ++p)
    {
        if (np != 0)
        {
            if (s_icCharMap[*p] == s_icCharMap[*np])
            {
                if (*++np == 0)
                {
                    return reinterpret_cast<const char*>(hp);
                }
            }
            else
            {
                np = 0;
            }
        }
        else if (s_icCharMap[*p] == s_icCharMap[*reinterpret_cast<const unsigned char*>(needle)])
        {
            np = reinterpret_cast<const unsigned char*>(needle)+1;
            hp = p;
        }
    }

    return 0;
}


//!
//! Strip all white space surrounding given delimiter and hyphens from string s.
//! Also trim leading and trailing white space. Save result in result. Return
//! result.
//!
String& Str::stripSpace(String& result, const char* s, char delim)
{

    // Look for given delimiter or hyphen, then strip consecutive
    // white spaces surrounding the located character.
    result.reset();
    for (const char* s1 = s; *s1;)
    {
        if ((*s1 == delim) || (*s1 == '-'))
        {
            unsigned int numSpaces = 0;
            for (const char* p = s1 - 1; p >= s; --p)
            {
                if (S32::isSpace(*p))
                {
                    ++numSpaces;
                }
                else
                {
                    break;
                }
            }
            if (numSpaces > 0)
            {
                result.truncate(result.length() - numSpaces);
            }
            result.append(1, *s1);
            while (S32::isSpace(*++s1));
        }
        else
        {
            result.append(1, *s1++);
        }
    }

    // Trim leading and trailing white space.
    // Return reference to <result>.
    result.trimSpace(true /*trimL*/, true /*trimR*/);
    return result;
}


//!
//! Strip all white space surrounding given delimiter and hyphens from given string
//! (length characters starting at s) and save result in result. Return result.
//!
String& Str::stripSpace(String& result, const char* s, size_t length, char delim)
{

    // Look for given delimiter or hyphen, then strip consecutive
    // white spaces surrounding the located character.
    result.reset();
    const char* s2 = s + length - 1;
    for (const char* s1 = s; s1 <= s2;)
    {
        if ((*s1 == delim) || (*s1 == '-'))
        {
            unsigned int numSpaces = 0;
            for (const char* p = s1 - 1; p >= s; --p)
            {
                if (S32::isSpace(*p))
                {
                    ++numSpaces;
                }
                else
                {
                    break;
                }
            }
            if (numSpaces > 0)
            {
                result.truncate(result.length() - numSpaces);
            }
            result.append(1, *s1);
            for (++s1; (s1 <= s2) && S32::isSpace(*s1); ++s1);
        }
        else
        {
            result.append(1, *s1++);
        }
    }

    // Trim leading and trailing white space.
    // Return reference to <result>.
    result.trimSpace(true /*trimL*/, true /*trimR*/);
    return result;
}


//!
//! Trim leading and/or trailing white space from string s and save
//! result in result. Return result.
//!
String& Str::trimSpace(String& result, const char* s, bool trimL, bool trimR)
{

    // Skip leading white space.
    const char* s1 = s;
    if (trimL)
    {
        for (; S32::isSpace(*s1); ++s1);
    }

    // Skip trailing white space.
    const char* s2 = s1;
    for (; *s2 != 0; ++s2);
    --s2;
    if (trimR)
    {
        for (; (s2 >= s1) && S32::isSpace(*s2); --s2);
    }

    // Form result.
    (s1 <= s2)?
        result.reset(s1, s2 - s1 + 1):
        result.reset();

    // Return reference to <result>.
    return result;
}


//!
//! Trim leading and/or trailing white space from given string (length
//! characters starting at s) and save result in result. Return result.
//!
String& Str::trimSpace(String& result, const char* s, size_t length, bool trimL, bool trimR)
{

    // Skip leading white space.
    const char* s1 = s;
    const char* s2 = s + length - 1;
    if (trimL)
    {
        for (; (s1 <= s2) && S32::isSpace(*s1); ++s1);
    }

    // Skip trailing white space.
    if (trimR)
    {
        for (; (s2 >= s1) && S32::isSpace(*s2); --s2);
    }

    // Form result.
    (s1 <= s2)?
        result.reset(s1, s2 - s1 + 1):
        result.reset();

    // Return reference to <result>.
    return result;
}


//!
//! Return true if given string (length bytes starting at s) is ASCII.
//!
bool Str::isAscii(const char* s, size_t length)
{
    bool answer = true;
    const char* pEnd = s + length;
    for (const char* p = s; p < pEnd; ++p)
    {
        if (!S32::isAscii(*p))
        {
            answer = false;
            break;
        }
    }

    return answer;
}


//!
//! Strip all white space surrounding given delimiter and hyphens from string s.
//! Also trim leading and trailing white space. Return s.
//!
char* Str::stripSpace(char* s, char delim)
{

    // Look for given delimiter or hyphen, then strip consecutive
    // white spaces surrounding the located character.
    char* result = s;
    for (const char* s1 = s; *s1;)
    {
        if ((*s1 == delim) || (*s1 == '-'))
        {
            for (const char* p = result - 1; p >= s; --p)
            {
                if (S32::isSpace(*p))
                {
                    --result;
                }
                else
                {
                    break;
                }
            }
            *result++ = *s1;
            while (S32::isSpace(*++s1));
        }
        else
        {
            *result++ = *s1++;
        }
    }

    // Don't forget the terminating null.
    *result = 0;

    // Trim leading and trailing white space.
    // Return <s>.
    return trimSpace(s, true /*trimL*/, true /*trimR*/);
}


//!
//! Trim leading and/or trailing white space from string s.
//! Return s.
//!
char* Str::trimSpace(char* s, bool trimL, bool trimR)
{

    // Skip leading white space.
    char* s1 = s;
    if (trimL)
    {
        for (; S32::isSpace(*s1); ++s1);
    }

    // Skip trailing white space.
    const char* s2 = s1;
    for (; *s2 != 0; ++s2);
    --s2;
    if (trimR)
    {
        for (; (s2 >= s1) && S32::isSpace(*s2); --s2);
    }

    // Form result.
    size_t length = s2 - s1 + 1;
    s1[length] = 0;
    if (s1 != s)
    {
        memmove(s, s1, length + 1);
    }

    // Return <s>.
    return s;
}


//!
//! Compare two null-terminated strings. Return a negative value if
//! item0 < item1. Return 0 if item0 == item1. Return a positive value
//! if item0 > item1.
//!
int Str::compareK(const void* item0, const void* item1)
{
    const char* k0 = static_cast<const char*>(item0);
    const char* k1 = static_cast<const char*>(item1);
    return strcmp(k0, k1);
}


//!
//! Compare two null-terminated strings. Ignore case. Return a negative
//! value if item0 < item1. Return 0 if item0 == item1. Return a positive
//! value if item0 > item1.
//!
int Str::compareKI(const void* item0, const void* item1)
{
    int rc;

    const unsigned char* p0 = reinterpret_cast<const unsigned char*>(item0);
    const unsigned char* p1 = reinterpret_cast<const unsigned char*>(item1);
    for (;; ++p0, ++p1)
    {
        int c0 = s_icCharMap[*p0];
        int c1 = s_icCharMap[*p1];
        if (c0 != c1)
        {
            rc = c0 - c1;
            break;
        }
        if (c0 == 0)
        {
            rc = 0;
            break;
        }
    }

    return rc;
}


//!
//! Compare two null-terminated strings. Ignore case. Compare at most
//! n characters. Return a negative value if item0 < item1. Return
//! 0 if item0 == item1. Return a positive value if item0 > item1.
//!
int Str::compareKIN(const char* item0, const char* item1, size_t n)
{
    int rc = 0;

    const unsigned char* p0 = reinterpret_cast<const unsigned char*>(item0);
    const unsigned char* p1 = reinterpret_cast<const unsigned char*>(item1);
    for (size_t i = 0; i < n; ++i, ++p0, ++p1)
    {
        int c0 = s_icCharMap[*p0];
        int c1 = s_icCharMap[*p1];
        if (c0 != c1)
        {
            rc = c0 - c1;
            break;
        }
        if (c0 == 0)
        {
            break;
        }
    }

    return rc;
}


//!
//! Compare two null-terminated strings. Ignore case. Reverse the normal
//! sense of comparison. Return a positive value if item0 < item1. Return
//! 0 if item0 == item1. Return a negative value if item0 > item1.
//!
int Str::compareKIR(const void* item0, const void* item1)
{
    return compareKI(item1, item0);
}


//!
//! Compare two null-terminated strings. Reverse the normal sense
//! of comparison. Return a positive value if item0 < item1. Return
//! 0 if item0 == item1. Return a negative value if item0 > item1.
//!
int Str::compareKR(const void* item0, const void* item1)
{
    const char* k0 = static_cast<const char*>(item0);
    const char* k1 = static_cast<const char*>(item1);
    return strcmp(k1, k0);
}


//!
//! Compare two null-terminated strings given their addresses. Return
//! a negative value if *item0 < *item1. Return 0 if *item0 == *item1.
//! Return a positive value if *item0 > *item1.
//!
int Str::compareP(const void* item0, const void* item1)
{
    const char* const* p0 = static_cast<const char* const*>(item0);
    const char* const* p1 = static_cast<const char* const*>(item1);
    return strcmp(*p0, *p1);
}


//!
//! Compare two null-terminated strings given their addresses. Ignore
//! case. Return a negative value if *item0 < *item1. Return 0 if
//! *item0 == *item1. Return a positive value if *item0 > *item1.
//!
int Str::comparePI(const void* item0, const void* item1)
{
    const char* const* p0 = static_cast<const char* const*>(item0);
    const char* const* p1 = static_cast<const char* const*>(item1);
    return compareKI(*p0, *p1);
}


//!
//! Compare two null-terminated strings given their addresses. Ignore
//! case. Reverse the normal sense of comparison. Return a positive
//! value if *item0 < *item1. Return 0 if *item0 == *item1. Return a
//! negative value if *item0 > *item1.
//!
int Str::comparePIR(const void* item0, const void* item1)
{
    const char* const* p0 = static_cast<const char* const*>(item0);
    const char* const* p1 = static_cast<const char* const*>(item1);
    return compareKI(*p1, *p0);
}


//!
//! Compare two null-terminated strings given their addresses. Reverse the
//! normal sense of comparison. Return a positive value if *item0 < *item1.
//! Return 0 if *item0 == *item1. Return a negative value if *item0 > *item1.
//!
int Str::comparePR(const void* item0, const void* item1)
{
    const char* const* p0 = static_cast<const char* const*>(item0);
    const char* const* p1 = static_cast<const char* const*>(item1);
    return strcmp(*p1, *p0);
}

END_NAMESPACE1
