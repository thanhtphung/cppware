/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_XML_ELEMENT_HPP
#define APPKIT_XML_ELEMENT_HPP

#include <ostream>
#include "appkit/String.hpp"
#include "syskit/macros.h"

DECLARE_CLASS1(syskit, Vec)

BEGIN_NAMESPACE1(appkit)

class StringPair;
class XmlDoc;


//! xml element
class XmlElement
    //!
    //! XML element. An XML document contains an optional prolog and one root
    //! element. An element can have kids. A typical element has a name and
    //! zero or more attributes. Some element does not have a name but only a
    //! body part (e.g., a comment does not have a name).
    //!
{

public:
    typedef bool(*cb0_t)(void* arg, XmlElement& element);
    typedef bool(*cb1_t)(void* arg, const XmlElement& element);

    static const char NAME_DELIM;

    XmlElement(const String& name, unsigned int attrCap = 0, unsigned int kidCap = 0);
    static void operator delete(void* p, size_t size);
    static void operator delete(void* p, void* buf);
    static void* operator new(size_t size);
    static void* operator new(size_t size, void* buf);
    String fullName() const;
    const String& name() const;

    // Attributes.
    StringPair& attr(size_t index);
    StringPair* findAttr(const String& n);
    StringPair* findAttr(const char* n);
    bool addAttr(const StringPair* nv);
    bool setAttr(size_t index, const StringPair* nv);
    const String& attrN(size_t index) const;
    const String& attrV(size_t index) const;
    const StringPair& attr(size_t index) const;
    const StringPair* findAttr(const String& n) const;
    const StringPair* findAttr(const char* n) const;
    unsigned int numAttrs() const;
    void freezeAttrs();

    // Attributes, variants supporting primitive data types.
    String getAttr(const String& n) const;
    String getAttr(const String& n, const String& defaultV) const;
    String getAttr(const char* n) const;
    String getAttr(const char* n, const String& defaultV) const;
    bool getAttrAsBool(const String& n, bool defaultV = false) const;
    bool getAttrAsBool(const char* n, bool defaultV = false) const;
    double getAttrAsD64(const String& n, double defaultV = 0.0) const;
    double getAttrAsD64(const char* n, double defaultV = 0.0) const;
    float getAttrAsF32(const String& n, float defaultV = 0.0f) const;
    float getAttrAsF32(const char* n, float defaultV = 0.0f) const;
    int getAttrAsS32(const String& n, int defaultV = 0) const;
    int getAttrAsS32(const char* n, int defaultV = 0) const;
    unsigned int getAttrAsU32(const String& n, unsigned int defaultV = 0) const;
    unsigned int getAttrAsU32(const char* n, unsigned int defaultV = 0) const;
    unsigned long long getAttrAsU64(const String& n, unsigned long long defaultV = 0) const;
    unsigned long long getAttrAsU64(const char* n, unsigned long long defaultV = 0) const;
    unsigned short getAttrAsU16(const String& n, unsigned short defaultV = 0) const;
    unsigned short getAttrAsU16(const char* n, unsigned short defaultV = 0) const;

    // Links.
    XmlElement& kid(size_t index);
    XmlElement* detach(XmlDoc& home);
    XmlElement* find(const String& path);
    XmlElement* findKid(const String& name);
    XmlElement* findKid(const char* name);
    XmlElement* findSib(const String& name);
    XmlElement* findSib(const char* name);
    XmlElement* mom();
    XmlElement* nextSib();
    XmlElement* prevSib();
    bool applyChildFirst(cb0_t cb, void* arg = 0);
    bool applyChildFirst(cb1_t cb, void* arg = 0) const;
    bool applyParentFirst(cb0_t cb, void* arg = 0);
    bool applyParentFirst(cb1_t cb, void* arg = 0) const;
    bool setKid(size_t index, const XmlElement* kid);
    const XmlElement& kid(size_t index) const;
    const XmlElement* find(const String& path) const;
    const XmlElement* findKid(const String& name) const;
    const XmlElement* findKid(const char* name) const;
    const XmlElement* findSib(const String& name) const;
    const XmlElement* findSib(const char* name) const;
    const XmlElement* mom() const;
    const XmlElement* nextSib() const;
    const XmlElement* prevSib() const;
    unsigned int numKids() const;
    unsigned int numSibs() const;
    void sterilize();

    virtual ~XmlElement();
    virtual String toXml(size_t indent = 0) const;
    virtual XmlElement* clone() const;
    virtual bool giveBirth(XmlElement* baby);
    virtual bool isUnknown() const;
    virtual const String& body() const;
    virtual void setBody(const String& body);

protected:
    XmlElement();
    String attrsInXml() const;

private:
    String name_;
    syskit::Vec* attr_;
    syskit::Vec* kid_;
    XmlElement* mom_;
    size_t index_;

    XmlElement(const XmlElement&); //prohibit usage
    const XmlElement& operator =(const XmlElement&); //prohibit usage

    StringPair* findAttrByName(const String&) const;
    StringPair* findAttrByName(const char*) const;
    XmlElement* findKidByName(const String&) const;
    XmlElement* findKidByName(const char*) const;
    XmlElement* findPath(const String&) const;
    XmlElement* findRelative(const String&) const;
    XmlElement* findSibByName(const String&) const;
    XmlElement* findSibByName(const char*) const;
    XmlElement* getNextSib() const;
    XmlElement* getPrevSib() const;

};

