/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_STRING_VEC_HPP
#define APPKIT_STRING_VEC_HPP

#include "appkit/String.hpp"
#include "syskit/Vec.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)

class DelimitedTxt;


//! vector of strings
class StringVec: private syskit::Vec
    //!
    //! A class representing an unsorted vector of strings. The vector has
    //! an initial capacity of capacity() items. The capacity can grow as
    //! needed if the growth factor is set to non-zero when constructed or
    //! afterwards using setGrowth(). If the vector is growable, growth can
    //! occur when items are added. Items are added using various forms of
    //! the add() method and removed using various forms of the rm() method.
    //! Updates can be expensive if many items need to be shifted up or down.
    //! For best update performance, items should be added to the tail end,
    //! and items should be removed without maintaining the current ordering.
    //! Example:
    //!\code
    //! StringVec vec;
    //! vec.add(item0);
    //! vec.addAtIndex(1 /*index*/, item1);
    //! vec.rm(item2, false /*maintainOrder*/);
    //! vec.rm(item3, true /*maintainOrder*/);
    //! vec.rmFromIndex(1 /*index*/, false /*maintainOrder*/);
    //! vec.rmFromIndex(0 /*index*/, true /*maintainOrder*/);
    //!\endcode
    //!
{

public:

    // From Growable.
    using Growable::INVALID_INDEX;
    using Growable::canGrow;
    using Growable::capacity;
    using Growable::growthFactor;
    using Growable::initialCap;

    // From Vec.
    using Vec::DefaultCap;
    using Vec::compare_t;
    using Vec::diff_t;
    using Vec::operator delete;
    using Vec::operator new;
    using Vec::empty;
    using Vec::numItems;
    using Vec::resize;
    using Vec::setGrowth;

    // Constructors.
    StringVec(DelimitedTxt& txt, bool trimLines = true, unsigned int capacity = DefaultCap, int growBy = -1, bool ignoreCase = false);
    StringVec(StringVec* that);
    StringVec(const StringVec& vec);
    StringVec(const StringVec& vec, size_t startAt, size_t itemCount, bool ignoreCase = false);
    StringVec(unsigned int capacity = DefaultCap, int growBy = -1, bool ignoreCase = false);

    // Operators.
    String operator [](size_t index) const;
    String& operator [](size_t index);
    bool operator !=(const StringVec& vec) const;
    bool operator ==(const StringVec& vec) const;
    const StringVec& operator =(StringVec* that);
    const StringVec& operator =(const StringVec& vec);

    // Vector operations.
    String stringify(const char* delim = 0, size_t maxItems = static_cast<size_t>(0) - 1) const;
    bool add(const String& item);
    bool addAtIndex(size_t index, const String& item, bool maintainOrder = true);
    bool addIfNotFound(const String& item);
    bool contains(const String& item) const;
    bool contains(const char* item) const;
    bool find(const String& item) const;
    bool find(const String& item, size_t& foundIndex) const;
    bool find(const char* item) const;
    bool find(const char* item, size_t& foundIndex) const;
    bool reset(DelimitedTxt& txt, bool trimLines = true);
    bool rm(const String& item, bool maintainOrder = false);
    bool rm(const char* item, bool maintainOrder = false);
    bool rmFromIndex(size_t index, String& removedItem, bool maintainOrder = false);
    bool rmFromIndex(size_t index, bool maintainOrder = false);
    bool rmTail();
    bool rmTail(String& removedItem);
    bool truncate(size_t numItems);
    unsigned int add(const String* array, size_t itemCount);
    unsigned int add(const StringVec& vec);
    unsigned int add(const StringVec& vec, size_t startAt, size_t itemCount);
    unsigned int add(size_t count, const String& item);
    unsigned int findIndex(const String& item) const;
    unsigned int findIndex(const char* item) const;
    unsigned int findMaxLength(bool useByteSize = false) const;
    unsigned int findMinLength(bool useByteSize = false) const;
    unsigned int reset(const StringVec& vec, size_t startAt, size_t itemCount);
    void reset();
    void setItem(size_t index, const String& item);
    void setItem(size_t index, const String& item, String& replacedItem);

    // Getters.
    const String& peek(size_t index) const;
    const String* const* raw() const;

    // Sort and search.
    bool search(compare_t compare, const String& item) const;
    bool search(compare_t compare, const String& item, size_t& foundIndex) const;
    bool search(const String& item) const;
    bool search(const String& item, size_t& foundIndex) const;
    bool search(const char* item) const;
    bool search(const char* item, size_t& foundIndex) const;
    void sort(bool reverseOrder = false);
    void sort(StringVec& sorted, bool reverseOrder = false) const;
    void sort(compare_t compare, bool reverseOrder = false);
    void sort(compare_t compare, StringVec& sorted, bool reverseOrder = false) const;

    // Override Vec.
    virtual ~StringVec();

private:
    String empty_;
    compare_t compare_;
    compare_t compareK_;

    bool doReset(DelimitedTxt&, bool);
    void cloneItems(size_t, size_t);
    void setCmpFuncs(bool);

};

