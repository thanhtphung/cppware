/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_XML_DOC_HPP
#define APPKIT_XML_DOC_HPP

#include <istream>
#include "appkit/String.hpp"
#include "appkit/XmlElement.hpp"
#include "syskit/macros.h"

DECLARE_CLASS1(syskit, MappedTxtFile)

BEGIN_NAMESPACE1(appkit)

class XmlLexer;
class XmlProlog;


//! xml document
class XmlDoc
    //!
    //! A class representing an XML (Extensible Markup Language) document. Current
    //! XML syntax is specified in <http://www.w3.org/TR/xml11/>. This class presents
    //! an XML document using the document object model (DOM).
    //!
{

public:

    // Constructors and destructor.
    XmlDoc(XmlProlog* prolog = 0, XmlElement* root = 0);
    XmlDoc(const String& path);
    XmlDoc(const char* path);
    XmlDoc(const std::istream& is);
    ~XmlDoc();

    bool isOk() const;
    const char* errDesc() const;

    String toXml() const;
    XmlElement& root();
    XmlElement* find(const String& fullName);
    bool loadFrom(const String& path);
    bool loadFrom(const char* path);
    bool loadFrom(const std::istream& is);
    bool loadFromXml(const String& xml);
    const XmlElement& root() const;
    const XmlElement* find(const String& fullName) const;
    const XmlProlog& prolog() const;
    void setProlog(XmlProlog* prolog);
    void setRoot(XmlElement* root);
    void setRoot(XmlElement* root, XmlElement*& replacedRoot);


    //! character data in xml
    class Cdata: public XmlElement
        //!
        //! Character data.
        //!
    {
    public:
        Cdata(const String& body);
        void reset(const String& body);
        virtual ~Cdata();
        virtual String toXml(size_t indent = 0) const;
        virtual XmlElement* clone() const;
        virtual bool giveBirth(XmlElement* baby);
        virtual const String& body() const;
        virtual void setBody(const String& body);
    private:
        String body_;
        Cdata(const Cdata&); //prohibit usage
        const Cdata& operator =(const Cdata&); //prohibit usage
    };


    //! comment in xml
    class Comment: public XmlElement
        //!
        //! XML comment.
        //!
    {
    public:
        Comment(const String& body);
        void reset(const String& body);
        virtual ~Comment();
        virtual String toXml(size_t indent = 0) const;
        virtual XmlElement* clone() const;
        virtual bool giveBirth(XmlElement* baby);
        virtual const String& body() const;
        virtual void setBody(const String& body);
    private:
        String body_;
        Comment(const Comment&); //prohibit usage
        const Comment& operator =(const Comment&); //prohibit usage
    };


    //! content in xml
    class Content: public XmlElement
        //!
        //! XmlElement content.
        //!
    {
    public:
        Content(const String& body);
        void reset(const String& body);
        virtual ~Content();
        virtual String toXml(size_t indent = 0) const;
        virtual XmlElement* clone() const;
        virtual bool giveBirth(XmlElement* baby);
        virtual const String& body() const;
        virtual void setBody(const String& body);
    private:
        String body_;
        Content(const Content&); //prohibit usage
        const Content& operator =(const Content&); //prohibit usage
    };


    //! empty xml element
    class EmptyElement: public XmlElement
        //!
        //! Empty element.
        //!
    {
    public:
        EmptyElement(const String& name, unsigned int attrCap = 0);
        virtual ~EmptyElement();
        virtual String toXml(size_t indent = 0) const;
        virtual XmlElement* clone() const;
        virtual bool giveBirth(XmlElement* baby);
    private:
        EmptyElement(const EmptyElement&); //prohibit usage
        const EmptyElement& operator =(const EmptyElement&); //prohibit usage
    };


    //! unknown xml element
    class UnknownElement: public XmlElement
        //!
        //! Unknown element.
        //!
    {
    public:
        UnknownElement(const String& body);
        void reset(const String& body);
        virtual ~UnknownElement();
        virtual String toXml(size_t indent = 0) const;
        virtual XmlElement* clone() const;
        virtual bool giveBirth(XmlElement* baby);
        virtual bool isUnknown() const;
        virtual const String& body() const;
        virtual void setBody(const String& body);
    private:
        String body_;
        UnknownElement(const UnknownElement&); //prohibit usage
        const UnknownElement& operator =(const UnknownElement&); //prohibit usage
    };


    //! xml document stats
    class Stat
        //!
        //! Available stats.
        //!
    {
    public:
        Stat(const XmlDoc& doc);
        unsigned int numAttrs() const;
        unsigned int numElements() const;
        unsigned int numUnknowns() const;
        void reset(const XmlDoc& doc);
    private:
        unsigned int numAttrs_;
        unsigned int numElements_;
        unsigned int numUnknowns_;
        Stat(const Stat&); //prohibit usage
        const Stat& operator =(const Stat&); //prohibit usage
        static bool summarize(void*, const XmlElement&);
    };

private:
    XmlElement* root_;
    XmlProlog* prolog_;
    const char* errDesc_;

    XmlDoc(const XmlDoc&); //prohibit usage
    const XmlDoc& operator =(const XmlDoc&); //prohibit usage

    XmlElement* findByName(const String&) const;
    void construct(const syskit::MappedTxtFile&);
    void construct(const std::istream&);

    friend class XmlLexer;

};

