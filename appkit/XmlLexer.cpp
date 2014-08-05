/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/Utf8.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/U8.hpp"
#include "appkit/XmlDoc.hpp"
#include "appkit/XmlElement.hpp"
#include "appkit/XmlLexer.hpp"
#include "appkit/XmlProlog.hpp"
#include "appkit/std.hpp"

using namespace syskit;

const char EQUALS_SIGN = '=';
const char NULL_CHAR = 0;
const char SINGLE_QUOTE = '\'';
const char SLASH = '/';
const size_t MAX_DIGITS = 7;
const size_t MAX_XDIGITS = 8;

// Pre-defined escaped sequences. Indexed by special character. Zero means
// no pre-defined escape sequence for given special character. If non-zero,
// first byte specifies escape sequence length.
const char* const ESC_SEQ[] =
{
    0, "\05&#1;", "\05&#2;", "\05&#3;", "\05&#4;", "\05&#5;", "\05&#6;", "\05&#7;",
    "\05&#8;", "\05&#9;", "\06&#10;", "\06&#11;", "\06&#12;", "\06&#13;", "\06&#14;", "\06&#15;",
    "\06&#16;", "\06&#17;", "\06&#18;", "\06&#19;", "\06&#20;", "\06&#21;", "\06&#22;", "\06&#23;",
    "\06&#24;", "\06&#25;", "\06&#26;", "\06&#27;", "\06&#28;", "\06&#29;", "\06&#30;", "\06&#31;",
    0, 0, "\07&quot;", 0, 0, 0, "\06&amp;", "\07&apos;",
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, "\05&lt;", 0, "\05&gt;"
};

//
// Reset string str with given UTF8 sequence (numU8s bytes starting at s).
// Given sequence is known to be ASCII if isAscii is positive, to be non-ASCII
// if isAscii is zero, and to contain some invalid bytes otherwise.
//
static void resetStr(appkit::String& str, int isAscii, const syskit::utf8_t* s, size_t numU8s)
{
    if (isAscii > 0) //ascii
    {
        str.reset(s, numU8s, numU8s);
    }
    else //non-ascii or containing invalid bytes
    {
        str.reset8(s, numU8s);
    }
}

BEGIN_NAMESPACE1(appkit)

//
// isXxx map.
// Indexed by unsigned char value.
// Generated using the following code.
//
// enum
// {
//   DigitM=0x01, EqOrSpaceZM=0x02, Quote1ZM=0x04, Quote2ZM=0x08,
//   QuoteZM=0x10, SpaceM=0x20, SpaceZM=0x40
// };
//
// unsigned char m[256] = {0};
// for (unsigned int i='0'; i<='9'; m[i++]|=DigitM);
// m['='] |= EqOrSpaceZM;
// m[' '] |= EqOrSpaceZM | SpaceM | SpaceZM;
// m['\t'] |= EqOrSpaceZM | SpaceM | SpaceZM;
// m['\r'] |= EqOrSpaceZM | SpaceM | SpaceZM;
// m['\n'] |= EqOrSpaceZM | SpaceM | SpaceZM;
// m['>'] |= EqOrSpaceZM | Quote1ZM | Quote2ZM | QuoteZM | SpaceZM;
// m['\''] |= Quote1ZM | QuoteZM;
// m['\"'] |= Quote2ZM | QuoteZM;
//
// for (unsigned int i=0, j=8; i<256; i=j, j+=8)
// {
//   printf(" ");
//   for (unsigned int k=i; k<j; ++k) printf(" 0x%02xU,", m[k]);
//   printf(" //0x%02x-0x%02x\n", i, j-1);
// }
//
const unsigned char XmlLexer::mask_[] =
{
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, //0x00-0x07
    0x00U, 0xc1U, 0xc1U, 0x00U, 0x00U, 0xc1U, 0x00U, 0x00U, //0x08-0x0f
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, //0x10-0x17
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, //0x18-0x1f
    0xc1U, 0x00U, 0x30U, 0x00U, 0x00U, 0x00U, 0x00U, 0x28U, //0x20-0x27
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x04U, 0x04U, 0x00U, //0x28-0x2f
    0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, //0x30-0x37
    0x04U, 0x04U, 0x06U, 0x00U, 0x00U, 0x01U, 0xb9U, 0x00U, //0x38-0x3f
    0x00U, 0x06U, 0x06U, 0x06U, 0x06U, 0x06U, 0x06U, 0x06U, //0x40-0x47
    0x06U, 0x06U, 0x06U, 0x06U, 0x06U, 0x06U, 0x06U, 0x06U, //0x48-0x4f
    0x06U, 0x06U, 0x06U, 0x06U, 0x06U, 0x06U, 0x06U, 0x06U, //0x50-0x57
    0x06U, 0x06U, 0x06U, 0x00U, 0x00U, 0x00U, 0x00U, 0x06U, //0x58-0x5f
    0x00U, 0x06U, 0x06U, 0x06U, 0x06U, 0x06U, 0x06U, 0x06U, //0x60-0x67
    0x06U, 0x06U, 0x06U, 0x06U, 0x06U, 0x06U, 0x06U, 0x06U, //0x68-0x6f
    0x06U, 0x06U, 0x06U, 0x06U, 0x06U, 0x06U, 0x06U, 0x06U, //0x70-0x77
    0x06U, 0x06U, 0x06U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, //0x78-0x7f
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, //0x80-0x87
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, //0x88-0x8f
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, //0x90-0x97
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, //0x98-0x9f
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, //0xa0-0xa7
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, //0xa8-0xaf
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, //0xb0-0xb7
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, //0xb8-0xbf
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, //0xc0-0xc7
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, //0xc8-0xcf
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, //0xd0-0xd7
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, //0xd8-0xdf
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, //0xe0-0xe7
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, //0xe8-0xef
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, //0xf0-0xf7
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U  //0xf8-0xff
};


//!
//! Construct an XML lexical analyzer with given analyzee.
//!
XmlLexer::XmlLexer(const String& lexee):
origLexee_(lexee)
{

    // Make a shallow copy of the analyzee.
    copyMade_ = false;
    lexee_ = origLexee_.raw();
    numU8s_ = origLexee_.byteSize();

    // Initialize kb_ when used.
}


//!
//! Construct an XML lexical analyzer with given seekable analyzee.
//! Instance holds a deep copy of the analyzee.
//!
XmlLexer::XmlLexer(const std::istream& lexee):
origLexee_()
{
    copyStream(lexee);

    // Initialize kb_ when used.
}


