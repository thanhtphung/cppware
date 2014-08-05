/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_XML_PROLOG_HPP
#define APPKIT_XML_PROLOG_HPP

#include "appkit/String.hpp"
#include "syskit/macros.h"

DECLARE_CLASS1(syskit, Vec)

BEGIN_NAMESPACE1(appkit)

class XmlElement;


//! xml document prolog
class XmlProlog
    //!
    //! Document prolog. A prolog contains an XML declaration and zero or
    //! more miscellaneous items (comments, etc.).
    //!
{

public:
    XmlProlog();
    XmlProlog(const String& version, const String& encoding, const String& standalone);
    ~XmlProlog();

    // XML declaraction.
    const String& encoding() const;
    const String& standalone() const;
    const String& version() const;
    void setEncoding(const String& encoding);
    void setStandalone(const String& Standalone);
    void setVersion(const String& version);

    String toXml() const;
    XmlProlog* clone() const;
    const XmlElement& item(size_t index) const;
    unsigned int numItems() const;
    void addItem(const XmlElement* item);
    void freezeItems();
    void reset(const String& version, const String& encoding, const String& standalone);

private:
    String encoding_;
    String standalone_;
    String version_;
    syskit::Vec* misc_;

    XmlProlog(const XmlProlog&); //prohibit usage
    const XmlProlog& operator =(const XmlProlog&); //prohibit usage

    String declInXml() const;
    void destruct();

};

END_NAMESPACE1

#include "syskit/Vec.hpp"

BEGIN_NAMESPACE1(appkit)

//! Return the encoding declaration (e.g., "UTF-8").
inline const String& XmlProlog::encoding() const
{
    return encoding_;
}

//! Return the standalone declaration (e.g., "yes").
inline const String& XmlProlog::standalone() const
{
    return standalone_;
}

//! Return the version declaration (e.g., "1.1").
inline const String& XmlProlog::version() const
{
    return version_;
}

//! Return the miscellaneous item residing at given index.
inline const XmlElement& XmlProlog::item(size_t index) const
{
    const XmlElement* item = static_cast<const XmlElement*>(misc_->peek(index));
    return *item;
}

//! Return the number of miscellaneous items in the prolog.
inline unsigned int XmlProlog::numItems() const
{
    return (misc_ != 0)? misc_->numItems(): 0;
}

//! Add given miscellaneous item to this prolog. Take over ownership of the
//! item and destroy it via the delete operator when the prolog is destructed.
inline void XmlProlog::addItem(const XmlElement* item)
{
    if (misc_ == 0) misc_ = new syskit::Vec(syskit::Vec::DefaultCap, -1 /*growBy*/);
    misc_->add(const_cast<XmlElement*>(item));
}

//! Assume no more miscellaneous items will be added to the prolog and clean
//! up the internals to minimize memory use. Still, more items can be added to
//! the prolog if necessary.
inline void XmlProlog::freezeItems()
{
    if (misc_ != 0) misc_->resize(misc_->numItems());
}

//! Remove all miscellaneous items and reset instance with given XML declaraction.
inline void XmlProlog::reset(const String& version, const String& encoding, const String& standalone)
{
    destruct();
    misc_ = 0;
    encoding_ = encoding;
    standalone_ = standalone;
    version_ = version;
}

//! Reset instance with given encoding declaration.
inline void XmlProlog::setEncoding(const String& encoding)
{
    encoding_ = encoding;
}

//! Reset instance with given standalone declaration.
inline void XmlProlog::setStandalone(const String& standalone)
{
    standalone_ = standalone;
}

//! Reset instance with given version declaration.
inline void XmlProlog::setVersion(const String& version)
{
    version_ = version;
}

END_NAMESPACE1

#endif