//! Append the XML form of the element to the given output stream. Return
//! the resulting output stream.
inline std::ostream& operator <<(std::ostream& os, const XmlElement& element)
{
    String xml(element.toXml());
    return os.write(xml.ascii(), static_cast<std::streamsize>(xml.length()));
}

END_NAMESPACE1

#include "appkit/StringPair.hpp"
#include "syskit/BufPool.hpp"
#include "syskit/Vec.hpp"

BEGIN_NAMESPACE1(appkit)

inline void XmlElement::operator delete(void* p, size_t size)
{
    syskit::BufPool::freeBuf(p, size);
}

inline void XmlElement::operator delete(void* /*p*/, void* /*buf*/)
{
}

inline void* XmlElement::operator new(size_t size)
{
    void* buf = syskit::BufPool::allocateBuf(size);
    return buf;
}

inline void* XmlElement::operator new(size_t /*size*/, void* buf)
{
    return buf;
}

//! Return associated value of given attribute. Associated value of a non-existent
//! attribute is an empty string.
inline String XmlElement::getAttr(const String& n) const
{
    const StringPair* nv = findAttrByName(n);
    return (nv != 0)? nv->v(): String();
}

//! Return associated value of given attribute. Associated value of a non-existent
//! attribute is the given default value.
inline String XmlElement::getAttr(const String& n, const String& defaultV) const
{
    const StringPair* nv = findAttrByName(n);
    return (nv != 0)? nv->v(): defaultV;
}

//! Return associated value of given attribute. Associated value of a non-existent
//! attribute is an empty string.
inline String XmlElement::getAttr(const char* n) const
{
    const StringPair* nv = findAttrByName(n);
    return (nv != 0)? nv->v(): String();
}

//! Return associated value of given attribute. Associated value of a non-existent
//! attribute is the given default value.
inline String XmlElement::getAttr(const char* n, const String& defaultV) const
{
    const StringPair* nv = findAttrByName(n);
    return (nv != 0)? nv->v(): defaultV;
}

//! Return the name-value pair for the attribute residing at given index.
inline StringPair& XmlElement::attr(size_t index)
{
    StringPair* nv = static_cast<StringPair*>(attr_->peek(index));
    return *nv;
}

//! Locate given attribute. Return the located name-value pair if found.
//! Return zero otherwise.
inline StringPair* XmlElement::findAttr(const String& n)
{
    return findAttrByName(n);
}