//!
//! Construct an XML lexical analyzer with given analyzee (byteSize bytes
//! starting at lexee). Instance holds a shallow copy of the analyzee.
//!
XmlLexer::XmlLexer(const utf8_t* lexee, size_t byteSize):
origLexee_()
{

    // Make a shallow copy of the analyzee.
    copyMade_ = false;
    lexee_ = lexee;
    numU8s_ = static_cast<unsigned int>(byteSize);

    // Initialize kb_ when used.
}


XmlLexer::~XmlLexer()
{
    if (copyMade_)
    {
        delete lexee_;
    }
}


//!
//! Form and return XML string from given native string by converting special
//! characters to XML escape sequences. For example, "abc&123" --> "abc&amp;123";
//!
String XmlLexer::escape(const String& native)
{

    // Return copy of empty string.
    if (native.empty())
    {
        return native;
    }

    // Look for pre-defined special characters that must be converted into XML
    // escape sequences. Copy characters in bulk when possible. Avoid copying
    // if no conversion occurs.
    String xml;
    Utf8 c;
    const utf8_t* p = native.raw();
    const utf8_t* p0 = 0;
    size_t numChars = 0;
    size_t numU8s = 0;
    for (size_t bytesDecoded;; p += bytesDecoded, ++numChars, numU8s += bytesDecoded)
    {

        bytesDecoded = c.decode(p);
        if (c == 0)
        {
            if (p0 > native.raw())
            {
                xml.append(p0, numU8s, numChars);
            }
            break;
        }

        // Conversion required for pre-defined special character.
        if (c <= '>')
        {
            const char* escSeq = ESC_SEQ[c];
            if (escSeq != 0)
            {
                if (p0 != 0)
                {
                    xml.append(p0, numU8s, numChars);
                    p0 = 0;
                }
                xml.append(escSeq + 1, *escSeq);
                continue;
            }
        }

        // No conversion required for all other characters.
        if (p0 == 0)
        {
            p0 = p;
            numChars = 0;
            numU8s = 0;
        }
    }

    // Return XML form.
    return xml.empty()? native: xml;
}


//!
//! Form and return native string from given XML string by converting XML escape
//! sequences to special characters. For example, "abc&amp;123" --> "abc&123";
//! Invalid escape sequences are allowed to pass through. For example, "&%" --> "&%";
//!
String XmlLexer::unescape(const String& xml)
{

    // Return copy of empty string.
    if (xml.empty())
    {
        return xml;
    }

    // Resolve XML escape sequences using a hard-coded finite state automaton.
    // Copy characters in bulk when possible. Avoid copying if no conversion
    // occurs.
    String native;
    Utf8 c;
    escKb_t kb = {&native, 0, xml.raw(), 0, unescape0, 0, 0, 0};
    for (size_t bytesDecoded;; kb.curByte += bytesDecoded, ++kb.numChars, kb.numU8s += bytesDecoded)
    {

        bytesDecoded = c.decode(kb.curByte);
        if (c == 0)
        {
            if (kb.p0 > xml.raw())
            {
                native.append(kb.p0, kb.numU8s, kb.numChars);
            }
            break;
        }

        kb.unescape(kb, c);
    }

    // Return native form.
    return native.empty()? xml: native;
}


size_t XmlLexer::countInvalidBytes(const utf8_t* s, size_t numU8s)
{
    size_t length;
    const utf8_t* p = s;
    const utf8_t* pEnd = p + numU8s;
    do
    {
        length = Utf8::getSeqLength(*p);
        if ((length >= Utf8::MinSeqLength) && (length <= Utf8::MaxSeqLength))
        {
            break;
        }
    } while (++p < pEnd);

    size_t invalidBytes = p - s;
    return invalidBytes;
}


//
// Current byte is a space.
// Fast forward through subsequent spacing, if any.
//
void XmlLexer::skipSpaces()
{
    const utf8_t* p = kb_.curByte + 1;
    for (size_t remainingBytes = kb_.remainingBytes - 1; remainingBytes > 0; ++p, --remainingBytes)
    {
        if (!isSpace(*p))
        {
            size_t fastForwardBy = p - kb_.curByte - 1;
            kb_.curByte += fastForwardBy;
            kb_.remainingBytes -= fastForwardBy;
            break;
        }
    }
}


//!
//! Scan the XML lexical analyzee and form and return an appropriate document.
//! The returned document is constructed for the caller. Caller must destroy
//! the document using the delete operator when done. The scan can fail if the
//! XML input cannot be interpreted properly. To validate the returned document,
//! use XmlDoc::isOk() and XmlDoc::errDesc().
//!
XmlDoc* XmlLexer::scan()
{
    Utf8 c8;
    kb_.reset(&XmlLexer::scan0, lexee_, numU8s_);
    for (size_t bytesDecoded = 0; kb_.remainingBytes > 0; kb_.curByte += bytesDecoded, kb_.remainingBytes -= bytesDecoded)
    {

        bytesDecoded = c8.decode(kb_.curByte, kb_.remainingBytes);
        if (bytesDecoded == 0)
        {
            c8.resetWithValidChar(Utf8::DefaultChar);
            kb_.isAscii = -1;
            bytesDecoded = countInvalidBytes(kb_.curByte, kb_.remainingBytes);
        }
        else if ((bytesDecoded > 1) && (kb_.isAscii > 0))
        {
            kb_.isAscii = 0;
        }

        utf32_t c = c8;
        (this->*kb_.scan)(c);
    }

    if ((kb_.root != 0) && (kb_.scanErr == 0) && (kb_.scan != &XmlLexer::scanEpilog))
    {
        kb_.scanErr = "some element not properly terminated";
    }

    XmlDoc* doc = new XmlDoc(kb_.prolog, kb_.root);
    doc->errDesc_ = errDesc();
    return doc;
}