StringVec operator +(const StringVec& a, const String& b);
StringVec operator +(const StringVec& a, const StringVec& b);
StringVec operator -(const StringVec& a, const String& b);
StringVec operator -(const StringVec& a, const StringVec& b);

//! Peek at given index and return the residing item. The return
//! value is an empty string if given index is invalid.
inline String StringVec::operator [](size_t index) const
{
    return (index < numItems())?
        *static_cast<const String*>(Vec::operator [](index)):
        empty_;
}

//! Peek at given index and return the residing item.
//! Behavior is unpredictable if given index is invalid.
inline String& StringVec::operator [](size_t index)
{
    return (index < numItems())?
        *static_cast<String*>(Vec::operator [](index)):
        empty_;
}

//! Return true if this vector does not equal given vector.
inline bool StringVec::operator !=(const StringVec& vec) const
{
    return !(operator ==(vec));
}

//! Locate given item using linear search. Return true if found.
//! Return false otherwise.
inline bool StringVec::contains(const String& item) const
{
    size_t foundIndex;
    bool found = Vec::find(&item, compare_, foundIndex);
    return found;
}

//! Locate given item using linear search. Return true if found.
//! Return false otherwise.
inline bool StringVec::contains(const char* item) const
{
    size_t foundIndex;
    bool found = Vec::find(item, compareK_, foundIndex);
    return found;
}

//! Locate given item using linear search. Return true if found.
//! Return false otherwise.
inline bool StringVec::find(const String& item) const
{
    size_t foundIndex;
    bool found = Vec::find(&item, compare_, foundIndex);
    return found;
}

//! Locate given item using linear search. Return true if found (also return
//! the located index in foundIndex). Return false and INVALID_INDEX otherwise.
inline bool StringVec::find(const String& item, size_t& foundIndex) const
{
    bool found = Vec::find(&item, compare_, foundIndex);
    return found;
}

//! Locate given item using linear search. Return true if found.
//! Return false otherwise.
inline bool StringVec::find(const char* item) const
{
    size_t foundIndex;
    bool found = Vec::find(item, compareK_, foundIndex);
    return found;
}

//! Locate given item using linear search. Return true if found (also return
//! the located index in foundIndex). Return false and INVALID_INDEX otherwise.
inline bool StringVec::find(const char* item, size_t& foundIndex) const
{
    bool found = Vec::find(item, compareK_, foundIndex);
    return found;
}

//! Locate given item using linear search. Return true if found.
//! Reset instance with lines from given delimited text. Lines become
//! vector items. By default, lines are trimmed. Use trimLines to specify
//! otherwise. Return true if successful. Return false otherwise (result
//! is partial since instance could not grow to accomodate all lines).
inline bool StringVec::reset(DelimitedTxt& txt, bool trimLines)
{
    reset();
    bool ok = doReset(txt, trimLines);
    return ok;
}