//! Locate given attribute. Return the located name-value pair if found.
//! Return zero otherwise.
inline StringPair* XmlElement::findAttr(const char* n)
{
    return findAttrByName(n);
}

//! Return the kid residing at given index.
inline XmlElement& XmlElement::kid(size_t index)
{
    XmlElement* kid = static_cast<XmlElement*>(kid_->peek(index));
    return *kid;
}

//! Locate an element given its path (e.g., "./kid/grandkid", "kid/grandkid",
//! "../sib", "../ant/cousin", "/grandma/mom/me", "kid"). Return located
//! element. Return zero if not found.
inline XmlElement* XmlElement::find(const String& path)
{
    return findPath(path);
}

//! Locate given kid. Return the located kid if found. Return zero otherwise.
inline XmlElement* XmlElement::findKid(const String& name)
{
    return findKidByName(name);
}

//! Locate given kid. Return the located kid if found. Return zero otherwise.
inline XmlElement* XmlElement::findKid(const char* name)
{
    return findKidByName(name);
}

//! Locate given sibling. Return the located sibling if found. Return zero otherwise.
inline XmlElement* XmlElement::findSib(const String& name)
{
    return findSibByName(name);
}

//! Locate given sibling. Return the located sibling if found. Return zero otherwise.
inline XmlElement* XmlElement::findSib(const char* name)
{
    return findSibByName(name);
}

inline XmlElement* XmlElement::getPrevSib() const
{
    return ((mom_ != 0) && (index_ > 0))? static_cast<XmlElement*>(mom_->kid_->peek(index_ - 1)): 0;
}

//! Return the mom of this element.
//! Return zero if this is the root element in the document.
inline XmlElement* XmlElement::mom()
{
    return mom_;
}

//! Return the next sibling of this element.
//! Return zero if none.
inline XmlElement* XmlElement::nextSib()
{
    return getNextSib();
}

//! Return the previous sibling of this element.
//! Return zero if none.
inline XmlElement* XmlElement::prevSib()
{
    return getPrevSib();
}

//! Add given attribute to this element. Take over ownership of the attribute
//! and destroy it via the delete operator when the element is destructed.
inline bool XmlElement::addAttr(const StringPair* nv)
{
    if (attr_ == 0) attr_ = new syskit::Vec(syskit::Vec::DefaultCap, -1 /*growBy*/);
    return attr_->add(const_cast<StringPair*>(nv));
}

//! Return associated value of given attribute. Associated value of a non-existent
//! attribute is the given default value.
inline bool XmlElement::getAttrAsBool(const String& n, bool defaultV) const
{
    const StringPair* nv = findAttrByName(n);
    return (nv != 0)? nv->vAsBool(): defaultV;
}

//! Return associated value of given attribute. Associated value of a non-existent
//! attribute is the given default value.
inline bool XmlElement::getAttrAsBool(const char* n, bool defaultV) const
{
    const StringPair* nv = findAttrByName(n);
    return (nv != 0)? nv->vAsBool(): defaultV;
}

//! Replace the attribute residing at given index. Take over ownership of the given
//! object and destroy it via the delete operator when the element is destructed.
//! Return true if successful. Return false otherwise (i.e., invalid index).
inline bool XmlElement::setAttr(size_t index, const StringPair* nv)
{
    if ((attr_ == 0) || (index >= attr_->numItems())) return false;
    delete static_cast<const StringPair*>(attr_->peek(index));
    attr_->setItem(index, const_cast<StringPair*>(nv));
    return true;
}

//! Return the element's name.
inline const String& XmlElement::name() const
{
    return name_;
}

//! Return the name of the attribute residing at given index.
inline const String& XmlElement::attrN(size_t index) const
{
    const StringPair* nv = static_cast<const StringPair*>(attr_->peek(index));
    return nv->n();
}