//
// Make element from given UTF-8 sequence ("<name...>" or "<name.../>").
//
XmlElement* XmlLexer::mkElement(const utf8_t* lAngle, const utf8_t* rAngle, size_t ampCount, int isAscii)
{

    // Element name. Element name has been validated.
    attrBound_t bound;
    bound.n0 = lAngle + 1;
    const utf8_t* space;
    for (space = bound.n0 + 1; (!isSpaceZ(*space)); ++space);
    size_t numU8s = space - bound.n0;
    String n;
    resetStr(n, isAscii, bound.n0, numU8s);

    // Empty or non-empty element?
    XmlElement* baby;
    if (rAngle[-1] == SLASH)
    {
        if (space == rAngle)
        {
            n.truncate(n.length() - 1);
        }
        baby = new XmlDoc::EmptyElement(n);
    }
    else
    {
        baby = new XmlElement(n);
    }

    // Attributes.
    if (space < rAngle)
    {
        for (String v; findAttrBound(bound, space, rAngle); space = bound.v1 + 1)
        {
            numU8s = bound.n1 - bound.n0 + 1;
            resetStr(n, isAscii, bound.n0, numU8s);
            numU8s = bound.v1 - bound.v0 - 1;
            resetStr(v, isAscii, bound.v0 + 1, numU8s);
            if (ampCount > 0)
            {
                v = unescape(v);
            }
            StringPair* nv = new StringPair(n, v);
            baby->addAttr(nv);
        }
        baby->freezeAttrs();
    }

    // Return new element.
    return baby;
}


//
// Replace consecutive embedded spaces with a single blank. Assume no
// leading spaces. Trim trailing spaces. Given content is known to be
// ASCII. Return result as a content element.
//
XmlElement* XmlLexer::normalizeSpace(const utf8_t* s8, size_t numU8s)
{

    // Copy characters in bulk when possible.
    String result;
    bool noAmps = true;
    size_t spaceCount = 0;
    const utf8_t* p = s8;
    const utf8_t* notCopiedYet0 = p;
    const utf8_t* notCopiedYet1 = notCopiedYet0 - 1;
    for (const utf8_t* pEnd = p + numU8s; p < pEnd; ++p)
    {

        if (isSpace(*p))
        {
            ++spaceCount;
            continue;
        }

        if (*p == '&')
        {
            noAmps = false;
        }

        if (spaceCount == 0)
        {
            ++notCopiedYet1;
        }
        else if ((spaceCount == 1) && (p[-1] == ' '))
        {
            notCopiedYet1 += 2;
        }
        else
        {
            size_t n = notCopiedYet1 - notCopiedYet0 + 1;
            result.append(notCopiedYet0, n, n);
            result.append(1, ' ');
            notCopiedYet0 = p;
            notCopiedYet1 = notCopiedYet0;
        }
        spaceCount = 0;
    }

    // Need full copy now?
    // Need partial copy of the tail end?
    size_t n = notCopiedYet1 - notCopiedYet0 + 1;
    result.append(notCopiedYet0, n, n);

    // Return new content element.
    XmlElement* baby = new XmlDoc::Content(noAmps? result: unescape(result));
    return baby;
}


//
// Replace consecutive embedded spaces with a single blank. Assume no
// leading spaces. Trim trailing spaces. Given content is known to have
// non-ASCII characters or to contain some invalid bytes. Return result
// as a content element.
//
XmlElement* XmlLexer::normalizeSpace8(const utf8_t* s8, size_t numU8s)
{
    Utf8 c8;
    Utf8Seq seq;
    char blank = 0;
    bool noAmps = true;
    const utf8_t* p = s8;
    for (size_t bytesDecoded, remainingBytes = numU8s; remainingBytes > 0; p += bytesDecoded, remainingBytes -= bytesDecoded)
    {
        bytesDecoded = c8.decode(p, remainingBytes);
        if (bytesDecoded == 0) c8.resetWithValidChar(Utf8::DefaultChar), bytesDecoded = countInvalidBytes(p, remainingBytes);
        utf32_t c = c8;
        switch (c)
        {
        case ' ':
        case '\n':
        case '\r':
        case '\t':
            blank = ' ';
            break;
        case '&':
            noAmps = false;
            // no break
        default:
            if (blank != 0)
            {
                seq += blank;
                blank = 0;
            }
            seq += c8;
            break;
        }
    }

    // Return new content element.
    seq += NULL_CHAR;
    unsigned int byteSize;
    size_t numChars = seq.numChars();
    utf8_t* s = seq.detachRaw(byteSize);
    String result;
    result.attachRaw(s, byteSize, numChars);
    XmlElement* baby = new XmlDoc::Content(noAmps? result: unescape(result));
    return baby;
}


//
// Make prolog from given ASCII sequence representing the XML declaration
// part ("<?xml .*?>"). Return zero if the required version attribute is
// not specified.
//
XmlProlog* XmlLexer::mkProlog(const utf8_t* lAngle, const utf8_t* rAngle)
{
    XmlProlog* prolog;
    attrBound_t bound;
    if (findAttrBound(bound, lAngle + 5, rAngle) && (memcmp(bound.n0, "version", 7) == 0))
    {
        String encoding;
        String standalone;
        String version;
        version.reset8(bound.v0 + 1, bound.v1 - bound.v0 - 1);
        if (findAttrBound(bound, bound.v1 + 1, rAngle))
        {
            if (memcmp(bound.n0, "encoding", 8) == 0)
            {
                encoding.reset8(bound.v0 + 1, bound.v1 - bound.v0 - 1);
                if (findAttrBound(bound, bound.v1 + 1, rAngle) && (memcmp(bound.n0, "standalone", 10) == 0))
                {
                    standalone.reset8(bound.v0 + 1, bound.v1 - bound.v0 - 1);
                }
            }
            else if (memcmp(bound.n0, "standalone", 10) == 0)
            {
                standalone.reset8(bound.v0 + 1, bound.v1 - bound.v0 - 1);
            }
        }
        prolog = new XmlProlog(version, encoding, standalone);
    }

    else
    {
        prolog = 0;
    }

    return prolog;
}


//
// Find attribute boundaries.
// Return true if successful.
//
bool XmlLexer::findAttrBound(attrBound_t& attrBound, const utf8_t* space, const utf8_t* rAngle)
{

    // Attribute name.
    attrBound_t bound;
    for (bound.n0 = space + 1; isSpace(*bound.n0); ++bound.n0);
    if (bound.n0 == rAngle) return false; //no name-start
    const utf8_t* eq;
    for (eq = bound.n0 + 1; (!isEqOrSpaceZ(*eq)); ++eq);
    if (eq == rAngle) return false; //no name-end
    bound.n1 = eq - 1;

    // Equals sign.
    if (*eq != EQUALS_SIGN)
    {
        for (; isSpace(*eq); ++eq);
        if (*eq != EQUALS_SIGN) return false; //no equals sign?
    }

    // Attribute value.
    for (bound.v0 = eq + 1; (!isQuoteZ(*bound.v0)); ++bound.v0);
    if (bound.v0 == rAngle) return false; //no value-start?
    bound.v1 = bound.v0 + 1;
    if (*bound.v0 == SINGLE_QUOTE) for (; (!isQuote1Z(*bound.v1)); ++bound.v1);
    else for (; (!isQuote2Z(*bound.v1)); ++bound.v1);

    // Return result.
    attrBound = bound;
    return true;
}


