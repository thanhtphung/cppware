/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/sys.hpp"

#include "appkit-pch.h"
#include "appkit/DelimitedTxt.hpp"
#include "appkit/Str.hpp"
#include "appkit/U16Set.hpp"
#include "appkit/U32.hpp"

using namespace syskit;

const char TYPE[] = "U16";

BEGIN_NAMESPACE1(appkit)

const U16Set::key_t U16Set::VALID_MAX = 0xffffU;
const U16Set::key_t U16Set::VALID_MIN = 0;


//!
//! Construct a duplicate instance of the given set.
//!
U16Set::U16Set(const U16Set& set):
Set(set)
{
    numKeys_ = set.numKeys_;
    numRanges_ = set.numRanges_;
    validRange_ = set.validRange_;

    rangeVec_ = new range_t[capacity()];
    memcpy(rangeVec_, set.rangeVec_, numRanges_ * sizeof(*rangeVec_));
}


//!
//! Construct a set with keys specified in string s. Keys in this
//! set must be in the given range. This set has an initial capacity of
//! capacity ranges. The set grows by doubling. If given capacity
//! is zero, then an initial capacity of one will be used instead. Use
//! delim to override the default comma delimiter. If given
//! delimiter is zero or a hyphen, the default comma delimiter will be
//! used.
//!
U16Set::U16Set(const char* s, char delim, key_t validMin, key_t validMax, unsigned int capacity):
Set(TYPE, capacity)
{
    construct(validMin, validMax);
    add(s, delim);
}


//!
//! Construct a set with keys specified in string s. Keys in this
//! set must be in the given range. This set has an initial capacity of
//! capacity ranges. The set grows by doubling. If given capacity
//! is zero, then an initial capacity of one will be used instead.
//!
U16Set::U16Set(const char* s, key_t validMin, key_t validMax, unsigned int capacity):
Set(TYPE, capacity)
{
    construct(validMin, validMax);
    add(s, ',');
}


//!
//! Construct an empty set. Keys in this set must be in the given range.
//! This set has an initial capacity of capacity ranges. The set
//! grows by doubling. If given capacity is zero, then an initial capacity
//! of one will be used instead.
//!
U16Set::U16Set(key_t validMin, key_t validMax, unsigned int capacity):
Set(TYPE, capacity)
{
    construct(validMin, validMax);
}


U16Set::~U16Set()
{
    delete[] rangeVec_;
}


//!
//! Return true if this set equals given set.
//!
bool U16Set::operator ==(const U16Set& set) const
{
    return (numKeys_ == set.numKeys_) &&
        (numRanges_ == set.numRanges_) &&
        (memcmp(rangeVec_, set.rangeVec_, numRanges_ * sizeof(*rangeVec_)) == 0);
}


//!
//! Compute the intersection of this set and given set and
//! save the result in this set. Return reference to self.
//!
const U16Set& U16Set::operator &=(const U16Set& set)
{

    // If intersecting self, make it a no-op.
    if (this != &set)
    {
        U16Set tmp(*this);
        tmp.rm(set);
        rm(tmp);
    }

    // Return reference to self.
    return *this;
}


//!
//! Assignment operator.
//!
const U16Set& U16Set::operator =(const U16Set& set)
{

    // Prevent self assignment.
    if (this != &set)
    {

        // Grow to accomodate source.
        if (set.numRanges_ > capacity())
        {
            delete[] rangeVec_;
            rangeVec_ = new range_t[setNextCap(set.numRanges_)];
        }

        // Copy from given set.
        numKeys_ = set.numKeys_;
        numRanges_ = set.numRanges_;
        validRange_ = set.validRange_;
        memcpy(rangeVec_, set.rangeVec_, numRanges_ * sizeof(*rangeVec_));
    }

    // Return reference to self.
    return *this;
}


//!
//! Compute the union of this set and given set and save
//! the result in this set. Return reference to self.
//!
const U16Set& U16Set::operator |=(const U16Set& set)
{

    // If unionizing self, make it a no-op.
    if (this != &set)
    {
        add(set);
    }

    return *this;
}


//!
//! Clone set. Return cloned set. The cloned set is allocated using
//! the heap and is to be freed by the user using the delete operator.
//!
Set* U16Set::clone() const
{
    return new U16Set(*this);
}


//!
//! Return ASCII form of this set using given range delimiter. ASCII form
//! of an empty set is an empty string. ASCII form of a non-empty set has
//! the following format: "$lo(-$hi)?[$rangeDelimiter$lo(-$hi)?]*". Sample
//! ASCII form with comma as range delimiter:
//!\code
//! 2-3,5,7,11,13,15-21
//!\endcode
//! Sample ASCII form with newline as range delimiter:
//!\code
//! 2-3
//! 5
//! 7
//! 11
//! 13
//! 15-21
//!\endcode
//!
String U16Set::toString(const char* delim) const
{
    char buf[1 + U32::MaxDigits];
    buf[0] = '-';

    String rangeDelim(delim);
    String s;
    const range_t* p = rangeVec_;
    for (const range_t* pEnd = p + numRanges_; p < pEnd; s += rangeDelim, ++p)
    {
        size_t n = U32::toDigits(p->loKey, buf + 1);
        s.append(buf + 1, n);
        if (p->loKey != p->hiKey)
        {
            n = U32::toDigits(p->hiKey, buf + 1);
            s.append(buf, n + 1);
        }
    }

    s.truncate(s.length() - rangeDelim.length());
    return s;
}


