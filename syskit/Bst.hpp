/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_BST_HPP
#define SYSKIT_BST_HPP

#include "syskit/Growable.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)

class Vec;


//! binary search table of opaque items
class Bst: public Growable
    //!
    //! A class representing a binary search table of opaque items. The table has
    //! an initial capacity of capacity() items. The capacity can grow as needed if
    //! the growth factor is set to non-zero when constructed or afterwards using
    //! setGrowth(). If the table is growable, growth can occur when items are added.
    //! Items are added using the add() method and removed using various forms of the
    //! rm() method. Searches are provided by the find() methods. The binary search
    //! methods are efficient in general, but any method requiring updates might have
    //! to shift many items up or down. If updates are frequent and the table capacity 
    //! is relatively large, consider using some Vec class if items don't need to be
    //! in order at all times, or some Tree/HashTable class if items do need to be in
    //! order. Example:
    //!\code
    //! Bst bst;
    //! :
    //! bst.add(item0); //add item0 to table
    //! bst.rm(item1);  //remove item1 from table
    //! :
    //! bst = vec;      //populate table with a Vec instance
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
    typedef void* item_t;

    // Constructors.
    Bst(compare_t compare, item_t* item, size_t numItems, int growBy = 0);
    Bst(compare_t compare = 0, unsigned int capacity = DefaultCap, int growBy = 0);
    Bst(const Bst& bst);
    Bst(const Vec& vec, compare_t compare);

    // Operators.
    bool operator !=(const Bst& bst) const;
    bool operator ==(const Bst& bst) const;
    const Bst& operator =(const Bst& bst);
    const Bst& operator =(const Vec& vec);
    item_t operator [](size_t index) const;

    // Table operations.
    bool add(item_t item);
    bool addIfNotFound(item_t item);
    bool find(const void* item) const;
    bool find(const void* item, compare_t compare) const;
    bool find(const void* item, compare_t, item_t& foundItem) const;
    bool find(const void* item, compare_t, item_t& foundItem, size_t& foundIndex) const;
    bool find(const void* item, compare_t, size_t& foundIndex) const;
    bool find(const void* item, item_t& foundItem) const;
    bool find(const void* item, item_t& foundItem, size_t& foundIndex) const;
    bool find(const void* item, size_t& foundIndex) const;
    bool rm(const void* item);
    bool rm(const void* item, compare_t compare);
    bool rm(const void* item, compare_t, item_t& removedItem);
    bool rm(const void* item, item_t& removedItem);
    bool rmFromIndex(size_t index);
    bool rmFromIndex(size_t index, item_t& removedItem);
    unsigned int findIndex(const void* item) const;
    unsigned int findIndex(const void* item, compare_t) const;
    void reset();

    // Getters.
    compare_t cmpFunc() const;
    item_t peek(size_t index) const;
    item_t* detachRaw();
    item_t* detachRaw(unsigned int& numItems);
    unsigned int numItems() const;
    void* const* raw() const;

    // Override Growable.
    virtual ~Bst();
    virtual bool resize(unsigned int newCap);

protected:
    bool add(item_t item, size_t& addedAtIndex, bool allowDuplicates = false);
    void setItem(size_t index, item_t item);

private:
    compare_t compare_;
    item_t* item_;
    unsigned int numItems_;

    void copyFrom(void* const*, size_t);

    static int compare(const void*, const void*);

};

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline Bst::item_t Bst::operator [](size_t index) const
{
    return item_[index];
}

//! Return true if this table does not equal given table.
//! That is, if both do not have the same number of entries
//! or if corresponding entries are not equal.
inline bool Bst::operator !=(const Bst& bst) const
{
    return !(*this == bst);
}