bool XmlLexer::isValidName(utf32_t c)
{

    // [-.:_A-Za-z0-9]
    if ((c <= MaxAscii) && (mask_[c] & NameM))
    {
        return true;
    }

    // #x00B7
    // #x0300-#x036F
    // #x203F-#x2040
    // #x00C0-#x00D6
    // #x00D8-#x00F6
    // #x00F8-#x02FF
    // #x0370-#x037D
    // #x037F-#x1FFF
    // #x200C-#x200D
    // #x2070-#x218F
    // #x2C00-#x2FEF
    // #x3001-#xD7FF
    // #xF900-#xFDCF
    // #xFDF0-#xFFFD
    // #x10000-#xEFFFF
    return (c == 0x00B7) ||
        ((c >= 0x0300) && (c <= 0x036F)) ||
        ((c >= 0x203F) && (c <= 0x2040)) ||
        ((c >= 0x00C0) && (c <= 0x00D6)) ||
        ((c >= 0x00D8) && (c <= 0x00F6)) ||
        ((c >= 0x00F8) && (c <= 0x02FF)) ||
        ((c >= 0x0370) && (c <= 0x037D)) ||
        ((c >= 0x037F) && (c <= 0x1FFF)) ||
        ((c >= 0x200C) && (c <= 0x200D)) ||
        ((c >= 0x2070) && (c <= 0x218F)) ||
        ((c >= 0x2C00) && (c <= 0x2FEF)) ||
        ((c >= 0x3001) && (c <= 0xD7FF)) ||
        ((c >= 0xF900) && (c <= 0xFDCF)) ||
        ((c >= 0xFDF0) && (c <= 0xFFFD)) ||
        ((c >= 0x10000) && (c <= 0xEFFFF));
}


bool XmlLexer::isValidName0(utf32_t c)
{

    // [:_A-Za-z]
    if ((c <= MaxAscii) && (mask_[c] & Name0M))
    {
        return true;
    }

    // #x00C0-#x00D6
    // #x00D8-#x00F6
    // #x00F8-#x02FF
    // #x0370-#x037D
    // #x037F-#x1FFF
    // #x200C-#x200D
    // #x2070-#x218F
    // #x2C00-#x2FEF
    // #x3001-#xD7FF
    // #xF900-#xFDCF
    // #xFDF0-#xFFFD
    // #x10000-#xEFFFF
    return ((c >= 0x00C0) && (c <= 0x00D6)) ||
        ((c >= 0x00D8) && (c <= 0x00F6)) ||
        ((c >= 0x00F8) && (c <= 0x02FF)) ||
        ((c >= 0x0370) && (c <= 0x037D)) ||
        ((c >= 0x037F) && (c <= 0x1FFF)) ||
        ((c >= 0x200C) && (c <= 0x200D)) ||
        ((c >= 0x2070) && (c <= 0x218F)) ||
        ((c >= 0x2C00) && (c <= 0x2FEF)) ||
        ((c >= 0x3001) && (c <= 0xD7FF)) ||
        ((c >= 0xF900) && (c <= 0xFDCF)) ||
        ((c >= 0xFDF0) && (c <= 0xFFFD)) ||
        ((c >= 0x10000) && (c <= 0xEFFFF));
}


//!
//! Return true if given name is a valid element name.
//!
bool XmlLexer::validateElementName(const String& elementName)
{
    if (elementName.empty())
    {
        bool ok = true;
        return ok;
    }

    utf32_t c = elementName[0];
    if (!isValidName0(c))
    {
        bool ok = false;
        return ok;
    }

    bool ok;
    size_t length = elementName.length();
    if (length > 1)
    {
        const Utf8Seq& s = elementName.asUtf8Seq();
        size_t badCharFoundAt;
        ok = s.applyLoToHi(validateElementName, &badCharFoundAt) || (badCharFoundAt == length);
    }
    else
    {
        ok = true;
    }

    return ok;
}


//
// Return true if given character is valid for a non-leading character in an element name.
//
bool XmlLexer::validateElementName(void* arg, size_t index, utf32_t c)
{
    bool keepGoing = isValidName(c);
    if (!keepGoing)
    {
        size_t* badCharFoundAt = static_cast<size_t*>(arg);
        *badCharFoundAt = index;
    }

    return keepGoing;
}


void XmlLexer::addElement(XmlElement* baby)
{

    // Baby element belongs to current mom.
    if (kb_.mom != 0)
    {
        kb_.mom->giveBirth(baby);
    }

    // Assume baby element is part of the prolog.
    else if ((kb_.root == 0) && (kb_.prolog != 0))
    {
        kb_.prolog->addItem(baby);
    }

    // Abort.
    else
    {
        ++kb_.orphanCount;
        delete baby;
    }
}


//
// An escape sequence was resolved into given character.
// Append it to the native form.
//
void XmlLexer::appendEsc(escKb_t& kb, char esc)
{
    if (kb.p0 != 0)
    {
        size_t escSeqLength = kb.curByte - kb.amp;
        kb.native->append(kb.p0, kb.numU8s - escSeqLength, kb.numChars - escSeqLength);
        kb.p0 = 0;
    }

    *kb.native += esc;
}


//
// An escape sequence was resolved into given character.
// Append it to the native form.
//
void XmlLexer::appendEsc(escKb_t& kb, utf32_t esc)
{
    if (kb.p0 != 0)
    {
        size_t escSeqLength = kb.curByte - kb.amp;
        kb.native->append(kb.p0, kb.numU8s - escSeqLength, kb.numChars - escSeqLength);
        kb.p0 = 0;
    }

    Utf8 esc8;
    esc8.resetWithValidChar(esc);
    *kb.native += esc8;
}


void XmlLexer::copyStream(const std::istream& lexee)
{

    // Make a deep copy of the analyzee.
    std::streambuf* buf = lexee.rdbuf();
    std::streamsize numU8s = buf->pubseekoff(0, std::ios::end, std::ios::in);
    utf8_t* cloned = new utf8_t[static_cast<unsigned int>(numU8s)];
    buf->pubseekpos(0, std::ios::in);
    sgetn(buf, reinterpret_cast<char*>(cloned), numU8s);
    copyMade_ = true;
    lexee_ = cloned;
    numU8s_ = static_cast<unsigned int>(numU8s);
}


