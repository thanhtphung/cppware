/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_U16_SET_HPP
#define APPKIT_U16_SET_HPP

#include "appkit/Set.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)


#if _WIN32
#pragma pack(push,4)
#endif

//! set of unsigned 16-bit numbers
class U16Set: public Set
    //!
    //! A class representing a set of unsigned short keys. Implemented
    //! as an array of contiguous ranges. A U16Set instance has type
    //! "U16" if type() is used. A U16Set instance consists of zero
    //! or more valid unsigned short keys. Valid unsigned short keys
    //! have range [validMin()..validMax()]. Strings can be used
    //! to specify keys in a set. A valid string specification is a
    //! sequence of tokens, and each token is either a valid key or a
    //! valid key range ("lo-hi"). The token delimiter is a comma by
    //! default but can be overridden. White-spaces surrounding the
    //! token delimiter are allowed. Example:
    //!\code
    //! U16Set set;
    //! set.add(2, 11);   //keys: 2-11
    //! set.rm(4);        //keys: 2-3,5-11
    //! set.rm("6,8-10"); //keys: 2-3,5,7,11
    //!\endcode
    //!
{

public:
    enum
    {
        DefaultCap = 32
    };

    typedef unsigned short key_t;
    typedef unsigned int keyCount_t;

    static const key_t VALID_MAX;
    static const key_t VALID_MIN;

    // Constructors.
    U16Set(const U16Set& set);
    U16Set(const char* s, char delim, key_t validMin = VALID_MIN, key_t validMax = VALID_MAX, unsigned int capacity = DefaultCap);
    U16Set(const char* s, key_t validMin = VALID_MIN, key_t validMax = VALID_MAX, unsigned int capacity = DefaultCap);
    U16Set(key_t validMin = VALID_MIN, key_t validMax = VALID_MAX, unsigned int capacity = DefaultCap);

    // Operators.
    bool operator !=(const U16Set& set) const;
    bool operator ==(const U16Set& set) const;
    const U16Set& operator &=(const U16Set& set);
    const U16Set& operator =(const U16Set& set);
    const U16Set& operator |=(const U16Set& set);

    // Set queries.
    bool contains(const U16Set& set) const;
    bool contains(const char* s, char delim = ',') const;
    bool contains(key_t key) const;
    bool contains(key_t loKey, key_t hiKey) const;
    bool overlaps(const U16Set& set) const;
    key_t maxKey() const;
    key_t minKey() const;
    key_t validMax() const;
    key_t validMin() const;
    keyCount_t numKeys() const;
    unsigned int numRanges() const;

    // Set queries via range index.
    key_t hiKeyAt(size_t index) const;
    key_t loKeyAt(size_t index) const;
    unsigned int findIndex(key_t key) const;
    void getRange(size_t index, key_t& loKey, key_t& hiKey) const;

    // Set modifications.
    bool add(const U16Set& set);
    bool add(const char* s, char delim = ',');
    bool add(key_t key);
    bool add(key_t loKey, key_t hiKey);
    bool rm(const U16Set& set);
    bool rm(const char* s, char delim = ',');
    bool rm(key_t key);
    bool rm(key_t loKey, key_t hiKey);
    void reset();

    // String utilities.
    bool isValid(const char* s, char delim = ',', bool beStrict = true) const;
    bool isValid(key_t key) const;

    // Override Growable.
    virtual ~U16Set();
    virtual bool resize(unsigned int newCap);

    // Override Set.
    virtual Set* clone() const;
    virtual String toString(const char* delim = ",") const;
    virtual compareResult_e compare(const Set& set) const;

    // Override-able.
    virtual bool pick(key_t& removedKey);

    static int compareP(const void* item0, const void* item1);


    //! set iterator
    class Itor
        //!
        //! A class representing a U16Set iterator. It provides a scheme to
        //! traverse the keys in a U16Set instance. Example:
        //!\code
        //! U16Set::Itor it(set);
        //! U16Set::key_t loKey;
        //! U16Set::key_t hiKey;
        //! while (it.nextRange(loKey, hiKey))
        //! {
        //!   //do something with each range in the set
        //! }
        //!\endcode
        //!
    {

    public:
        typedef unsigned short key_t;
        typedef bool(*keyCb0_t)(void* arg, key_t key);
        typedef bool(*rangeCb0_t)(void* arg, key_t loKey, key_t hiKey);
        typedef void(*keyCb1_t)(void* arg, key_t key);
        typedef void(*rangeCb1_t)(void* arg, key_t loKey, key_t hiKey);

        // Constructors and destructor.
        Itor();
        Itor(const U16Set& set, bool makeCopy = false);
        ~Itor();

        // Iterator support.
        bool applyHiToLo(keyCb0_t cb, void* arg = 0) const;
        bool applyHiToLo(rangeCb0_t cb, void* arg = 0) const;
        bool applyLoToHi(keyCb0_t cb, void* arg = 0) const;
        bool applyLoToHi(rangeCb0_t cb, void* arg = 0) const;
        bool curKey(key_t& key) const;
        bool curRange(key_t& loKey, key_t& hiKey) const;
        bool nextKey(key_t& key);
        bool nextRange(key_t& loKey, key_t& hiKey);
        bool prevKey(key_t& key);
        bool prevRange(key_t& loKey, key_t& hiKey);
        void applyHiToLo(keyCb1_t cb, void* arg = 0) const;
        void applyHiToLo(rangeCb1_t cb, void* arg = 0) const;
        void applyLoToHi(keyCb1_t cb, void* arg = 0) const;
        void applyLoToHi(rangeCb1_t cb, void* arg = 0) const;
        void reset();

        // Utilities.
        const U16Set* set() const;
        void attach(const U16Set& set, bool makeCopy = false);
        void detach();

    private:
        const U16Set* set_;
        size_t curIndex_;
        key_t curKey_;
        bool copyMade_;

        Itor(const Itor&); //prohibit usage
        const Itor& operator =(const Itor&); //prohibit usage

    };

private:
    typedef struct
    {
        key_t loKey;
        key_t hiKey;
    } range_t;

    range_t* rangeVec_;

    keyCount_t numKeys_;
    range_t validRange_;
    unsigned int numRanges_;

    bool addAt(size_t, key_t, key_t);
    bool addNear(size_t, key_t, key_t);
    bool contains(key_t, key_t, unsigned int&) const;
    bool rmAt(size_t, key_t, key_t);
    bool rmNear(size_t, key_t, key_t);
    bool toKey(const char*, key_t&) const;
    bool toRange(const char*, key_t&, key_t&, bool) const;
    void construct(key_t, key_t);
    void rmOverlaps(size_t);

};

