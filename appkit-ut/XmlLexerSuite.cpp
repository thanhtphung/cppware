#include "appkit/XmlElement.hpp"
#include "appkit/XmlLexer.hpp"
#include "syskit/MappedTxtFile.hpp"
#include "syskit/Utf8.hpp"

#include "appkit-ut-pch.h"
#include "XmlLexerSuite.hpp"

using namespace appkit;
using namespace syskit;

typedef struct
{
    const char* native;
    const char* xml;
} sample_t;

const char INVALID[] =
"&%&quot&az;&amy;&apx;&#w;&#12345678;&#55450;&gv;&#x123456789;&#xd89a;&#x;&lu;&#t;"
"&qt;&qus;&quor;&#x123q;";

const sample_t SAMPLE[] =
{
    {"", ""},
    {"abc-123", "abc-123"},
    {"abc&123", "abc&amp;123"},
    {"\"&'<>", "&quot;&amp;&apos;&lt;&gt;"},
    {"\01\02\03\04\05\06\07\010", "&#1;&#2;&#3;&#4;&#5;&#6;&#7;&#8;"},
    {"\011\012\013\014\015\016\017\020", "&#9;&#10;&#11;&#12;&#13;&#14;&#15;&#16;"},
    {"\021\022\023\024\025\026\027\030", "&#17;&#18;&#19;&#20;&#21;&#22;&#23;&#24;"},
    {"\031\032\033\034\035\036\037 ", "&#25;&#26;&#27;&#28;&#29;&#30;&#31; "}
};

const size_t NUM_SAMPLES = sizeof(SAMPLE) / sizeof(SAMPLE[0]);


XmlLexerSuite::XmlLexerSuite()
{
}


XmlLexerSuite::~XmlLexerSuite()
{
}


