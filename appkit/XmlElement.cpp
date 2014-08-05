/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/DelimitedTxt.hpp"
#include "appkit/XmlElement.hpp"
#include "appkit/XmlLexer.hpp"

using namespace syskit;

const size_t INDENT = 2;

BEGIN_NAMESPACE1(appkit)

const char XmlElement::NAME_DELIM = '/';


//!
//! Construct a no-name element.
//!
XmlElement::XmlElement():
name_()
{
    attr_ = 0;
    index_ = 0;
    kid_ = 0;
    mom_ = 0;
}


//!
//! Construct an element with given name and some initial capacities. A default
//! capacity of zero indicates no pre-allocation. A non-zero attribute capacity
//! causes some pre-allocation for at least attrCap attributes. A non-zero kid
//! capacity causes some pre-allocation for a least kidCap kids.
//!
XmlElement::XmlElement(const String& name, unsigned int attrCap, unsigned int kidCap):
name_(name)
{
    attr_ = (attrCap == 0)? 0: new Vec(attrCap, -1 /*growBy*/);
    index_ = 0;
    kid_ = (kidCap == 0)? 0: new Vec(kidCap, -1 /*growBy*/);
    mom_ = 0;
}


//!
//! Destruct element.
//! Destruct kids before destructing self.
//!
XmlElement::~XmlElement()
{
    if (kid_ != 0)
    {
        for (void* item; kid_->rmTail(item); delete static_cast<XmlElement*>(item));
        delete kid_;
    }

    if (attr_ != 0)
    {
        for (void* item; attr_->rmTail(item); delete static_cast<StringPair*>(item));
        delete attr_;
    }
}


//!
//! Return the XML form for the attributes.
//! Return an empty string if this element has no attributes.
//!
String XmlElement::attrsInXml() const
{
    String xml;
    if (attr_ != 0)
    {
        for (size_t i = 0, numAttrs = attr_->numItems(); i < numAttrs; ++i)
        {
            const StringPair* nv = static_cast<const StringPair*>(attr_->peek(i));
            xml += ' ';
            xml += nv->n();
            xml += "=\"";
            xml += XmlLexer::escape(nv->v());
            xml += '"';
        }
    }

    return xml;
}


//!
//! Form and return my full name. A full name is a normalized pathname
//! specifying the element's name and all of its ancestors. A childless
//! element has no terminating marker (e.g., "/grandma/mom/me"), and a
//! mother element has a terminating marker (e.g., "/grandma/mom/me/").
//!
String XmlElement::fullName() const
{
    String pathname(1, NAME_DELIM);
    pathname += name_;
    if (kid_ != 0)
    {
        pathname.append(1, NAME_DELIM);
    }

    String child;
    for (const XmlElement* p = mom_; p != 0; p = p->mom_)
    {
        child = pathname;
        pathname.reset(1, NAME_DELIM);
        pathname += p->name_;
        pathname += child;
    }

    return pathname;
}


//!
//! Return the XML form for this element content.
//!
String XmlElement::toXml(size_t indent) const
{

    // start.
    String xml(indent, ' ');
    xml += '<';
    xml += name_;
    xml += attrsInXml();
    xml += '>';

    // kids.
    size_t indentEnd = indent;
    if (kid_ != 0)
    {
        size_t numKids = kid_->numItems();
        if ((numKids == 1) &&
            (static_cast<const XmlElement*>(kid_->peek(0))->kid_ == 0) &&
            (static_cast<const XmlElement*>(kid_->peek(0))->name_.empty()))
        {
            xml += static_cast<const XmlElement*>(kid_->peek(0))->toXml(indent);
            indentEnd = 0;
        }
        else
        {
            for (size_t i = 0; i < numKids; ++i)
            {
                const XmlElement* kid = static_cast<const XmlElement*>(kid_->peek(i));
                xml += '\n';
                indent += INDENT;
                xml += kid->toXml(indent);
                indent -= INDENT;
            }
            xml += '\n';
        }
    }
    else
    {
        xml += '\n';
    }

    // end.
    xml.append(indentEnd, ' ');
    xml += "</";
    xml += name_;
    xml += '>';
    return xml;
}


