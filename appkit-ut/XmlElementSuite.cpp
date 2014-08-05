#include "appkit/XmlDoc.hpp"
#include "appkit/XmlElement.hpp"
#include "appkit/XmlLexer.hpp"

#include "appkit-ut-pch.h"
#include "XmlElementSuite.hpp"

using namespace appkit;
using namespace syskit;


XmlElementSuite::XmlElementSuite()
{
}


XmlElementSuite::~XmlElementSuite()
{
}


bool XmlElementSuite::cb0(void* arg, const appkit::XmlElement& element)
{
    bool keepGoing = (arg == 0)? (false): (element.mom() == 0);
    return keepGoing;
}


bool XmlElementSuite::cb1(void* arg, const appkit::XmlElement& element)
{
    String* s = static_cast<String*>(arg);
    *s += element.name();

    bool keepGoing = true;
    return keepGoing;
}


void XmlElementSuite::testCtor00()
{
    String name("name");
    XmlElement element(name, 0, 0);
    bool ok = ((element.name() == name) && element.body().empty());
    CPPUNIT_ASSERT(ok);
    ok = ((element.numAttrs() == 0) && (element.numKids() == 0) && (element.numSibs() == 0));
    CPPUNIT_ASSERT(ok);
    ok = ((element.mom() == 0) && (element.nextSib() == 0) && (element.prevSib() == 0));
    CPPUNIT_ASSERT(ok);
    ok = (element.toXml(0) == "<name>\n</name>");
    CPPUNIT_ASSERT(ok);
    ok = (element.toXml(4) == "    <name>\n    </name>");
    CPPUNIT_ASSERT(ok);

    ok = (element.addAttr(new StringPair(String("n0"), String("v0"))) && (element.numAttrs() == 1));
    CPPUNIT_ASSERT(ok);
    ok = (element.toXml(0) == "<name n0=\"v0\">\n</name>");
    CPPUNIT_ASSERT(ok);

    ok = (element.addAttr(new StringPair(String("n1"), String("v\01"))) && (element.numAttrs() == 2));
    CPPUNIT_ASSERT(ok);
    ok = (element.toXml(0) == "<name n0=\"v0\" n1=\"v&#1;\">\n</name>");
    CPPUNIT_ASSERT(ok);

    ok = ((element.attr(0).n() == "n0") && (element.attr(0).v() == "v0") &&
        (element.attr(1).n() == "n1") && (element.attr(1).v() == "v\01") &&
        (element.attrN(0) == "n0") && (element.attrV(0) == "v0") &&
        (element.attrN(1) == "n1") && (element.attrV(1) == "v\01"));
    CPPUNIT_ASSERT(ok);
}


void XmlElementSuite::testCtor01()
{
    XmlElement* grandma = new XmlElement("grandma");
    XmlElement* mom = new XmlElement("mom");
    XmlElement* uncle = new XmlDoc::Content("uncle");
    XmlElement* aunt = new XmlDoc::UnknownElement("<#aunt#>");
    XmlElement* bro = new XmlDoc::Comment("bro");
    XmlElement* me = new XmlDoc::EmptyElement("me");
    XmlElement* sis = new XmlDoc::Cdata("sis");
    bool ok = grandma->giveBirth(uncle) &&
        grandma->giveBirth(mom) &&
        grandma->giveBirth(aunt) &&
        mom->giveBirth(bro) &&
        mom->giveBirth(me) &&
        mom->giveBirth(sis);
    CPPUNIT_ASSERT(ok);

    ok = ((grandma->numKids() == 3) &&
        (grandma->kid(0).body() == "uncle") &&
        (grandma->kid(1).name() == "mom") &&
        (grandma->kid(2).body() == "<#aunt#>") &&
        (grandma->body() == "uncle"));
    CPPUNIT_ASSERT(ok);

    ok = ((mom->numKids() == 3) &&
        (mom->numSibs() == 2) &&
        (uncle->numSibs() == 2) &&
        (aunt->numSibs() == 2));
    CPPUNIT_ASSERT(ok);

    ok = ((me->numSibs() == 2) &&
        (me->prevSib() == bro) &&
        (me->nextSib() == sis) &&
        (me->mom() == mom));
    CPPUNIT_ASSERT(ok);

    const XmlElement* cloned = grandma->clone();
    ok = (cloned->toXml() == "<grandma>\n"
        "  uncle\n"
        "  <mom>\n"
        "    <!--bro-->\n"
        "    <me/>\n"
        "    <![CDATA[sis]]>\n"
        "  </mom>\n"
        "  <#aunt#>\n"
        "</grandma>");
    CPPUNIT_ASSERT(ok);
    delete cloned;
    delete grandma;
}