void XmlLexer::popElement(const utf8_t* lAngle, const utf8_t* /*rAngle*/)
{
    if (kb_.mom != 0)
    {
        const utf8_t* n0 = lAngle + 2;
        const utf8_t* space;
        for (space = n0; (!isSpaceZ(*space)); ++space);
        size_t numU8s = space - n0;
        String n;
        resetStr(n, kb_.isAscii, n0, numU8s);
        if (n == kb_.mom->name())
        {
            kb_.mom->sterilize();
            kb_.mom = const_cast<XmlElement*>(kb_.mom->mom());
            if (kb_.mom == 0)
            {
                kb_.scan = &XmlLexer::scanEpilog;
                kb_.unknown = kb_.curByte + 1;
            }
        }
    }
}


void XmlLexer::pushElement(XmlElement* baby)
{

    // Baby element belongs to current mom.
    if (kb_.mom != 0)
    {
        kb_.mom->giveBirth(baby);
    }

    // Baby element can serve as root element.
    else
    {
        kb_.root = baby;
    }

    kb_.mom = baby;
}


//!
//! Reset analyzer with given analyzee.
//!
void XmlLexer::reset(const String& lexee)
{
    if (copyMade_)
    {
        delete lexee_;
    }

    origLexee_ = lexee;
    copyMade_ = false;
    lexee_ = origLexee_.raw();
    numU8s_ = origLexee_.byteSize();
}


//!
//! Reset analyzer with given seekable analyzee.
//! Instance holds a deep copy of the analyzee.
//!
void XmlLexer::reset(const std::istream& lexee)
{
    if (copyMade_)
    {
        delete lexee_;
    }
    else
    {
        origLexee_.reset();
    }

    copyStream(lexee);
}


//!
//! Reset analyzer with given analyzee.
//! Instance holds a shallow copy of the analyzee.
//!
void XmlLexer::reset(const utf8_t* lexee, size_t byteSize)
{
    if (copyMade_)
    {
        delete lexee_;
    }
    else
    {
        origLexee_.reset();
    }

    copyMade_ = false;
    lexee_ = lexee;
    numU8s_ = static_cast<unsigned int>(byteSize);
}


//
// Initial state. Ignore space. Look for left-angle indicating start of lexemes,
// and look for right-angle indicating end of unknown lexemes.
//
void XmlLexer::scan0(utf32_t c)
{

    // If a space is seen at this state, it's reasonable to assume they come in bunches.
    // Fast forward through them.
    if ((c <= MaxAscii) && isSpace(static_cast<utf8_t>(c)))
    {
        skipSpaces();
        return;
    }

    switch (c)
    {
    case '<':
        if (kb_.unknown != 0)
        {
            kb_.unknown = 0;
        }
        kb_.ampCount = 0;
        kb_.isAscii = 1;
        kb_.lAngle = kb_.curByte;
        kb_.scan = &XmlLexer::scanTagStart;
        break;
    case '>':
        if (kb_.lAngle != 0)
        {
            size_t numU8s = kb_.curByte - kb_.lAngle + 1;
            String body;
            resetStr(body, kb_.isAscii, kb_.lAngle, numU8s);
            addElement(new XmlDoc::UnknownElement(body));
            kb_.ampCount = 0;
            kb_.isAscii = 1;
        }
        kb_.unknown = 0;
        break;
    default:
        if (kb_.curElement == 0)
        {
            kb_.unknown = kb_.curByte;
        }
        break;
    }
}


//
// Wait for end of character data.
//
void XmlLexer::scanCdata(utf32_t c)
{
    switch (c)
    {
    case ']':
        kb_.scan = &XmlLexer::scanCdataZ0;
        break;
    default:
        break;
    }
}


//
// Wait for start of character data.
//
void XmlLexer::scanCdataA0(utf32_t c)
{
    switch (c)
    {
    case 'C':
        kb_.scan = &XmlLexer::scanCdataA1;
        break;
    default:
        kb_.scan = &XmlLexer::scan0;
        kb_.unknown = kb_.lAngle;
        break;
    }
}


//
// Wait for start of character data.
//
void XmlLexer::scanCdataA1(utf32_t c)
{
    switch (c)
    {
    case 'D':
        kb_.scan = &XmlLexer::scanCdataA2;
        break;
    default:
        kb_.scan = &XmlLexer::scan0;
        kb_.unknown = kb_.lAngle;
        break;
    }
}


//
// Wait for start of character data.
//
void XmlLexer::scanCdataA2(utf32_t c)
{
    switch (c)
    {
    case 'A':
        kb_.scan = &XmlLexer::scanCdataA3;
        break;
    default:
        kb_.scan = &XmlLexer::scan0;
        kb_.unknown = kb_.lAngle;
        break;
    }
}


//
// Wait for start of character data.
//
void XmlLexer::scanCdataA3(utf32_t c)
{
    switch (c)
    {
    case 'T':
        kb_.scan = &XmlLexer::scanCdataA4;
        break;
    default:
        kb_.scan = &XmlLexer::scan0;
        kb_.unknown = kb_.lAngle;
        break;
    }
}


//
// Wait for start of character data.
//
void XmlLexer::scanCdataA4(utf32_t c)
{
    switch (c)
    {
    case 'A':
        kb_.scan = &XmlLexer::scanCdataA5;
        break;
    default:
        kb_.scan = &XmlLexer::scan0;
        kb_.unknown = kb_.lAngle;
        break;
    }
}


//
// Wait for start of character data.
//
void XmlLexer::scanCdataA5(utf32_t c)
{
    switch (c)
    {
    case '[':
        kb_.scan = &XmlLexer::scanCdata;
        break;
    default:
        kb_.scan = &XmlLexer::scan0;
        kb_.unknown = kb_.lAngle;
        break;
    }
}


//
// Wait for end of character data.
//
void XmlLexer::scanCdataZ0(utf32_t c)
{
    switch (c)
    {
    case ']':
        kb_.scan = &XmlLexer::scanCdataZ1;
        break;
    default:
        kb_.scan = &XmlLexer::scanCdata;
        break;
    }
}


