#include "appkit/Directory.hpp"
#include "appkit/XmlDoc.hpp"
#include "appkit/XmlElement.hpp"
#include "appkit/XmlProlog.hpp"
#include "appkit/std.hpp"
#include "syskit/MappedTxtFile.hpp"

#include "appkit-ut-pch.h"
#include "XmlDocSuite.hpp"

using namespace appkit;
using namespace syskit;


XmlDocSuite::XmlDocSuite()
{
}


XmlDocSuite::~XmlDocSuite()
{
}


bool XmlDocSuite::loadXmlFile(void* arg, const Directory& parent, const String& childName)
{
    if (Directory::nameIsDir(childName))
    {
        Directory dir(parent.path() + childName);
        bool ok = dir.apply(loadXmlFile, 0);
        return ok;
    }

    if (childName.endsWith(".vcxproj", true /*ignoreCase*/) ||
        childName.endsWith(".props", true /*ignoreCase*/) ||
        childName.endsWith(".filters", true /*ignoreCase*/))
    {
        bool readOnly = true;
        bool skipBom = false;
        MappedTxtFile xmlFile((parent.path() + childName).widen(), readOnly, skipBom);
        String xml;
        xml.resetX(xmlFile.image(), (size_t)xmlFile.size());
        XmlDoc xmlDoc;
        if ((!xmlDoc.loadFromXml(xml)) || (XmlDoc::Stat(xmlDoc).numUnknowns() > 0))
        {
            return false;
        }
    }

    return true;
}