//!
//! Compare with given set. Return result.
//!
U16Set::compareResult_e U16Set::compare(const Set& set) const
{

    // Incompatible.
    if (set.type() != TYPE)
    {
        return Unknown;
    }

    // This set is not empty.
    compareResult_e rc;
    const U16Set* that = (const U16Set*)(&set);
    if (numRanges_ > 0)
    {
        if (that->numRanges_ > 0)
        {
            if (operator ==(*that))
            {
                rc = Equal;
            }
            else if (contains(*that))
            {
                rc = Superset;
            }
            else if (that->contains(*this))
            {
                rc = Subset;
            }
            else if (overlaps(*that))
            {
                rc = Overlap;
            }
            else
            {
                rc = Disjoint;
            }
        }
        else
        {
            rc = EmptySuperset;
        }
    }

    // This set is empty.
    else
    {
        rc = (that->numRanges_ > 0)? EmptySubset: Empty;
    }

    // Return result.
    return rc;
}


//!
//! Add keys in set to this set. Return true
//! if at least one new key was added successfully.
//!
bool U16Set::add(const U16Set& set)
{
    keyCount_t oldNumKeys = numKeys_;
    for (size_t i = 0; i < set.numRanges_; ++i)
    {
        const range_t& r = set.rangeVec_[i];
        add(r.loKey, r.hiKey);
    }

    // Return true if at least one new key was added successfully.
    bool modified = numKeys_ > oldNumKeys;
    return modified;
}


//!
//! Add keys specified by string s. Return true if at least one new
//! key was added successfully. Use delimiter to override the default
//! comma delimiter. If given delimiter is zero or a hyphen, the default
//! comma delimiter will be used.
//!
bool U16Set::add(const char* s, char delim)
{

    // Remember number of keys before adding.
    keyCount_t oldNumKeys = numKeys_;

    // Look at input string as delimited text.
    validateDelim(delim);
    bool makeCopy = false;
    DelimitedTxt txt(s, strlen(s), makeCopy, delim);

    // Add one line at a time.
    String line;
    bool trimLine = true;
    while (txt.next(line, trimLine))
    {
        bool beStrict = false;
        bool trimL = true;
        bool trimR = true;
        line.trimSpace(trimL, trimR);
        key_t loKey;
        key_t hiKey;
        if (toKey(line.ascii(), loKey))
        {
            add(loKey, loKey);
        }
        else if (toRange(line.ascii(), loKey, hiKey, beStrict))
        {
            add(loKey, hiKey);
        }
    }

    // Return true if at least one new key was added successfully.
    bool modified = numKeys_ > oldNumKeys;
    return modified;
}


//!
//! Add given range "loKey-hiKey" to this set.
//! Invalid range is ignored. A range is valid if the low key
//! is less than or equal to the high key and if one of the
//! limiting keys is a valid key for this set. For example,
//! if this set allows two-digit decimal numbers (10-99) only,
//! adding 5-15 is allowed and is equivalent to adding 10-15,
//! and adding 0-1000 is allowed and is equivalent to adding
//! 10-99, but adding 0-9 is ignored. Return true if at least
//! one new key was added successfully.
//!
bool U16Set::add(key_t loKey, key_t hiKey)
{

    // Ignore invalid range.
    if ((loKey > hiKey) || (hiKey < validRange_.loKey) || (loKey > validRange_.hiKey))
    {
        return false;
    }

    // Restrict operation range.
    if (loKey < validRange_.loKey)
    {
        loKey = validRange_.loKey;
    }
    else if (loKey > validRange_.hiKey)
    {
        loKey = validRange_.hiKey;
    }
    else if (hiKey < validRange_.loKey)
    {
        hiKey = validRange_.loKey;
    }
    else if (hiKey > validRange_.hiKey)
    {
        hiKey = validRange_.hiKey;
    }

    // Instance is empty. Add new entry at index 0.
    keyCount_t oldNumKeys = numKeys_;
    if (numRanges_ == 0)
    {
        size_t i = 0;
        addAt(i, loKey, hiKey);
    }

    // Locate candidate insert position. If possible, modify existing
    // entry. Otherwise, add a new entry near the candidate position.
    else
    {
        bool found = false;
        int loI = 0;
        int hiI = numRanges_ - 1;
        unsigned int midI;
        do
        {
            midI = (loI + hiI) >> 1;
            const range_t& r = rangeVec_[midI];
            if (r.loKey > hiKey)
            {
                hiI = midI - 1;
            }
            else if (r.hiKey < loKey)
            {
                loI = midI + 1;
            }
            else
            {
                if ((loKey >= r.loKey) && (hiKey <= r.hiKey))
                {
                    found = true;
                }
                break;
            }
        } while (loI <= hiI);

        if (!found)
        {
            addNear(midI, loKey, hiKey);
        }
    }

    // Return true if at least one new key was added successfully.
    bool modified = numKeys_ > oldNumKeys;
    return modified;
}


