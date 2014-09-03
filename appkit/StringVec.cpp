/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/DelimitedTxt.hpp"
#include "appkit/String.hpp"
#include "appkit/StringVec.hpp"

using namespace syskit;

BEGIN_NAMESPACE1(appkit)


//!
//! Construct instance with lines from given delimited text. Lines become
//! vector items. By default, lines are trimmed. Use trimLines to specify
//! otherwise. The vector does not grow if growBy is zero, exponentially
//! grows by doubling if growBy is negative, and grows by growBy items
//! otherwise. Construction is partial if vector cannot grow to accomodate
//! all lines.
//!
StringVec::StringVec(DelimitedTxt& txt, bool trimLines, unsigned int capacity, int growBy, bool ignoreCase):
Vec(capacity, growBy),
empty_()
{
    setCmpFuncs(ignoreCase);
    doReset(txt, trimLines);
}


//!
//! Construct instance using guts from that. That is, move vector contents from
//! that into this. That instance is now disabled and must not be used further.
//!
StringVec::StringVec(StringVec* that):
Vec(that),
empty_()
{
    compare_ = that->compare_;
    compareK_ = that->compareK_;
}


//!
//! Construct a duplicate instance of the given vector.
//!
StringVec::StringVec(const StringVec& vec):
Vec(vec),
empty_()
{
    compare_ = vec.compare_;
    compareK_ = vec.compareK_;

    size_t startAt = 0;
    size_t itemCount = Vec::numItems();
    cloneItems(startAt, itemCount);
}


//!
//! Construct instance with given subset (itemCount items starting at startAt).
//!
StringVec::StringVec(const StringVec& vec, size_t startAt, size_t itemCount, bool ignoreCase):
Vec(vec, startAt, itemCount),
empty_()
{
    setCmpFuncs(ignoreCase);

    startAt = 0;
    itemCount = Vec::numItems();
    cloneItems(startAt, itemCount);
}


//!
//! Construct an empty vector with initial capacity of capacity items.
//! The vector does not grow if growBy is zero, exponentially grows by
//! doubling if growBy is negative, and grows by growBy items otherwise.
//!
StringVec::StringVec(unsigned int capacity, int growBy, bool ignoreCase):
Vec(capacity, growBy),
empty_()
{
    setCmpFuncs(ignoreCase);
}


StringVec::~StringVec()
{
    reset();
}


//!
//! Just as a string can be expanded into a vector of strings, a vector of strings
//! can be collapsed back into one string. This method collapses the vector into a
//! string by concatenating up to maxItems members. The members are separated by
//! delim if delim is non-zero. A trailing "..." is used to indicate there are more
//! members in the vector. Return the resulting concatenated string.
//!
String StringVec::stringify(const char* delim, size_t maxItems) const
{
    String s;
    const String* const* p0 = raw();
    size_t length = (numItems() <= maxItems)? numItems(): maxItems;
    for (size_t i = 0; i < length; ++i)
    {
        if ((delim != 0) && (i > 0))
        {
            s += delim;
        }
        s += *p0[i];
    }

    if (length < numItems())
    {
        s += "...";
    }

    return s;
}


//!
//! Return true if this vector equals given vector. That is, if both
//! have the same number of items and corresponding items are identical.
//!
bool StringVec::operator ==(const StringVec& vec) const
{
    bool eq;
    if (numItems() != vec.numItems())
    {
        eq = false;
    }
    else
    {
        eq = true;
        const String* const* p0 = raw();
        const String* const* p1 = vec.raw();
        for (size_t i = 0, length = numItems(); i < length; ++i)
        {
            if (*p0[i] != *p1[i])
            {
                eq = false;
                break;
            }
        }
    }

    return eq;
}


//!
//! Reset and move the vector contents including guts from that into this. That
//! instance is now disabled and must not be used further.
//!
const StringVec& StringVec::operator =(StringVec* that)
{

    // No-op if operating against the same vector.
    if (this != that)
    {
        reset();
        Vec::operator =(that);
        compare_ = that->compare_;
        compareK_ = that->compareK_;
    }

    // Return reference to self.
    return *this;
}