//! Return the value of the attribute residing at given index.
inline const String& XmlElement::attrV(size_t index) const
{
    const StringPair* nv = static_cast<const StringPair*>(attr_->peek(index));
    return nv->v();
}

//! Return the name-value pair for the attribute residing at given index.
inline const StringPair& XmlElement::attr(size_t index) const
{
    const StringPair* nv = static_cast<const StringPair*>(attr_->peek(index));
    return *nv;
}

//! Locate given attribute. Return the located name-value pair if found.
//! Return zero otherwise.
inline const StringPair* XmlElement::findAttr(const String& n) const
{
    return findAttrByName(n);
}

//! Locate given attribute. Return the located name-value pair if found.
//! Return zero otherwise.
inline const StringPair* XmlElement::findAttr(const char* n) const
{
    return findAttrByName(n);
}

//! Return the kid residing at given index.
inline const XmlElement& XmlElement::kid(size_t index) const
{
    const XmlElement* kid = static_cast<const XmlElement*>(kid_->peek(index));
    return *kid;
}

//! Locate an element given its path (e.g., "./kid/grandkid", "kid/grandkid",
//! "../sib", "../ant/cousin", "/grandma/mom/me", "kid"). Return located
//! element. Return zero if not found.
inline const XmlElement* XmlElement::find(const String& path) const
{
    return findPath(path);
}

//! Replace the kid residing at given index. Take over ownership of the given
//! object and destroy it via the delete operator when the element is destructed.
//! Return true if successful. Return false otherwise (i.e., invalid index).
inline bool XmlElement::setKid(size_t index, const XmlElement* kid)
{
    if ((kid_ == 0) || (index >= kid_->numItems())) return false;
    delete static_cast<XmlElement*>(kid_->peek(index));
    kid_->setItem(index, const_cast<XmlElement*>(kid));
    return true;
}

//! Locate given kid. Return the located kid if found. Return zero otherwise.
inline const XmlElement* XmlElement::findKid(const String& name) const
{
    return findKidByName(name);
}

//! Locate given kid. Return the located kid if found. Return zero otherwise.
inline const XmlElement* XmlElement::findKid(const char* name) const
{
    return findKidByName(name);
}

//! Locate given sibling. Return the located sibling if found. Return zero otherwise.
inline const XmlElement* XmlElement::findSib(const String& name) const
{
    return findSibByName(name);
}

//! Locate given sibling. Return the located sibling if found. Return zero otherwise.
inline const XmlElement* XmlElement::findSib(const char* name) const
{
    return findSibByName(name);
}

//! Return the mom of this element.
//! Return zero if this is the root element in the document.
inline const XmlElement* XmlElement::mom() const
{
    return mom_;
}

//! Return the next sibling of this element.
//! Return zero if none.
inline const XmlElement* XmlElement::nextSib() const
{
    return getNextSib();
}

//! Return the previous sibling of this element.
//! Return zero if none.
inline const XmlElement* XmlElement::prevSib() const
{
    return getPrevSib();
}

//! Return associated value of given attribute. Associated value of a non-existent
//! attribute is the given default value.
inline double XmlElement::getAttrAsD64(const String& n, double defaultV) const
{
    const StringPair* nv = findAttrByName(n);
    return (nv != 0)? nv->vAsD64(): defaultV;
}

//! Return associated value of given attribute. Associated value of a non-existent
//! attribute is the given default value.
inline double XmlElement::getAttrAsD64(const char* n, double defaultV) const
{
    const StringPair* nv = findAttrByName(n);
    return (nv != 0)? nv->vAsD64(): defaultV;
}

//! Return associated value of given attribute. Associated value of a non-existent
//! attribute is the given default value.
inline float XmlElement::getAttrAsF32(const String& n, float defaultV) const
{
    const StringPair* nv = findAttrByName(n);
    return (nv != 0)? nv->vAsF32(): defaultV;
}