//
// Locate given attribute. Return the located name-value pair if found.
// Return zero otherwise.
//
StringPair* XmlElement::findAttrByName(const String& n) const
{
    StringPair* found = 0;
    size_t numAttrs = (attr_ != 0)? attr_->numItems(): 0;
    for (size_t i = 0; i < numAttrs; ++i)
    {
        StringPair* nv = static_cast<StringPair*>(attr_->peek(i));
        if (nv->n() == n)
        {
            found = nv;
            break;
        }
    }

    return found;
}


//
// Locate given attribute. Return the located name-value pair if found.
// Return zero otherwise.
//
StringPair* XmlElement::findAttrByName(const char* n) const
{
    StringPair* found = 0;
    size_t numAttrs = (attr_ != 0)? attr_->numItems(): 0;
    for (size_t i = 0; i < numAttrs; ++i)
    {
        StringPair* nv = static_cast<StringPair*>(attr_->peek(i));
        if (nv->n() == n)
        {
            found = nv;
            break;
        }
    }

    return found;
}


//!
//! Return cloned element which must be deleted via the delete operator when done.
//!
XmlElement* XmlElement::clone() const
{

    // Attributes.
    XmlElement* cloned = new XmlElement(name_);
    if (attr_ != 0)
    {
        Vec* attr = new Vec(*attr_);
        for (size_t i = 0, numAttrs = attr_->numItems(); i < numAttrs; ++i)
        {
            const StringPair* nv = static_cast<const StringPair*>(attr_->peek(i));
            attr->setItem(i, new StringPair(*nv));
        }
        cloned->attr_ = attr;
    }

    // Kids.
    if (kid_ != 0)
    {
        Vec* kid = new Vec(*kid_);
        for (size_t i = 0, numKids = kid_->numItems(); i < numKids; ++i)
        {
            const XmlElement* element = static_cast<const XmlElement*>(kid_->peek(i));
            XmlElement* baby = element->clone();
            baby->index_ = i;
            baby->mom_ = cloned;
            kid->setItem(i, baby);
        }
        cloned->kid_ = kid;
    }

    // Return cloned element.
    return cloned;
}


//!
//! Detach this element from its housing document. No-op if element
//! is already detached (i.e., not held in any document). Return the
//! detached element. Caller takes over ownership of this element
//! which must be destroyed when appropriate.
//!
XmlElement* XmlElement::detach(XmlDoc& home)
{

    // Not a root element?
    if (mom_ != 0)
    {

        // Remove self from siblings.
        mom_->kid_->rmFromIndex(index_, true /*maintainOrder*/);
        const Vec* sibVec = mom_->kid_;
        size_t i = index_;
        mom_ = 0;
        index_ = 0;

        // Adjust younger siblings' indices.
        for (size_t numSibs = sibVec->numItems(); i < numSibs; ++i)
        {
            XmlElement* sib = static_cast<XmlElement*>(sibVec->peek(i));
            sib->index_ = i;
        }
    }

    // Detach root.
    else if (this == &home.root())
    {
        XmlElement* dummy;
        home.setRoot(0, dummy);
    }

    // Return the detached element.
    return this;
}


XmlElement* XmlElement::findPath(const String& path) const
{

    // Nothing found if path is empty.
    const char* s = path.ascii();
    if (s[0] == 0)
    {
        return 0;
    }

    // Start at this element if path is relative.
    // Start at root element otherwise.
    String name;
    DelimitedTxt txt(path, false /*makeCopy*/, XmlElement::NAME_DELIM);
    XmlElement* found = const_cast<XmlElement*>(this);
    if (s[0] == NAME_DELIM)
    {
        for (; found->mom_ != 0; found = found->mom_);
        txt.next(name);
        if (!txt.next(name))
        {
            return found;
        }
        if ((txt.trimLine(name) != found->name_))
        {
            return 0;
        }
    }

    // Locate element checking one level at a time.
    // Allow relative paths ("."=self and ".."=mom).
    while (txt.next(name))
    {
        txt.trimLine(name);
        found = found->findRelative(name);
        if (found == 0)
        {
            break;
        }
    }

    // Return located element.
    // Return zero if not found.
    return found;
}