//!
//! Copy the vector contents from given vector. This vector might grow to
//! accomodate the source. If it cannot grow, items at the tail end will
//! be dropped.
//!
const StringVec& StringVec::operator =(const StringVec& vec)
{

    // Prevent self assignment.
    if (this == &vec)
    {
        return *this;
    }

    // Truncate unused entries.
    truncate(vec.numItems());

    // Update existing entries.
    size_t numUpdates = Vec::numItems();
    const String* const* p = vec.raw();
    for (size_t i = 0; i < numUpdates; ++i)
    {
        String* s = static_cast<String*>(Vec::peek(i));
        *s = *p[i];
    }

    // Create new entries.
    if (numUpdates < vec.numItems())
    {
        size_t startAt = Vec::numItems();
        size_t itemCount = vec.numItems() - numUpdates;
        Vec::add(itemCount, 0 /*item*/);
        for (size_t i = startAt, iEnd = Vec::numItems(); i < iEnd; ++i)
        {
            String* clone = new String(*p[i]);
            Vec::setItem(i, clone);
        }
    }

    // Return reference to self.
    return *this;
}


//!
//! Add given item to the tail end. Return true if successful (i.e.,
//! vector is not full). Return false otherwise.
//!
bool StringVec::add(const String& item)
{
    String* clone = new String(item);
    bool ok = Vec::add(clone);
    if (!ok)
    {
        delete clone;
    }

    return ok;
}


//!
//! Add given item at given index. If maintainOrder is true, items are shifted
//! down. Otherwise, the existing item at given index, if any, moves to the end.
//! Return true if successful (i.e., vector is not full). Return false otherwise.
//!
bool StringVec::addAtIndex(size_t index, const String& item, bool maintainOrder)
{
    String* clone = new String(item);
    bool ok = Vec::addAtIndex(index, clone, maintainOrder);
    if (!ok)
    {
        delete clone;
    }

    return ok;
}


//!
//! Add given item to the tail end only if it's unique. Return true if
//! item was added successful. Return false otherwise (vector is full or
//! item already exists).
//!
bool StringVec::addIfNotFound(const String& item)
{
    String* clone = new String(item);
    bool ok = Vec::addIfNotFound(clone, compare_);
    if (!ok)
    {
        delete clone;
    }

    return ok;
}


//
// Reset instance with lines from given delimited text. Lines become
// vector items. By default, lines are trimmed. Use trimLines to specify
// otherwise. Return true if successful. Return false otherwise (result
// is partial since instance could not grow to accomodate all lines).
//
bool StringVec::doReset(DelimitedTxt& txt, bool trimLines)
{
    const char* line;
    size_t length;
    bool ok = true;
    for (txt.reset(); txt.next(line, length);)
    {

        if (trimLines)
        {
            length = txt.trimLine(line, length);
        }

        unsigned int capacity = static_cast<unsigned int>(length);
        String* clone = new String(capacity);
        clone->reset8(reinterpret_cast<const utf8_t*>(line), length);
        bool ok = Vec::add(clone);
        if (!ok)
        {
            delete clone;
            ok = false;
            break;
        }
    }

    return ok;
}


//!
//! Locate and remove given item using linear search. Return true if found.
//! Return false otherwise. Item removal might affect vector ordering. If
//! maintainOrder is true, appropriate items would be shifted up. Otherwise,
//! the last item would move up to replace the removed item.
//!
bool StringVec::rm(const String& item, bool maintainOrder)
{
    void* removed;
    bool ok = Vec::rm(&item, compare_, removed, maintainOrder);
    if (ok)
    {
        delete static_cast<String*>(removed);
    }

    return ok;
}


//!
//! Locate and remove given item using linear search. Return true if found.
//! Return false otherwise. Item removal might affect vector ordering. If
//! maintainOrder is true, appropriate items would be shifted up. Otherwise,
//! the last item would move up to replace the removed item.
//!
bool StringVec::rm(const char* item, bool maintainOrder)
{
    void* removed;
    bool ok = Vec::rm(item, compareK_, removed, maintainOrder);
    if (ok)
    {
        delete static_cast<String*>(removed);
    }

    return ok;
}