//
// Add new entry at index i (shift any existing entries down).
// Use loKey and hiKey for new entry. Return true if
// successful.
//
bool U16Set::addAt(size_t i, key_t loKey, key_t hiKey)
{

    // Expand vector if necessary.
    // Shift bottom entries if necessary.
    bool ok;
    if ((numRanges_ < capacity()) || grow())
    {
        range_t* p = rangeVec_ + i;
        if (i < numRanges_)
        {
            memmove(p + 1, p, (numRanges_ - i)*sizeof(*rangeVec_));
        }
        p->loKey = loKey;
        p->hiKey = hiKey;
        ++numRanges_;
        numKeys_ += hiKey - loKey + 1U;
        ok = true;
    }

    // Could not expand vector.
    else
    {
        ok = false;
    }

    // Return status.
    return ok;
}


//
// Add given range loKey-hiKey near the ith entry.
// Return true if successful.
//
bool U16Set::addNear(size_t i, key_t loKey, key_t hiKey)
{

    // New range is to the left of the ith entry.
    bool ok = true;
    range_t& r = rangeVec_[i];
    if (r.loKey > hiKey)
    {
        if (r.loKey == (hiKey + 1))
        {
            r.loKey = loKey;
            numKeys_ += hiKey - loKey + 1U;
        }
        else
        {
            ok = addAt(i, loKey, hiKey);
        }
    }

    // New range is to the right of the ith entry.
    else if (r.hiKey < loKey)
    {
        if (r.hiKey == (loKey - 1))
        {
            r.hiKey = hiKey;
            numKeys_ += hiKey - loKey + 1U;
        }
        else
        {
            ok = addAt(++i, loKey, hiKey);
        }
    }

    // New range overlaps.
    else
    {
        keyCount_t oldNumKeys = r.hiKey - r.loKey + 1U;
        if (r.loKey > loKey)
        {
            r.loKey = loKey;
        }
        if (r.hiKey < hiKey)
        {
            r.hiKey = hiKey;
        }
        numKeys_ += (r.hiKey - r.loKey + 1UL) - oldNumKeys;
    }

    // Remove overlaps and return status.
    rmOverlaps(i);
    return ok;
}


//!
//! Return true if this set is identical to or is a superset of given set.
//!
bool U16Set::contains(const U16Set& set) const
{
    bool answer = true;
    if (*this == set)
    {
        return answer;
    }

    for (size_t i = 0, numRanges = set.numRanges_; i < numRanges; ++i)
    {
        const range_t& r = set.rangeVec_[i];
        if (!contains(r.loKey, r.hiKey))
        {
            answer = false;
            break;
        }
    }

    return answer;
}


//!
//! Return true if this set is identical to or is a superset of given set.
//! Use delimiter to override the default comma delimiter. If given
//! delimiter is zero or a hyphen, the default comma delimiter will be used.
//!
bool U16Set::contains(const char* s, char delim) const
{

    // Assume yes.
    bool answer = true;

    // Look at input string as delimited text.
    validateDelim(delim);
    bool makeCopy = false;
    DelimitedTxt txt(s, strlen(s), makeCopy, delim);

    // Check one line at a time.
    String line;
    bool trimLine = true;
    while (txt.next(line, trimLine))
    {
        bool beStrict = true;
        bool trimL = true;
        bool trimR = true;
        line.trimSpace(trimL, trimR);
        key_t loKey;
        key_t hiKey;
        if (toKey(line.ascii(), loKey))
        {
            if (!contains(loKey, loKey))
            {
                answer = false;
                break;
            }
        }
        else if (toRange(line.ascii(), loKey, hiKey, beStrict))
        {
            if (!contains(loKey, hiKey))
            {
                answer = false;
                break;
            }
        }
        else
        {
            answer = false;
            break;
        }
    }

    // Return true if this set is identical to or is a superset of given set.
    return answer;
}


//
// Return true if this instance contains all numbers in the
// given range. Invalid range is ignored. A range is valid
// if the low key is less than or equal to the high key and
// if both of the limiting keys are valid for this set.
//
bool U16Set::contains(key_t loKey, key_t hiKey, unsigned int& foundIndex) const
{

    // Search.
    bool found = false;
    int loI = 0;
    int hiI = numRanges_ - 1;
    if ((loKey >= validRange_.loKey) && (hiKey <= validRange_.hiKey) && (loKey <= hiKey))
    {
        while (loI <= hiI)
        {
            unsigned int midI = (loI + hiI) >> 1;
            const range_t& r = rangeVec_[midI];
            if (r.loKey > hiKey)
            {
                hiI = midI - 1;
            }
            else if (r.hiKey < loKey)
            {
                loI = midI + 1;
            }
            else
            {
                if ((loKey >= r.loKey) && (hiKey <= r.hiKey))
                {
                    foundIndex = midI;
                    found = true;
                }
                break;
            }
        }
    }

    // Return search status.
    return found;
}


//!
//! See if string s is a valid unsigned short set specification.
//! A valid string specification is a sequence of tokens, and each token
//! is either a valid key or a valid key range ("lo-hi"). The token
//! delimiter is a comma by default but can be overridden. White-spaces
//! surrounding the token delimiter are allowed. A key range is valid
//! if the low key is less than or equal to the high key. In strict mode,
//! both of the limiting keys must be valid for this set. In non-strict
//! mode, only one of the limiting keys must be valid for this set. Use
//! delimiter to override the default comma delimiter. If given
//! delimiter is zero or a hyphen, the default comma delimiter will be
//! used.
//!
bool U16Set::isValid(const char* s, char delim, bool beStrict) const
{

    // Assume valid.
    bool valid = true;

    // Look at input string as delimited text.
    validateDelim(delim);
    bool makeCopy = false;
    DelimitedTxt txt(s, strlen(s), makeCopy, delim);

    // Validate one line at a time.
    String line;
    bool trimLine = true;
    while (txt.next(line, trimLine))
    {
        bool trimL = true;
        bool trimR = true;
        line.trimSpace(trimL, trimR);
        key_t hiKey;
        key_t loKey;
        if ((!toKey(line.ascii(), loKey)) && (!toRange(line.ascii(), loKey, hiKey, beStrict)))
        {
            valid = false;
            break;
        }
    }

    // Return status.
    return valid;
}


