/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/DelimitedTxt.hpp"
#include "appkit/StringDic.hpp"
#include "appkit/StringVec.hpp"

using namespace syskit;

BEGIN_NAMESPACE

using namespace appkit;

typedef struct
{
    String* dicAsString;
    String* lineDelim;
    char kvDelim;
} stringifyArg_t;

END_NAMESPACE

BEGIN_NAMESPACE1(appkit)


//!
//! Construct a dictionary with given key-value pairs. Each line in txt contains
//! a key-value pair delimited by kvDelim. Use trimLines to indicate if lines
//! need to be trimmed (i.e., if a line delimiter is or is not included in the
//! value part). In case of duplicates, the newer key-value pair is used.
//!
StringDic::StringDic(DelimitedTxt& txt, char kvDelim, bool trimLines, bool ignoreCase):
tree_(ignoreCase? String::comparePI: String::compareP)
{
    compareK_ = ignoreCase? String::compareKPI: String::compareKP;
    doReset(txt, kvDelim, trimLines);
}


//!
//! Construct instance using guts from that. That is, move dictionary contents from
//! that into this.
//!
StringDic::StringDic(StringDic* that):
tree_(&that->tree_)
{
    compareK_ = that->compareK_;
}


//!
//! Construct an empty dictionary.
//!
StringDic::StringDic(bool ignoreCase):
tree_(ignoreCase? String::comparePI: String::compareP)
{
    compareK_ = ignoreCase? String::compareKPI: String::compareKP;
}


//!
//! Construct a duplicate instance of the given dictionary.
//!
StringDic::StringDic(const StringDic& dic):
tree_(dic.tree_.cmpFunc())
{
    compareK_ = dic.ignoreCase()? String::compareKPI: String::compareKP;
    dic.tree_.applyParentFirst(cloneKv, &tree_);
}


StringDic::~StringDic()
{
    void* arg = 0;
    tree_.applyChildFirst(deleteKv, arg);
}


//!
//! Reset and add key-value pairs from dic into this. If necessary, drop key-value
//! pairs which are inappropriate for this dictionary (i.e., distinct keys from one
//! dictionary might be duplicates in another).
//!
const StringDic& StringDic::operator =(const StringDic& dic)
{

    // Prevent self assignment.
    if (this != &dic)
    {
        reset();
        Tree::cb1_t cb = (compareK_ == dic.compareK_)? cloneKv: addKv;
        dic.tree_.applyParentFirst(cb, &tree_);
    }

    // Return reference to self.
    return *this;
}


//!
//! Just as a string can be expanded into a dictionary of key-value pairs, the
//! dictionary can be collapsed back into one string. This method collapses the
//! dictionary into a string by concatenating the key-value pairs in order. For
//! each key-value pair, kvDelim is used to delimited the key and value. The
//! key-value pairs are delimited by given lineDelim. Return the concatenated
//! string (e.g., "k0=v0" "\n" k1=v1" "\n".)
//!
String StringDic::stringify(char kvDelim, const char* lineDelim) const
{
    String s;
    String delim(lineDelim);
    stringifyArg_t arg;
    arg.dicAsString = &s;
    arg.lineDelim = &delim;
    arg.kvDelim = kvDelim;
    tree_.apply(stringifyKv, &arg);
    return s;
}


//!
//! If given key does not exist, create and return new key-value pair. Otherwise,
//! return existing key-value pair. Use kvAdded to indicate if returned key-value
//! pair has just been created.
//!
StringPair* StringDic::getKv(const String& k, bool& kvAdded)
{
    StringPair* kv;
    void* foundItem;
    if (tree_.find(&k, foundItem))
    {
        kv = static_cast<StringPair*>(foundItem);
        kvAdded = false;
    }
    else
    {
        kv = new StringPair(k);
        kvAdded = tree_.add(kv);
    }

    return kv;
}


StringPair* StringDic::getKv(const String& k, const String& v, bool& kvAdded)
{
    StringPair* kv;
    void* foundItem;
    if (tree_.find(&k, foundItem))
    {
        kv = static_cast<StringPair*>(foundItem);
        kvAdded = false;
    }
    else
    {
        kv = new StringPair(k, v);
        kvAdded = tree_.add(kv);
    }

    return kv;
}


