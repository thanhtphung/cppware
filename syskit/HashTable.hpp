/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_HASH_TABLE_HPP
#define SYSKIT_HASH_TABLE_HPP

#include <new>
#include "syskit/Growable.hpp"
#include "syskit/macros.h"

class HashTableSuite;

BEGIN_NAMESPACE1(syskit)


//! hash table of opaque items
class HashTable: public Growable
    //!
    //! A class representing a hash table of opaque items. Chaining is used
    //! to resolve hash collisions. Items should be unique. If they are not,
    //! a successful search would return any matching item. Items are added
    //! using the add() method and removed using the rm() method. Searches are
    //! provided by the find() methods. A hash table has zero or more buckets,
    //! and each bucket holds zero or more items. The utilized hash function
    //! dictates which bucket an item must reside in. Searching of items
    //! residing in the same bucket requires a linear search within the bucket.
    //! Best performance is achieved if the hash function is efficient and if
    //! the hash function distributes the items evenly in the available buckets
    //! and if the average itemsPerBucket ratio is reasonably small. It's
    //! assumed a simple modular hash function is used. The number of buckets
    //! remains a prime number when buckets are added to avoid an undesirable
    //! itemsPerBucket ratio.
    //!
{

public:
    enum
    {
        DefaultCap = 131
    };

    typedef void* item_t;
    typedef bool(*cb0_t)(void* arg, item_t item);

    //! Return non-zero if items differ, and zero otherwise.
    typedef int(*diff_t)(const void* item0, const void* item1);

    typedef unsigned int(*hash_t)(const void* item, size_t numBuckets);
    typedef void(*cb1_t)(void* arg, item_t item);

    // Constructors.
    HashTable(diff_t diff, hash_t hash, unsigned int capacity = DefaultCap, double bucketCap = 1.0);
    HashTable(const HashTable& table);

    // Operators.
    const HashTable& operator =(const HashTable& table);

    // Hash table management.
    bool add(item_t item);
    bool add(item_t item, item_t& replacedItem);
    bool addIfNotFound(item_t item);
    bool addIfNotFound(item_t item, item_t& foundItem);
    bool find(const void* item) const;
    bool find(const void* item, diff_t diff) const;
    bool find(const void* item, diff_t diff, item_t& foundItem) const;
    bool find(const void* item, item_t& foundItem) const;
    bool rm(const void* item);
    bool rm(const void* item, diff_t diff);
    bool rm(const void* item, diff_t, item_t& removedItem);
    bool rm(const void* item, item_t& removedItem);
    void reset();

    // Getters.
    diff_t cmpFunc() const;
    double bucketCap() const;
    hash_t hashFunc() const;
    unsigned int numBuckets() const;
    unsigned int numEmptyBuckets() const;
    unsigned int numInUseBuckets() const;
    unsigned int numItems() const;
    unsigned int peakBucketSize() const;
    unsigned int usagePeak() const;

    // Iterator support.
    bool apply(cb0_t cb, void* arg = 0) const;
    void apply(cb1_t cb, void* arg = 0) const;

    // Override Growable.
    virtual ~HashTable();
    virtual bool resize(unsigned int newCap);
    virtual bool setGrowth(int growBy);

protected:
    virtual bool grow();

private:
    typedef struct node_s
    {
        struct node_s* next;
        item_t item;
        static void operator delete(void* p, size_t size);
        static void* operator new(size_t size);
    } node_t;

    diff_t diff_;
    double bucketCap_;
    hash_t hash_;
    node_t** bucket_;
    unsigned int* bucketSize_;
    unsigned int maxItems_;
    unsigned int numEmptyBuckets_;
    unsigned int numItems_;
    unsigned int peakBucketSize_;
    unsigned int usagePeak_;

    bool addHere(size_t, item_t);
    void copy(const HashTable*);
    void copy(const node_t* const*, size_t);
    void rehash(node_t* const*, unsigned int);

    friend class ::HashTableSuite;

};

END_NAMESPACE1

#include "syskit/BufPool.hpp"

BEGIN_NAMESPACE1(syskit)

//! Return the comparison function used when items are compared.
inline HashTable::diff_t HashTable::cmpFunc() const
{
    return diff_;
}

//! Return the utilized hash function. The hash function dictates
//! which bucket an item must reside in.
inline HashTable::hash_t HashTable::hashFunc() const
{
    return hash_;
}

//! Add given item to the table only if the same item doesn't exist.
//! The table's hash function dictates where (in which bucket) this
//! new item will reside. Behavior is unpredictable if the hash
//! function does not behave. Return true if successful. Return
//! false otherwise (table is full or item already exists).
inline bool HashTable::addIfNotFound(item_t item)
{
    item_t foundItem;
    bool ok = addIfNotFound(item, foundItem);
    return ok;
}

//! Locate given item. Return true if found. Return false otherwise.
inline bool HashTable::find(const void* item) const
{
    item_t foundItem;
    bool found = find(item, diff_, foundItem);
    return found;
}

//! Locate given item. Return true if found. Return false otherwise.
//! Use given compatible comparison function for this search.
inline bool HashTable::find(const void* item, diff_t diff) const
{
    item_t foundItem;
    bool found = find(item, diff, foundItem);
    return found;
}

//! Locate given item. Behavior is unpredictable if the hash function
//! does not behave. Return true if found (also return the found item
//! in foundItem). Return false otherwise.
inline bool HashTable::find(const void* item, item_t& foundItem) const
{
    bool found = find(item, diff_, foundItem);
    return found;
}

//! Remove given item. If found, remove it from the table and return
//! true. Return false otherwise.
inline bool HashTable::rm(const void* item)
{
    item_t removedItem;
    bool found = rm(item, diff_, removedItem);
    return found;
}

//! Remove given item. Use given compatible comparison function. If found,
//! remove it from the table and return true. Return false otherwise.
inline bool HashTable::rm(const void* item, diff_t diff)
{
    item_t removedItem;
    bool found = rm(item, diff, removedItem);
    return found;
}

//! Locate given item. Behavior is unpredictable if the hash function does not
//! behave. If found, remove it from the table and return true (also return the
//! removed item in removedItem). Return false otherwise.
inline bool HashTable::rm(const void* item, item_t& removedItem)
{
    bool found = rm(item, diff_, removedItem);
    return found;
}

//! Return the bucket capacity. On average, a bucket can hold at most
//! bucketCap() items. When this threshold is reached, more buckets are
//! added to accomodate the growth.
inline double HashTable::bucketCap() const
{
    return bucketCap_;
}

//! Return the number of buckets in the hash table.
inline unsigned int HashTable::numBuckets() const
{
    return capacity();
}

//! Return the number of empty buckets in the hash table.
inline unsigned int HashTable::numEmptyBuckets() const
{
    return numEmptyBuckets_;
}

//! Return the number of in-use buckets in the hash table.
inline unsigned int HashTable::numInUseBuckets() const
{
    return (capacity() - numEmptyBuckets_);
}

//! Return the current number of items in the hash table.
inline unsigned int HashTable::numItems() const
{
    return numItems_;
}

inline unsigned int HashTable::peakBucketSize() const
{
    return peakBucketSize_;
}

//! Return the usage peak.
//! This is high of the number of items in the hash table.
inline unsigned int HashTable::usagePeak() const
{
    return usagePeak_;
}

inline void HashTable::node_t::operator delete(void* p, size_t size)
{
    BufPool::freeBuf(p, size);
}

inline void* HashTable::node_t::operator new(size_t size)
{
    void* buf = BufPool::allocateBuf(size);
    return buf;
}

END_NAMESPACE1

#endif
