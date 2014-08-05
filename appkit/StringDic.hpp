/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_STRING_DIC_HPP
#define APPKIT_STRING_DIC_HPP

#include "appkit/String.hpp"
#include "syskit/Tree.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)

class DelimitedTxt;
class StringPair;
class StringVec;


//! dictionary of key-value strings
class StringDic
    //!
    //! A class representing a dictionary of key-value strings. Key-value pairs
    //! are added using the add() and associate() methods, and are removed using
    //! the rm() methods. Searches are provided by the find() methods. Implemented
    //! using a 2-3-4 tree of StringPair instances. Example:
    //!\code
    //! StringDic dic;
    //! :
    //! dic.add(k0, v0);       //add k0-v0 key-value pair
    //! dic.add(k1, v1);       //add k1-v1 key-value pair
    //! dic.associate(k0, v2); //update k0's value
    //! dic.rm(k1);            //remove k1 association
    //!\endcode
    //!
{

public:
    typedef bool(*cb0_t)(void* arg, const String& k, String& v);
    typedef bool(*cb1_t)(void* arg, const String& k, const String& v);

    // Constructors and destructor.
    StringDic(DelimitedTxt& txt, char kvDelim = '=', bool trimLines = true, bool ignoreCase = false);
    StringDic(StringDic* that);
    StringDic(bool ignoreCase = false);
    StringDic(const StringDic& dic);
    ~StringDic();

    // Operators.
    String operator [](const String& k) const;
    String operator [](const char* k) const;
    String& operator [](const String& k);
    String& operator [](const char* k);
    bool operator !=(const StringDic& dic) const;
    bool operator ==(const StringDic& dic) const;
    const StringDic& operator +=(const StringDic& dic);
    const StringDic& operator =(StringDic* that);
    const StringDic& operator =(const StringDic& dic);
    const StringDic& operator -=(const StringDic& dic);
    const StringDic& operator -=(const StringVec& keys);

    // Mapping operations.
    String stringify(char kvDelim = '=', const char* lineDelim = "\n") const;
    bool add(const String& k, const String& v);
    bool add(const StringDic& dic);
    bool associate(const String& k, const String& v);
    bool associate(const String& k, const String& v, String& oldV);
    bool associate(const String& k, const String& v, char delim);
    bool associate(const StringDic& dic);
    bool contains(const String& k) const;
    bool contains(const String& k, const String& v) const;
    bool contains(const StringDic& dic) const;
    bool contains(const char* k) const;
    bool contains(const char* k, const String& v) const;
    bool contains(const char* k, const char* v) const;
    bool reset(DelimitedTxt& txt, char kvDelim = '=', bool trimLines = true);
    bool rm(const String& k);
    bool rm(const String& k, String& removedV);
    bool rm(const StringDic& dic);
    bool rm(const StringVec& keys);
    bool rm(const char* k);
    bool rm(const char* k, String& removedV);
    const String* find(const String& k, const String* defaultV = 0) const;
    const String* find(const char* k, const String* defaultV = 0) const;
    void any(String& k, String& v) const;
    void compare(const StringDic& b4, StringDic* added = 0, StringDic* updated = 0, StringDic* removed = 0) const;
    void mergeInto(StringDic& result);
    void rebalance();
    void reset();

    // Getters.
    bool ignoreCase() const;
    unsigned int numKvPairs() const;

    // Variants supporting primitive value types.
    String getValue(const String& k, const String& defaultV = String()) const;
    String getValue(const char* k, const String& defaultV = String()) const;
    StringPair* getKv(const String& k, bool& kvAdded);
    StringPair* getKv(const char* k, bool& kvAdded);
    bool associate(const String& k, bool v);
    bool associate(const String& k, const char* v);
    bool associate(const String& k, double v);
    bool associate(const String& k, float v);
    bool associate(const String& k, int v);
    bool associate(const String& k, unsigned int v);
    bool associate(const String& k, unsigned long long v);
    bool associate(const String& k, unsigned short v);
    bool getValueAsBool(const String& k, bool defaultV = false) const;
    bool getValueAsBool(const char* k, bool defaultV = false) const;
    const StringPair* getKv(const String& k) const;
    const StringPair* getKv(const char* k) const;
    const char* getValueAsAscii(const String& k, const char* defaultV = 0) const;
    const char* getValueAsAscii(const char* k, const char* defaultV = 0) const;
    double getValueAsD64(const String& k, double defaultV = 0.0) const;
    double getValueAsD64(const char* k, double defaultV = 0.0) const;
    float getValueAsF32(const String& k, float defaultV = 0.0f) const;
    float getValueAsF32(const char* k, float defaultV = 0.0f) const;
    int getValueAsS32(const String& k, int defaultV = 0) const;
    int getValueAsS32(const char* k, int defaultV = 0) const;
    unsigned int getValueAsU32(const String& k, unsigned int defaultV = 0) const;
    unsigned int getValueAsU32(const char* k, unsigned int defaultV = 0) const;
    unsigned long long getValueAsU64(const String& k, unsigned long long defaultV = 0) const;
    unsigned long long getValueAsU64(const char* k, unsigned long long defaultV = 0) const;
    unsigned short getValueAsU16(const String& k, unsigned short defaultV = 0) const;
    unsigned short getValueAsU16(const char* k, unsigned short defaultV = 0) const;

    // Iterator support.
    bool apply(cb0_t cb, void* arg = 0);
    bool apply(cb1_t cb, void* arg = 0) const;
    bool vectorize(StringVec& k, StringVec& v) const;

private:
    typedef struct
    {
        cb0_t cb;
        void* arg;
    } arg0_t;

    typedef struct
    {
        cb1_t cb;
        void* arg;
    } arg1_t;

    syskit::Tree tree_;
    syskit::Tree::compare_t compareK_;

    StringPair* getKv(const String&, const String&, bool&);
    void doReset(DelimitedTxt&, char, bool);
    void prepVec(StringVec&) const;

    static bool containsKv(void*, void*);
    static bool proxy0(void*, void*);
    static bool proxy1(void*, void*);
    static void addKv(void*, void*);
    static void associateKv(void*, void*);
    static void cloneKv(void*, void*);
    static void copyKv(syskit::Tree&, StringPair*);
    static void copyKv(void*, void*);
    static void deleteKv(void*, void*);
    static void findAddsAndUpdates(void*, void*);
    static void findRemoves(void*, void*);
    static void findUpdates(void*, void*);
    static void mergeKv(void*, void*);
    static void rmKv(void*, void*);
    static void stringifyKv(void*, void*);
    static void vectorizeKv(void*, void*);

};

