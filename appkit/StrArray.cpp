/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/StrArray.hpp"
#include "appkit/U32.hpp"

BEGIN_NAMESPACE1(appkit)


//!
//! Locate given item using linear search. Ignore case if ignoreCase is true.
//! If allowItemAsIndex is true, interpret item as array index if necessary. For
//! example, if allowItemAsIndex is true, then either "third" or "2" can be used
//! to locate the third item in an array containing {"first", "second", "third"}.
//! Return true if found (also return the located index in foundIndex). Return
//! false otherwise.
//!
bool StrArray::find(const char* item, size_t& foundIndex, bool ignoreCase, bool allowItemAsIndex) const
{
    bool found = false;
    const char* const* p = item_;
    compare_t compare = ignoreCase? Str::compareKI: Str::compareK;
    for (const char* const* pEnd = p + numItems_; p < pEnd; ++p)
    {
        if (compare(*p, item) == 0)
        {
            found = true;
            foundIndex = p - item_;
            break;
        }
    }

    if ((!found) && allowItemAsIndex && U32::isValid(item, 0, numItems_ - 1))
    {
        found = true;
        foundIndex = U32(item);
    }

    return found;
}


//!
//! Reset instance with given null-terminated array of null-terminated
//! strings. All copies, if any, are shallow. Make instance read-only.
//!
void StrArray::reset(const char* const* array)
{
    roItem_ = array;

    unsigned int i = 0;
    for (; array[i] != 0; ++i);
    numItems_ = i;
}

END_NAMESPACE1
