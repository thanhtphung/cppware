/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <string.h>
#include "syskit/sys.hpp"

#include "appkit-pch.h"
#include "appkit/S32.hpp"
#include "appkit/U32.hpp"
#include "appkit/U64.hpp"
#include "appkit/U8.hpp"

using namespace syskit;

BEGIN_NAMESPACE1(appkit)

typedef union
{
    const void* opaque; //must be first
    U64::item_t item;
} opaque_t;


U64::U64(const String* s, item_t defaultV)
{
    if ((s == 0) || s->empty())
    {
        v_ = defaultV;
    }
    else
    {
        char* pEnd;
        const char* p = s->ascii();
        item_t v = strtoull(p, &pEnd, 0);
        v_ = ((pEnd != p) && (*pEnd == 0))? v: defaultV;
    }
}


U64::item_t U64::toU64(const char* s, size_t length, size_t* bytesUsed)
{
    const size_t MAX_LENGTH = 127;
    char* pEnd;
    int c;
    item_t v;
    if ((length <= 0) || (length > MAX_LENGTH))
    {
        v = 0;
        *bytesUsed = 0;
    }

    else if (((c = s[length - 1]) == 0) || S32::isSpace(c))
    {
        v = strtoull(s, &pEnd, 0);
        *bytesUsed = pEnd - s;
    }

    // Use stack for temporary null-terminated string copy.
    else
    {
        char buf[MAX_LENGTH + 1];
        memcpy(buf, s, length);
        buf[length] = 0;
        v = strtoull(buf, &pEnd, 0);
        *bytesUsed = pEnd - buf;
    }

    return v;
}


//!
//! Return true if given string (length characters starting at s) holds an unsigned
//! 64-bit number and nothing else.
//!
bool U64::isValid(const char* s, size_t length)
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
        strtoull(s, &pEnd, 0);
        return (pEnd == (s + length - 1));
    }

    // Use stack for temporary null-terminated string copy.
    char buf[MAX_LENGTH + 1];
    memcpy(buf, s, length);
    buf[length] = 0;
    strtoull(buf, &pEnd, 0);
    return (pEnd == (buf + length));
}


//!
//! Compare two unsigned long longs. Return a negative value if item0 < item1.
//! Return 0 if item0 == item1. Return a positive value if item0 > item1.
//!
int U64::compareK(const void* item0, const void* item1)
{
    opaque_t k0 = {item0};
    opaque_t k1 = {item1};
    return (k0.item < k1.item)? (-1): ((k0.item>k1.item)? 1: 0);
}


//!
//! Compare two unsigned long longs. Reverse the normal sense of comparison.
//! Return a positive value if item0 < item1. Return 0 if item0 == item1.
//! Return a negative value if item0 > item1.
//!
int U64::compareKR(const void* item0, const void* item1)
{
    opaque_t k0 = {item0};
    opaque_t k1 = {item1};
    return (k0.item < k1.item)? (1): ((k0.item>k1.item)? -1: 0);
}


//!
//! Compare two unsigned long longs given their addresses. Return a negative
//! value if *item0 < *item1. Return 0 if *item0 == *item1. Return a
//! positive value if *item0 > *item1.
//!
int U64::compareP(const void* item0, const void* item1)
{
    item_t k0 = *static_cast<const item_t*>(item0);
    item_t k1 = *static_cast<const item_t*>(item1);
    return (k0 < k1)? (-1): ((k0>k1)? 1: 0);
}


//!
//! Compare two unsigned long longs given their addresses. Reverse the normal
//! sense of comparison. Return a positive value if *item0 < *item1. Return
//! 0 if *item0 == *item1. Return a negative value if *item0 > *item1.
//!
int U64::comparePR(const void* item0, const void* item1)
{
    item_t k0 = *static_cast<const item_t*>(item0);
    item_t k1 = *static_cast<const item_t*>(item1);
    return (k0 < k1)? (1): ((k0>k1)? -1: 0);
}


//!
//! Modular hash function for an unsigned long long.
//! Return a non-negative number less than numBuckets.
//!
unsigned int U64::hashK(const void* item, size_t numBuckets)
{
    opaque_t k = {item};
    unsigned int i = static_cast<unsigned int>(k.item % numBuckets);
    return i;
}