//
// Wait for end of character data.
//
void XmlLexer::scanCdataZ1(utf32_t c)
{
    switch (c)
    {

    case '>':
    {
        size_t numU8s = kb_.curByte - kb_.lAngle + 1 - 9 /*head*/ - 3 /*tail*/; //"<![CDATA[body]]>"
        String body;
        resetStr(body, kb_.isAscii, kb_.lAngle + 9, numU8s);
        addElement(new XmlDoc::Cdata(body));
        kb_.ampCount = 0;
        kb_.isAscii = 1;
        if (kb_.mom != 0)
        {
            kb_.scan = &XmlLexer::scanContent0;
            kb_.text = kb_.curByte + 1;
        }
        else
        {
            kb_.scan = &XmlLexer::scan0;
        }
        break;
    }

    case ']':
        kb_.scan = &XmlLexer::scanCdataZ1;
        break;

    default:
        kb_.scan = &XmlLexer::scanCdata;
        break;

    }
}


//
// Wait for end of comment.
//
void XmlLexer::scanComment(utf32_t c)
{
    switch (c)
    {
    case '-':
        kb_.scan = &XmlLexer::scanCommentZ0;
        break;
    default:
        break;
    }
}


//
// Wait for start of comment.
//
void XmlLexer::scanCommentA0(utf32_t c)
{
    switch (c)
    {
    case '-':
        kb_.scan = &XmlLexer::scanComment;
        break;
    default:
        kb_.scan = &XmlLexer::scan0;
        kb_.unknown = kb_.lAngle;
        break;
    }
}


//
// Wait for end of comment.
//
void XmlLexer::scanCommentZ0(utf32_t c)
{
    switch (c)
    {
    case '-':
        kb_.scan = &XmlLexer::scanCommentZ1;
        break;
    default:
        kb_.scan = &XmlLexer::scanComment;
        break;
    }
}


//
// Wait for end of comment.
//
void XmlLexer::scanCommentZ1(utf32_t c)
{
    switch (c)
    {

    case '>':
    {
        size_t numU8s = kb_.curByte - kb_.lAngle + 1 - 4 /*head*/ - 3 /*tail*/; //"<!--body-->"
        String body;
        resetStr(body, kb_.isAscii, kb_.lAngle + 4, numU8s);
        addElement(new XmlDoc::Comment(body));
        kb_.ampCount = 0;
        kb_.isAscii = 1;
        if (kb_.mom != 0)
        {
            kb_.scan = &XmlLexer::scanContent0;
            kb_.text = kb_.curByte + 1;
        }
        else
        {
            kb_.scan = &XmlLexer::scan0;
        }
        break;
    }

    default:
        kb_.scan = &XmlLexer::scan0;
        kb_.unknown = kb_.lAngle;
        break;

    }
}


//
// Wait for element-end.
//
void XmlLexer::scanContent(utf32_t /*c*/)
{
    const utf8_t* p = kb_.curByte;
    const utf8_t* pEnd = p + kb_.remainingBytes;
    for (unsigned int n; p < pEnd; p += n)
    {

        // Content ends here.
        if (*p == '<')
        {
            kb_.setLexee(p, pEnd - p);
            size_t numU8s = kb_.curByte - kb_.text;
            XmlElement* baby = (kb_.isAscii > 0)? normalizeSpace(kb_.text, numU8s): normalizeSpace8(kb_.text, numU8s);
            addElement(baby);
            kb_.ampCount = 0;
            kb_.isAscii = 1;
            kb_.lAngle = kb_.curByte;
            kb_.scan = &XmlLexer::scanTagStart;
            kb_.text = 0;
            return;
        }

        n = Utf8::getSeqLength(*p);
        if ((n < Utf8::MinSeqLength) || (n > Utf8::MaxSeqLength)) kb_.isAscii = -1, n = 1; //skip invalid bytes
        else if ((n > Utf8::MinSeqLength) && (kb_.isAscii > 0)) kb_.isAscii = 0;
    }
}


//
// Trim leading spaces.
// Wait for element-end.
//
void XmlLexer::scanContent0(utf32_t c)
{

    // If a space is seen at this state, it's reasonable to assume they come in bunches.
    // Fast forward through them.
    if ((c <= MaxAscii) && isSpace(static_cast<utf8_t>(c)))
    {
        const utf8_t* p = kb_.curByte;
        skipSpaces();
        kb_.text += kb_.curByte - p + 1;
        return;
    }

    switch (c)
    {
    case '<':
        kb_.ampCount = 0;
        kb_.isAscii = 1;
        kb_.lAngle = kb_.curByte;
        kb_.scan = &XmlLexer::scanTagStart;
        kb_.text = 0;
        break;
    default:
        kb_.scan = &XmlLexer::scanContent;
        break;
    }
}


//
// Wait for end of XML declaration.
//
void XmlLexer::scanDecl(utf32_t c)
{
    switch (c)
    {
    case '?':
        kb_.scan = &XmlLexer::scanDeclZ0;
        break;
    default:
        if (c >= MaxAscii)
        {
            kb_.scan = &XmlLexer::scan0;
            kb_.unknown = kb_.lAngle;
        }
        break;
    }
}


//
// Wait for start of XML declaration.
//
void XmlLexer::scanDeclA0(utf32_t c)
{
    switch (c)
    {
    case 'x':
        kb_.scan = &XmlLexer::scanDeclA1;
        break;
    default:
        kb_.scan = &XmlLexer::scan0;
        kb_.unknown = kb_.lAngle;
        break;
    }
}


//
// Wait for start of XML declaration.
//
void XmlLexer::scanDeclA1(utf32_t c)
{
    switch (c)
    {
    case 'm':
        kb_.scan = &XmlLexer::scanDeclA2;
        break;
    default:
        kb_.scan = &XmlLexer::scan0;
        kb_.unknown = kb_.lAngle;
        break;
    }
}


//
// Wait for start of XML declaration.
//
void XmlLexer::scanDeclA2(utf32_t c)
{
    switch (c)
    {
    case 'l':
        kb_.scan = &XmlLexer::scanDeclA3;
        break;
    default:
        kb_.scan = &XmlLexer::scan0;
        kb_.unknown = kb_.lAngle;
        break;
    }
}


//
// Wait for start of XML declaration.
//
void XmlLexer::scanDeclA3(utf32_t c)
{
    if ((c <= MaxAscii) && isSpace(static_cast<utf8_t>(c)))
    {
        kb_.scan = &XmlLexer::scanDecl;
    }
    else
    {
        kb_.scan = &XmlLexer::scan0;
        kb_.unknown = kb_.lAngle;
    }
}