//!
//! Return true if this set and given set overlap. That
//! is, there's at least one key residing in both sets.
//!
bool U16Set::overlaps(const U16Set& set) const
{

    // Minimize the number of iterations in the following loop when the
    // sets might overlap. "a overlaps b" is equivalent to "b overlaps a",
    // but "a overlaps b" is, in general, more efficient if a has fewer
    // ranges.
    if (numRanges_ > set.numRanges_)
    {
        return set.overlaps(*this);
    }

    // The individual ranges might overlap if both sets are non-empty and
    // if the combined ranges do overlap.
    bool answer = false;
    if ((numRanges_ > 0) &&
        (set.numRanges_ > 0) &&
        (rangeVec_[0].loKey <= set.rangeVec_[set.numRanges_ - 1].hiKey) &&
        (rangeVec_[numRanges_ - 1].hiKey >= set.rangeVec_[0].loKey))
    {
        for (size_t i = 0; i < numRanges_; ++i)
        {
            const range_t& r = rangeVec_[i];
            if (set.contains(r.loKey, r.loKey) || set.contains(r.hiKey, r.hiKey))
            {
                answer = true;
                break;
            }
        }
    }

    // Return true if this set and given set overlap.
    return answer;
}


//!
//! Pick one key out of this set. Return true if successful.
//!
bool U16Set::pick(key_t& removedKey)
{
    bool ok;
    if (numRanges_ == 0)
    {
        ok = false;
    }

    else
    {
        unsigned long long rand64 = rdtsc();
        size_t i = static_cast<size_t>(rand64 % numRanges_);
        const range_t& r = rangeVec_[i];
        key_t key = r.loKey + static_cast<key_t>((rand64 >> 3) % (r.hiKey - r.loKey + 1UL));
        ok = rmAt(i, key, key);
        if (ok)
        {
            removedKey = key;
        }
    }

    return ok;
}


//!
//! Resize set. Given new capacity must be non-zero and must not be
//! less than the current number of ranges. Return true if successful.
//!
bool U16Set::resize(unsigned int newCap)
{
    bool ok;
    if ((newCap == 0) || (newCap < numRanges_))
    {
        ok = false;
    }

    else
    {
        ok = true;
        if (newCap != capacity())
        {
            range_t* rangeVec = new range_t[newCap];
            memcpy(rangeVec, rangeVec_, numRanges_ * sizeof(*rangeVec_));
            delete[] rangeVec_;
            rangeVec_ = rangeVec;
            setCapacity(newCap);
        }
    }

    return ok;
}


//!
//! Remove keys in given set from this set. Return true if at
//! least one key was removed successfully. The given set can
//! include non-existent keys. For example, if this set has
//! only odd numbers from 1 to 99, removing keys between 0
//! and 50 is allowed and will result in removal of odd numbers
//! from 1 to 49.
//!
bool U16Set::rm(const U16Set& set)
{
    keyCount_t oldNumKeys = numKeys_;
    for (unsigned int i = set.numRanges_; i > 0;)
    {
        const range_t& r = set.rangeVec_[--i];
        rm(r.loKey, r.hiKey);
    }

    bool modified = numKeys_ < oldNumKeys;
    return modified;
}


//!
//! Remove keys specified by string s from this set.
//! Return true if at least one key was removed successfully.
//! The given keys can include non-existent keys. For example, if this
//! set has only odd numbers from 1 to 99, removing keys between 0 and
//! 50 is allowed and will result in removal of odd numbers from 1 to
//! 49. Use delim to override the default comma delimiter.
//! If given delimiter is zero or a hyphen, the default comma delimiter
//! will be used.
//!
bool U16Set::rm(const char* s, char delim)
{

    // Remember number of keys before removing.
    keyCount_t oldNumKeys = numKeys_;

    // Look at input string as delimited text.
    validateDelim(delim);
    bool makeCopy = false;
    DelimitedTxt txt(s, strlen(s), makeCopy, delim);

    // Remove one line at a time.
    String line;
    bool trimLine = true;
    while (txt.next(line, trimLine))
    {
        bool beStrict = false;
        bool trimL = true;
        bool trimR = true;
        line.trimSpace(trimL, trimR);
        key_t loKey;
        key_t hiKey;
        if (toKey(line.ascii(), loKey))
        {
            rm(loKey, loKey);
        }
        else if (toRange(line.ascii(), loKey, hiKey, beStrict))
        {
            rm(loKey, hiKey);
        }
    }

    // Return true if at least one key was removed successfully.
    return numKeys_ < oldNumKeys;
}