//!
//! Modular hash function for an unsigned long long given its address.
//! Return a non-negative number less than numBuckets.
//!
unsigned int U64::hashP(const void* item, size_t numBuckets)
{
    item_t k = *static_cast<const item_t*>(item);
    unsigned int i = static_cast<unsigned int>(k % numBuckets);
    return i;
}


//!
//! Return the ASCII column width of given value if displayed as an
//! unsigned decimal number (%u). For example, 0..9 requires one
//! character, 10..99 requires two, and 100..999 requires three, etc.
//!
unsigned int U64::numDigits(item_t item)
{
    static const item_t s_boundary[] =
    {
        9ULL,                   //1 --digits
        99ULL,                  //2
        999ULL,                 //3
        9999ULL,                //4
        99999ULL,               //5
        999999ULL,              //6
        9999999ULL,             //7
        99999999ULL,            //8
        999999999ULL,           //9
        9999999999ULL,          //10
        99999999999ULL,         //11
        999999999999ULL,        //12
        9999999999999ULL,       //13
        99999999999999ULL,      //14
        999999999999999ULL,     //15
        9999999999999999ULL,    //16
        99999999999999999ULL,   //17
        999999999999999999ULL,  //18
        9999999999999999999ULL, //19
        0xffffffffffffffffULL   //20
    };

    size_t i = (item <= s_boundary[6])? 0: ((item <= s_boundary[12])? 5: 11);
    for (; item > s_boundary[i]; ++i);
    return static_cast<unsigned int>(i + 1);
}


//!
//! Convert number to decimal digits.
//! Return number of decimal digits in the number.
//!
unsigned int U64::toDigits(item_t item, char digit[MaxDigits])
{
    if ((item | 0xffffffff00000000ULL) == 0)
    {
        unsigned int numDigits = U32::toDigits(static_cast<U32::item_t>(item), digit);
        return numDigits;
    }

    // Extract decimal digits. Least significant digit first. One at a time.
    char tmp[MaxDigits];
    char* p = tmp + MaxDigits;
    do
    {
        *--p = '0' + static_cast<char>(item % 10);
    } while (item /= 10);

    // Save result.
    size_t numDigits = MaxDigits - (p - tmp);
    memcpy(digit, p, numDigits);

    // Return number of decimal digits in the number.
    return static_cast<unsigned int>(numDigits);
}


//!
//! Swap the byte ordering of given items (numMetis long longs starting at meti).
//! Save result in the item buffer which is big enough for the operation.
//!
void U64::bswap(const item_t* meti, size_t numMetis, item_t* item)
{
    const item_t* pMeti;
    const item_t* pMetiEnd;
    item_t* pItem;
    for (pMeti = meti, pItem = item, pMetiEnd = pMeti + numMetis; pMeti < pMetiEnd; ++pMeti)
    {
        *pItem++ = bswap64(*pMeti);
    }
}


//!
//! Swap the byte ordering of given items.
//!
void U64::bswap(item_t* item, size_t numItems)
{
    item_t* p;
    const item_t* pEnd;
    for (p = item, pEnd = p + numItems; p < pEnd; ++p)
    {
        *p = bswap64(*p);
    }
}


//!
//! Convert 64-bit number to uppercase hex digits.
//! For example, item=0xa1b2c3d41a2b3c4dULL --> xdigit[0]='A' xdigit[1]='1'
//! xdigit[2]='B' xdigit[3]='2' xdigit[4]='C' xdigit[5]='3' xdigit[6]='D'
//! xdigit[7]='4'...
//!
void U64::toXDIGITS(item_t item, char xdigit[NumXdigits])
{
    item_t v = item;
    for (int i = NumXdigits - 2; i >= 0; v >>= 8, i -= 2)
    {
        U8::toXDIGITS(static_cast<unsigned char>(v), xdigit + i);
    }
}


//!
//! Convert 64-bit number to lowercase hex digits.
//! For example, item=0xa1b2c3d41a2b3c4dULL --> xdigit[0]='a' xdigit[1]='1'
//! xdigit[2]='b' xdigit[3]='2' xdigit[4]='c' xdigit[5]='3' xdigit[6]='d'
//! xdigit[7]='4'...
//!
void U64::toXdigits(item_t item, char xdigit[NumXdigits])
{
    item_t v = item;
    for (int i = NumXdigits - 2; i >= 0; v >>= 8, i -= 2)
    {
        U8::toXdigits(static_cast<unsigned char>(v), xdigit + i);
    }
}

END_NAMESPACE1
