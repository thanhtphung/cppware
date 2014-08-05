/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_STR_ARRAY_HPP
#define APPKIT_STR_ARRAY_HPP

#include <sys/types.h>
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)

class String;


//! Array of null-terminated strings.
class StrArray
    //!
    //! This class represents a fixed-size array of null-terminated strings. All
    //! copies, if any, are shallow. For comparison, StringVec is a flexible-sized
    //! array of deep-copied strings.
    //!
{

public:

    //! Return a negative value if item0<item1, a positive value if item 0>item 1, and zero otherwise.
    typedef int(*compare_t)(const void* item0, const void* item1);

    // Constructors.
    StrArray(const char** array);
    StrArray(const char* const* array);
    StrArray(const char** array, size_t numItems);
    StrArray(const char* const* array, size_t numItems);

    // Operators.
    const char* operator [](size_t index) const;
    const char*& operator [](size_t index);

    // Array operations.
    bool contains(const String& item, bool ignoreCase = false) const;
    bool contains(const char* item, bool ignoreCase = false) const;
    bool find(const String& item, bool ignoreCase = false) const;
    bool find(const String& item, size_t& foundIndex, bool ignoreCase = false, bool allowItemAsIndex = false) const;
    bool find(const char* item, bool ignoreCase = false) const;
    bool find(const char* item, size_t& foundIndex, bool ignoreCase = false, bool allowItemAsIndex = false) const;
    const char* peek(size_t index) const;
    const char* const* raw() const;
    unsigned int numItems() const;
    void reset(const char** array);
    void reset(const char* const* array);
    void reset(const char** array, size_t numItems);
    void reset(const char* const* array, size_t numItems);
    void setItem(size_t index, const char* item);
    void setItem(size_t index, const char* item, const char*& replacedItem);

    // Sort and search.
    bool search(compare_t compare, const String& item) const;
    bool search(compare_t compare, const String& item, size_t& foundIndex) const;
    bool search(compare_t compare, const char* item) const;
    bool search(compare_t compare, const char* item, size_t& foundIndex) const;
    bool search(const String& item) const;
    bool search(const String& item, size_t& foundIndex) const;
    bool search(const char* item) const;
    bool search(const char* item, size_t& foundIndex) const;
    void sort(bool reverseOrder = false);
    void sort(compare_t compare, bool reverseOrder = false);

private:
    union
    {
        const char** item_;
        const char* const* roItem_;
    };
    unsigned int numItems_;

};

END_NAMESPACE1

#include "appkit/Str.hpp"
#include "appkit/String.hpp"
#include "syskit/Vec.hpp"

BEGIN_NAMESPACE1(appkit)

//! Construct instance from given null-terminated array of null-terminated
//! strings. All copies, if any, are shallow.
inline StrArray::StrArray(const char** array)
{
    const char* const* p = array;
    reset(p);
}

//! Construct a read-only instance from given null-terminated array of
//! null-terminated strings. All copies, if any, are shallow.
inline StrArray::StrArray(const char* const* array)
{
    reset(array);
}

//! Construct instance from given array of null-terminated strings. Given
//! array holds numItems strings. All copies, if any, are shallow.
inline StrArray::StrArray(const char** array, size_t numItems)
{
    item_ = array;
    numItems_ = static_cast<unsigned int>(numItems);
}