//!
//! If given key does not exist, create and return new key-value pair. Otherwise,
//! return existing key-value pair. Use kvAdded to indicate if returned key-value
//! pair has just been created.
//!
StringPair* StringDic::getKv(const char* k, bool& kvAdded)
{
    StringPair* kv;
    void* foundItem;
    if (tree_.find(k, compareK_, foundItem))
    {
        kv = static_cast<StringPair*>(foundItem);
        kvAdded = false;
    }
    else
    {
        kv = new StringPair(k);
        kvAdded = tree_.add(kv);
    }

    return kv;
}


//!
//! Add new key-value pair. Return true if successful. Return false otherwise
//! (duplicate key).
//!
bool StringDic::add(const String& k, const String& v)
{
    StringPair* kv = new StringPair(k, v);
    bool ok = tree_.add(kv);
    if (!ok)
    {
        delete kv;
    }

    return ok;
}


//!
//! If given key does not exist, create new key-value pair and return true.
//! Otherwise, update the associated value and return false (also return the
//! old value).
//!
bool StringDic::associate(const String& k, const String& v, String& oldV)
{
    bool kvAdded;
    StringPair* kv = getKv(k, v, kvAdded);
    if (!kvAdded)
    {
        oldV = kv->v();
        kv->setV(v);
    }

    return kvAdded;
}


//!
//! If given key does not exist, create new key-value pair and return true.
//! Otherwise, update the associated value and return false. If delim is zero,
//! new value becomes associated value (e.g., "k=old" --> "k=new"). Otherwise,
//! associated value is the concatenation of old value, the delimiter delim,
//! and new value (e.g., "k=old" --> "k=old,new").
//!
bool StringDic::associate(const String& k, const String& v, char delim)
{
    bool kvAdded;
    StringPair* kv = getKv(k, v, kvAdded);
    if (!kvAdded)
    {
        (delim == 0)? kv->setV(v): kv->addV(v, delim);
    }

    return kvAdded;
}


//!
//! Associate with key-value pairs from dic. That is, copy key-value pairs from dic
//! into this. For existing keys, their associated values are updated with values
//! from dic. Return true if some change occurred.
//!
bool StringDic::associate(const StringDic& dic)
{
    bool modified = false;
    void* arg[2] = {&tree_, &modified};
    dic.tree_.applyParentFirst(copyKv, arg);
    return modified;
}


//
// Callback to compare two dictionaries. The callback terminates the iterating
// as soon as a key-value pair from one dictionary is non-existent in the other.
//
bool StringDic::containsKv(void* arg, void* item)
{
    const Tree& tree = *static_cast<const Tree*>(arg);
    void* foundItem;
    bool keepGoing;
    if (tree.find(item, foundItem))
    {
        const StringPair* key = static_cast<const StringPair*>(item);
        const StringPair* foundKv = static_cast<const StringPair*>(foundItem);
        keepGoing = (key->v() == foundKv->v());
    }
    else
    {
        keepGoing = false;
    }

    return keepGoing;
}


bool StringDic::proxy0(void* arg, void* item)
{
    const arg0_t& r = *static_cast<const arg0_t*>(arg);
    StringPair* kv = static_cast<StringPair*>(item);
    bool keepGoing = r.cb(r.arg, kv->k(), kv->v());
    return keepGoing;
}


bool StringDic::proxy1(void* arg, void* item)
{
    const arg1_t& r = *static_cast<const arg1_t*>(arg);
    const StringPair* kv = static_cast<const StringPair*>(item);
    bool keepGoing = r.cb(r.arg, kv->k(), kv->v());
    return keepGoing;
}


//!
//! Reset the dictionary with given key-value pairs. Each line in txt contains
//! a key-value pair delimited by kvDelim. Use trimLines to indicate if lines
//! need to be trimmed (i.e., if a line delimiter is or is not included in the
//! value part). In case of duplicates, the newer key-value pair is used. Return
//! true if the resulting dictionary is non-empty.
//!
bool StringDic::reset(DelimitedTxt& txt, char kvDelim, bool trimLines)
{
    reset();
    doReset(txt, kvDelim, trimLines);
    bool ok = (tree_.numItems() > 0);
    return ok;
}


//!
//! Remove given key-value pair from dictionary. Return true if successful.
//! Return false otherwise (key not found). Frequent key-value pair removal
//! can cause a dictionary to become somewhat unbalanced. A suspected unbalanced
//! dictionary can be rebalanced using the rebalance() method.
//!
bool StringDic::rm(const String& k)
{
    void* removedItem;
    bool ok = tree_.rm(&k, removedItem);
    if (ok)
    {
        const StringPair* kv = static_cast<const StringPair*>(removedItem);
        delete kv;
    }

    return ok;
}


