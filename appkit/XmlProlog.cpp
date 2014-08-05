/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/XmlElement.hpp"
#include "appkit/XmlProlog.hpp"

using namespace syskit;

BEGIN_NAMESPACE1(appkit)


//!
//! Construct an empty prolog.
//! Its XML form is an empty string.
//!
XmlProlog::XmlProlog():
encoding_(),
standalone_(),
version_()
{
    misc_ = 0;
}


//!
//! Construct prolog with given declaraction and no miscellaneous items. Its XML form
//! is an empty string if given attributes are empty. For version "1.1", encoding "utf-8",
//! this instance would generate this XML stream: "<?xml version="1.1" encoding="utf-8"?>".
//!
XmlProlog::XmlProlog(const String& version, const String& encoding, const String& standalone):
encoding_(encoding),
standalone_(standalone),
version_(version)
{
    misc_ = 0;
}


XmlProlog::~XmlProlog()
{
    destruct();
}


//
// Return the XML form for the declaration part.
//
String XmlProlog::declInXml() const
{

    // XML form of an empty XML declaration is an empty string.
    if (version_.empty() && encoding_.empty() && standalone_.empty())
    {
        return version_;
    }

    String xml("<?xml");
    if (!version_.empty())
    {
        xml += " version=\"";
        xml += version_;
        xml += '"';
    }
    if (!encoding_.empty())
    {
        xml += " encoding=\"";
        xml += encoding_;
        xml += '"';
    }
    if (!standalone_.empty())
    {
        xml += " standalone=\"";
        xml += standalone_;
        xml += "\"?>";
    }
    else
    {
        xml += "?>";
    }

    // Return the XML form.
    return xml;
}


//!
//! Return the XML form for this prolog.
//!
String XmlProlog::toXml() const
{
    String xml(declInXml());
    if (misc_ != 0)
    {
        for (size_t i = 0, numItems = misc_->numItems(); i < numItems; ++i)
        {
            const XmlElement* item = static_cast<const XmlElement*>(misc_->peek(i));
            xml += '\n';
            xml += item->toXml();
        }
    }

    return xml;
}


//!
//! Return cloned prolog which must be deleted via the delete operator when done.
//!
XmlProlog* XmlProlog::clone() const
{
    XmlProlog* cloned = new XmlProlog(version_, encoding_, standalone_);
    if (misc_ != 0)
    {
        Vec* misc = new Vec(*misc_);
        for (size_t i = 0, numItems = misc->numItems(); i < numItems; ++i)
        {
            const XmlElement* item = static_cast<const XmlElement*>(misc->peek(i));
            misc->setItem(i, item->clone());
        }
        cloned->misc_ = misc;
    }

    // Return cloned prolog.
    return cloned;
}


void XmlProlog::destruct()
{
    if (misc_ != 0)
    {
        for (void* item; misc_->rmTail(item); delete static_cast<XmlElement*>(item));
        delete misc_;
    }
}

END_NAMESPACE1