//! Assume vector is sorted, locate given item using binary search.
//! Return true if found. Behavior is unpredictable if vector is unsorted.
inline bool StringVec::search(const String& item) const
{
    size_t foundIndex;
    bool found = Vec::search(&item, compare_, foundIndex);
    return found;
}

//! Assume vector is sorted, locate given item using binary search.
//! Return true if found. Behavior is unpredictable if vector is unsorted.
inline bool StringVec::search(compare_t compare, const String& item) const
{
    size_t foundIndex;
    bool found = Vec::search(&item, compare, foundIndex);
    return found;
}

//! Assume vector is sorted using given comparison function, locate given item
//! using binary search. Return true if found (also return the located index in
//! foundIndex). Return false otherwise (also return the nearest item in foundIndex).
//! Behavior is unpredictable if vector is unsorted.
inline bool StringVec::search(compare_t compare, const String& item, size_t& foundIndex) const
{
    bool found = Vec::search(&item, compare, foundIndex);
    return found;
}

//! Assume vector is sorted, locate given item using binary search.
//! Return true if found (also return the located index in foundIndex).
//! Return false otherwise (also return the nearest item in foundIndex).
//! Behavior is unpredictable if vector is unsorted.
inline bool StringVec::search(const String& item, size_t& foundIndex) const
{
    bool found = Vec::search(&item, compare_, foundIndex);
    return found;
}

//! Assume vector is sorted, locate given item using binary search.
//! Return true if found. Behavior is unpredictable if vector is unsorted.
inline bool StringVec::search(const char* item) const
{
    size_t foundIndex;
    bool found = Vec::search(item, compareK_, foundIndex);
    return found;
}

//! Assume vector is sorted, locate given item using binary search.
//! Return true if found (also return the located index in foundIndex).
//! Return false otherwise (also return the nearest item in foundIndex).
//! Behavior is unpredictable if vector is unsorted.
inline bool StringVec::search(const char* item, size_t& foundIndex) const
{
    bool found = Vec::search(item, compareK_, foundIndex);
    return found;
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline const String& StringVec::peek(size_t index) const
{
    const String& s = *static_cast<const String*>(Vec::peek(index));
    return s;
}

//! Return the internal raw vector. To be used with extra care.
inline const String* const* StringVec::raw() const
{
    const String* const* p = reinterpret_cast<const String* const*>(Vec::raw());
    return p;
}

//! Locate the index of given item using linear search. Return located
//! index if found. Return INVALID_INDEX otherwise.
inline unsigned int StringVec::findIndex(const String& item) const
{
    size_t foundIndex;
    Vec::find(&item, compare_, foundIndex);
    return static_cast<unsigned int>(foundIndex);
}

//! Locate the index of given item using linear search. Return located
//! index if found. Return INVALID_INDEX otherwise.
inline unsigned int StringVec::findIndex(const char* item) const
{
    size_t foundIndex;
    Vec::find(item, compareK_, foundIndex);
    return static_cast<unsigned int>(foundIndex);
}

//! Sort vector. Order is ascending if reverseOrder is false and is descending
//! if reverseOrder is true.
inline void StringVec::sort(bool reverseOrder)
{
    Vec::sort(compare_, reverseOrder);
}

//! Sort vector using given comparison function.
inline void StringVec::sort(compare_t compare, bool reverseOrder)
{
    Vec::sort(compare, reverseOrder);
}

//! Sort vector and save results in sorted. Order is ascending if reverseOrder
//! is false and is descending if reverseOrder is true.
inline void StringVec::sort(StringVec& sorted, bool reverseOrder) const
{
    sort(compare_, sorted, reverseOrder);
}

inline StringVec operator +(const StringVec& a, const String& b)
{
    StringVec result(a);
    result.add(b);
    return &result; //move guts from result to returned vector
}

inline StringVec operator +(const StringVec& a, const StringVec& b)
{
    StringVec result(a);
    result.add(b);
    return &result; //move guts from result to returned vector
}

inline StringVec operator -(const StringVec& a, const String& b)
{
    StringVec result(a);
    result.rm(b);
    return &result; //move guts from result to returned vector
}

END_NAMESPACE1

#endif