//!
//! Remove given key-value pair from dictionary. Return true if successful
//! (also return the removed value). Return false otherwise (key not found).
//! Frequent key-value pair removal can cause a dictionary to become somewhat
//! unbalanced. A suspected unbalanced dictionary can be rebalanced using the
//! rebalance() method.
//!
bool StringDic::rm(const String& k, String& removedV)
{
    void* removedItem;
    bool ok = tree_.rm(&k, removedItem);
    if (ok)
    {
        const StringPair* kv = static_cast<const StringPair*>(removedItem);
        removedV = kv->v();
        delete kv;
    }

    return ok;
}


//!
//! Remove given key-value pairs from this dictionary. Return true if at least one
//! key-value pair was removed successfully. Return false otherwise (key-value pairs
//! not found). Frequent key-value pair removal can cause a dictionary to become
//! somewhat unbalanced. A suspected unbalanced dictionary can be rebalanced using
//! the rebalance() method.
//!
bool StringDic::rm(const StringDic& dic)
{
    bool modified;
    if (this == &dic)
    {
        modified = (tree_.numItems() > 0);
        reset();
    }
    else
    {
        unsigned int b4 = tree_.numItems();
        dic.tree_.applyChildFirst(rmKv, &tree_);
        modified = (tree_.numItems() < b4);
    }

    return modified;
}


//!
//! Remove key-value pairs with given keys from this dictionary. Return true if at
//! least one key-value pair was removed successfully. Return false otherwise (keys
//! not found). Frequent key-value pair removal can cause a dictionary to become
//! somewhat unbalanced. A suspected unbalanced dictionary can be rebalanced using
//! the rebalance() method.
//!
bool StringDic::rm(const StringVec& keys)
{
    unsigned int b4 = tree_.numItems();
    const String* const* p0 = keys.raw();
    for (size_t i = keys.numItems(); i > 0; rm(*p0[--i]));
    return (tree_.numItems() < b4);
}


//!
//! Remove given key-value pair from dictionary. Return true if successful.
//! Return false otherwise (key not found). Frequent key-value pair removal
//! can cause a dictionary to become somewhat unbalanced. A suspected unbalanced
//! dictionary can be rebalanced using the rebalance() method.
//!
bool StringDic::rm(const char* k)
{
    void* removedItem;
    bool ok = tree_.rm(k, compareK_, removedItem);
    if (ok)
    {
        const StringPair* kv = static_cast<const StringPair*>(removedItem);
        delete kv;
    }

    return ok;
}


//!
//! Remove given key-value pair from dictionary. Return true if successful
//! (also return the removed value). Return false otherwise (key not found).
//! Frequent key-value pair removal can cause a dictionary to become somewhat
//! unbalanced. A suspected unbalanced dictionary can be rebalanced using the
//! rebalance() method.
//!
bool StringDic::rm(const char* k, String& removedV)
{
    void* removedItem;
    bool ok = tree_.rm(k, compareK_, removedItem);
    if (ok)
    {
        const StringPair* kv = static_cast<const StringPair*>(removedItem);
        removedV = kv->v();
        delete kv;
    }

    return ok;
}


//!
//! Look at dictionary as two string vectors. Return true if successful.
//! Return false otherwise (result is partial due to insufficient capacity
//! in provided vectors).
//!
bool StringDic::vectorize(StringVec& k, StringVec& v) const
{
    prepVec(k);
    prepVec(v);
    StringVec* vec[2] = {&k, &v};
    tree_.apply(vectorizeKv, vec);
    bool ok = (k.numItems() == tree_.numItems()) && (v.numItems() == tree_.numItems());
    return ok;
}


//
// Callback to copy key-value pairs from one dictionary to another. Drop duplicate keys.
//
void StringDic::addKv(void* arg, void* item)
{
    Tree& dst = *static_cast<Tree*>(arg);
    const StringPair& src = *static_cast<const StringPair*>(item);
    StringPair* cloned = new StringPair(src);
    if (!dst.add(cloned))
    {
        delete cloned;
    }
}


//!
//! Return any key-value pair in the dictionary.
//! Return values are empty strings for an empty dictionary.
//!
void StringDic::any(String& k, String& v) const
{
    const StringPair* kv = static_cast<const StringPair*>(tree_.any());
    if (kv == 0)
    {
        k.reset();
        v.reset();
    }
    else
    {
        k = kv->k();
        v = kv->v();
    }
}