//
// Wait for end of XML declaration.
//
void XmlLexer::scanDeclZ0(utf32_t c)
{
    switch (c)
    {
    case '>':
        if (kb_.prolog == 0)
        {
            kb_.prolog = mkProlog(kb_.lAngle, kb_.curByte);
        }
        kb_.scan = &XmlLexer::scan0;
        break;
    default:
        kb_.scan = &XmlLexer::scanDecl;
        break;
    }
}


//
// Wait for element-start.
//
void XmlLexer::scanElement(utf32_t /*c*/)
{
    XmlElement* baby;
    const utf8_t* p = kb_.curByte;
    const utf8_t* pEnd = p + kb_.remainingBytes;
    for (unsigned int n; p < pEnd; p += n)
    {

        switch (*p)
        {
        case '&':
            ++kb_.ampCount;
            n = 1;
            break;
        case '/':
            kb_.setLexee(p, pEnd - p);
            kb_.scan = &XmlLexer::scanEmptyElementZ0;
            return;
        case '>':
            kb_.setLexee(p, pEnd - p);
            baby = mkElement(kb_.lAngle, kb_.curByte, kb_.ampCount, kb_.isAscii);
            pushElement(baby);
            kb_.ampCount = 0;
            kb_.isAscii = 1;
            kb_.scan = &XmlLexer::scanContent0;
            kb_.text = kb_.curByte + 1;
            return;
        default:
            n = Utf8::getSeqLength(*p);
            if ((n < Utf8::MinSeqLength) || (n > Utf8::MaxSeqLength)) kb_.isAscii = -1, n = 1; //skip invalid bytes
            else if ((n > Utf8::MinSeqLength) && (kb_.isAscii > 0)) kb_.isAscii = false;
            break;
        }
    }
}


//
// Validate element name.
// Wait for element-start.
//
void XmlLexer::scanElementA0(utf32_t /*c*/)
{
    XmlElement* baby;
    Utf8 c8;
    const utf8_t* p = kb_.curByte;
    for (size_t bytesDecoded, remainingBytes = kb_.remainingBytes; remainingBytes > 0; p += bytesDecoded, remainingBytes -= bytesDecoded)
    {

        bytesDecoded = c8.decode(p, remainingBytes);
        if (bytesDecoded == 0)
        {
            c8.resetWithValidChar(Utf8::DefaultChar);
            kb_.isAscii = -1;
            bytesDecoded = countInvalidBytes(p, remainingBytes);
        }
        else if ((bytesDecoded > 1) && (kb_.isAscii > 0))
        {
            kb_.isAscii = 0;
        }

        utf32_t c = c8;
        switch (c)
        {
        case '/':
            kb_.setLexee(p, remainingBytes);
            kb_.scan = &XmlLexer::scanEmptyElementZ0;
            return;
        case '>':
            kb_.setLexee(p, remainingBytes);
            baby = mkElement(kb_.lAngle, kb_.curByte, kb_.ampCount, kb_.isAscii);
            pushElement(baby);
            kb_.ampCount = 0;
            kb_.isAscii = 1;
            kb_.scan = &XmlLexer::scanContent0;
            kb_.text = kb_.curByte + 1;
            return;
        default:
            if ((c <= MaxAscii) && isSpace(static_cast<utf8_t>(c)))
            {
                kb_.setLexee(p, remainingBytes);
                kb_.scan = &XmlLexer::scanElement;
                return;
            }
            if (!isValidName(c))
            {
                kb_.setLexee(p, remainingBytes);
                kb_.scan = &XmlLexer::scan0;
                kb_.unknown = kb_.lAngle;
                return;
            }
            break;
        }
    }
}


//
// Wait for element-end.
//
void XmlLexer::scanElementEndZ0(utf32_t /*c*/)
{
    const utf8_t* p = kb_.curByte;
    const utf8_t* pEnd = p + kb_.remainingBytes;
    for (unsigned int n; p < pEnd; p += n)
    {

        // Element ends here.
        if (*p == '>')
        {
            kb_.setLexee(p, pEnd - p);
            popElement(kb_.lAngle, kb_.curByte);
            kb_.ampCount = 0;
            kb_.isAscii = 1;
            if (kb_.mom != 0)
            {
                kb_.scan = &XmlLexer::scanContent0;
                kb_.text = kb_.curByte + 1;
            }
            return;
        }

        n = Utf8::getSeqLength(*p);
        if ((n < Utf8::MinSeqLength) || (n > Utf8::MaxSeqLength)) kb_.isAscii = -1, n = 1; //skip invalid bytes
        else if ((n > Utf8::MinSeqLength) && (kb_.isAscii > 0)) kb_.isAscii = 0;
    }
}


//
// Wait for end of empty element.
//
void XmlLexer::scanEmptyElementZ0(utf32_t c)
{
    XmlElement* baby;
    switch (c)
    {

    case '>':
        baby = mkElement(kb_.lAngle, kb_.curByte, kb_.ampCount, kb_.isAscii);
        kb_.ampCount = 0;
        kb_.isAscii = 1;
        if (kb_.mom != 0)
        {
            addElement(baby);
            kb_.scan = &XmlLexer::scanContent0;
            kb_.text = kb_.curByte + 1;
        }
        else
        {
            kb_.root = baby;
            kb_.scan = &XmlLexer::scanEpilog;
            kb_.unknown = kb_.curByte + 1;
        }
        break;

    default:
        kb_.scan = &XmlLexer::scanElement;
        break;

    }
}


//
// Ignore epilog.
//
void XmlLexer::scanEpilog(utf32_t /*c*/)
{
}


//
// Wait for start of comment or character data.
//
void XmlLexer::scanLAngleBang(utf32_t c)
{
    switch (c)
    {
    case '-':
        kb_.scan = &XmlLexer::scanCommentA0;
        break;
    case '[':
        kb_.scan = &XmlLexer::scanCdataA0;
        break;
    default:
        kb_.scan = &XmlLexer::scan0;
        kb_.unknown = kb_.lAngle;
        break;
    }
}


//
// Wait for start of some lexeme.
//
void XmlLexer::scanTagStart(utf32_t c)
{
    switch (c)
    {
    case '!':
        kb_.scan = &XmlLexer::scanLAngleBang;
        break;
    case '/':
        kb_.scan = &XmlLexer::scanElementEndZ0;
        break;
    case '?':
        kb_.scan = &XmlLexer::scanDeclA0;
        break;
    default:
        if (isValidName0(c))
        {
            kb_.scan = &XmlLexer::scanElementA0;
        }
        else
        {
            kb_.scan = &XmlLexer::scan0;
            kb_.unknown = kb_.lAngle;
        }
        break;
    }
}