//
// Interfaces under test:
// - String XmlLexer::escape();
//
void XmlLexerSuite::testEscape00()
{
    String native;
    String xml;
    bool ok = true;
    for (unsigned int i = 0; i < NUM_SAMPLES; ++i)
    {
        native = SAMPLE[i].native;
        xml = XmlLexer::escape(native);
        if (xml != SAMPLE[i].xml)
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - XmlDoc* XmlLexer::scan();
// - void XmlLexer::reset(const utf8_t*, size_t);
//
void XmlLexerSuite::testScan00()
{
    const utf8_t COMMENT_ORPHAN[] = "<!--no prolog-->";
    XmlLexer lexer(COMMENT_ORPHAN, sizeof(COMMENT_ORPHAN) - 1);
    XmlDoc* doc = lexer.scan();
    bool ok = (!doc->isOk());
    CPPUNIT_ASSERT(ok);
    delete doc;

    const utf8_t CDATA_ORPHAN[] = "<![CDATA[Cdata ([[]]) ]]>";
    lexer.reset(CDATA_ORPHAN, sizeof(CDATA_ORPHAN) - 1);
    doc = lexer.scan();
    ok = (lexer.lexee() == CDATA_ORPHAN) && (!doc->isOk());
    CPPUNIT_ASSERT(ok);
    delete doc;

    const utf8_t BAD_NAME[] = "<#root comment='invalid-name'/>";
    lexer.reset(BAD_NAME, sizeof(BAD_NAME) - 1);
    doc = lexer.scan();
    ok = (lexer.lexee() == BAD_NAME) && (!doc->isOk());
    CPPUNIT_ASSERT(ok);
    delete doc;

    const utf8_t BAD_COMMENT[] = "<!--no end--:>";
    lexer.reset(BAD_COMMENT, sizeof(BAD_COMMENT) - 1);
    doc = lexer.scan();
    ok = (!doc->isOk());
    CPPUNIT_ASSERT(ok);
    delete doc;

    const utf8_t BAD_UTF8A[] = "\x80";
    lexer.reset(BAD_UTF8A, sizeof(BAD_UTF8A) - 1);
    doc = lexer.scan();
    ok = (!doc->isOk());
    CPPUNIT_ASSERT(ok);
    delete doc;

    const utf8_t BAD_UTF8B[] = "<a\x80>";
    lexer.reset(BAD_UTF8B, sizeof(BAD_UTF8B) - 1);
    doc = lexer.scan();
    ok = (!doc->isOk());
    CPPUNIT_ASSERT(ok);
    delete doc;
}


//
// Interfaces under test:
// - XmlDoc* XmlLexer::scan();
//
void XmlLexerSuite::testScan01()
{
    MappedTxtFile src(L"../../../etc/bad-sample.xml");
    XmlLexer lexer(src.image(), static_cast<size_t>(src.size()));
    XmlDoc* doc = lexer.scan();
    bool ok = doc->isOk();
    CPPUNIT_ASSERT(ok);

    const XmlElement& root = doc->root();
    size_t numKids = root.numKids();
    for (size_t i = 0; i < numKids; ++i)
    {
        if (!root.kid(i).name().empty())
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    delete doc;
}


//
// Interfaces under test:
// - XmlDoc* XmlLexer::scan();
// - void XmlLexer::reset(const String&);
// - void XmlLexer::reset(const std::istream&);
// - void XmlLexer::reset(const utf8_t*, size_t);
//
void XmlLexerSuite::testScan02()
{
    const utf8_t DECL0[] = "<?xml version='version' standalone='yes'?>";
    XmlLexer lexer(DECL0, sizeof(DECL0) - 1);
    XmlDoc* doc = lexer.scan();
    bool ok = doc->isOk();
    CPPUNIT_ASSERT(ok);
    delete doc;

    std::istringstream iss(std::string("<?xml <!--not-good-->>"));
    lexer.reset(iss);
    doc = lexer.scan();
    ok = (!doc->isOk());
    CPPUNIT_ASSERT(ok);
    delete doc;

    const utf8_t DECL1[] = "<?xml version='?'?>";
    lexer.reset(DECL1, sizeof(DECL1) - 1);
    doc = lexer.scan();
    ok = doc->isOk();
    CPPUNIT_ASSERT(ok);
    delete doc;

    const utf8_t DECL2[] = "<?xml encoding='?'?>";
    lexer.reset(DECL2, sizeof(DECL2) - 1);
    doc = lexer.scan();
    ok = (!doc->isOk());
    CPPUNIT_ASSERT(ok);
    delete doc;

    // Non-ASCII XML declaration.
    String decl("<?xml version='1.1' encoding=");
    unsigned int value = 0x80U;
    Utf8 c(value);
    decl += c;
    lexer.reset(iss); //deep copy after shallow copy -- mainly for better test coverage
    lexer.reset(iss); //deep copy before shallow copy -- mainly for better test coverage
    lexer.reset(decl);
    doc = lexer.scan();
    ok = (!doc->isOk());
    CPPUNIT_ASSERT(ok);
    delete doc;
}


//
// Interfaces under test:
// - XmlDoc* XmlLexer::scan();
//
void XmlLexerSuite::testScan03()
{
    const utf8_t EMPTY_ROOT[] = "<empty-root/>";
    XmlLexer lexer(EMPTY_ROOT, sizeof(EMPTY_ROOT) - 1);
    XmlDoc* doc = lexer.scan();
    bool ok = (doc->isOk() && (doc->toXml() == "<empty-root/>\n"));
    CPPUNIT_ASSERT(ok);
    delete doc;

    const utf8_t EXTRA_SPACES[] = "<a> \r\n\t need \r\nsome\t condensing\r\n\t</a>";
    lexer.reset(EXTRA_SPACES, sizeof(EXTRA_SPACES) - 1);
    doc = lexer.scan();
    ok = (doc->isOk() && (doc->toXml() == "<a>need some condensing</a>\n"));
    CPPUNIT_ASSERT(ok);
    delete doc;
}


//
// Interfaces under test:
// - String XmlLexer::unescape(const String&);
//
void XmlLexerSuite::testUnescape00()
{
    String native;
    String xml;
    bool ok = true;
    for (unsigned int i = 0; i < NUM_SAMPLES; ++i)
    {
        xml = SAMPLE[i].xml;
        native = XmlLexer::unescape(xml);
        if (native != SAMPLE[i].native)
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
}


//
// Resolution of hexadecimal escape sequences.
//
// Interfaces under test:
// - String XmlLexer::unescape(const String&);
//
void XmlLexerSuite::testUnescape01()
{
    const utf32_t RESULT[] = {0x1U, 0x23U, 0x456U, 0x789abU};
    const size_t NUM_RESULTS = sizeof(RESULT) / sizeof(RESULT[0]);
    String xml("&#x1;&#X23;&#x456;&#x789ab;");
    String native = XmlLexer::unescape(xml);

    bool ok = true;
    for (unsigned int i = 0; i < NUM_RESULTS; ++i)
    {
        if (native[i] != RESULT[i])
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
}


//
// Pass-through of invalid escape sequences.
//
// Interfaces under test:
// - String XmlLexer::unescape(const String&);
//
void XmlLexerSuite::testUnescape02()
{
    String xml(INVALID);
    String native = XmlLexer::unescape(xml);
    bool ok = (native == xml);
    CPPUNIT_ASSERT(ok);
}


void XmlLexerSuite::testValidateElementName00()
{
    String name("member;range=0-1499");
    bool ok = (!XmlLexer::validateElementName(name));
    CPPUNIT_ASSERT(ok);

    name = "member";
    ok = XmlLexer::validateElementName(name);
    CPPUNIT_ASSERT(ok);

    name = ";member;range=0-1499";
    ok = (!XmlLexer::validateElementName(name));
    CPPUNIT_ASSERT(ok);

    name.reset();
    ok = XmlLexer::validateElementName(name);
    CPPUNIT_ASSERT(ok);

    name = ":";
    ok = XmlLexer::validateElementName(name);
    CPPUNIT_ASSERT(ok);
}