//
// Callback to associate key-value pairs from one dictionary in another.
//
void StringDic::associateKv(void* arg, void* item)
{
    Tree& dst = *static_cast<Tree*>(arg);
    void* foundItem;
    if (dst.find(item, foundItem))
    {
        StringPair* foundKv = static_cast<StringPair*>(foundItem);
        const StringPair* src = static_cast<const StringPair*>(item);
        foundKv->setV(src->v());
    }
    else
    {
        const StringPair* src = static_cast<const StringPair*>(item);
        dst.add(new StringPair(*src));
    }
}


//
// Callback to clone key-value pairs from one dictionary to another. Assume the dictionaries
// are compatible. That is, key-value pairs unique in one must be unique in the other.
//
void StringDic::cloneKv(void* arg, void* item)
{
    Tree& dst = *static_cast<Tree*>(arg);
    const StringPair& src = *static_cast<const StringPair*>(item);
    dst.add(new StringPair(src));
}


//!
//! Compare to a before snapshot. Determine added/updated/removed key-value pairs. All results
//! are optional. Use a non-zero resulting dictionary to retrieve the specific difference. The
//! comparison code can compute adds and updates with one iteration, and computing removes requires
//! another iteration. So if both adds and updates are needed, they should be computed at the
//! same time.
//!
void StringDic::compare(const StringDic& b4, StringDic* added, StringDic* updated, StringDic* removed) const
{
    if (added)
    {

        // Iterate this dictionary.
        // The entries found in the before snapshot have been updated if their values differ.
        // The entries not in the before snapshot have been added.
        added->reset();
        if (updated)
        {
            updated->reset();
            void* arg[3] = {const_cast<Tree*>(&b4.tree_), &added->tree_, &updated->tree_};
            tree_.apply(findAddsAndUpdates, arg);
        }

        // Iterate this dictionary.
        // The entries not in the before snapshot have been added.
        else
        {
            void* arg[2] = {const_cast<Tree*>(&b4.tree_), &added->tree_};
            tree_.apply(findRemoves, arg);
        }
    }

    // Iterate this dictionary.
    // The entries found in the before snapshot have been updated if their values differ.
    else if (updated)
    {
        updated->reset();
        void* arg[2] = {const_cast<Tree*>(&b4.tree_), &updated->tree_};
        tree_.apply(findUpdates, arg);
    }

    // Iterate the before snapshot.
    // The entries not in this dictionary have been removed.
    if (removed)
    {
        removed->reset();
        void* arg[2] = {const_cast<Tree*>(&tree_), &removed->tree_};
        b4.tree_.apply(findRemoves, arg);
    }
}


void StringDic::copyKv(Tree& dst, StringPair* src)
{
    void* foundItem;
    if (dst.find(src, foundItem))
    {
        StringPair* foundKv = static_cast<StringPair*>(foundItem);
        foundKv->setV(src->v());
    }
    else
    {
        dst.add(new StringPair(*src));
    }
}


//
// Callback to associate key-value pairs from one dictionary in another. Also report
// if any change occurred.
//
void StringDic::copyKv(void* arg, void* item)
{
    void** p = static_cast<void**>(arg);
    Tree& tree = *static_cast<Tree*>(p[0]);
    StringPair* src = static_cast<StringPair*>(item);

    // If change already occurred, don't need to check for changes any more.
    bool* modified = static_cast<bool*>(p[1]);
    if (*modified)
    {
        copyKv(tree, src);
        return;
    }

    // If key exists, update its value. However, no-op if no change.
    void* foundItem;
    if (tree.find(src, foundItem))
    {
        StringPair* dst = static_cast<StringPair*>(foundItem);
        if (src->v() != dst->v())
        {
            dst->setV(src->v());
            *modified = true;
        }
        return;
    }

    // Clone key-value pair.
    tree.add(new StringPair(*src));
    *modified = true;
}


void StringDic::deleteKv(void* /*arg*/, void* item)
{
    const StringPair* kv = static_cast<const StringPair*>(item);
    delete kv;
}


void StringDic::doReset(DelimitedTxt& txt, char kvDelim, bool trimLines)
{
    String line;
    String k;
    String v;
    for (txt.reset(); txt.next(line, trimLines);)
    {
        size_t startAt = 0;
        size_t kLength = line.find(kvDelim, startAt);
        if (kLength != String::INVALID_INDEX)
        {
            k = String(line, startAt, kLength);
            k.trimSpace();
            v = String(line, kLength + 1, line.length() - kLength - 1);
            v.trimSpace();
            v.dequote();
            associate(k, v);
        }
        else
        {
            break;
        }
    }
}


