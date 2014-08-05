/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/sys.hpp"

#include "appkit-pch.h"
#include "appkit/U16.hpp"

using namespace syskit;

BEGIN_NAMESPACE1(appkit)


U16::U16(const String* s, item_t defaultV)
{
    if ((s == 0) || s->empty())
    {
        v_ = defaultV;
    }
    else
    {
        size_t bytesUsed;
        unsigned int u32 = U32(s->ascii(), &bytesUsed);
        v_ = ((bytesUsed > 0) && (u32 <= 0xffffU))? static_cast<item_t>(u32): defaultV;
    }
}


//!
//! Return the checksum of given items. The checksum is the
//! 16-bit ones's complement of the ones's complement sum of
//! the given data.
//!
U16::item_t U16::checksum(const item_t* item, size_t numItems)
{
    size_t sum = 0;
    const item_t* p;
    const item_t* pEnd;
    for (p = item, pEnd = p + numItems; p < pEnd; sum += *p++);

    sum = (sum & 0x0000ffffU) + (sum >> 16);
    if ((sum & 0xffff0000U) != 0)
    {
        ++sum;
    }

    return static_cast<item_t>(~sum);
}


//!
//! Return the checksum of given items. The checksum is the
//! 16-bit ones's complement of the ones's complement sum of
//! the given data.
//!
U16::item_t U16::checksum(const item_t* itemA, size_t numItemAs, const item_t* itemB, size_t numItemBs)
{
    size_t sum = 0;
    const item_t* p;
    const item_t* pEnd;
    for (p = itemA, pEnd = p + numItemAs; p < pEnd; sum += *p++);
    for (p = itemB, pEnd = p + numItemBs; p < pEnd; sum += *p++);

    sum = (sum & 0x0000ffffU) + (sum >> 16);
    if ((sum & 0xffff0000U) != 0)
    {
        ++sum;
    }

    return static_cast<item_t>(~sum);
}


//!
//! Return true if given string (length characters starting at s) holds an unsigned
//! 16-bit number and nothing else.
//!
bool U16::isValid(const char* s, size_t length)
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
        return ((pEnd == (s + length - 1)) && (u32 <= 0xffffU));
    }

    // Use stack for temporary null-terminated string copy.
    char buf[MAX_LENGTH + 1];
    memcpy(buf, s, length);
    buf[length] = 0;
    unsigned int u32 = std::strtoul(buf, &pEnd, 0);
    return ((pEnd == (buf + length)) && (u32 <= 0xffffU));
}


//!
//! Swap the byte ordering of given items (numMetis shorts starting at meti).
//! Save result in the item buffer which is big enough for the operation.
//!
void U16::bswap(const item_t* meti, size_t numMetis, item_t* item)
{
    const item_t* pMeti;
    const item_t* pMetiEnd;
    item_t* pItem;
    for (pMeti = meti, pItem = item, pMetiEnd = pMeti + numMetis; pMeti < pMetiEnd; ++pMeti)
    {
        *pItem++ = bswap16(*pMeti);
    }
}


//!
//! Swap the byte ordering of given items.
//!
void U16::bswap(item_t* item, size_t numItems)
{
    item_t* p;
    const item_t* pEnd;
    for (p = item, pEnd = p + numItems; p < pEnd; ++p)
    {
        *p = bswap16(*p);
    }
}

END_NAMESPACE1
