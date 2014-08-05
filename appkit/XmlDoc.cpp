/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/MappedTxtFile.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/DelimitedTxt.hpp"
#include "appkit/XmlDoc.hpp"
#include "appkit/XmlLexer.hpp"

using namespace syskit;

BEGIN_NAMESPACE1(appkit)


//!
//! Construct document with given prolog and document root. Constructed
//! document takes over ownership of the given objects which will be
//! destroyed via the delete operator when appropriate. Zero values for
//! prolog and root are allowed and either attribute can be reset afterwards.
//!
XmlDoc::XmlDoc(XmlProlog* prolog, XmlElement* root)
{
    errDesc_ = 0;
    prolog_ = (prolog != 0)? prolog: new XmlProlog;
    root_ = (root != 0)? root: new UnknownElement(String());
}


//!
//! Construct document from given XML file. Construction can fail if the
//! given path cannot be interpreted as an XML file. Use isOk() to determine
//! if the construction is successful. Use errDesc() for more info in case
//! of failure.
//!
XmlDoc::XmlDoc(const String& path)
{
    bool readOnly = true;
    bool skipBom = true;
    MappedTxtFile file(path.widen(), readOnly, skipBom);
    construct(file);
}


//!
//! Construct document from given XML file. Construction can fail if the
//! given path cannot be interpreted as an XML file. Use isOk() to determine
//! if the construction is successful. Use errDesc() for more info in case
//! of failure.
//!
XmlDoc::XmlDoc(const char* path)
{
    String s(path);
    bool readOnly = true;
    bool skipBom = true;
    MappedTxtFile file(s.widen(), readOnly, skipBom);
    construct(file);
}


//!
//! Construct document from given XML stream. Construction can fail if the
//! given stream cannot be interpreted as an XML stream. Use isOk() to determine
//! if the construction is successful. Use errDesc() for more info in case
//! of failure.
//!
XmlDoc::XmlDoc(const std::istream& is)
{
    construct(is);
}


XmlDoc::~XmlDoc()
{
    delete root_;
    delete prolog_;
}


//!
//! Return the XML form for this document.
//!
String XmlDoc::toXml() const
{
    String xml(prolog_->toXml());
    size_t prologLength = xml.length();
    if (prologLength > 0)
    {
        xml += '\n';
    }

    xml += root_->toXml();
    if (xml.length() > prologLength)
    {
        xml += '\n';
    }

    return xml;
}


//!
//! Locate descendent given the full name (e.g., "/grandma/mom/me"). Return
//! located descendent. Return zero if not found.
//!
XmlElement* XmlDoc::findByName(const String& fullName) const
{

    // Not a full name?
    const char* line;
    size_t length;
    DelimitedTxt txt(fullName, false /*makeCopy*/, XmlElement::NAME_DELIM);
    if (!txt.next(line, length))
    {
        return 0;
    }

    // Mismatched root?
    String name;
    if ((!txt.next(name)) || (txt.trimLine(name) != root_->name()))
    {
        return 0;
    }

    // Find descendent. Start from root. One generation at a time.
    XmlElement* found = root_;
    while (txt.next(name))
    {
        txt.trimLine(name);
        found = found->findKid(name);
        if (found == 0)
        {
            break;
        }
    }

    return found;
}


//!
//! Reset instance with given XML file. Return true if successful.
//! Use errDesc() for more info in case of failure.
//!
bool XmlDoc::loadFrom(const String& path)
{
    delete root_;
    delete prolog_;
    bool readOnly = true;
    bool skipBom = true;
    MappedTxtFile file(path.widen(), readOnly, skipBom);
    construct(file);
    return (errDesc_ == 0);
}


//!
//! Reset instance with given XML file. Return true if successful.
//! Use errDesc() for more info in case of failure.
//!
bool XmlDoc::loadFrom(const char* path)
{
    delete root_;
    delete prolog_;
    String s(path);
    bool readOnly = true;
    bool skipBom = true;
    MappedTxtFile file(s.widen(), readOnly, skipBom);
    construct(file);
    return (errDesc_ == 0);
}


//!
//! Reset instance with given XML text. Return true if successful.
//! Use errDesc() for more info in case of failure.
//!
bool XmlDoc::loadFromXml(const String& xml)
{
    delete root_;
    delete prolog_;

    XmlLexer lexer(xml);
    XmlDoc* doc = lexer.scan();
    errDesc_ = doc->errDesc_;
    prolog_ = doc->prolog_;
    root_ = doc->root_;
    doc->prolog_ = 0;
    doc->root_ = 0;
    delete doc;

    return (errDesc_ == 0);
}