XmlElement* XmlElement::findKidByName(const String& name) const
{
    XmlElement* found = 0;
    size_t numKids = (kid_ != 0)? kid_->numItems(): 0;
    for (size_t i = 0; i < numKids; ++i)
    {
        XmlElement* kid = static_cast<XmlElement*>(kid_->peek(i));
        if (kid->name_ == name)
        {
            found = kid;
            break;
        }
    }

    return found;
}


XmlElement* XmlElement::findKidByName(const char* name) const
{
    XmlElement* found = 0;
    size_t numKids = (kid_ != 0)? kid_->numItems(): 0;
    for (size_t i = 0; i < numKids; ++i)
    {
        XmlElement* kid = static_cast<XmlElement*>(kid_->peek(i));
        if (kid->name_ == name)
        {
            found = kid;
            break;
        }
    }

    return found;
}


//
// Locate kid element.
// Allow relative paths ("."=self and ".."=mom).
//
XmlElement* XmlElement::findRelative(const String& name) const
{
    const char* s = name.ascii();
    if (s[0] == '.')
    {
        if (s[1] == 0)
        {
            return const_cast<XmlElement*>(this);
        }
        if ((s[1] == '.') && (s[2] == 0))
        {
            return mom_;
        }
    }

    return findKidByName(name);
}


XmlElement* XmlElement::findSibByName(const String& name) const
{
    XmlElement* found = 0;
    if (mom_ != 0)
    {
        const Vec* sibVec = mom_->kid_;
        for (size_t i = 0, numSibs = sibVec->numItems(); i < numSibs; ++i)
        {
            if (i != index_)
            {
                XmlElement* sib = static_cast<XmlElement*>(sibVec->peek(i));
                if (sib->name_ == name)
                {
                    found = sib;
                    break;
                }
            }
        }
    }

    return found;
}


XmlElement* XmlElement::findSibByName(const char* name) const
{
    XmlElement* found = 0;
    if (mom_ != 0)
    {
        const Vec* sibVec = mom_->kid_;
        for (size_t i = 0, numSibs = sibVec->numItems(); i < numSibs; ++i)
        {
            if (i != index_)
            {
                XmlElement* sib = static_cast<XmlElement*>(sibVec->peek(i));
                if (sib->name_ == name)
                {
                    found = sib;
                    break;
                }
            }
        }
    }

    return found;
}


XmlElement* XmlElement::getNextSib() const
{
    XmlElement* next = 0;
    if (mom_ != 0)
    {
        const Vec* sib = mom_->kid_;
        size_t index = index_ + 1;
        if (sib->numItems() > index)
        {
            next = static_cast<XmlElement*>(sib->peek(index));
        }
    }

    return next;
}


//!
//! Recursively apply callback to descendent elements including self. Children
//! first. Younger siblings first. The callback should return true to continue
//! iterating and should return false to abort iterating. Return false if the
//! callback aborted the iterating. Return true otherwise.
//!
bool XmlElement::applyChildFirst(cb0_t cb, void* arg)
{
    if (kid_ != 0)
    {
        for (size_t i = kid_->numItems(); i > 0;)
        {
            XmlElement* kid = static_cast<XmlElement*>(kid_->peek(--i));
            bool keepGoing = kid->applyChildFirst(cb, arg);
            if (!keepGoing)
            {
                return keepGoing;
            }
        }
    }

    bool keepGoing = cb(arg, *this);
    return keepGoing;
}