//!
//! Remove item from given index. If given index is valid, return true (also set
//! removedItem to the removed item). Return false otherwise. If maintainOrder is
//! true, appropriate items will be shifted up. Otherwise, the last item will
//! move up to replace the removed item.
//!
bool StringVec::rmFromIndex(size_t index, String& removedItem, bool maintainOrder)
{
    void* removed;
    bool ok = Vec::rmFromIndex(index, removed, maintainOrder);
    if (ok)
    {
        const String* s = static_cast<String*>(removed);
        removedItem = *s;
        delete s;
    }

    return ok;
}


//!
//! Remove item from given index. If given index is valid, return true.
//! Return false otherwise. If maintainOrder is true, appropriate items
//! will be shifted up. Otherwise, the last item will move up to replace
//! the removed item.
//!
bool StringVec::rmFromIndex(size_t index, bool maintainOrder)
{
    void* removed;
    bool ok = Vec::rmFromIndex(index, removed, maintainOrder);
    if (ok)
    {
        delete static_cast<String*>(removed);
    }

    return ok;
}


//!
//! Remove the item from the tail end. Return true if successful. Return
//! false otherwise (i.e., vector is empty).
//!
bool StringVec::rmTail()
{
    void* removed;
    bool ok = Vec::rmTail(removed);
    if (ok)
    {
        delete static_cast<String*>(removed);
    }

    return ok;
}


//!
//! Remove the item from the tail end. Return true if successful (also set
//! removedItem to the removed item). Return false otherwise (i.e., vector
//! is empty).
//!
bool StringVec::rmTail(String& removedItem)
{
    void* removed;
    bool ok = Vec::rmTail(removed);
    if (ok)
    {
        const String* s = static_cast<String*>(removed);
        removedItem = *s;
        delete s;
    }

    return ok;
}


//!
//! Truncate vector if it has more than numItems items.
//! Return true if truncation occured.
//!
bool StringVec::truncate(size_t numItems)
{
    const String* const* p = raw();
    for (size_t i = Vec::numItems(); i > numItems; delete p[--i]);
    bool truncated = Vec::truncate(numItems);
    return truncated;
}


//!
//! Add given items (array of itemCount strings). Return number of items
//! successfully added. This number can be less than the given item count
//! if growth is required but this vector cannot grow.
//!
unsigned int StringVec::add(const String* array, size_t itemCount)
{
    size_t startAt = Vec::numItems();
    itemCount = Vec::add(itemCount, 0);
    for (size_t i = 0, j = startAt; i < itemCount; ++i, ++j)
    {
        String* clone = new String(array[i]);
        Vec::setItem(j, clone);
    }

    return static_cast<unsigned int>(itemCount);
}


//!
//! Add given items. Return number of items successfully added. This
//! number can be less than the given item count if growth is required
//! but this vector cannot grow.
//!
unsigned int StringVec::add(const StringVec& vec)
{
    size_t startAt = Vec::numItems();
    unsigned int itemCount = Vec::add(vec);
    cloneItems(startAt, itemCount);
    return itemCount;
}


//!
//! Add given subset (itemCount items starting at startAt). Return number
//! of items successfully added. This number can be less than itemCount
//! if growth is required but this vector cannot grow.
//!
unsigned int StringVec::add(const StringVec& vec, size_t startAt, size_t itemCount)
{
    size_t i0 = Vec::numItems();
    unsigned int itemCount32 = Vec::add(vec, startAt, itemCount);
    cloneItems(i0, itemCount32);
    return itemCount32;
}


//!
//! Add count entries to the tail end. Use item for the new values.
//! Return number of entries successfully added. This number can be
//! less than count if growth is required but the vector cannot grow.
//!
unsigned int StringVec::add(size_t count, const String& item)
{
    size_t startAt = Vec::numItems();
    unsigned int itemCount = Vec::add(count, const_cast<String*>(&item));
    cloneItems(startAt, itemCount);
    return itemCount;
}