StringDic operator +(const StringDic& a, const StringDic& b);
StringDic operator -(const StringDic& dic, const String& k);
StringDic operator -(const StringDic& a, const StringDic& b);
StringDic operator -(const StringDic& a, const StringVec& b);

END_NAMESPACE1

#include "appkit/StringPair.hpp"

BEGIN_NAMESPACE1(appkit)

//! Return associated value of given key. Associated value of a non-existent
//! key is an empty string.
inline String StringDic::operator [](const String& k) const
{
    void* foundItem;
    return tree_.find(&k, foundItem)? static_cast<const StringPair*>(foundItem)->v(): String();
}

//! Return associated value of given key. Associated value of a non-existent
//! key is an empty string.
inline String StringDic::operator [](const char* k) const
{
    void* foundItem;
    return tree_.find(k, compareK_, foundItem)? static_cast<const StringPair*>(foundItem)->v(): String();
}

//! If given key does not exist, create new key-value pair using an empty
//! value. Otherwise, use existing key-value pair. Return associated value.
inline String& StringDic::operator [](const String& k)
{
    bool kvAdded;
    return getKv(k, kvAdded)->v();
}

//! If given key does not exist, create new key-value pair using an empty
//! value. Otherwise, use existing key-value pair. Return associated value.
inline String& StringDic::operator [](const char* k)
{
    bool kvAdded;
    return getKv(k, kvAdded)->v();
}