END_NAMESPACE1

#include <ostream>
#include "appkit/XmlProlog.hpp"

BEGIN_NAMESPACE1(appkit)

//! Return the root element.
inline XmlElement& XmlDoc::root()
{
    return *root_;
}

//! Locate descendent given the full name (e.g., "/grandma/mom/me"). Return
//! located descendent. Return zero if not found.
inline XmlElement* XmlDoc::find(const String& fullName)
{
    return findByName(fullName);
}

//! Return true if instance was successfully constructed.
//! Use errDesc() to get an error description if necessary.
inline bool XmlDoc::isOk() const
{
    return (errDesc_ == 0);
}

//! Reset instance with given XML stream. Return true if successful.
//! Use errDesc() for more info in case of failure.
inline bool XmlDoc::loadFrom(const std::istream& is)
{
    delete root_;
    delete prolog_;
    construct(is);
    return (errDesc_ == 0);
}

//! Return the root element.
inline const XmlElement& XmlDoc::root() const
{
    return *root_;
}

//! Locate descendent given the full name (e.g., "/grandma/mom/me"). Return
//! located descendent. Return zero if not found.
inline const XmlElement* XmlDoc::find(const String& fullName) const
{
    return findByName(fullName);
}

//! Return the document's prolog.
inline const XmlProlog& XmlDoc::prolog() const
{
    return *prolog_;
}

//! Return zero if instance was successfully constructed.
//! Return an error description otherwise.
inline const char* XmlDoc::errDesc() const
{
    return errDesc_;
}

//! Update instance with given prolog. Take over ownership of the given
//! object which will be destroyed by the document when appropriate.
inline void XmlDoc::setProlog(XmlProlog* prolog)
{
    delete prolog_;
    prolog_ = (prolog != 0)? prolog: new XmlProlog;
}

//! Update instance with given document root. Take over ownership of the
//! given object which will be destroyed by the document when appropriate.
inline void XmlDoc::setRoot(XmlElement* root)
{
    delete root_;
    root_ = (root != 0)? root: new UnknownElement(String());
}

//! Update instance with given document root. Take over ownership of the
//! given object which will be destroyed by the document when appropriate.
inline void XmlDoc::setRoot(XmlElement* root, XmlElement*& replacedRoot)
{
    replacedRoot = root_;
    root_ = (root != 0)? root: new UnknownElement(String());
}

//! Reset instance with given body.
inline void XmlDoc::Cdata::reset(const String& body)
{
    body_ = body;
}

//! Reset instance with given body.
inline void XmlDoc::Comment::reset(const String& body)
{
    body_ = body;
}

//! Reset instance with given body.
inline void XmlDoc::Content::reset(const String& body)
{
    body_ = body;
}

inline XmlDoc::Stat::Stat(const XmlDoc& doc)
{
    reset(doc);
}

inline unsigned int XmlDoc::Stat::numAttrs() const
{
    return numAttrs_;
}

inline unsigned int XmlDoc::Stat::numElements() const
{
    return numElements_;
}

inline unsigned int XmlDoc::Stat::numUnknowns() const
{
    return numUnknowns_;
}

//! Reset instance with given body.
inline void XmlDoc::UnknownElement::reset(const String& body)
{
    body_ = body;
}

//! Reset XmlDoc instance with given XML stream. Resulting document might not be
//! okay if given XML stream is malformed. Use XmlDoc::isOk() and XmlDoc::errDesc()
//! for validation.
inline const std::istream& operator >>(const std::istream& is, XmlDoc& doc)
{
    doc.loadFrom(is);
    return is;
}

//! Append the XML form of the XmlDoc instance to the given output stream. Return
//! the resulting output stream.
inline std::ostream& operator <<(std::ostream& os, const XmlDoc& doc)
{
    String xml(doc.toXml());
    return os.write(xml.ascii(), static_cast<std::streamsize>(xml.length()));
}

END_NAMESPACE1

#endif