//!
//! Remove keys in given range from this set. Return true if at
//! least one key was removed successfully. The given range can
//! include non-existent keys. For example, if this set has only
//! odd numbers from 1 to 99, removing keys between 0 and 50 is
//! allowed and will result in removal of odd numbers from 1 to
//! 49.
//!
bool U16Set::rm(key_t loKey, key_t hiKey)
{
    keyCount_t oldNumKeys = numKeys_;

    int loI = 0;
    int hiI = numRanges_ - 1;
    if (loKey <= hiKey)
    {
        while (loI <= hiI)
        {
            unsigned int midI = (loI + hiI) >> 1;
            const range_t& r = rangeVec_[midI];
            if (r.loKey > hiKey)
            {
                hiI = midI - 1;
            }
            else if (r.hiKey < loKey)
            {
                loI = midI + 1;
            }
            else
            {
                rmNear(midI, loKey, hiKey);
                break;
            }
        }
    }

    bool modified = numKeys_ < oldNumKeys;
    return modified;
}


//
// Remove given range from the ith entry. Return true if successful.
//
bool U16Set::rmAt(size_t i, key_t loKey, key_t hiKey)
{

    // Assume ok.
    bool ok = true;
    range_t& r = rangeVec_[i];
    keyCount_t numKeys = r.hiKey - r.loKey + 1U;

    // Remove whole entry or just a left portion.
    if (loKey <= r.loKey)
    {
        if (hiKey >= r.hiKey)
        {
            --numRanges_;
            if (i != numRanges_)
            {
                memmove(rangeVec_ + i, rangeVec_ + i + 1, (numRanges_ - i)*sizeof(*rangeVec_));
            }
            numKeys_ -= numKeys;
        }
        else
        {
            r.loKey = hiKey + 1;
            numKeys_ += (r.hiKey - r.loKey + 1UL) - numKeys;
        }
    }

    // Remove a right portion or a middle portion. If only
    // a middle portion is removed, split this entry into two.
    else
    {
        if (hiKey < r.hiKey)
        {
            ok = addAt(i + 1, hiKey + 1, r.hiKey);
            range_t& r1 = rangeVec_[i]; //r might have been invalidated by addAt()
            r1.hiKey = loKey - 1;
            numKeys_ += (r1.hiKey - r1.loKey + 1UL) - numKeys;
        }
        else
        {
            r.hiKey = loKey - 1;
            numKeys_ += (r.hiKey - r.loKey + 1UL) - numKeys;
        }
    }

    // Return status.
    return ok;
}


//
// Remove given range loKey-hiKey near the ith entry.
// Return true if successful.
//
bool U16Set::rmNear(size_t i, key_t loKey, key_t hiKey)
{

    // Locate affected entries to the left. Assume not too many
    // entries are affected, so linear search seems acceptable
    // here.
    int i1;
    for (i1 = static_cast<int>(i); i1 > 0; --i1)
    {
        if (rangeVec_[i1 - 1].hiKey < loKey)
        {
            break;
        }
    }

    // Locate affected entries to the right. Assume not too many
    // entries are affected, so linear search seems acceptable
    // here.
    int maxI2 = numRanges_ - 1;
    int i2;
    for (i2 = static_cast<int>(i); i2 < maxI2; ++i2)
    {
        if (rangeVec_[i2 + 1].loKey > hiKey)
        {
            break;
        }
    }

    // Remove affected entries (entries i1 through i2 are
    // affected). Entries must be removed from bottom up
    // since rmAt() might perform the removal by delete an
    // existing entry or insert a new entry.
    bool ok = false;
    for (int j = i2; j >= i1; --j)
    {
        if (rmAt(j, loKey, hiKey))
        {
            ok = true;
        }
    }

    // Remove overlaps and return status.
    rmOverlaps(i);
    return ok;
}


//!
//! Convert string s to key. Return true if s specifies a valid key for this set.
//!
bool U16Set::toKey(const char* s, key_t& key) const
{
    char* p;
    unsigned int k = strtoul(s, &p, 0);
    bool ok = (S32::isDigit(*s) && (p != s) && (*p == 0) && (k <= validRange_.hiKey) && (k >= validRange_.loKey));
    if (ok)
    {
        key = static_cast<key_t>(k);
    }

    return ok;
}


//!
//! Convert string s to loKey and hiKey. Return true if s specifies a valid key
//! range. A key range is valid if the low key is less than or equal to the
//! high key. In strict mode, both of the limiting keys must be valid for this
//! set. In non-strict mode, only one of the limiting keys must be valid for
//! this set.
//!
bool U16Set::toRange(const char* s, key_t& loKey, key_t& hiKey, bool beStrict) const
{

    // Assume failure.
    bool ok = false;

    // Assume string s has format "lo-hi" (e.g., "56-678").
    char* p;
    unsigned int lo = strtoul(s, &p, 0);
    if (S32::isDigit(*s) && (p != s) && (*p == '-'))
    {
        s = p + 1;
        unsigned int hi = strtoul(s, &p, 0);
        if (S32::isDigit(*s) && (p != s) && (*p == 0))
        {
            loKey = static_cast<key_t>(lo);
            hiKey = static_cast<key_t>(hi);
            if (beStrict)
            {
                ok = ((lo <= hi) && (lo >= validRange_.loKey) && (hi <= validRange_.hiKey));
            }
            else
            {
                ok = ((lo <= hi) && ((lo >= validRange_.loKey) || (hi <= validRange_.hiKey)));
            }
        }
    }

    // Return true if s is a valid range of keys.
    return ok;
}