//!
//! Return the length of the longest string in the vector. If useByteSize
//! is true, the decision is made based on raw lengths in bytes. Otherwise,
//! the decision is made based on string lengths in characters. Similarly,
//! the returned value is in bytes if useByteSize is true, and in characters
//! otherwise.
//!
unsigned int StringVec::findMaxLength(bool useByteSize) const
{
    const String* const* p = raw();
    unsigned int maxLength = 0;

    if (useByteSize)
    {
        for (size_t i = 0, length = numItems(); i < length; ++i)
        {
            const String& s = *p[i];
            if (s.byteSize() > maxLength)
            {
                maxLength = s.byteSize();
            }
        }
    }

    else
    {
        for (size_t i = 0, length = numItems(); i < length; ++i)
        {
            const String& s = *p[i];
            if (s.length() > maxLength)
            {
                maxLength = s.length();
            }
        }
    }

    return maxLength;
}


//!
//! Return the length of the shortest string in the vector. If useByteSize
//! is true, the decision is made based on raw lengths in bytes. Otherwise,
//! the decision is made based on string lengths in characters. Similarly,
//! the returned value is in bytes if useByteSize is true, and in characters
//! otherwise.
//!
unsigned int StringVec::findMinLength(bool useByteSize) const
{
    const String* const* p = raw();
    unsigned int minLength = 0xffffffffU;

    if (useByteSize)
    {
        for (size_t i = 0, length = numItems(); i < length; ++i)
        {
            const String& s = *p[i];
            if (s.byteSize() < minLength)
            {
                minLength = s.byteSize();
            }
        }
    }

    else
    {
        for (size_t i = 0, length = numItems(); i < length; ++i)
        {
            const String& s = *p[i];
            if (s.length() < minLength)
            {
                minLength = s.length();
            }
        }
    }

    return (minLength == 0xffffffffU)? 0: minLength;
}


//!
//! Reset with given subset (itemCount items starting at startAt).
//! Return number of items successfully added. This number can be
//! less than itemCount if growth is required but this vector cannot
//! grow.
//!
unsigned int StringVec::reset(const StringVec& vec, size_t startAt, size_t itemCount)
{
    reset();
    size_t i0 = 0;
    unsigned int itemCount32 = Vec::reset(vec, startAt, itemCount);
    cloneItems(i0, itemCount32);
    return itemCount32;
}


void StringVec::cloneItems(size_t startAt, size_t itemCount)
{
    const String* const* p = raw();
    for (size_t i = startAt, iEnd = startAt + itemCount; i < iEnd; ++i)
    {
        String* clone = new String(*p[i]);
        Vec::setItem(i, clone);
    }
}


//!
//! Reset the vector by removing all items.
//!
void StringVec::reset()
{
    const String* const* p = raw();
    for (size_t i = Vec::numItems(); i > 0; delete p[--i]);
    Vec::reset();
}


void StringVec::setCmpFuncs(bool ignoreCase)
{
    if (ignoreCase)
    {
        compare_ = String::comparePI;
        compareK_ = String::compareKPI;
    }
    else
    {
        compare_ = String::compareP;
        compareK_ = String::compareKP;
    }
}


//!
//! Replace the item at given index. Don't do any error checking.
//! Behavior is unpredictable if given index is invalid.
//!
void StringVec::setItem(size_t index, const String& item)
{
    String* s = static_cast<String*>(Vec::peek(index));
    *s = item;
}


//!
//! Replace the item at given index. Don't do any error checking.
//! Behavior is unpredictable if given index is invalid.
//!
void StringVec::setItem(size_t index, const String& item, String& replacedItem)
{
    String* s = static_cast<String*>(Vec::peek(index));
    replacedItem = *s;
    *s = item;
}


//!
//! Sort vector and save results in sorted. Use given comparison function.
//!
void StringVec::sort(compare_t compare, StringVec& sorted, bool reverseOrder) const
{
    sorted.reset();
    Vec::sort(compare, sorted, reverseOrder);
    size_t itemCount = sorted.numItems();
    size_t startAt = 0;
    sorted.cloneItems(startAt, itemCount);
}


StringVec operator -(const StringVec& a, const StringVec& b)
{
    StringVec result(a);
    const String* const* p = b.raw();
    for (size_t i = 0, numItems = b.numItems(); i < numItems; result.rm(*p[i++]));
    return &result; //move guts from result to returned vector
}

END_NAMESPACE1