//
// Initial state. Look for escape sequences starting with ampersand.
//
void XmlLexer::unescape0(escKb_t& kb, utf32_t c)
{
    switch (c)
    {
    case '&':
        kb.amp = kb.curByte;
        kb.unescape = unescape1;
        // no break
    default:
        if (kb.p0 == 0)
        {
            kb.p0 = kb.curByte;
            kb.numChars = 0;
            kb.numU8s = 0;
        }
        break;
    }
}


//
// Start of escape sequence.
//
void XmlLexer::unescape1(escKb_t& kb, utf32_t c)
{
    switch (c)
    {
    case '#':
        kb.unescape = unescapeNum;
        break;
    case 'a':
        kb.unescape = unescapeA;
        break;
    case 'g':
        kb.unescape = unescapeG;
        break;
    case 'l':
        kb.unescape = unescapeL;
        break;
    case 'q':
        kb.unescape = unescapeQ;
        break;
    default:
        kb.unescape = unescape0;
        break;
    }
}


//
// End of escape sequence.
//
void XmlLexer::unescape9(escKb_t& kb, utf32_t c)
{
    switch (c)
    {
    case ';':
        appendEsc(kb, kb.esc);
        // no break
    default:
        kb.unescape = unescape0;
        break;
    }
}


void XmlLexer::unescapeA(escKb_t& kb, utf32_t c)
{
    switch (c)
    {
    case 'm':
        kb.unescape = unescapeAm;
        break;
    case 'p':
        kb.unescape = unescapeAp;
        break;
    default:
        kb.unescape = unescape0;
        break;
    }
}


void XmlLexer::unescapeAm(escKb_t& kb, utf32_t c)
{
    switch (c)
    {
    case 'p':
        kb.esc = '&';
        kb.unescape = unescape9;
        break;
    default:
        kb.unescape = unescape0;
        break;
    }
}


void XmlLexer::unescapeAp(escKb_t& kb, utf32_t c)
{
    switch (c)
    {
    case 'o':
        kb.unescape = unescapeApo;
        break;
    default:
        kb.unescape = unescape0;
        break;
    }
}


void XmlLexer::unescapeApo(escKb_t& kb, utf32_t c)
{
    switch (c)
    {
    case 's':
        kb.esc = '\'';
        kb.unescape = unescape9;
        break;
    default:
        kb.unescape = unescape0;
        break;
    }
}


//
// "&#[0-9]+"
//
void XmlLexer::unescapeDec(escKb_t& kb, utf32_t c)
{
    if (isDigit(c))
    {
        return;
    }

    const utf8_t* p0;
    size_t numDigits;
    switch (c)
    {

    case ';':
        p0 = kb.amp + 2;
        numDigits = kb.curByte - p0;
        if (numDigits <= MAX_DIGITS)
        {
            utf32_t esc = 0;
            for (const utf8_t* p = p0; p < kb.curByte; esc = (esc * 10) + (*p++) - '0');
            if (Utf8::isValid(esc))
            {
                appendEsc(kb, esc);
            }
        }
        // no break

    default:
        kb.unescape = unescape0;
        break;

    }
}


void XmlLexer::unescapeG(escKb_t& kb, utf32_t c)
{
    switch (c)
    {
    case 't':
        kb.esc = '>';
        kb.unescape = unescape9;
        break;
    default:
        kb.unescape = unescape0;
        break;
    }
}


//
// "&#[xX][0-9A-Fa-f]+"
//
void XmlLexer::unescapeHex(escKb_t& kb, utf32_t c)
{
    if (isXdigit(c))
    {
        return;
    }

    const utf8_t* p0;
    size_t numXdigits;
    switch (c)
    {

    case ';':
        p0 = kb.amp + 3;
        numXdigits = kb.curByte - p0;
        if (numXdigits <= MAX_XDIGITS)
        {
            utf32_t esc = 0;
            for (const utf8_t* p = p0; p < kb.curByte; esc = (esc << 4) + U8::xdigitToNibble(*p++));
            if (Utf8::isValid(esc))
            {
                appendEsc(kb, esc);
            }
        }
        // no break

    default:
        kb.unescape = unescape0;
        break;

    }
}


void XmlLexer::unescapeL(escKb_t& kb, utf32_t c)
{
    switch (c)
    {
    case 't':
        kb.esc = '<';
        kb.unescape = unescape9;
        break;
    default:
        kb.unescape = unescape0;
        break;
    }
}


void XmlLexer::unescapeNum(escKb_t& kb, utf32_t c)
{
    if (isDigit(c))
    {
        kb.unescape = unescapeDec;
        return;
    }

    switch (c)
    {
    case 'X':
    case 'x':
        kb.unescape = unescapeX;
        break;
    default:
        kb.unescape = unescape0;
        break;
    }
}


void XmlLexer::unescapeQ(escKb_t& kb, utf32_t c)
{
    switch (c)
    {
    case 'u':
        kb.unescape = unescapeQu;
        break;
    default:
        kb.unescape = unescape0;
        break;
    }
}


void XmlLexer::unescapeQu(escKb_t& kb, utf32_t c)
{
    switch (c)
    {
    case 'o':
        kb.unescape = unescapeQuo;
        break;
    default:
        kb.unescape = unescape0;
        break;
    }
}


void XmlLexer::unescapeQuo(escKb_t& kb, utf32_t c)
{
    switch (c)
    {
    case 't':
        kb.esc = '"';
        kb.unescape = unescape9;
        break;
    default:
        kb.unescape = unescape0;
        break;
    }
}


void XmlLexer::unescapeX(escKb_t& kb, utf32_t c)
{
    kb.unescape = isXdigit(c)? unescapeHex: unescape0;
}


void XmlLexer::lexKb_t::reset(scan_t scan, const utf8_t* curByte, size_t remainingBytes)
{
    ampCount = 0;
    curElement = 0;
    isAscii = 1;
    lAngle = 0;
    mom = 0;
    orphanCount = 0;
    prolog = 0;
    root = 0;
    scanErr = 0;
    text = 0;
    unknown = 0;

    this->scan = scan;
    setLexee(curByte, remainingBytes);
}

END_NAMESPACE1