//!
//! Compare two instances given their addresses. Return a negative value if
//! *item0 < *item1. Return 0 if *item0 == *item1. Return a positive value
//! if *item0 > *item1.
//!
int U16Set::compareP(const void* item0, const void* item1)
{
    int rc;
    const U16Set* k0 = static_cast<const U16Set*>(item0);
    const U16Set* k1 = static_cast<const U16Set*>(item1);
    for (size_t i = 0, i0End = k0->numRanges_, i1End = k1->numRanges_;; ++i)
    {

        if (i == i0End)
        {
            rc = (i == i1End)? 0: -1;
            break;
        }

        if (i == i1End)
        {
            rc = 1;
            break;
        }

        const range_t& r0 = k0->rangeVec_[i];
        const range_t& r1 = k1->rangeVec_[i];
        if (r0.loKey == r1.loKey)
        {
            if (r0.hiKey == r1.hiKey)
            {
                continue;
            }
            rc = (r0.hiKey < r1.hiKey)? -1: 1;
            break;
        }
        rc = (r0.loKey < r1.loKey)? -1: 1;
        break;
    }

    return rc;
}


//
// Construct an empty instance.
//
void U16Set::construct(key_t validMin, key_t validMax)
{
    numKeys_ = 0;
    numRanges_ = 0;
    validRange_.hiKey = validMax;
    validRange_.loKey = validMin;

    rangeVec_ = new range_t[capacity()];
}


//
// Remove overlaps near the ith entry.
//
void U16Set::rmOverlaps(size_t i)
{

    // No-op if set is empty.
    if (numRanges_ == 0)
    {
        return;
    }

    // Allow out-of-range index.
    if (i >= numRanges_)
    {
        i = numRanges_ - 1;
    }

    // See if left entries can be combined.
    const range_t& r = rangeVec_[i];
    size_t i1;
    if (r.loKey)
    {
        key_t loerKey = r.loKey - 1;
        for (i1 = i; i1 > 0; --i1)
        {
            if (rangeVec_[i1 - 1].hiKey < loerKey)
            {
                break;
            }
        }
    }
    else
    {
        i1 = 0;
    }

    // See if right entries can be combined.
    size_t i2;
    unsigned int maxI2 = numRanges_ - 1;
    if (r.hiKey != VALID_MAX)
    {
        key_t hierKey = r.hiKey + 1;
        for (i2 = i; i2 < maxI2; ++i2)
        {
            if (rangeVec_[i2 + 1].loKey > hierKey)
            {
                break;
            }
        }
    }
    else
    {
        i2 = maxI2;
    }

    // Some entries (from i1 through i2) can be combined into one.
    if (i1 != i2)
    {

        // Combine entries and adjust number of keys.
        for (size_t j = i1; j <= i2; ++j)
        {
            const range_t& r = rangeVec_[j];
            numKeys_ -= r.hiKey - r.loKey + 1;
        }
        range_t& r1 = rangeVec_[i1];
        const range_t& r2 = rangeVec_[i2];
        if (r1.loKey > r.loKey)
        {
            r1.loKey = r.loKey;
        }
        r1.hiKey = (r2.hiKey < r.hiKey)? r.hiKey: r2.hiKey;
        numKeys_ += r1.hiKey - r1.loKey + 1;

        // Shift bottom entries up.
        numRanges_ -= static_cast<unsigned int>(i2 - i1);
        if (i2 != maxI2)
        {
            memmove(rangeVec_ + i1 + 1, rangeVec_ + i2 + 1, (maxI2 - i2)*sizeof(*rangeVec_));
        }
    }
}


//!
//! Construct an unattached U16Set iterator.
//!
U16Set::Itor::Itor()
{
    copyMade_ = false;
    curIndex_ = INVALID_INDEX;
    set_ = 0;
}


//!
//! Construct a U16Set iterator. Attach iterator to given set.
//! A deep copy of the given set is made if makeCopy
//! is true. Normally, a deep copy should be made only if the
//! given set can change during iterating.
//!
U16Set::Itor::Itor(const U16Set& set, bool makeCopy)
{
    copyMade_ = false;
    set_ = 0;

    // attach() is also responsible for resetting the iterator and
    // that's why curIndex_ does not have to be initialized here.
    attach(set, makeCopy);
}


//!
//! Destruct iterator.
//!
U16Set::Itor::~Itor()
{

    // Destroy private copy.
    if (copyMade_)
    {
        delete set_;
    }
}


//!
//! Iterate the set from high to low. Invoke callback at each key.
//! The callback should return true to continue iterating and should
//! return false to abort iterating. Return false if the callback
//! aborted the iterating. Return true otherwise. Ignore if iterator
//! is unattached.
//!
bool U16Set::Itor::applyHiToLo(keyCb0_t cb, void* arg) const
{

    // Ignore if iterator is unattached.
    if (set_ != 0)
    {
        const U16Set::range_t* p0 = set_->rangeVec_;
        const U16Set::range_t* p = p0 + set_->numRanges_ - 1;
        for (; p >= p0; --p)
        {
            for (key_t key = p->hiKey;; --key)
            {
                if (!cb(arg, key))
                {
                    return false;
                }
                if (key == p->loKey)
                {
                    break;
                }
            }
        }
    }

    // Return true to indicate the iterating was not aborted.
    return true;
}