//! Return associated value of given attribute. Associated value of a non-existent
//! attribute is the given default value.
inline float XmlElement::getAttrAsF32(const char* n, float defaultV) const
{
    const StringPair* nv = findAttrByName(n);
    return (nv != 0)? nv->vAsF32(): defaultV;
}

//! Return associated value of given attribute. Associated value of a non-existent
//! attribute is the given default value.
inline int XmlElement::getAttrAsS32(const String& n, int defaultV) const
{
    const StringPair* nv = findAttrByName(n);
    return (nv != 0)? nv->vAsS32(): defaultV;
}

//! Return associated value of given attribute. Associated value of a non-existent
//! attribute is the given default value.
inline int XmlElement::getAttrAsS32(const char* n, int defaultV) const
{
    const StringPair* nv = findAttrByName(n);
    return (nv != 0)? nv->vAsS32(): defaultV;
}

//! Return associated value of given attribute. Associated value of a non-existent
//! attribute is the given default value.
inline unsigned int XmlElement::getAttrAsU32(const String& n, unsigned int defaultV) const
{
    const StringPair* nv = findAttrByName(n);
    return (nv != 0)? nv->vAsU32(): defaultV;
}

//! Return associated value of given attribute. Associated value of a non-existent
//! attribute is the given default value.
inline unsigned int XmlElement::getAttrAsU32(const char* n, unsigned int defaultV) const
{
    const StringPair* nv = findAttrByName(n);
    return (nv != 0)? nv->vAsU32(): defaultV;
}

//! Return the number of attributes specified in this element.
inline unsigned int XmlElement::numAttrs() const
{
    return (attr_ != 0)? attr_->numItems(): 0;
}

//! Return the number of kids in this element.
inline unsigned int XmlElement::numKids() const
{
    return (kid_ != 0)? kid_->numItems(): 0;
}

//! Return the number of siblings this kid has.
inline unsigned int XmlElement::numSibs() const
{
    return (mom_ != 0)? (mom_->kid_->numItems() - 1): (0);
}

//! Return associated value of given attribute. Associated value of a non-existent
//! attribute is the given default value.
inline unsigned long long XmlElement::getAttrAsU64(const String& n, unsigned long long defaultV) const
{
    const StringPair* nv = findAttrByName(n);
    return (nv != 0)? nv->vAsU64(): defaultV;
}

//! Return associated value of given attribute. Associated value of a non-existent
//! attribute is the given default value.
inline unsigned long long XmlElement::getAttrAsU64(const char* n, unsigned long long defaultV) const
{
    const StringPair* nv = findAttrByName(n);
    return (nv != 0)? nv->vAsU64(): defaultV;
}

//! Return associated value of given attribute. Associated value of a non-existent
//! attribute is the given default value.
inline unsigned short XmlElement::getAttrAsU16(const String& n, unsigned short defaultV) const
{
    const StringPair* nv = findAttrByName(n);
    return (nv != 0)? nv->vAsU16(): defaultV;
}

//! Return associated value of given attribute. Associated value of a non-existent
//! attribute is the given default value.
inline unsigned short XmlElement::getAttrAsU16(const char* n, unsigned short defaultV) const
{
    const StringPair* nv = findAttrByName(n);
    return (nv != 0)? nv->vAsU16(): defaultV;
}

//! Assume no more attributes will be added to the element and clean up the
//! internals to minimize memory use. Still, more attributes can be added to
//! the element if necessary.
inline void XmlElement::freezeAttrs()
{
    if (attr_ != 0) attr_->resize(attr_->numItems());
}

//! Sterilize element, but make it reversible. That is, assume no more kids will
//! be added to the element and clean up the internals to minimize memory use.
//! Still, more kids can be added to the element if necessary.
inline void XmlElement::sterilize()
{
    if (kid_ != 0) kid_->resize(kid_->numItems());
}

END_NAMESPACE1

#endif