//! Return true if this dictionary does not equal given dictionary. That
//! is, if both do not have the same number of key-value pairs or if
//! corresponding key-value pairs are not equal.
inline bool StringDic::operator !=(const StringDic& dic) const
{
    bool eq = (tree_.numItems() == dic.tree_.numItems())? tree_.apply(containsKv, const_cast<syskit::Tree*>(&dic.tree_)): false;
    return (!eq);
}

//! Return true if this dictionary equals given dictionary. That is, if
//! both have the same number of key-value pairs and if corresponding
//! key-value pairs are equal.
inline bool StringDic::operator ==(const StringDic& dic) const
{
    bool eq = (tree_.numItems() == dic.tree_.numItems())? tree_.apply(containsKv, const_cast<syskit::Tree*>(&dic.tree_)): false;
    return eq;
}

//! Associate with key-value pairs from dic. That is, copy key-value pairs from dic
//! into this. For existing keys, their associated values are updated with values
//! from dic.
inline const StringDic& StringDic::operator +=(const StringDic& dic)
{
    dic.tree_.applyParentFirst(associateKv, &tree_);
    return *this;
}

//! Reset and move the dictionary contents from that into this. Assume the dictionaries
//! are compatible. That is, items unique in that are also unique in this.
inline const StringDic& StringDic::operator =(StringDic* that)
{
    if (this != that) tree_ = &that->tree_;
    return *this;
}

//! Remove given key-value pairs from this dictionary.
//! Return reference to self.
inline const StringDic& StringDic::operator -=(const StringDic& dic)
{
    rm(dic);
    return *this;
}

//! Remove key-value pairs with given keys from this dictionary.
//! Return reference to self.
inline const StringDic& StringDic::operator -=(const StringVec& keys)
{
    rm(keys);
    return *this;
}

//! Return associated value of given key. Associated value of a non-existent
//! key is the given default value.
inline String StringDic::getValue(const String& k, const String& defaultV) const
{
    void* foundItem;
    return tree_.find(&k, foundItem)? static_cast<const StringPair*>(foundItem)->v(): defaultV;
}

//! Return associated value of given key. Associated value of a non-existent
//! key is the given default value.
inline String StringDic::getValue(const char* k, const String& defaultV) const
{
    void* foundItem;
    return tree_.find(k, compareK_, foundItem)? static_cast<const StringPair*>(foundItem)->v(): defaultV;
}

//! Add key-value pairs from dic into this. If necessary, drop key-value pairs
//! which are inappropriate for this dictionary (i.e., distinct keys from one
//! dictionary might be duplicates in another). Return true if at least one new
//! key-value pair was added successfully.
inline bool StringDic::add(const StringDic& dic)
{
    unsigned int b4 = tree_.numItems();
    dic.tree_.applyParentFirst(addKv, &tree_);
    return (tree_.numItems() > b4);
}

//! Apply callback to key-value pairs in order. The callback should return
//! true to continue iterating and should return false to abort iterating.
//! Return false if the callback aborted the iterating. Return true otherwise.
inline bool StringDic::apply(cb0_t cb, void* arg)
{
    arg0_t arg0 = {cb, arg};
    bool ok = tree_.apply(proxy0, &arg0);
    return ok;
}

//! Apply callback to key-value pairs in order. The callback should return
//! true to continue iterating and should return false to abort iterating.
//! Return false if the callback aborted the iterating. Return true otherwise.
inline bool StringDic::apply(cb1_t cb, void* arg) const
{
    arg1_t arg1 = {cb, arg};
    bool ok = tree_.apply(proxy1, &arg1);
    return ok;
}