//
// Interfaces under test:
// - XmlDoc::Cdata::Cdata(const String&);
// - XmlDoc::Cdata::~Cdata();
// - String XmlDoc::Cdata::toXml(size_t) const;
// - bool XmlDoc::Cdata::giveBirth(XmlElement*);
// - const String& XmlDoc::Cdata::body() const;
// - void XmlDoc::Cdata::reset(const String&);
// - void XmlDoc::Cdata::setBody(const String&);
//
void XmlDocSuite::testCdata00()
{
    String body("body");
    XmlDoc::Cdata cdata(body);
    bool ok = (cdata.name().empty() && (cdata.body() == body));
    CPPUNIT_ASSERT(ok);

    // Indent is in effect for cdata only if there are siblings.
    ok = (cdata.toXml(0) == "<![CDATA[body]]>");
    CPPUNIT_ASSERT(ok);
    cdata.reset(String());
    ok = (cdata.toXml(4) == "<![CDATA[]]>");
    CPPUNIT_ASSERT(ok);

    XmlElement& e = cdata;
    e.setBody("new-body");
    ok = (cdata.body() == "new-body");
    CPPUNIT_ASSERT(ok);

    ok = (!cdata.giveBirth(new XmlDoc::Comment(String())));
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - XmlDoc::Comment::Comment(const String&);
// - XmlDoc::Comment::~Comment();
// - String XmlDoc::Comment::toXml(size_t) const;
// - bool XmlDoc::Comment::giveBirth(XmlElement*);
// - const String& XmlDoc::Comment::body() const;
// - void XmlDoc::Comment::reset(const String&);
// - void XmlDoc::Comment::setBody(const String&);
//
void XmlDocSuite::testComment00()
{
    String body("comment");
    XmlDoc::Comment comment(body);
    bool ok = (comment.name().empty() && (comment.body() == body));
    CPPUNIT_ASSERT(ok);

    // Indent is in effect for comment only if there are siblings.
    ok = (comment.toXml(0) == "<!--comment-->");
    CPPUNIT_ASSERT(ok);
    comment.reset(String());
    ok = (comment.toXml(4) == "<!---->");
    CPPUNIT_ASSERT(ok);

    XmlElement& e = comment;
    e.setBody("new-comment");
    ok = (comment.body() == "new-comment");
    CPPUNIT_ASSERT(ok);

    ok = (!comment.giveBirth(new XmlDoc::Cdata(String())));
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - XmlDoc::Content::Content(const String&);
// - XmlDoc::Content::~Content();
// - String XmlDoc::Content::toXml(size_t) const;
// - bool XmlDoc::Content::giveBirth(XmlElement*);
// - const String& XmlDoc::Content::body() const;
// - void XmlDoc::Content::reset(const String&);
// - void XmlDoc::Content::setBody(const String&);
//
void XmlDocSuite::testContent00()
{
    String body("content");
    XmlDoc::Content content(body);
    bool ok = (content.name().empty() && (content.body() == body));
    CPPUNIT_ASSERT(ok);

    // Indent is in effect for content only if there are siblings.
    ok = (content.toXml(0) == "content");
    CPPUNIT_ASSERT(ok);
    content.reset("&<>'\"\01\x1f");
    ok = (content.toXml(4) == "&amp;&lt;&gt;&apos;&quot;&#1;&#31;");
    CPPUNIT_ASSERT(ok);

    XmlElement& e = content;
    e.setBody("new-content");
    ok = (content.body() == "new-content");
    CPPUNIT_ASSERT(ok);

    ok = (!content.giveBirth(new XmlDoc::EmptyElement(String())));
    CPPUNIT_ASSERT(ok);
}


void XmlDocSuite::testCtor00()
{
    MappedTxtFile src(L"../../../etc/good-sample.xml");
    String xmlIn;
    xmlIn.reset8(src.image(), static_cast<size_t>(src.size()));

    XmlDoc doc("../../../etc/good-sample.xml");
    String xmlOut = doc.toXml();
    bool ok = (doc.isOk() && (doc.errDesc() == 0) && (xmlIn == xmlOut));
    CPPUNIT_ASSERT(ok);
    std::ostringstream oss;
    oss << doc;
    ok = (xmlOut == oss.str().c_str());
    CPPUNIT_ASSERT(ok);

    XmlDoc::Stat stat(doc);
    ok = (stat.numAttrs() == 5) && (stat.numElements() == 42) && (stat.numUnknowns() == 0);
    CPPUNIT_ASSERT(ok);

    const XmlElement& e0 = doc.root();
    const XmlElement* e = e0.clone();
    String root = e0.toXml();
    ok = (e->mom() == 0);
    CPPUNIT_ASSERT(ok);
    String cloned(e->toXml());
    delete e;
    ok = (root == cloned);
    CPPUNIT_ASSERT(ok);
    oss.str(std::string());
    oss << doc.root();
    ok = (root == oss.str().c_str());
    CPPUNIT_ASSERT(ok);
}


void XmlDocSuite::testCtor01()
{
    XmlProlog* prolog = 0;
    XmlElement* root = 0;
    XmlDoc doc(prolog, root);
    bool ok = (doc.isOk() && (doc.errDesc() == 0) && doc.toXml().empty());
    CPPUNIT_ASSERT(ok);
    ok = doc.prolog().toXml().empty();
    CPPUNIT_ASSERT(ok);
}


void XmlDocSuite::testCtor02()
{

    // Empty file.
    String path0("../../../etc/empty.txt");
    XmlDoc doc0(path0);
    bool ok = ((!doc0.isOk()) && (doc0.errDesc() != 0));
    CPPUNIT_ASSERT(ok);

    // Not a file.
    XmlDoc doc2("../../../etc/");
    ok = ((!doc2.isOk()) && (doc2.errDesc() != 0));
    CPPUNIT_ASSERT(ok);
}


void XmlDocSuite::testEmptyElement00()
{
    String name("name");
    XmlDoc::EmptyElement element(name, 1);
    bool ok = ((element.name() == name) && element.body().empty());
    CPPUNIT_ASSERT(ok);
    ok = ((element.numAttrs() == 0) && (element.numKids() == 0) && (element.numSibs() == 0));
    CPPUNIT_ASSERT(ok);
    ok = ((element.mom() == 0) && (element.nextSib() == 0) && (element.prevSib() == 0));
    CPPUNIT_ASSERT(ok);
    ok = (element.toXml(0) == "<name/>");
    CPPUNIT_ASSERT(ok);
    ok = (element.toXml(1) == " <name/>");
    CPPUNIT_ASSERT(ok);

    ok = (!element.giveBirth(new XmlElement(String())));
    CPPUNIT_ASSERT(ok);
}


void XmlDocSuite::testFind00()
{
    XmlDoc doc("../../../etc/good-sample.xml");
    String fullName("/root/uncle");
    const XmlElement* element = doc.find(fullName);
    bool ok = ((element != 0) && (element->fullName() == fullName));
    CPPUNIT_ASSERT(ok);

    fullName = "/root/mom1/";
    element = doc.find(fullName);
    ok = ((element != 0) && (element->fullName() == fullName));
    CPPUNIT_ASSERT(ok);

    // Not a full name.
    ok = (!doc.find(""));
    CPPUNIT_ASSERT(ok);

    // Mismatched root.
    ok = ((!doc.find("/")) && (!doc.find("/not-a-root/mom1/")));
    CPPUNIT_ASSERT(ok);

    // Not found.
    ok = (!doc.find("/root/mom1/non-existent-child"));
    CPPUNIT_ASSERT(ok);
}


void XmlDocSuite::testLoadFrom00()
{
    XmlDoc doc;
    String path0("../../../etc/good-sample.xml");
    bool ok = doc.loadFrom(path0);
    CPPUNIT_ASSERT(ok);

    bool readOnly = true;
    bool skipBom = false;
    MappedTxtFile src(L"../../../etc/sample-utf8.xml", readOnly, skipBom);
    String xmlIn;
    xmlIn.resetX(src.image(), static_cast<size_t>(src.size()));
    ok = doc.loadFrom("../../../etc/sample-utf8.xml");
    CPPUNIT_ASSERT(ok);
    String xmlOut(doc.toXml());
    ok = (xmlIn == xmlOut);
    CPPUNIT_ASSERT(ok);

    ok = doc.loadFrom("../../../etc/sample-utf8-xtra-spaces.xml");
    CPPUNIT_ASSERT(ok);
    xmlOut = doc.toXml();
    ok = (xmlIn == xmlOut);
    CPPUNIT_ASSERT(ok);

    src.remap(L"../../../etc/sample-utf16.xml", readOnly);
    xmlIn.resetX(src.image(), static_cast<size_t>(src.size()));
    ok = doc.loadFrom("../../../etc/sample-utf16.xml");
    CPPUNIT_ASSERT(ok);
    xmlOut = doc.toXml();
    ok = (xmlIn == xmlOut);
    CPPUNIT_ASSERT(ok);

    ok = doc.loadFrom("../../../etc/good-sample.xml");
    CPPUNIT_ASSERT(ok);

    String path1("../../../etc/bad-sample.xml");
    ok = doc.loadFrom(path1);
    CPPUNIT_ASSERT(ok);
    XmlDoc::Stat stat(doc);
    ok = (stat.numAttrs() == 0) && (stat.numElements() == 20) && (stat.numUnknowns() == 17);
    CPPUNIT_ASSERT(ok);

    xmlIn = doc.toXml();
    XmlProlog* prolog = doc.prolog().clone();
    XmlElement* root = doc.root().clone();
    doc.setProlog(prolog);
    doc.setRoot(root);
    xmlOut = doc.toXml();
    ok = (xmlIn == xmlOut);
    CPPUNIT_ASSERT(ok);
}


void XmlDocSuite::testLoadFrom01()
{
    String path("../../../etc/good-sample.xml");
    ifstream* is = new ifstream(path);
    XmlDoc doc(*is);
    delete is;
    bool ok = doc.isOk();
    CPPUNIT_ASSERT(ok);

    // Empty file.
    path = "../../../etc/empty.txt";
    is = new ifstream(path);
    *is >> doc;
    delete is;
    ok = (!doc.isOk());
    CPPUNIT_ASSERT(ok);

    // Not a file.
    path = "../../../etc/";
    is = new ifstream(path);
    *is >> doc;
    delete is;
    ok = (!doc.isOk());
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - bool XmlDoc::loadFromXml(const String&);
//
void XmlDocSuite::testLoadFrom02()
{
    MappedTxtFile src(L"../../../etc/good-sample.xml");
    String xmlIn;
    xmlIn.reset8(src.image(), static_cast<size_t>(src.size()));
    XmlDoc doc;
    bool ok = (doc.loadFromXml(xmlIn) && (doc.errDesc() == 0));
    CPPUNIT_ASSERT(ok);

    String xmlOut = doc.toXml();
    ok = (xmlIn == xmlOut);
    CPPUNIT_ASSERT(ok);

    ok = doc.loadFrom("../../../etc/good-sample-xtra-spaces.xml");
    CPPUNIT_ASSERT(ok);
    ok = (xmlIn == doc.toXml());
    CPPUNIT_ASSERT(ok);

    xmlIn.truncate(xmlIn.length() - 8); //missing terminating </root>
    ok = ((!doc.loadFromXml(xmlIn)) && (doc.errDesc() != 0));
    CPPUNIT_ASSERT(ok);

    xmlIn = "<#not-a-good-name/>";
    ok = ((!doc.loadFromXml(xmlIn)) && (doc.errDesc() != 0));
    CPPUNIT_ASSERT(ok);
}


void XmlDocSuite::testLoadFrom03()
{

#if 0
    Directory dir("../../../../");
    bool ok = dir.apply(loadXmlFile, 0);
    CPPUNIT_ASSERT(ok);
#endif
}


void XmlDocSuite::testProlog00()
{
    XmlProlog prolog0;
    bool ok = (prolog0.version().empty() && prolog0.encoding().empty() && prolog0.standalone().empty());
    CPPUNIT_ASSERT(ok);
    ok = ((prolog0.numItems() == 0) && prolog0.toXml().empty());
    CPPUNIT_ASSERT(ok);

    String encoding("UTF-8");
    String standalone("yes");
    String version("1.1");
    XmlProlog prolog1(version, encoding, standalone);
    ok = (prolog1.version() == version && prolog1.encoding() == encoding && prolog1.standalone() == standalone);
    CPPUNIT_ASSERT(ok);
    ok = (prolog1.toXml() == "<?xml version=\"1.1\" encoding=\"UTF-8\" standalone=\"yes\"?>");
    CPPUNIT_ASSERT(ok);
    version = "1.0";
    encoding = "utf-8";
    standalone = "no";
    prolog1.reset(version, encoding, standalone);
    ok = (prolog1.toXml() == "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>");
    CPPUNIT_ASSERT(ok);

    prolog1.setEncoding(String());
    prolog1.setStandalone(String());
    ok = (prolog1.toXml() == "<?xml version=\"1.0\"?>");
    CPPUNIT_ASSERT(ok);

    prolog1.setEncoding(encoding);
    prolog1.setVersion(String());
    ok = (prolog1.toXml() == "<?xml encoding=\"utf-8\"?>");
    CPPUNIT_ASSERT(ok);

    prolog1.setStandalone(standalone);
    ok = (prolog1.toXml() == "<?xml encoding=\"utf-8\" standalone=\"no\"?>");
    CPPUNIT_ASSERT(ok);
}


void XmlDocSuite::testProlog01()
{
    String s;
    XmlProlog prolog0("1.0", s, s);
    XmlDoc::Comment* item0 = new XmlDoc::Comment("item0");
    XmlDoc::Comment* item1 = new XmlDoc::Comment("item1");
    prolog0.addItem(item0);
    prolog0.addItem(item1);
    prolog0.freezeItems();
    bool ok = ((prolog0.numItems() == 2) && (&prolog0.item(0) == item0) && (&prolog0.item(1) == item1));
    CPPUNIT_ASSERT(ok);

    ok = (prolog0.toXml() == "<?xml version=\"1.0\"?>\n<!--item0-->\n<!--item1-->");
    CPPUNIT_ASSERT(ok);

    XmlProlog prolog1;
    XmlProlog* cloned = prolog1.clone();
    ok = cloned->toXml().empty();
    CPPUNIT_ASSERT(ok);
    delete cloned;
}


//
// Interfaces under test:
// - XmlDoc::UnknownElement::UnknownElement(const String&);
// - XmlDoc::UnknownElement::~UnknownElement();
// - String XmlDoc::UnknownElement::toXml(size_t) const;
// - bool XmlDoc::UnknownElement::giveBirth(XmlElement*);
// - const String& XmlDoc::UnknownElement::body() const;
// - void XmlDoc::UnknownElement::reset(const String&);
// - void XmlDoc::UnknownElement::setBody(const String&);
//
void XmlDocSuite::testUnknownElement00()
{
    String body("<!UNKNOWN/>");
    XmlDoc::UnknownElement unknown(body);
    bool ok = (unknown.name().empty() && (unknown.body() == body));
    CPPUNIT_ASSERT(ok);

    // Indent is in effect for an unknown element only if there are siblings.
    ok = (unknown.toXml(0) == body);
    CPPUNIT_ASSERT(ok);
    unknown.reset("<>");
    ok = (unknown.toXml(3) == "<>");
    CPPUNIT_ASSERT(ok);

    XmlElement& e = unknown;
    e.setBody("still-unknown");
    ok = (unknown.body() == "still-unknown");
    CPPUNIT_ASSERT(ok);

    ok = (!unknown.giveBirth(new XmlDoc::UnknownElement(String())));
    CPPUNIT_ASSERT(ok);
}