//!
//! Iterate the set from high to low. Invoke callback at each range.
//! The callback should return true to continue iterating and should
//! return false to abort iterating. Return false if the callback
//! aborted the iterating. Return true otherwise. Ignore if iterator
//! is unattached.
//!
bool U16Set::Itor::applyHiToLo(rangeCb0_t cb, void* arg) const
{

    // Ignore if iterator is unattached.
    bool ok = true;
    if (set_ != 0)
    {
        const U16Set::range_t* p0 = set_->rangeVec_;
        const U16Set::range_t* p = p0 + set_->numRanges_ - 1;
        for (; p >= p0; --p)
        {
            if (!cb(arg, p->loKey, p->hiKey))
            {
                ok = false;
                break;
            }
        }
    }

    // Return true if the iterating was not aborted.
    return ok;
}


//!
//! Iterate the set from low to high. Invoke callback at each key.
//! The callback should return true to continue iterating and should
//! return false to abort iterating. Return false if the callback
//! aborted the iterating. Return true otherwise. Ignore if iterator
//! is unattached.
//!
bool U16Set::Itor::applyLoToHi(keyCb0_t cb, void* arg) const
{

    // Ignore if iterator is unattached.
    if (set_ != 0)
    {
        const U16Set::range_t* p = set_->rangeVec_;
        const U16Set::range_t* pEnd = p + set_->numRanges_;
        for (; p < pEnd; ++p)
        {
            for (key_t key = p->loKey;; ++key)
            {
                if (!cb(arg, key))
                {
                    return false;
                }
                if (key == p->hiKey)
                {
                    break;
                }
            }
        }
    }

    // Return true to indicate the iterating was not aborted.
    return true;
}


//!
//! Iterate the set from low to high. Invoke callback at each range.
//! The callback should return true to continue iterating and should
//! return false to abort iterating. Return false if the callback
//! aborted the iterating. Return true otherwise. Ignore if iterator
//! is unattached.
//!
bool U16Set::Itor::applyLoToHi(rangeCb0_t cb, void* arg) const
{

    // Ignore if iterator is unattached.
    bool ok = true;
    if (set_ != 0)
    {
        const U16Set::range_t* p = set_->rangeVec_;
        const U16Set::range_t* pEnd = p + set_->numRanges_;
        for (; p < pEnd; ++p)
        {
            if (!cb(arg, p->loKey, p->hiKey))
            {
                ok = false;
                break;
            }
        }
    }

    // Return true if the iterating was not aborted.
    return ok;
}


//!
//! Retrieve the current range. Return true if there's one. Return false otherwise
//! (iterating has not started or if set is empty). Behavior is unpredictable if
//! the iterator is unattached.
//!
bool U16Set::Itor::curRange(key_t& loKey, key_t& hiKey) const
{
    bool ok;
    if (curIndex_ == INVALID_INDEX)
    {
        ok = false;
    }
    else
    {
        const range_t& r = set_->rangeVec_[curIndex_];
        loKey = r.loKey;
        hiKey = r.hiKey;
        ok = true;
    }

    return ok;
}


//!
//! Retrieve the next key. Return true if there's one. Return
//! false otherwise (if set is empty or if there's no more
//! keys). Behavior is unpredictable if the iterator is unattached.
//! The first invocation after construction or reset() will return
//! the first key.
//!
bool U16Set::Itor::nextKey(key_t& key)
{

    // Assume unsuccessful.
    bool ok = false;

    // Retrieve first key.
    if (curIndex_ == INVALID_INDEX)
    {
        if (set_->numRanges_ > 0)
        {
            ok = true;
            curIndex_ = 0;
            curKey_ = set_->rangeVec_[curIndex_].loKey;
            key = curKey_;
        }
    }

    // Retrieve next key.
    else if (curIndex_ < set_->numRanges_)
    {
        if (curKey_ == set_->rangeVec_[curIndex_].hiKey)
        {
            if (++curIndex_ < set_->numRanges_)
            {
                ok = true;
                curKey_ = set_->rangeVec_[curIndex_].loKey;
                key = curKey_;
            }
        }
        else
        {
            ok = true;
            ++curKey_;
            key = curKey_;
        }
    }

    // Return true if successful.
    return ok;
}


//!
//! Retrieve the next range. Return true if there's one. Return
//! false otherwise (if set is empty or if there's no more
//! ranges). Behavior is unpredictable if the iterator is unattached.
//! The first invocation after construction or reset() will return
//! the first range.
//!
bool U16Set::Itor::nextRange(key_t& loKey, key_t& hiKey)
{

    // Assume unsuccessful.
    bool ok = false;

    // Retrieve first range.
    if (curIndex_ == INVALID_INDEX)
    {
        if (set_->numRanges_ > 0)
        {
            ok = true;
            curIndex_ = 0;
            const U16Set::range_t& r = set_->rangeVec_[curIndex_];
            loKey = r.loKey;
            hiKey = r.hiKey;
            curKey_ = loKey;
        }
    }

    // Retrieve next range.
    else if (++curIndex_ < set_->numRanges_)
    {
        ok = true;
        const U16Set::range_t& r = set_->rangeVec_[curIndex_];
        loKey = r.loKey;
        hiKey = r.hiKey;
        curKey_ = loKey;
    }

    // There's no more ranges.
    else
    {
        curIndex_ = set_->numRanges_ - 1;
    }

    // Return true if successful.
    return ok;
}