//! If given key does not exist, create new key-value pair and return true.
//! Otherwise, update the associated value and return false.
inline bool StringDic::associate(const String& k, bool v)
{
    bool kvAdded;
    getKv(k, kvAdded)->setV(v);
    return kvAdded;
}

//! If given key does not exist, create new key-value pair and return true.
//! Otherwise, update the associated value and return false. That is, new
//! value becomes associated value (e.g., "k=old" --> "k=new").
inline bool StringDic::associate(const String& k, const String& v)
{
    bool kvAdded;
    StringPair* kv = getKv(k, v, kvAdded);
    if (!kvAdded) kv->setV(v);
    return kvAdded;
}

//! If given key does not exist, create new key-value pair and return true.
//! Otherwise, update the associated value and return false.
inline bool StringDic::associate(const String& k, const char* v)
{
    bool kvAdded;
    getKv(k, kvAdded)->setV(v);
    return kvAdded;
}

//! If given key does not exist, create new key-value pair and return true.
//! Otherwise, update the associated value and return false.
inline bool StringDic::associate(const String& k, double v)
{
    bool kvAdded;
    getKv(k, kvAdded)->setV(v);
    return kvAdded;
}

//! If given key does not exist, create new key-value pair and return true.
//! Otherwise, update the associated value and return false.
inline bool StringDic::associate(const String& k, float v)
{
    bool kvAdded;
    getKv(k, kvAdded)->setV(v);
    return kvAdded;
}

//! If given key does not exist, create new key-value pair and return true.
//! Otherwise, update the associated value and return false.
inline bool StringDic::associate(const String& k, int v)
{
    bool kvAdded;
    getKv(k, kvAdded)->setV(v);
    return kvAdded;
}

//! If given key does not exist, create new key-value pair and return true.
//! Otherwise, update the associated value and return false.
inline bool StringDic::associate(const String& k, unsigned int v)
{
    bool kvAdded;
    getKv(k, kvAdded)->setV(v);
    return kvAdded;
}

//! If given key does not exist, create new key-value pair and return true.
//! Otherwise, update the associated value and return false.
inline bool StringDic::associate(const String& k, unsigned long long v)
{
    bool kvAdded;
    getKv(k, kvAdded)->setV(v);
    return kvAdded;
}

//! If given key does not exist, create new key-value pair and return true.
//! Otherwise, update the associated value and return false.
inline bool StringDic::associate(const String& k, unsigned short v)
{
    bool kvAdded;
    getKv(k, kvAdded)->setV(v);
    return kvAdded;
}

//! Return true if given key exists in the dictionary.
inline bool StringDic::contains(const String& k) const
{
    bool found = tree_.find(&k);
    return found;
}

//! Return true if given key-value pair exists in the dictionary.
inline bool StringDic::contains(const String& k, const String& v) const
{
    void* foundItem;
    return tree_.find(&k, foundItem)? (static_cast<const StringPair*>(foundItem)->v() == v): (false);
}

//! Return true if this dictionary contains given dictionary.
inline bool StringDic::contains(const StringDic& dic) const
{
    bool answer = (tree_.numItems() >= dic.tree_.numItems())? dic.tree_.apply(containsKv, const_cast<syskit::Tree*>(&tree_)): false;
    return answer;
}

//! Return true if given key exists in the dictionary.
inline bool StringDic::contains(const char* k) const
{
    bool found = tree_.find(k, compareK_);
    return found;
}

//! Return true if given key-value pair exists in the dictionary.
inline bool StringDic::contains(const char* k, const String& v) const
{
    void* foundItem;
    return tree_.find(k, compareK_, foundItem)? (static_cast<const StringPair*>(foundItem)->v() == v): (false);
}

//! Return true if given key-value pair exists in the dictionary.
inline bool StringDic::contains(const char* k, const char* v) const
{
    void* foundItem;
    return tree_.find(k, compareK_, foundItem)? (static_cast<const StringPair*>(foundItem)->v() == v): (false);
}

