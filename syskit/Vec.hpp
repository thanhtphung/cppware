/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_VEC_HPP
#define SYSKIT_VEC_HPP

#include "syskit/Growable.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


#if _WIN32
#pragma pack(push,4)
#endif

//! vector of opaque items
class Vec: public Growable
    //!
    //! A class representing an unsorted vector of opaque items. The vector
    //! has an initial capacity of capacity() items. The capacity can grow
    //! as needed if the growth factor is set to non-zero when constructed or
    //! afterwards using setGrowth(). If the vector is growable, growth can
    //! occur when items are added. Items are added using various forms of
    //! the add() method and removed using various forms of the rm() method.
    //! Updates can be expensive if many items need to be shifted up or down.
    //! For best update performance, items should be added to the tail end,
    //! and items should be removed without maintaining the current ordering.
    //! Example:
    //!\code
    //! Vec vec;
    //! vec.add(item0);
    //! vec.addAtIndex(1 /*index*/, item1, false /*maintainOrder*/);
    //! vec.addAtIndex(0 /*index*/, item2, true /*maintainOrder*/);
    //! vec.rmFromIndex(1 /*index*/, false /*maintainOrder*/);
    //! vec.rmFromIndex(0 /*index*/, true /*maintainOrder*/);
    //!\endcode
    //!
{

public:
    enum
    {
        DefaultCap = 64
    };

    //! Return a negative value if item0<item1, a positive value if item 0>item 1, and zero otherwise.
    typedef int(*compare_t)(const void* item0, const void* item1);

    //! Return non-zero if items differ, and zero otherwise.
    typedef int(*diff_t)(const void* item0, const void* item1);

    typedef struct
    {
        // Search for.
        const void* item;
        compare_t compare;
        // Search in.
        const void* const* raw;
        size_t numItems;
    } searchArg_t;
    typedef void* item_t;

    // Constructors.
    Vec(Vec* that);
    Vec(const Vec& vec);
    Vec(const Vec& vec, size_t startAt, size_t itemCount);
    Vec(unsigned int capacity = DefaultCap, int growBy = 0);

    // Operators.
    bool operator !=(const Vec& vec) const;
    bool operator ==(const Vec& vec) const;
    const Vec& operator =(Vec* that);
    const Vec& operator =(const Vec& vec);
    item_t operator [](size_t index) const;
    item_t& operator [](size_t index);
    static void operator delete(void* p, size_t size);
    static void operator delete(void* p, void* buf);
    static void* operator new(size_t size);
    static void* operator new(size_t size, void* buf);

    // Vector operations.
    bool add(item_t item);
    bool addAtIndex(size_t index, item_t item, bool maintainOrder = true);
    bool addIfNotFound(item_t item);
    bool addIfNotFound(item_t item, diff_t diff);
    bool empty() const;
    bool find(const void* item, diff_t diff) const;
    bool find(const void* item, diff_t diff, size_t& foundIndex) const;
    bool find(const void* item, size_t& foundIndex) const;
    bool rm(const void* item, bool maintainOrder = false);
    bool rm(const void* item, diff_t diff, bool maintainOrder = false);
    bool rm(const void* item, diff_t diff, item_t& removedItem, bool maintainOrder = false);
    bool rmFromIndex(size_t index, bool maintainOrder = false);
    bool rmFromIndex(size_t index, item_t& removedItem, bool maintainOrder = false);
    bool rmFromIndex(size_t startAt, size_t itemCount, bool maintainOrder = true);
    bool rmTail();
    bool rmTail(item_t& removedItem);
    bool truncate(size_t numItems);
    unsigned int add(const Vec& vec);
    unsigned int add(const Vec& vec, size_t startAt, size_t itemCount);
    unsigned int add(size_t count, item_t item);
    unsigned int add(void* const* raw, size_t itemCount);
    unsigned int findIndex(const void* item) const;
    unsigned int findIndex(const void* item, diff_t diff) const;
    unsigned int reset(const Vec& vec, size_t startAt, size_t itemCount);
    void reset();
    void setItem(size_t index, item_t item);
    void setItem(size_t index, item_t item, item_t& replacedItem);

    // Getters.
    item_t peek(size_t index) const;
    item_t* detachRaw();
    item_t* detachRaw(unsigned int& numItems);
    unsigned int numItems() const;
    void* const* raw() const;

    // Sort and search.
    bool equals(const Vec& vec, diff_t diff) const;
    bool search(const void* item, compare_t compare) const;
    bool search(const void* item, compare_t compare, size_t& foundIndex) const;
    void sort(compare_t compare, Vec& sorted, bool reverseOrder = false) const;
    void sort(compare_t compare, bool reverseOrder = false);

    // Override Growable.
    virtual ~Vec();
    virtual bool resize(unsigned int newCap);

    static bool search(const searchArg_t& arg);
    static bool search(const searchArg_t& arg, size_t& foundIndex);
    static item_t findKthSmallest(item_t* item, size_t itemCount, size_t k, compare_t compare);
    static void sort(item_t* raw, size_t numItems, compare_t compare, bool reverseOrder = false);

private:
    item_t* item_;
    unsigned int numItems_;

};