void XmlDoc::construct(const MappedTxtFile& file)
{
    if (file.isOk())
    {
        XmlDoc* doc;
        const Bom& bom = file.bom();
        if ((bom == Bom::None) || (bom == Bom::Utf8))
        {
            XmlLexer lexer(file.image(), static_cast<size_t>(file.size()));
            doc = lexer.scan();
        }
        else
        {
            String lexee;
            lexee.resetX(bom, file.image(), static_cast<size_t>(file.size()));
            XmlLexer lexer(lexee);
            doc = lexer.scan();
        }
        errDesc_ = doc->errDesc_;
        prolog_ = doc->prolog_;
        root_ = doc->root_;
        doc->prolog_ = 0;
        doc->root_ = 0;
        delete doc;
    }
    else
    {
        errDesc_ = "cannot access file";
        prolog_ = new XmlProlog;
        root_ = new UnknownElement(String());
    }
}


void XmlDoc::construct(const std::istream& is)
{
    if (is.good())
    {
        XmlLexer lexer(is);
        XmlDoc* doc = lexer.scan();
        errDesc_ = doc->errDesc_;
        prolog_ = doc->prolog_;
        root_ = doc->root_;
        doc->prolog_ = 0;
        doc->root_ = 0;
        delete doc;
    }
    else
    {
        errDesc_ = "cannot access stream";
        prolog_ = new XmlProlog;
        root_ = new UnknownElement(String());
    }
}


//!
//! Construct instance with given body. For body "xxx", this instance
//! would generate this XML stream: "<![CDATA[xxx]]>".
//!
XmlDoc::Cdata::Cdata(const String& body):
XmlElement(),
body_(body)
{
}


XmlDoc::Cdata::~Cdata()
{
}


//!
//! Return the XML form for this cdata.
//!
String XmlDoc::Cdata::toXml(size_t indent) const
{
    String xml;
    if (numSibs() > 0)
    {
        xml.append(indent, ' ');
    }

    xml += "<![CDATA[";
    xml += body_;
    xml += "]]>";
    return xml;
}


//!
//! Return cloned element which must be deleted via the delete operator when done.
//!
XmlElement* XmlDoc::Cdata::clone() const
{
    return new Cdata(body_);
}


//!
//! Make baby a child of this element. Take over ownership of the given object which
//! will be destroyed by the element when appropriate. Return true if successful.
//!
bool XmlDoc::Cdata::giveBirth(XmlElement* baby)
{

    // Kids not allowed in a cdata element.
    delete baby;
    bool ok = false;
    return ok;
}


//!
//! Return the body part of the cdata. For example, the body part
//! is "body" for this XML stream: "<![CDATA[body]]>".
//!
const String& XmlDoc::Cdata::body() const
{
    return body_;
}


//!
//! Update the body part.
//!
void XmlDoc::Cdata::setBody(const String& body)
{
    body_ = body;
}


//!
//! Construct instance with given body. For body "xxx", this instance
//! would generate this XML stream: "<--xxx-->".
//!
XmlDoc::Comment::Comment(const String& body):
XmlElement(),
body_(body)
{
}


XmlDoc::Comment::~Comment()
{
}


//!
//! Return the XML form for this comment.
//!
String XmlDoc::Comment::toXml(size_t indent) const
{
    String xml;
    if (numSibs() > 0)
    {
        xml.append(indent, ' ');
    }

    xml += "<!--";
    xml += body_;
    xml += "-->";
    return xml;
}


//!
//! Return cloned element which must be deleted via the delete operator when done.
//!
XmlElement* XmlDoc::Comment::clone() const
{
    return new Comment(body_);
}


//!
//! Make baby a child of this element. Take over ownership of the given object which
//! will be destroyed by the element when appropriate. Return true if successful.
//!
bool XmlDoc::Comment::giveBirth(XmlElement* baby)
{

    // Kids not allowed in a comment element.
    delete baby;
    bool ok = false;
    return ok;
}


//!
//! Return the body part of the comment. For example, the body part
//! is "body" for this XML stream: "<!--body-->".
//!
const String& XmlDoc::Comment::body() const
{
    return body_;
}


//!
//! Update the body part.
//!
void XmlDoc::Comment::setBody(const String& body)
{
    body_ = body;
}


//!
//! Construct instance with given body. For body "xxx" in a one-kid element
//! named "element", the mothering element would generate this XML stream:
//! "<element>xxx</element>".
//!
XmlDoc::Content::Content(const String& body):
XmlElement(),
body_(body)
{
}


XmlDoc::Content::~Content()
{
}


//!
//! Return the XML form for this element content.
//!
String XmlDoc::Content::toXml(size_t indent) const
{
    if (numSibs() == 0)
    {
        return XmlLexer::escape(body_);
    }

    String xml(indent, ' ');
    xml += XmlLexer::escape(body_);
    return xml;
}