//! Return associated value of given key. Associated value of a non-existent
//! key is the given default value.
inline bool StringDic::getValueAsBool(const String& k, bool defaultV) const
{
    void* foundItem;
    return tree_.find(&k, foundItem)? static_cast<const StringPair*>(foundItem)->vAsBool(): defaultV;
}

//! Return associated value of given key. Associated value of a non-existent
//! key is the given default value.
inline bool StringDic::getValueAsBool(const char* k, bool defaultV) const
{
    void* foundItem;
    return tree_.find(k, compareK_, foundItem)? static_cast<const StringPair*>(foundItem)->vAsBool(): defaultV;
}

//! Return true if case is ignored.
inline bool StringDic::ignoreCase() const
{
    bool answer = (compareK_ == String::compareKPI);
    return answer;
}

//! Return associated value of given key. Associated value of a non-existent
//! key is the given default value.
inline const String* StringDic::find(const String& k, const String* defaultV) const
{
    void* foundItem;
    const String* v = tree_.find(&k, foundItem)? &static_cast<const StringPair*>(foundItem)->v(): defaultV;
    return v;
}

//! Return associated value of given key. Associated value of a non-existent
//! key is the given default value.
inline const String* StringDic::find(const char* k, const String* defaultV) const
{
    void* foundItem;
    const String* v = tree_.find(k, compareK_, foundItem)? &static_cast<const StringPair*>(foundItem)->v(): defaultV;
    return v;
}

inline const StringPair* StringDic::getKv(const String& k) const
{
    void* foundItem;
    return tree_.find(&k, foundItem)? static_cast<const StringPair*>(foundItem): 0;
}

inline const StringPair* StringDic::getKv(const char* k) const
{
    void* foundItem;
    return tree_.find(k, compareK_, foundItem)? static_cast<const StringPair*>(foundItem): 0;
}

//! Return associated value of given key. Associated value of a non-existent
//! key is the given default value.
inline const char* StringDic::getValueAsAscii(const String& k, const char* defaultV) const
{
    void* foundItem;
    const char* v = tree_.find(&k, foundItem)? static_cast<const StringPair*>(foundItem)->vAsAscii(): defaultV;
    return v;
}

//! Return associated value of given key. Associated value of a non-existent
//! key is the given default value.
inline const char* StringDic::getValueAsAscii(const char* k, const char* defaultV) const
{
    void* foundItem;
    const char* v = tree_.find(k, compareK_, foundItem)? static_cast<const StringPair*>(foundItem)->vAsAscii(): defaultV;
    return v;
}

//! Return associated value of given key. Associated value of a non-existent
//! key is the given default value.
inline double StringDic::getValueAsD64(const String& k, double defaultV) const
{
    void* foundItem;
    return tree_.find(&k, foundItem)? static_cast<const StringPair*>(foundItem)->vAsD64(): defaultV;
}

//! Return associated value of given key. Associated value of a non-existent
//! key is the given default value.
inline double StringDic::getValueAsD64(const char* k, double defaultV) const
{
    void* foundItem;
    return tree_.find(k, compareK_, foundItem)? static_cast<const StringPair*>(foundItem)->vAsD64(): defaultV;
}

//! Return associated value of given key. Associated value of a non-existent
//! key is the given default value.
inline float StringDic::getValueAsF32(const String& k, float defaultV) const
{
    void* foundItem;
    return tree_.find(&k, foundItem)? static_cast<const StringPair*>(foundItem)->vAsF32(): defaultV;
}

//! Return associated value of given key. Associated value of a non-existent
//! key is the given default value.
inline float StringDic::getValueAsF32(const char* k, float defaultV) const
{
    void* foundItem;
    return tree_.find(k, compareK_, foundItem)? static_cast<const StringPair*>(foundItem)->vAsF32(): defaultV;
}

