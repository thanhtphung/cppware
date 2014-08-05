/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/MappedTxtFile.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/DicFile.hpp"
#include "appkit/StringDic.hpp"
#include "appkit/XmlDoc.hpp"

using namespace syskit;

const char DOT = '.';
const char SLASH = '/';

BEGIN_NAMESPACE1(appkit)


DicFile::DicFile(const String& dicPath, bool ignoreCase)
{
    dicPath_ = new String(dicPath);
    doc_ = new XmlDoc(*dicPath_);
    ignoreCase_ = ignoreCase;
    ok_ = doc_->isOk();
}


DicFile::~DicFile()
{
    delete doc_;
    delete dicPath_;
}


//!
//! Load dictionary file into a dictionary. Return an empty dictionary if unsuccessful or if
//! already loaded once. The returned dictionary is to be freed by the caller via the delete
//! operator when done.
//!
StringDic* DicFile::load() const
{

    // Allow loading once only. Free the xml resource after the first load.
    StringDic* dic;
    if (doc_ != 0)
    {
        dic = loadDic();
        delete doc_;
        doc_ = 0;
    }
    else
    {
        dic = 0;
    }

    return dic? dic: new StringDic(ignoreCase_);
}


//
// Load dictionary file into a dictionary. Return zero if unsuccessful or if already loaded
// once. The returned dictionary is to be freed by the caller via the delete operator when
// done.
//
StringDic* DicFile::loadDic() const
{
    const XmlElement& root = doc_->root();
    String name("Dictionary");
    const XmlElement* e = root.findKid(name);
    if (e == 0)
    {
        return 0;
    }

    // Populate dictionary with contents from file.
    StringDic* dic = new StringDic(ignoreCase_);
    size_t prefixLength = e->fullName().length();
    void* arg[2] = {dic, &prefixLength};
    e->applyParentFirst(loadPair, arg);

    return dic;
}


bool DicFile::loadPair(StringDic& dic, const XmlElement& e, const String& k)
{

    // Dictionarize this element body, if it's non-empty.
    const String& body = e.body();
    if (!body.empty())
    {
        dic.add(k, body);
    }

    // Also convert its attributes into dictionary entries, if any.
    String attrK;
    for (size_t i = 0, numAttrs = e.numAttrs(); i < numAttrs; ++i)
    {
        const StringPair& attr = e.attr(i);
        attrK = k + DOT + attr.k();
        dic.add(attrK, attr.v());
    }

    bool keepGoing = true;
    return keepGoing;
}


//
// Callback to dictionarize XML elements. If an element has a name and a body, make
// it a dictionary entry using the body as the value part. For the key part, use the
// XML full name but ignore the prefix containing the dictionary root. For example,
// the effective key of "/xxx/Dictionary/abc" is "abc", and the effective key of
// "/xxx/Dictionary/a/b/c" is "a/b/c". Also convert element's attributes into
// dictionary entries. Append attribute name to its dotted element name to form
// corresponding dictionary key. For example, the effective key of attribute n for
// element abc is "abc.n". If element or attribute names are duplicated, only the
// first one is significant.
//
bool DicFile::loadPair(void* arg, const XmlElement& e)
{

    // Only interested in named elements.
    if (e.name().empty())
    {
        bool keepGoing = true;
        return keepGoing;
    }

    // Only interested in descendent elements below the /xxx/Dictionary root.
    void** p = static_cast<void**>(arg);
    size_t prefixLength = *static_cast<const size_t*>(p[1]);
    String k(e.fullName().substr(prefixLength));
    if (k.empty())
    {
        bool keepGoing = true;
        return keepGoing;
    }

    // The terminating slash from the full name needs to be removed.
    if (k.ascii()[k.byteSize() - 2] == SLASH)
    {
        k.truncate(k.length() - 1);
    }

    StringDic* dic = static_cast<StringDic*>(p[0]);
    bool keepGoing = loadPair(*dic, e, k);
    return keepGoing;
}


bool DicFile::save(const StringDic& dic)
{

    // Allow saving once only. Free the xml resource after the first save.
    bool ok;
    if (doc_ != 0)
    {
        ok = saveDic(dic);
        delete doc_;
        doc_ = 0;
    }
    else
    {
        ok = false;
    }

    return ok;
}