//!
//! Return cloned element which must be deleted via the delete operator when done.
//!
XmlElement* XmlDoc::Content::clone() const
{
    return new Content(body_);
}


//!
//! Make baby a child of this element. Take over ownership of the given object which
//! will be destroyed by the element when appropriate. Return true if successful.
//!
bool XmlDoc::Content::giveBirth(XmlElement* baby)
{

    // Kids not allowed in a content element.
    delete baby;
    bool ok = false;
    return ok;
}


//!
//! Return the body part of the element content. For example, the body part
//! is "body" for this XML stream: "<content>body</content>".
//!
const String& XmlDoc::Content::body() const
{
    return body_;
}


//!
//! Update the body part.
//!
void XmlDoc::Content::setBody(const String& body)
{
    body_ = body;
}


//!
//! Construct an empty element with given name and an initial attribute capacity.
//! A default capacity of zero indicates no pre-allocation. A non-zero capacity
//! causes some pre-allocation for at least attrCap attributes.
//!
XmlDoc::EmptyElement::EmptyElement(const String& name, unsigned int attrCap):
XmlElement(name, attrCap)
{
}


XmlDoc::EmptyElement::~EmptyElement()
{
}


//!
//! Return the XML form for this empty element.
//!
String XmlDoc::EmptyElement::toXml(size_t indent) const
{
    String xml(indent, ' ');
    xml += '<';
    xml += name();
    xml += attrsInXml();
    xml += "/>";

    return xml;
}


//!
//! Return cloned element which must be deleted via the delete operator when done.
//!
XmlElement* XmlDoc::EmptyElement::clone() const
{
    unsigned int attrCap = numAttrs();
    XmlElement* cloned = new EmptyElement(name(), attrCap);
    for (size_t i = 0, numAttrs = XmlElement::numAttrs(); i < numAttrs; ++i)
    {
        const StringPair& src = attr(i);
        const StringPair* dst = new StringPair(src);
        cloned->addAttr(dst);
    }

    return cloned;
}


//!
//! Make baby a child of this element. Take over ownership of the given object which
//! will be destroyed by the element when appropriate. Return true if successful.
//!
bool XmlDoc::EmptyElement::giveBirth(XmlElement* baby)
{

    // Kids not allowed in an empty element.
    delete baby;
    bool ok = false;
    return ok;
}


void XmlDoc::Stat::reset(const XmlDoc& doc)
{
    numAttrs_ = 0;
    numElements_ = 0;
    numUnknowns_ = 0;
    doc.root_->applyParentFirst(summarize, this);
}


bool XmlDoc::Stat::summarize(void* arg, const XmlElement& element)
{
    Stat* stat = static_cast<Stat*>(arg);
    stat->numAttrs_ += element.numAttrs();
    ++stat->numElements_;
    if (element.isUnknown())
    {
        ++stat->numUnknowns_;
    }

    bool keepGoing = true;
    return keepGoing;
}


//!
//! Construct instance with given body. There's no additional XML processing
//! for unknown elements, so for body "<!UNKNOWN/>", this instance would
//! generate this XML stream: "<!UNKNOWN/>".
//!
XmlDoc::UnknownElement::UnknownElement(const String& body):
XmlElement(),
body_(body)
{
}


XmlDoc::UnknownElement::~UnknownElement()
{
}


//!
//! Return the XML form for this unknown element.
//!
String XmlDoc::UnknownElement::toXml(size_t indent) const
{
    if (numSibs() == 0)
    {
        return body_;
    }

    String xml(indent, ' ');
    xml += body_;
    return xml;
}


//!
//! Return cloned element which must be deleted via the delete operator when done.
//!
XmlElement* XmlDoc::UnknownElement::clone() const
{
    return new UnknownElement(body_);
}


//!
//! Make baby a child of this element. Take over ownership of the given object which
//! will be destroyed by the element when appropriate. Return true if successful.
//!
bool XmlDoc::UnknownElement::giveBirth(XmlElement* baby)
{

    // Kids not allowed in an unknown element.
    delete baby;
    bool ok = false;
    return ok;
}


//!
//! Return true if element is unknown.
//!
bool XmlDoc::UnknownElement::isUnknown() const
{
    bool unknown = true;
    return unknown;
}


//!
//! Return the body part of this unknown element. For example, the body part
//! is "<#unknown-element>" for this XML stream: "<#unknown-element>".
//!
const String& XmlDoc::UnknownElement::body() const
{
    return body_;
}


//!
//! Update the body part.
//!
void XmlDoc::UnknownElement::setBody(const String& body)
{
    body_ = body;
}

END_NAMESPACE1