//! Return associated value of given key. Associated value of a non-existent
//! key is the given default value.
inline int StringDic::getValueAsS32(const String& k, int defaultV) const
{
    void* foundItem;
    return tree_.find(&k, foundItem)? static_cast<const StringPair*>(foundItem)->vAsS32(): defaultV;
}

//! Return associated value of given key. Associated value of a non-existent
//! key is the given default value.
inline int StringDic::getValueAsS32(const char* k, int defaultV) const
{
    void* foundItem;
    return tree_.find(k, compareK_, foundItem)? static_cast<const StringPair*>(foundItem)->vAsS32(): defaultV;
}

//! Return associated value of given key. Associated value of a non-existent
//! key is the given default value.
inline unsigned int StringDic::getValueAsU32(const String& k, unsigned int defaultV) const
{
    void* foundItem;
    return tree_.find(&k, foundItem)? static_cast<const StringPair*>(foundItem)->vAsU32(): defaultV;
}

//! Return associated value of given key. Associated value of a non-existent
//! key is the given default value.
inline unsigned int StringDic::getValueAsU32(const char* k, unsigned int defaultV) const
{
    void* foundItem;
    return tree_.find(k, compareK_, foundItem)? static_cast<const StringPair*>(foundItem)->vAsU32(): defaultV;
}

//! Return the current number of key-value pairs in the dictionary.
inline unsigned int StringDic::numKvPairs() const
{
    return tree_.numItems();
}

//! Return associated value of given key. Associated value of a non-existent
//! key is the given default value.
inline unsigned long long StringDic::getValueAsU64(const String& k, unsigned long long defaultV) const
{
    void* foundItem;
    return tree_.find(&k, foundItem)? static_cast<const StringPair*>(foundItem)->vAsU64(): defaultV;
}

//! Return associated value of given key. Associated value of a non-existent
//! key is the given default value.
inline unsigned long long StringDic::getValueAsU64(const char* k, unsigned long long defaultV) const
{
    void* foundItem;
    return tree_.find(k, compareK_, foundItem)? static_cast<const StringPair*>(foundItem)->vAsU64(): defaultV;
}

//! Return associated value of given key. Associated value of a non-existent
//! key is the given default value.
inline unsigned short StringDic::getValueAsU16(const String& k, unsigned short defaultV) const
{
    void* foundItem;
    return tree_.find(&k, foundItem)? static_cast<const StringPair*>(foundItem)->vAsU16(): defaultV;
}

//! Return associated value of given key. Associated value of a non-existent
//! key is the given default value.
inline unsigned short StringDic::getValueAsU16(const char* k, unsigned short defaultV) const
{
    void* foundItem;
    return tree_.find(k, compareK_, foundItem)? static_cast<const StringPair*>(foundItem)->vAsU16(): defaultV;
}

//! Frequent key-value pair removal can cause a dictionary to become somewhat
//! unbalanced. This method can be used to rebalance a suspected unbalanced
//! dictionary.
inline void StringDic::rebalance()
{
    tree_.rebalance();
}

//! Reset the dictionary by removing all key-value pairs.
inline void StringDic::reset()
{
    tree_.applyChildFirst(deleteKv, 0 /*arg*/);
    tree_.reset();
}

inline StringDic operator +(const StringDic& a, const StringDic& b)
{
    StringDic result(a);
    result += b;
    return &result; //move guts from result to returned dictionary
}

//! Some potentially-preferred syntax for "(dic.rm(k), dic)".
inline StringDic operator -(const StringDic& dic, const String& k)
{
    StringDic result(dic);
    result.rm(k);
    return &result; //move guts from result to returned dictionary
}

inline StringDic operator -(const StringDic& a, const StringDic& b)
{
    StringDic result(a);
    result -= b;
    return &result; //move guts from result to returned dictionary
}

inline StringDic operator -(const StringDic& a, const StringVec& b)
{
    StringDic result(a);
    result -= b;
    return &result; //move guts from result to returned dictionary
}

END_NAMESPACE1

#endif