//
// Callback to determine added/updated key-value pairs from comparing two dictionaries.
//
void StringDic::findAddsAndUpdates(void* arg, void* item)
{
    void** p = static_cast<void**>(arg);
    const Tree& b4 = *static_cast<const Tree*>(p[0]);
    Tree* added = static_cast<Tree*>(p[1]);
    Tree* updated = static_cast<Tree*>(p[2]);

    void* foundItem;
    if (b4.find(item, foundItem))
    {
        const StringPair* foundKv = static_cast<const StringPair*>(foundItem);
        const StringPair* key = static_cast<const StringPair*>(item);
        if (foundKv->v() != key->v())
        {
            updated->add(new StringPair(*key));
        }
    }
    else
    {
        const StringPair& key = *static_cast<const StringPair*>(item);
        added->add(new StringPair(key));
    }
}


//
// Callback to determine removed key-value pairs from comparing two dictionaries.
//
void StringDic::findRemoves(void* arg, void* item)
{
    void** p = static_cast<void**>(arg);
    const Tree& tree = *static_cast<const Tree*>(p[0]);
    Tree* removed = static_cast<Tree*>(p[1]);

    if (!tree.find(item))
    {
        const StringPair& key = *static_cast<const StringPair*>(item);
        removed->add(new StringPair(key));
    }
}


//
// Callback to determine updated key-value pairs from comparing two dictionaries.
//
void StringDic::findUpdates(void* arg, void* item)
{
    void** p = static_cast<void**>(arg);
    const Tree& b4 = *static_cast<const Tree*>(p[0]);
    Tree* updated = static_cast<Tree*>(p[1]);

    void* foundItem;
    if (b4.find(item, foundItem))
    {
        const StringPair* foundKv = static_cast<const StringPair*>(foundItem);
        const StringPair* key = static_cast<const StringPair*>(item);
        if (foundKv->v() != key->v())
        {
            updated->add(new StringPair(*key));
        }
    }
}


void StringDic::mergeKv(void* arg, void* item)
{
    Tree& tree = *static_cast<Tree*>(arg);
    void* foundItem;
    bool ok = tree.add(item, foundItem);
    if (!ok)
    {
        StringPair* src = static_cast<StringPair*>(item);
        StringPair* dst = static_cast<StringPair*>(foundItem);
        dst->setV(src->v());
        delete src;
    }
}


//!
//! Move key-value pairs from this into result. If there's an existing key in
//! result, update its value.
//!
void StringDic::mergeInto(StringDic& result)
{
    if ((result.tree_.numItems() == 0) && (compareK_ == result.compareK_))
    {
        result.tree_ = &tree_; //move guts into result
    }
    else
    {
        void* arg = &result.tree_;
        tree_.applyParentFirst(mergeKv, arg);
        tree_.reset();
    }
}


void StringDic::prepVec(StringVec& vec) const
{
    vec.reset();
    unsigned int minCap = tree_.numItems();
    if (vec.capacity() < minCap)
    {
        vec.resize(minCap);
    }
}


void StringDic::rmKv(void* arg, void* item)
{
    Tree& tree = *static_cast<Tree*>(arg);
    void* foundItem;
    if (tree.find(item, foundItem))
    {
        const StringPair* foundKv = static_cast<const StringPair*>(foundItem);
        const StringPair* key = static_cast<const StringPair*>(item);
        if (foundKv->v() == key->v())
        {
            tree.rm(foundItem);
            delete foundKv;
        }
    }
}


void StringDic::stringifyKv(void* arg, void* item)
{
    stringifyArg_t& r = *static_cast<stringifyArg_t*>(arg);
    String& s = *r.dicAsString;
    const StringPair& kv = *static_cast<const StringPair*>(item);
    s += kv.k();
    s += r.kvDelim;
    s += kv.v();
    s += *r.lineDelim;
}


void StringDic::vectorizeKv(void* arg, void* item)
{
    StringVec* const* vec = static_cast<StringVec* const*>(arg);
    const StringPair& kv = *static_cast<const StringPair*>(item);
    vec[0]->add(kv.k());
    vec[1]->add(kv.v());
}

END_NAMESPACE1