//!
//! Recursively apply callback to descendent elements including self. Children
//! first. Younger siblings first. The callback should return true to continue
//! iterating and should return false to abort iterating. Return false if the
//! callback aborted the iterating. Return true otherwise.
//!
bool XmlElement::applyChildFirst(cb1_t cb, void* arg) const
{
    if (kid_ != 0)
    {
        for (size_t i = kid_->numItems(); i > 0;)
        {
            const XmlElement* kid = static_cast<const XmlElement*>(kid_->peek(--i));
            bool keepGoing = kid->applyChildFirst(cb, arg);
            if (!keepGoing)
            {
                return keepGoing;
            }
        }
    }

    bool keepGoing = cb(arg, *this);
    return keepGoing;
}


//!
//! Recursively apply callback to descendent elements including self. Parent
//! first. The callback should return true to continue iterating and should
//! return false to abort iterating. Return false if the callback aborted the
//! iterating. Return true otherwise.
//!
bool XmlElement::applyParentFirst(cb0_t cb, void* arg)
{
    bool keepGoing = cb(arg, *this);
    if (!keepGoing)
    {
        return keepGoing;
    }

    if (kid_ != 0)
    {
        for (size_t i = 0, numKids = kid_->numItems(); i < numKids; ++i)
        {
            XmlElement* kid = static_cast<XmlElement*>(kid_->peek(i));
            keepGoing = kid->applyParentFirst(cb, arg);
            if (!keepGoing)
            {
                return keepGoing;
            }
        }
    }

    return keepGoing;
}


//!
//! Recursively apply callback to descendent elements including self. Parent
//! first. The callback should return true to continue iterating and should
//! return false to abort iterating. Return false if the callback aborted the
//! iterating. Return true otherwise.
//!
bool XmlElement::applyParentFirst(cb1_t cb, void* arg) const
{
    bool keepGoing = cb(arg, *this);
    if (!keepGoing)
    {
        return keepGoing;
    }

    if (kid_ != 0)
    {
        for (size_t i = 0, numKids = kid_->numItems(); i < numKids; ++i)
        {
            const XmlElement* kid = static_cast<const XmlElement*>(kid_->peek(i));
            keepGoing = kid->applyParentFirst(cb, arg);
            if (!keepGoing)
            {
                return keepGoing;
            }
        }
    }

    return keepGoing;
}


//!
//! Make baby a child of this element. Take over ownership of the given object which
//! will be destroyed by the element when appropriate. Return true if successful.
//!
bool XmlElement::giveBirth(XmlElement* baby)
{
    if (kid_ == 0)
    {
        kid_ = new Vec(Vec::DefaultCap, -1 /*growBy*/);
    }

    baby->index_ = kid_->numItems();
    baby->mom_ = this;
    kid_->add(baby);
    bool ok = true;
    return ok;
}


//!
//! Return true if element is unknown.
//!
bool XmlElement::isUnknown() const
{
    bool unknown = false;
    return unknown;
}


//!
//! Return the body part of the element. Return an empty string if the element
//! does not have a body. As an exception, if the element does not have a body,
//! but its first kid does, the first kid's body is returned. For example, for
//! this XML stream: "<no-body><a></a><content>body</content></no-body>", the
//! body part is empty for the "no-body" element, and the body part is "body"
//! for the "content" element.
//!
const String& XmlElement::body() const
{
    if (kid_ != 0)
    {
        const XmlElement* kid = static_cast<const XmlElement*>(kid_->peek(0));
        if (kid->kid_ == 0)
        {
            return kid->body();
        }
    }

    static String s_body;
    return s_body;
}


//!
//! Update the body part, if any. As an exception, if the element does not have a body,
//! but its first kid does, the first kid's body is updated.
//!
void XmlElement::setBody(const String& body)
{
    if (kid_ != 0)
    {
        XmlElement* kid = static_cast<XmlElement*>(kid_->peek(0));
        if (kid->kid_ == 0)
        {
            kid->setBody(body);
        }
    }
}

END_NAMESPACE1