bool DicFile::saveDic(const StringDic& dic)
{
    XmlElement& root = doc_->root();
    String name("Dictionary");
    XmlElement* e = root.findKid(name);
    if (e == 0)
    {
        bool ok = false;
        return ok;
    }

    // Update elements with given dictionary.
    size_t prefixLength = e->fullName().length();
    bool modified = false;
    StringDic processedKeys;
    void* arg[4] = {const_cast<StringDic*>(&dic), &prefixLength, &modified, &processedKeys};
    e->applyParentFirst(savePair, arg);

    // Update file if modified.
    bool ok = modified? saveFile(): true;
    return ok;
}


bool DicFile::saveFile() const
{

    // Assume no byte-order-marker.
    // Give up if cannot access destination path.
    String xmlOut(doc_->toXml());
    unsigned long long fileSize = xmlOut.byteSize() - 1;
    bool failIfExists = false;
    Bom bom(Bom::None);
    MappedTxtFile file(dicPath_->widen(), fileSize, failIfExists, bom);
    bool ok = file.isOk();
    if (!ok)
    {
        return ok;
    }

    // Assumption is confirmed.
    unsigned int size = static_cast<unsigned int>(fileSize);
    bom = Bom::decode(file.image(), size);
    if (bom == Bom::None)
    {
        memcpy(file.image(), xmlOut.raw(), size);
        ok = true;
    }

    // Assumption is incorrect.
    else
    {
        bool addBom = true;
        unsigned char* raw = xmlOut.formUtfX(size, bom, addBom);
        ok = file.resize(size) && memcpy(file.image(), raw, size);
        delete[] raw;
    }

    return ok;
}


bool DicFile::savePair(StringDic& processedKeys, XmlElement& e, const String& k, const StringDic& dic)
{

    // Update this element body to match the dictionary entry.
    bool elementUpdated = false;
    const String& body = e.body();
    String empty;
    if ((!body.empty()) && processedKeys.add(k, empty))
    {
        const StringPair* kv = dic.getKv(k);
        if (kv && (kv->v() != body))
        {
            e.setBody(kv->v());
            elementUpdated = true;
        }
    }

    // Also update its attributes to match the dictionary entries.
    String attrK;
    for (size_t i = 0, numAttrs = e.numAttrs(); i < numAttrs; ++i)
    {
        StringPair& attr = e.attr(i);
        attrK = k + DOT + attr.k();
        if (processedKeys.add(attrK, empty))
        {
            const StringPair* kv = dic.getKv(attrK);
            if (kv && (kv->v() != attr.v()))
            {
                attr.setV(kv->v());
                elementUpdated = true;
            }
        }
    }

    return elementUpdated;
}


//
// Callback to serialize dictionary entries. If an element has a name and a body, it
// should be a dictionary entry using the body as the value part. For the key part,
// use the XML full name but ignore the prefix containing the dictionary root. For
// example, the effective key of "/xxx/Dictionary/abc" is "abc", and the effective
// key of "/xxx/Dictionary/a/b/c" is "a/b/c". Element attributes should also be
// dictionary entries. Append attribute name to its dotted element name to form
// corresponding dictionary key. For example, the effective key of attribute n for
// element abc is "abc.n". If element or attribute names are duplicated, only the
// first one is significant.
//
bool DicFile::savePair(void* arg, XmlElement& e)
{

    // Only interested in named elements.
    if (e.name().empty())
    {
        bool keepGoing = true;
        return keepGoing;
    }

    // Only interested in descendent elements below the /xxx/Dictionary root.
    void** p = static_cast<void**>(arg);
    size_t prefixLength = *static_cast<const size_t*>(p[1]);
    String k(e.fullName().substr(prefixLength));
    if (k.empty())
    {
        bool keepGoing = true;
        return keepGoing;
    }

    // The terminating slash from the full name needs to be removed.
    if (k.ascii()[k.byteSize() - 2] == SLASH)
    {
        k.truncate(k.length() - 1);
    }

    StringDic* processedKeys = static_cast<StringDic*>(p[3]);
    const StringDic* dic = static_cast<const StringDic*>(p[0]);
    bool elementUpdated = savePair(*processedKeys, e, k, *dic);
    if (elementUpdated)
    {
        bool* modified = static_cast<bool*>(p[2]);
        *modified = true;
    }

    bool keepGoing = true;
    return keepGoing;
}


const String& DicFile::dicPath() const
{
    return *dicPath_;
}


//!
//! Return zero if instance was successfully constructed.
//! Return an error description otherwise.
//!
const char* DicFile::errDesc() const
{
    return ((!ok_) && (doc_ != 0))? (doc_->errDesc()): (0);
}

END_NAMESPACE1