#if _WIN32
#pragma pack(pop)
#endif

//! Return true if this set does not equal given set.
inline bool U16Set::operator !=(const U16Set& set) const
{
    return !(operator ==(set));
}

//! Return the high key at given index. Don't do any error checking.
//! Behavior is unpredictable if given index is invalid.
inline U16Set::key_t U16Set::hiKeyAt(size_t index) const
{
    return rangeVec_[index].hiKey;
}

//! Return the low key at given index. Don't do any error checking.
//! Behavior is unpredictable if given index is invalid.
inline U16Set::key_t U16Set::loKeyAt(size_t index) const
{
    return rangeVec_[index].loKey;
}

//! Return the current maximum key in this set.
//! Return validMin() if this set is empty.
inline U16Set::key_t U16Set::maxKey() const
{
    return numRanges_? rangeVec_[numRanges_ - 1].hiKey: validRange_.loKey;
}

//! Return the current minimum key in this set.
//! Return validMax() if this set is empty.
inline U16Set::key_t U16Set::minKey() const
{
    return numRanges_? rangeVec_[0].loKey: validRange_.hiKey;
}

//! Return the valid maximal key for this set.
inline U16Set::key_t U16Set::validMax() const
{
    return validRange_.hiKey;
}

//! Return the valid minimal key for this set.
inline U16Set::key_t U16Set::validMin() const
{
    return validRange_.loKey;
}

//! Return the current number of keys in this set.
inline U16Set::keyCount_t U16Set::numKeys() const
{
    return numKeys_;
}

//! Add key key to this set. Return true if successful.
inline bool U16Set::add(key_t key)
{
    return add(key, key);
}

//! Return true if key is a member of this set.
inline bool U16Set::contains(key_t key) const
{
    unsigned int foundIndex;
    return contains(key, key, foundIndex);
}

//! Return true if this instance contains all numbers in the
//! given range. Invalid range is ignored. A range is valid
//! if the low key is less than or equal to the high key and
//! if both of the limiting keys are valid for this set.
inline bool U16Set::contains(key_t loKey, key_t hiKey) const
{
    unsigned int foundIndex;
    return contains(loKey, hiKey, foundIndex);
}

//! Return true if key is a valid key for this set.
inline bool U16Set::isValid(key_t key) const
{
    return (key <= validRange_.hiKey) && (key >= validRange_.loKey);
}

//! Remove key key from this set. Return true if successful.
inline bool U16Set::rm(key_t key)
{
    return rm(key, key);
}

//! Locate given entry and return its range index. Return INVALID_INDEX if not found.
inline unsigned int U16Set::findIndex(key_t key) const
{
    unsigned int foundIndex;
    bool found = contains(key, key, foundIndex);
    return found? foundIndex: INVALID_INDEX;
}

//! Return the current number of ranges in this set.
inline unsigned int U16Set::numRanges() const
{
    return numRanges_;
}

//! Return the range at given index. Don't do any error checking.
//! Behavior is unpredictable if given index is invalid.
inline void U16Set::getRange(size_t index, key_t& loKey, key_t& hiKey) const
{
    const range_t& r = rangeVec_[index];
    loKey = r.loKey;
    hiKey = r.hiKey;
}

//! Reset the set by removing all keys.
inline void U16Set::reset()
{
    numKeys_ = 0;
    numRanges_ = 0;
}

//! Retrieve the current key. Return true if there's one. Return false otherwise
//! iterating has not started or if set is empty). Behavior is unpredictable if
//! the iterator is unattached.
inline bool U16Set::Itor::curKey(key_t& key) const
{
    return (curIndex_ == INVALID_INDEX)?
        (false):
        (key = curKey_, true);
}

//! Return the attached set. Return zero if unattached.
inline const U16Set* U16Set::Itor::set() const
{
    return set_;
}

//! Reset the iterator to its initial state. That is, nextKey()
//! will return the first key, nextRange() will return the first
//! range, prevKey() will return the last key, and prevRange()
//! will return the last range. Behavior is unpredictable if the
//! iterator is unattached.
inline void U16Set::Itor::reset()
{
    curIndex_ = INVALID_INDEX;
}

END_NAMESPACE1

#endif