//! Return the comparison function used when items are compared.
inline Bst::compare_t Bst::cmpFunc() const
{
    return compare_;
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline Bst::item_t Bst::peek(size_t index) const
{
    return item_[index];
}

//! Detach raw table from instance. The returned table is allocated
//! from the heap and is to be freed by the caller using the delete[]
//! operator when done. Instance is now disabled and must not be used
//! further.
inline Bst::item_t* Bst::detachRaw(unsigned int& numItems)
{
    numItems = numItems_;
    item_t* raw = detachRaw();
    return raw;
}

//! Add given item to the table, even if the same item already exists
//! in the table. Return true if successful. Return false otherwise (i.e.,
//! table is full).
inline bool Bst::add(item_t item)
{
    size_t addedAtIndex;
    bool ok = add(item, addedAtIndex, true /*allowDuplicates*/);
    return ok;
}

//! Add given item to the table. Return true if successful. Return
//! false otherwise (i.e., table is full or item already exists).
inline bool Bst::addIfNotFound(item_t item)
{
    size_t addedAtIndex;
    bool ok = add(item, addedAtIndex, false /*allowDuplicates*/);
    return ok;
}

//! Locate given item using binary search. Return true if found.
//! Return false otherwise.
inline bool Bst::find(const void* item) const
{
    size_t foundIndex;
    bool found = find(item, compare_, foundIndex);
    return found;
}

//! Locate given item using binary search. Use given compatible comparison function.
//! Return true if found. Return false otherwise.
inline bool Bst::find(const void* item, compare_t compare) const
{
    size_t foundIndex;
    bool found = find(item, compare, foundIndex);
    return found;
}

//! Locate given item using binary search. Use given compatible comparison function.
//! Return true if found (also set foundItem to the found item). Return false otherwise.
inline bool Bst::find(const void* item, compare_t compare, item_t& foundItem) const
{
    size_t foundIndex;
    bool found = find(item, compare, foundIndex)? ((foundItem = item_[foundIndex]), true): (false);
    return found;
}

//! Locate given item using binary search. Use given compatible comparison function.
//! Return true if found (also set foundItem to the found item and foundIndex to the
//! index of the located item). Return false otherwise (also set foundIndex to the
//! nearest item).
inline bool Bst::find(const void* item, compare_t compare, item_t& foundItem, size_t& foundIndex) const
{
    bool found = find(item, compare, foundIndex)? ((foundItem = item_[foundIndex]), true): (false);
    return found;
}

//! Locate given item using binary search. Return true if found (also set foundItem to
//! the found item). Return false otherwise.
inline bool Bst::find(const void* item, item_t& foundItem) const
{
    size_t foundIndex;
    bool found = find(item, compare_, foundIndex)? ((foundItem = item_[foundIndex]), true): (false);
    return found;
}

//! Locate given item using binary search. Return true if found (also set foundItem to
//! the found item and foundIndex to the index of the located item). Return false otherwise
//! (also set foundIndex to the nearest item).
inline bool Bst::find(const void* item, item_t& foundItem, size_t& foundIndex) const
{
    bool found = find(item, compare_, foundIndex)? ((foundItem = item_[foundIndex]), true): (false);
    return found;
}

//! Locate given item using binary search. Return true if found (also set foundIndex
//! to the index of the located item). Return false otherwise (also set foundIndex to
//! the nearest item).
inline bool Bst::find(const void* item, size_t& foundIndex) const
{
    bool found = find(item, compare_, foundIndex);
    return found;
}

//! Locate given item using binary search. If found, remove it
//! from the table and return true. Return false otherwise.
inline bool Bst::rm(const void* item)
{
    item_t removedItem;
    bool found = rm(item, compare_, removedItem);
    return found;
}

//! Locate given item using binary search. Use given compatible comparison function.
//! If found, remove it from the table and return true. Return false otherwise.
inline bool Bst::rm(const void* item, compare_t compare)
{
    item_t removedItem;
    bool found = rm(item, compare, removedItem);
    return found;
}

//! Locate given item using binary search. If found, remove it
//! from the table and return true (also set removedItem
//! to the removed item). Return false otherwise.
inline bool Bst::rm(const void* item, item_t& removedItem)
{
    bool found = rm(item, compare_, removedItem);
    return found;
}

//! Remove item at given index. Return true if successful
//! (i.e., given index is valid). Return false otherwise.
inline bool Bst::rmFromIndex(size_t index)
{
    item_t removedItem;
    bool ok = rmFromIndex(index, removedItem);
    return ok;
}

//! Locate the index of given item using binary search. Return located index if found.
//! Return INVALID_INDEX otherwise.
inline unsigned int Bst::findIndex(const void* item) const
{
    size_t foundIndex;
    bool found = find(item, compare_, foundIndex);
    return found? static_cast<unsigned int>(foundIndex): INVALID_INDEX;
}

//! Locate the index of given item using binary search. Use given compatible comparison
//! function. Return located index if found. Return INVALID_INDEX otherwise.
inline unsigned int Bst::findIndex(const void* item, compare_t compare) const
{
    size_t foundIndex;
    bool found = find(item, compare, foundIndex);
    return found? static_cast<unsigned int>(foundIndex): INVALID_INDEX;
}

//! Return the current number of items in the table.
inline unsigned int Bst::numItems() const
{
    return numItems_;
}

//! Reset the table by removing all items.
inline void Bst::reset()
{
    numItems_ = 0;
}

//! Replace the item at given index. Don't do any error checking.
//! Behavior is unpredictable if given index is invalid or if given
//! index is inappropriate for item.
inline void Bst::setItem(size_t index, item_t item)
{
    item_[index] = item;
}

//! Return the internal raw table. To be used with extra care.
inline void* const* Bst::raw() const
{
    return item_;
}

END_NAMESPACE1

#endif