//
// Interfaces under test:
// - XmlElement* XmlElement::detach(XmlDoc&);
//
void XmlElementSuite::testDetach00()
{
    XmlElement* mom = new XmlElement("mom");
    mom->giveBirth(new XmlElement("bro"));
    mom->giveBirth(new XmlElement("me"));
    mom->giveBirth(new XmlElement("sis"));

    // Already detached.
    XmlDoc doc;
    bool ok = (mom->detach(doc) == mom);
    CPPUNIT_ASSERT(ok);

    // Detaching the root element.
    doc.setRoot(mom);
    ok = ((mom->detach(doc) == mom) && (mom != &doc.root()));
    CPPUNIT_ASSERT(ok);

    // Detaching a non-root element.
    doc.setRoot(mom);
    delete mom->kid(1).detach(doc);
    ok = (mom->numKids() == 2);
    CPPUNIT_ASSERT(ok);
    delete mom->kid(1).detach(doc);
    ok = (mom->numKids() == 1);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - StringPair* XmlElement::findKid(const String&);
// - StringPair* XmlElement::findSib(const String&);
// - const StringPair* XmlElement::findKid(const String&) const;
// - const StringPair* XmlElement::findSib(const String&) const;
//
void XmlElementSuite::testFind00()
{
    XmlElement* grandma = new XmlElement("grandma");
    String name;
    bool ok = ((grandma->findKid(name) == 0) && (grandma->findSib(name) == 0));
    CPPUNIT_ASSERT(ok);

    grandma->giveBirth(new XmlElement("ant"));
    grandma->giveBirth(new XmlElement("mom"));
    grandma->giveBirth(new XmlElement("uncle"));
    const XmlElement* mom = grandma->findKid("mom");
    ok = ((mom != 0) && (mom->name() == "mom"));
    CPPUNIT_ASSERT(ok);

    ok = (!mom->findSib("mom"));
    CPPUNIT_ASSERT(ok);
    const XmlElement* ant = mom->findSib("ant");
    ok = ((ant != 0) && (ant->name() == "ant"));
    CPPUNIT_ASSERT(ok);
    ok = ((mom->findSib("mom") == 0) && (mom->findAttr("") == 0));
    CPPUNIT_ASSERT(ok);

    delete grandma;
}


//
// Interfaces under test:
// - StringPair* XmlElement::findAttr(const char*);
// - const StringPair* XmlElement::findAttr(const char*);
//
void XmlElementSuite::testFind01()
{
    const utf8_t ELEMENT[] = "<empty-element a=\"A\" bb=\"BB\" ccc=\"CCC\"/>";
    XmlLexer lexer(ELEMENT, sizeof(ELEMENT) - 1);
    XmlDoc* doc = lexer.scan();
    const XmlElement& root = doc->root();
    const StringPair* nv0 = root.findAttr("non-existent");
    bool ok = (nv0 == 0);
    CPPUNIT_ASSERT(ok);

    nv0 = root.findAttr("ccc");
    ok = ((nv0->n() == "ccc") && (nv0->v() == "CCC"));
    CPPUNIT_ASSERT(ok);

    StringPair* nv1 = doc->root().findAttr("bb");
    nv1->setV("not-BB");
    nv0 = (root.findAttr("bb"));
    ok = ((nv0->n() == "bb") && (nv0->v() == "not-BB"));
    CPPUNIT_ASSERT(ok);

    delete doc;
}


//
// Interfaces under test:
// - StringPair* XmlElement::findAttr(const String&);
// - const StringPair* XmlElement::findAttr(const String&);
//
void XmlElementSuite::testFind02()
{
    const utf8_t ELEMENT[] = "<empty-element a=\"A\" bb=\"BB\" ccc=\"CCC\"/>";
    XmlLexer lexer(ELEMENT, sizeof(ELEMENT) - 1);
    XmlDoc* doc = lexer.scan();
    const XmlElement& root = doc->root();
    String n("non-existent");
    const StringPair* nv0 = root.findAttr(n);
    bool ok = (nv0 == 0);
    CPPUNIT_ASSERT(ok);

    n = "ccc";
    nv0 = root.findAttr(n);
    ok = ((nv0->n() == "ccc") && (nv0->v() == "CCC"));
    CPPUNIT_ASSERT(ok);

    n = "bb";
    StringPair* nv1 = doc->root().findAttr(n);
    nv1->setV("not-BB");
    nv0 = (root.findAttr(n));
    ok = ((nv0->n() == "bb") && (nv0->v() == "not-BB"));
    CPPUNIT_ASSERT(ok);

    delete doc;
}


//
// Interfaces under test:
// - XmlElement* XmlElement::find(const String&);
// - const XmlElement* XmlElement::find(const String&) const;
//
void XmlElementSuite::testFind03()
{
    XmlElement* grandma = new XmlElement("grandma");
    grandma->giveBirth(new XmlElement("ant"));
    XmlElement* mom = new XmlElement("mom");
    grandma->giveBirth(mom);
    XmlElement* me = new XmlElement("me");
    mom->giveBirth(me);
    XmlElement* uncle = new XmlElement("uncle");
    grandma->giveBirth(uncle);

    String path;
    const XmlElement* p0 = grandma->find(path);
    XmlElement* p1 = grandma->find("/not-a-root/mom/");
    bool ok = (p0 == 0) && (p1 == 0) && (mom->find("../../../../") == 0);
    CPPUNIT_ASSERT(ok);

    p0 = grandma->find("/grandma/mom/me/../");
    p1 = mom->find("/grandma/uncle/");
    ok = (p0 == mom) && (p1 == uncle) && (uncle->find("/") == grandma);
    CPPUNIT_ASSERT(ok);

    p0 = uncle->find(".");
    p1 = mom->find("../");
    ok = (p0 == uncle) && (p1 == grandma) && (mom->find("/grandma") == grandma);
    CPPUNIT_ASSERT(ok);

    p0 = me->find("../../uncle/../mom/me/../me/.");
    p1 = mom->find("../mom/me");
    ok = (p0 == me) && (p1 == me);
    CPPUNIT_ASSERT(ok);

    delete grandma;
}


//
// Interfaces under test:
// - String XmlElement::fullName();
//
void XmlElementSuite::testFullName00()
{
    XmlElement* grandma = new XmlElement("grandma");
    bool ok = (grandma->fullName() == "/grandma");
    CPPUNIT_ASSERT(ok);

    XmlElement* mom = new XmlElement("mom");
    grandma->giveBirth(mom);
    ok = ((grandma->fullName() == "/grandma/") &&
        (mom->fullName() == "/grandma/mom"));
    CPPUNIT_ASSERT(ok);

    XmlElement* me = new XmlElement("me");
    mom->giveBirth(me);
    ok = ((grandma->fullName() == "/grandma/") &&
        (mom->fullName() == "/grandma/mom/") &&
        (me->fullName() == "/grandma/mom/me"));
    CPPUNIT_ASSERT(ok);

    void* arg = 0;
    ok = (!grandma->applyParentFirst(cb0, arg));
    CPPUNIT_ASSERT(ok);
    ok = (!grandma->applyChildFirst(cb0, arg));
    CPPUNIT_ASSERT(ok);

    String s;
    arg = &s;
    ok = (!grandma->applyParentFirst(cb0, arg));
    CPPUNIT_ASSERT(ok);
    ok = grandma->applyChildFirst(cb1, arg) && (s == "memomgrandma");
    CPPUNIT_ASSERT(ok);

    delete grandma;
}


void XmlElementSuite::testNew00()
{
    String name("e");
    XmlElement* e = new XmlElement(name);
    bool ok = (e != 0);
    CPPUNIT_ASSERT(ok);
    delete e;

    unsigned char buf[sizeof(*e)];
    e = new(buf)XmlElement(name);
    ok = (reinterpret_cast<unsigned char*>(e) == buf);
    CPPUNIT_ASSERT(ok);
    e->XmlElement::~XmlElement();
}


//
// Interfaces under test:
// - virtual void XmlElement::setBody(const String& body);
//
void XmlElementSuite::testSetBody00()
{
    XmlElement* e = new XmlElement("no-body");
    e->setBody("body");
    bool ok = e->body().empty();
    CPPUNIT_ASSERT(ok);

    XmlElement* kid = new XmlDoc::Content("kid");
    e->giveBirth(kid);
    ok = (e->body() == "kid");
    CPPUNIT_ASSERT(ok);
    e->setBody("same-kid");
    ok = (e->body() == "same-kid");
    CPPUNIT_ASSERT(ok);
    delete e;
}


void XmlElementSuite::testSize00()
{
    bool ok = (sizeof(XmlDoc::Cdata) == sizeof(void*) * 7) &&
        (sizeof(XmlDoc::Comment) == sizeof(void*) * 7) &&
        (sizeof(XmlDoc::Content) == sizeof(void*) * 7) &&
        (sizeof(XmlDoc::EmptyElement) == sizeof(void*) * 6) &&
        (sizeof(XmlDoc::UnknownElement) == sizeof(void*) * 7) &&
        (sizeof(XmlElement) == sizeof(void*) * 6); //Win32:24 x64:48
    CPPUNIT_ASSERT(ok);
}


void XmlElementSuite::testStringPair00()
{
    String n("name");
    StringPair nv(n);
    bool b = true;
    nv.setV(b);
    bool ok = (nv.k() == "name") && (nv.n() == "name") && (nv.v() == "true") && nv.vAsBool();
    CPPUNIT_ASSERT(ok);

    StringPair nv1(nv);
    ok = (nv1 == nv);
    CPPUNIT_ASSERT(ok);
    nv1 = nv1; //no-op
    ok = (nv1 == nv);
    CPPUNIT_ASSERT(ok);

    String stringV("a-string-value");
    nv.setV(stringV);
    ok = (nv.v() == stringV) && (nv != nv1);
    CPPUNIT_ASSERT(ok);

    const char* s = "null-terminated-string";
    nv.setV(s);
    ok = (nv.v() == s);
    CPPUNIT_ASSERT(ok);

    double d64 = 1.23456789012345;
    nv.setV(d64);
    ok = (nv.v() == "1.23456789012345") && (nv.vAsD64() == 1.23456789012345);
    CPPUNIT_ASSERT(ok);

    float f32 = 1.23456f;
    nv.setV(f32);
    ok = (nv.v() == "1.23456") && (nv.vAsF32() == 1.23456f);
    CPPUNIT_ASSERT(ok);

    int s32 = -123;
    nv.setV(s32);
    ok = (nv.v() == "-123") && (nv.vAsS32() == -123);
    CPPUNIT_ASSERT(ok);

    unsigned int u32 = 123;
    nv.setV(u32);
    ok = (nv.v() == "123") && (nv.vAsU32() == 123);
    CPPUNIT_ASSERT(ok);

    unsigned long long u64 = 12345678987654321ULL;
    nv.setV(u64);
    ok = (nv.v() == "12345678987654321") && (nv.vAsU64() == 12345678987654321ULL);
    CPPUNIT_ASSERT(ok);

    unsigned short u16 = 9876;
    nv.setV(u16);
    ok = (nv.v() == "9876") && (nv.vAsU16() == 9876);
    CPPUNIT_ASSERT(ok);
    stringV = nv.v();
    nv.addV(stringV, ':');
    ok = (nv.v() == "9876:9876");
    CPPUNIT_ASSERT(ok);

    nv = nv1;
    ok = (nv == nv1);
    CPPUNIT_ASSERT(ok);
}