//!
//! Retrieve the previous key. Return true if there's one. Return
//! false otherwise (if set is empty or if there's no more
//! keys). Behavior is unpredictable if the iterator is unattached.
//! The first invocation after construction or reset() will return
//! the last key.
//!
bool U16Set::Itor::prevKey(key_t& key)
{

    // Assume unsuccessful.
    bool ok = false;

    // Retrieve last key.
    if (curIndex_ == INVALID_INDEX)
    {
        if (set_->numRanges_ > 0)
        {
            ok = true;
            curIndex_ = set_->numRanges_ - 1;
            curKey_ = set_->rangeVec_[curIndex_].hiKey;
            key = curKey_;
        }
    }

    // Retrieve previous key.
    else if (curIndex_ < set_->numRanges_)
    {
        if (curKey_ == set_->rangeVec_[curIndex_].loKey)
        {
            if (--curIndex_ < set_->numRanges_)
            {
                ok = true;
                curKey_ = set_->rangeVec_[curIndex_].hiKey;
                key = curKey_;
            }
            else
            {
                curIndex_ = set_->numRanges_;
            }
        }
        else
        {
            ok = true;
            --curKey_;
            key = curKey_;
        }
    }

    // Return true if successful.
    return ok;
}


//!
//! Retrieve the previous range. Return true if there's one. Return
//! false otherwise (if set is empty or if there's no more
//! ranges). Behavior is unpredictable if the iterator is unattached.
//! The first invocation after construction or reset() will return
//! the last range.
//!
bool U16Set::Itor::prevRange(key_t& loKey, key_t& hiKey)
{

    // Assume unsuccessful.
    bool ok = false;

    // Retrieve last range.
    if (curIndex_ == INVALID_INDEX)
    {
        if (set_->numRanges_ > 0)
        {
            ok = true;
            curIndex_ = set_->numRanges_ - 1;
            const U16Set::range_t& r = set_->rangeVec_[curIndex_];
            loKey = r.loKey;
            hiKey = r.hiKey;
            curKey_ = hiKey;
        }
    }

    // Retrieve previous range.
    else if (--curIndex_ < set_->numRanges_)
    {
        ok = true;
        const U16Set::range_t& r = set_->rangeVec_[curIndex_];
        loKey = r.loKey;
        hiKey = r.hiKey;
        curKey_ = hiKey;
    }

    // There's no more ranges.
    else
    {
        curIndex_ = 0;
    }

    // Return true if successful.
    return ok;
}


//!
//! Iterate the set from high to low. Invoke callback at each key.
//! Ignore if iterator is unattached.
//!
void U16Set::Itor::applyHiToLo(keyCb1_t cb, void* arg) const
{

    // Ignore if iterator is unattached.
    if (set_ != 0)
    {
        const U16Set::range_t* p0 = set_->rangeVec_;
        const U16Set::range_t* p = p0 + set_->numRanges_ - 1;
        for (; p >= p0; --p)
        {
            for (key_t key = p->hiKey;; --key)
            {
                cb(arg, key);
                if (key == p->loKey)
                {
                    break;
                }
            }
        }
    }
}


//!
//! Iterate the set from high to low. Invoke callback at each range.
//! Ignore if iterator is unattached.
//!
void U16Set::Itor::applyHiToLo(rangeCb1_t cb, void* arg) const
{

    // Ignore if iterator is unattached.
    if (set_ != 0)
    {
        const U16Set::range_t* p0 = set_->rangeVec_;
        const U16Set::range_t* p = p0 + set_->numRanges_ - 1;
        for (; p >= p0; --p)
        {
            cb(arg, p->loKey, p->hiKey);
        }
    }
}


//!
//! Iterate the set from low to high. Invoke callback at each key.
//! Ignore if iterator is unattached.
//!
void U16Set::Itor::applyLoToHi(keyCb1_t cb, void* arg) const
{

    // Ignore if iterator is unattached.
    if (set_ != 0)
    {
        const U16Set::range_t* p = set_->rangeVec_;
        const U16Set::range_t* pEnd = p + set_->numRanges_;
        for (; p < pEnd; ++p)
        {
            for (key_t key = p->loKey;; ++key)
            {
                cb(arg, key);
                if (key == p->hiKey)
                {
                    break;
                }
            }
        }
    }
}


//!
//! Iterate the set from low to high. Invoke callback at each range.
//! Ignore if iterator is unattached.
//!
void U16Set::Itor::applyLoToHi(rangeCb1_t cb, void* arg) const
{

    // Ignore if iterator is unattached.
    if (set_ != 0)
    {
        const U16Set::range_t* p = set_->rangeVec_;
        const U16Set::range_t* pEnd = p + set_->numRanges_;
        for (; p < pEnd; ++p)
        {
            cb(arg, p->loKey, p->hiKey);
        }
    }
}


//!
//! Attach iterator to given set. A deep copy of
//! the given set is made if makeCopy is
//! true. Normally, a deep copy should be made only if
//! the given set can change during iterating.
//!
void U16Set::Itor::attach(const U16Set& set, bool makeCopy)
{

    // Destroy old copy.
    if (copyMade_)
    {
        delete set_;
    }

    // Make private copy of given set if necessary.
    set_ = makeCopy? new U16Set(set): &set;
    copyMade_ = makeCopy;

    // Reset iterator.
    reset();
}


//!
//! Detach from any attached set. Ignore if iterator is unattached.
//!
void U16Set::Itor::detach()
{
    if (copyMade_)
    {
        delete set_;
        copyMade_ = false;
    }

    set_ = 0;
}

END_NAMESPACE1