#if _WIN32
#pragma pack(pop)
#endif

END_NAMESPACE1

#include "syskit/BufPool.hpp"

BEGIN_NAMESPACE1(syskit)

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline Vec::item_t Vec::operator [](size_t index) const
{
    return item_[index];
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline Vec::item_t& Vec::operator [](size_t index)
{
    return item_[index];
}

//! Return true if this vector does not equal given vector.
//! This is a shallow comparison. Use equals() for deep comparisons.
inline bool Vec::operator !=(const Vec& vec) const
{
    bool ne = (numItems_ != vec.numItems_) || (memcmp(item_, vec.item_, numItems_ * sizeof(*item_)) != 0);
    return ne;
}

//! Return true if this vector equals given vector. That is, if both
//! have the same number of items and corresponding items are identical.
//! This is a shallow comparison. Use equals() for deep comparisons.
inline bool Vec::operator ==(const Vec& vec) const
{
    bool eq = (numItems_ == vec.numItems_) && (memcmp(item_, vec.item_, numItems_ * sizeof(*item_)) == 0);
    return eq;
}

inline void Vec::operator delete(void* p, size_t size)
{
    BufPool::freeBuf(p, size);
}

inline void Vec::operator delete(void* /*p*/, void* /*buf*/)
{
}

inline void* Vec::operator new(size_t size)
{
    void* buf = BufPool::allocateBuf(size);
    return buf;
}

inline void* Vec::operator new(size_t /*size*/, void* buf)
{
    return buf;
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline Vec::item_t Vec::peek(size_t index) const
{
    return item_[index];
}

//! Detach raw vector from instance. The returned vector is allocated
//! from the heap and is to be freed by the caller using the delete[]
//! operator when done. Instance is now disabled and must not be used
//! further.
inline Vec::item_t* Vec::detachRaw(unsigned int& numItems)
{
    numItems = numItems_;
    item_t* raw = detachRaw();
    return raw;
}

//! Add given item to the tail end only if it's unique. Return true if item
//! was added successful. Return false otherwise (vector is full or item
//! already exists).
inline bool Vec::addIfNotFound(item_t item)
{
    size_t foundIndex;
    bool ok = find(item, foundIndex)? false: add(item);
    return ok;
}

//! Add given item to the tail end only if it's unique. Use given comparison
//! function to compare items. Return true if item was added successful. Return
//! false otherwise (vector is full or item already exists).
inline bool Vec::addIfNotFound(item_t item, diff_t diff)
{
    size_t foundIndex;
    bool ok = find(item, diff, foundIndex)? false: add(item);
    return ok;
}

//! Return true if this vector is empty.
inline bool Vec::empty() const
{
    return (numItems_ == 0);
}

//! Locate given item using linear search. Return true if found.
inline bool Vec::find(const void* item, diff_t diff) const
{
    size_t foundIndex;
    bool found = find(item, diff, foundIndex);
    return found;
}

//! Locate and remove given item using linear search. Return true if found.
//! Return false otherwise. Item removal might affect vector ordering. If
//! maintainOrder is true, appropriate items would be shifted up. Otherwise,
//! the last item would move up to replace the removed item.
inline bool Vec::rm(const void* item, diff_t diff, bool maintainOrder)
{
    item_t removedItem;
    bool found = rm(item, diff, removedItem, maintainOrder);
    return found;
}

//! Remove item from given index. Return true if successful (i.e., given
//! index is valid). Return false otherwise. If maintainOrder is true,
//! appropriate items will be shifted up. Otherwise, the last item will
//! move up to replace the removed item.
inline bool Vec::rmFromIndex(size_t index, bool maintainOrder)
{
    item_t removedItem;
    bool ok = rmFromIndex(index, removedItem, maintainOrder);
    return ok;
}

//! Remove the item from the tail end. Return true if successful. Return
//! false otherwise (i.e., vector is empty).
inline bool Vec::rmTail()
{
    bool ok = (numItems_ > 0)? (--numItems_, true): false;
    return ok;
}

//! Remove the item from the tail end. Return true (also set removedItem to the
//! removed item) if successful. Return false otherwise (i.e., vector is empty).
inline bool Vec::rmTail(item_t& removedItem)
{
    bool ok = (numItems_ > 0)? ((removedItem = item_[--numItems_]), true): false;
    return ok;
}

//! Assume given vector (arg.numItems items in arg.raw) is sorted, locate
//! given item (arg.item) using binary search. When items are compared, use
//! the given comparison function (arg.compare). Return true if found.
//! Behavior is unpredictable if vector is unsorted.
inline bool Vec::search(const searchArg_t& arg)
{
    size_t foundIndex;
    bool found = search(arg, foundIndex);
    return found;
}

//! Assume vector is sorted, locate given item using binary search.
//! Return true if found. Behavior is unpredictable if vector is unsorted.
inline bool Vec::search(const void* item, compare_t compare) const
{
    size_t foundIndex;
    searchArg_t arg = {item, compare, item_, numItems_};
    bool found = search(arg, foundIndex);
    return found;
}

//! Assume vector is sorted, locate given item using binary search.
//! Return true if found (also return the located index in foundIndex).
//! Return false otherwise (also return the nearest item in foundIndex).
//! Behavior is unpredictable if vector is unsorted.
inline bool Vec::search(const void* item, compare_t compare, size_t& foundIndex) const
{
    searchArg_t arg = {item, compare, item_, numItems_};
    bool found = search(arg, foundIndex);
    return found;
}

//! Truncate vector if it has more than numItems items.
//! Return true if truncation occured.
inline bool Vec::truncate(size_t numItems)
{
    bool truncated = (numItems_ > numItems)? ((numItems_ = static_cast<unsigned int>(numItems)), true): (false);
    return truncated;
}

//! Add given items. Return number of items successfully added. This
//! number can be less than the given item count if growth is required
//! but this vector cannot grow.
inline unsigned int Vec::add(const Vec& vec)
{
    unsigned int numAdds = add(vec.item_, vec.numItems_);
    return numAdds;
}

//! Add given subset (itemCount items starting at startAt). Return number
//! of items successfully added. This number can be less than itemCount
//! if growth is required but this vector cannot grow.
inline unsigned int Vec::add(const Vec& vec, size_t startAt, size_t itemCount)
{
    unsigned int numAdds = add(vec.item_ + startAt, itemCount);
    return numAdds;
}

//! Locate the index of given item using linear search. Return located
//! index if found. Return INVALID_INDEX otherwise.
inline unsigned int Vec::findIndex(const void* item) const
{
    size_t foundIndex;
    find(item, foundIndex);
    return static_cast<unsigned int>(foundIndex);
}

//! Locate the index of given item using linear search. Return located
//! index if found. Return INVALID_INDEX otherwise.
inline unsigned int Vec::findIndex(const void* item, diff_t diff) const
{
    size_t foundIndex;
    find(item, diff, foundIndex);
    return static_cast<unsigned int>(foundIndex);
}

//! Return the current number of items in the vector.
inline unsigned int Vec::numItems() const
{
    return numItems_;
}

//! Reset with given subset (itemCount items starting at startAt).
//! Return number of items successfully added. This number can be
//! less than itemCount if growth is required but this vector cannot
//! grow.
inline unsigned int Vec::reset(const Vec& vec, size_t startAt, size_t itemCount)
{
    numItems_ = 0;
    unsigned int numAdds = add(vec.item_ + startAt, itemCount);
    return numAdds;
}

//! Reset the vector by removing all items.
inline void Vec::reset()
{
    numItems_ = 0;
}

//! Replace the item at given index. Don't do any error checking.
//! Behavior is unpredictable if given index is invalid.
inline void Vec::setItem(size_t index, item_t item)
{
    item_[index] = item;
}

//! Replace the item at given index (also return the replaced item
//! in replacedItem). Don't do any error checking. Behavior is
//! unpredictable if given index is invalid.
inline void Vec::setItem(size_t index, item_t item, item_t& replacedItem)
{
    replacedItem = item_[index];
    item_[index] = item;
}

//! Sort vector. Use given comparison function.
inline void Vec::sort(compare_t compare, bool reverseOrder)
{
    sort(item_, numItems_, compare, reverseOrder);
}

//! Return the internal raw vector. To be used with extra care.
inline void* const* Vec::raw() const
{
    return item_;
}

END_NAMESPACE1

#endif