//! Construct a read-only instance from given array of null-terminated strings.
//! Given array holds numItems strings. All copies, if any, are shallow.
inline StrArray::StrArray(const char* const* array, size_t numItems)
{
    roItem_ = array;
    numItems_ = static_cast<unsigned int>(numItems);
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline const char* StrArray::operator [](size_t index) const
{
    return item_[index];
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid or if
//! instance is read-only.
inline const char*& StrArray::operator [](size_t index)
{
    return item_[index];
}

//! Locate given item using linear search. Ignore case if ignoreCase is true.
//! Return true if found.
inline bool StrArray::contains(const String& item, bool ignoreCase) const
{
    size_t foundIndex;
    bool allowItemAsIndex = false;
    bool found = find(item.ascii(), foundIndex, ignoreCase, allowItemAsIndex);
    return found;
}

//! Locate given item using linear search. Ignore case if ignoreCase is true.
//! Return true if found.
inline bool StrArray::find(const String& item, bool ignoreCase) const
{
    size_t foundIndex;
    bool allowItemAsIndex = false;
    bool found = find(item.ascii(), foundIndex, ignoreCase, allowItemAsIndex);
    return found;
}

//! Locate given item using linear search. Ignore case if ignoreCase is true.
//! If allowItemAsIndex is true, interpret item as array index if necessary. For
//! example, if allowItemAsIndex is true, then either "third" or "2" can be used
//! to locate the third item in an array containing {"first", "second", "third"}.
//! Return true if found (also return the located index in foundIndex). Return
//! false otherwise.
inline bool StrArray::find(const String& item, size_t& foundIndex, bool ignoreCase, bool allowItemAsIndex) const
{
    bool found = find(item.ascii(), foundIndex, ignoreCase, allowItemAsIndex);
    return found;
}

//! Locate given item using linear search. Ignore case if ignoreCase is true.
//! Return true if found.
inline bool StrArray::contains(const char* item, bool ignoreCase) const
{
    size_t foundIndex;
    bool found = find(item, foundIndex, ignoreCase);
    return found;
}

//! Locate given item using linear search. Ignore case if ignoreCase is true.
//! Return true if found.
inline bool StrArray::find(const char* item, bool ignoreCase) const
{
    size_t foundIndex;
    bool found = find(item, foundIndex, ignoreCase);
    return found;
}

//! Assume array is sorted, locate given item using binary search.
//! Return true if found. Behavior is unpredictable if vector is unsorted.
inline bool StrArray::search(compare_t compare, const String& item) const
{
    size_t foundIndex;
    void** raw = const_cast<void**>(reinterpret_cast<const void**>(item_));
    syskit::Vec::searchArg_t arg = {item.ascii(), compare, raw, numItems_};
    bool found = syskit::Vec::search(arg, foundIndex);
    return found;
}

//! Assume array is sorted, locate given item using binary search.
//! Return true if found (also return the located index in foundIndex).
//! Return false otherwise (also return the nearest item in foundIndex).
//! Behavior is unpredictable if vector is unsorted.
inline bool StrArray::search(compare_t compare, const String& item, size_t& foundIndex) const
{
    void** raw = const_cast<void**>(reinterpret_cast<const void**>(item_));
    syskit::Vec::searchArg_t arg = {item.ascii(), compare, raw, numItems_};
    bool found = syskit::Vec::search(arg, foundIndex);
    return found;
}

//! Assume array is sorted, locate given item using binary search.
//! Return true if found. Behavior is unpredictable if vector is unsorted.
inline bool StrArray::search(compare_t compare, const char* item) const
{
    size_t foundIndex;
    void** raw = const_cast<void**>(reinterpret_cast<const void**>(item_));
    syskit::Vec::searchArg_t arg = {item, compare, raw, numItems_};
    bool found = syskit::Vec::search(arg, foundIndex);
    return found;
}

//! Assume array is sorted, locate given item using binary search.
//! Return true if found (also return the located index in foundIndex).
//! Return false otherwise (also return the nearest item in foundIndex).
//! Behavior is unpredictable if vector is unsorted.
inline bool StrArray::search(compare_t compare, const char* item, size_t& foundIndex) const
{
    void** raw = const_cast<void**>(reinterpret_cast<const void**>(item_));
    syskit::Vec::searchArg_t arg = {item, compare, raw, numItems_};
    bool found = syskit::Vec::search(arg, foundIndex);
    return found;
}

//! Assume array is sorted, locate given item using binary search.
//! Return true if found. Behavior is unpredictable if vector is unsorted.
inline bool StrArray::search(const String& item) const
{
    size_t foundIndex;
    void** raw = const_cast<void**>(reinterpret_cast<const void**>(item_));
    syskit::Vec::searchArg_t arg = {item.ascii(), Str::compareK, raw, numItems_};
    bool found = syskit::Vec::search(arg, foundIndex);
    return found;
}

//! Assume array is sorted, locate given item using binary search.
//! Return true if found (also return the located index in foundIndex).
//! Return false otherwise (also return the nearest item in foundIndex).
//! Behavior is unpredictable if vector is unsorted.
inline bool StrArray::search(const String& item, size_t& foundIndex) const
{
    void** raw = const_cast<void**>(reinterpret_cast<const void**>(item_));
    syskit::Vec::searchArg_t arg = {item.ascii(), Str::compareK, raw, numItems_};
    bool found = syskit::Vec::search(arg, foundIndex);
    return found;
}

//! Assume array is sorted, locate given item using binary search.
//! Return true if found. Behavior is unpredictable if vector is unsorted.
inline bool StrArray::search(const char* item) const
{
    size_t foundIndex;
    void** raw = const_cast<void**>(reinterpret_cast<const void**>(item_));
    syskit::Vec::searchArg_t arg = {item, Str::compareK, raw, numItems_};
    bool found = syskit::Vec::search(arg, foundIndex);
    return found;
}

//! Assume array is sorted, locate given item using binary search.
//! Return true if found (also return the located index in foundIndex).
//! Return false otherwise (also return the nearest item in foundIndex).
//! Behavior is unpredictable if vector is unsorted.
inline bool StrArray::search(const char* item, size_t& foundIndex) const
{
    void** raw = const_cast<void**>(reinterpret_cast<const void**>(item_));
    syskit::Vec::searchArg_t arg = {item, Str::compareK, raw, numItems_};
    bool found = syskit::Vec::search(arg, foundIndex);
    return found;
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline const char* StrArray::peek(size_t index) const
{
    return item_[index];
}

//! Return the internal raw array. To be used with extra care.
inline const char* const* StrArray::raw() const
{
    return roItem_;
}

//! Return the number of items in the array.
inline unsigned int StrArray::numItems() const
{
    return numItems_;
}

//! Reset instance with given null-terminated array of null-terminated
//! strings. All copies, if any, are shallow.
inline void StrArray::reset(const char** array)
{
    const char* const* p = array;
    reset(p);
}

//! Reset instance with given array of null-terminated strings. Given
//! array holds numItems strings. All copies, if any, are shallow.
inline void StrArray::reset(const char** array, size_t numItems)
{
    item_ = array;
    numItems_ = static_cast<unsigned int>(numItems);
}

//! Reset instance with given array of null-terminated strings. Given
//! array holds numItems strings. All copies, if any, are shallow. Make
//! instance read-only.
inline void StrArray::reset(const char* const* array, size_t numItems)
{
    roItem_ = array;
    numItems_ = static_cast<unsigned int>(numItems);
}

//! Replace the item at given index. Don't do any error checking. Behavior
//! is unpredictable if given index is invalid or if instance is read-only.
inline void StrArray::setItem(size_t index, const char* item)
{
    item_[index] = item;
}

//! Replace the item at given index (also return the replaced item in
//! replacedItem). Don't do any error checking. Behavior is unpredictable
//! if given index is invalid or if instance is read-only.
inline void StrArray::setItem(size_t index, const char* item, const char*& replacedItem)
{
    replacedItem = item_[index];
    item_[index] = item;
}

//! Sort array. Order is ascending if reverseOrder is false and is descending
//! if reverseOrder is true. Behavior is unpredictable in a read-only instance.
inline void StrArray::sort(bool reverseOrder)
{
    void** raw = const_cast<void**>(reinterpret_cast<const void**>(item_));
    syskit::Vec::sort(raw, numItems_, Str::compareK, reverseOrder);
}

//! Sort array using given comparison function.
//! Behavior is unpredictable in a read-only instance.
inline void StrArray::sort(compare_t compare, bool reverseOrder)
{
    void** raw = const_cast<void**>(reinterpret_cast<const void**>(item_));
    syskit::Vec::sort(raw, numItems_, compare, reverseOrder);
}

END_NAMESPACE1

#endif
