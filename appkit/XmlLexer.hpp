/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_XML_LEXER_HPP
#define APPKIT_XML_LEXER_HPP

#include <istream>
#include "appkit/String.hpp"
#include "appkit/XmlDoc.hpp"
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(appkit)

class XmlElement;
class XmlProlog;


//! xml lexical analyzer
class XmlLexer
    //!
    //! A class representing an XML lexical analyzer. A lexical analyzer analyzes
    //! a stream of text and reports the findings as a sequence of lexemes.
    //!
{

public:
    XmlLexer(const String& lexee);
    XmlLexer(const std::istream& lexee);
    XmlLexer(const syskit::utf8_t* lexee, size_t byteSize);
    ~XmlLexer();

    XmlDoc* scan();
    const syskit::utf8_t* lexee() const;
    const syskit::utf8_t* lexee(unsigned int& byteSize) const;
    unsigned int lexeeByteSize() const;
    void reset(const String& lexee);
    void reset(const std::istream& is);
    void reset(const syskit::utf8_t* lexee, size_t byteSize);
    static bool validateElementName(const String& elementName);

    // XML escape sequence resolution.
    static String escape(const String& native);
    static String unescape(const String& xml);

private:
    typedef struct
    {
        const syskit::utf8_t* n0;
        const syskit::utf8_t* n1;
        const syskit::utf8_t* v0;
        const syskit::utf8_t* v1;
    } attrBound_t;

    // Knowledgebase for escape sequence processing.
    struct escKb_s;
    typedef void(*unescape_t)(struct escKb_s&, syskit::utf32_t);
    typedef struct escKb_s
    {
        String* native;
        const syskit::utf8_t* amp;
        const syskit::utf8_t* curByte;
        const syskit::utf8_t* p0;
        unescape_t unescape;
        size_t numChars;
        size_t numU8s;
        char esc;
    } escKb_t;

    // Knowledgebase for lexical analysis.
    typedef void (XmlLexer::*scan_t)(syskit::utf32_t);
    typedef struct
    {
        XmlElement* mom;
        XmlElement* root;
        XmlProlog* prolog;
        const char* scanErr;
        const syskit::utf8_t* curByte;
        const syskit::utf8_t* curElement;
        const syskit::utf8_t* lAngle;
        const syskit::utf8_t* text;
        const syskit::utf8_t* unknown;
        int isAscii; //+=ascii 0=non-ascii -=invalid
        scan_t scan;
        size_t ampCount;
        size_t orphanCount;
        size_t remainingBytes;
        void reset(scan_t scan, const syskit::utf8_t* curByte, size_t remainingBytes);
        void setLexee(const syskit::utf8_t* curByte, size_t remainingBytes);
    } lexKb_t;

    enum
    {
        EqOrSpaceZM = 0x01,
        Name0M = 0x02,
        NameM = 0x04,
        Quote1ZM = 0x08,
        Quote2ZM = 0x10,
        QuoteZM = 0x20,
        SpaceM = 0x40,
        SpaceZM = 0x80,

        MaxAscii = 0x7f
    };

    String origLexee_;
    bool copyMade_;
    const syskit::utf8_t* lexee_;
    lexKb_t kb_;
    unsigned int numU8s_;

    static const unsigned char mask_[];

    XmlLexer(const XmlLexer&); //prohibit usage
    const XmlLexer& operator =(const XmlLexer&); //prohibit usage

    // XML scanner support.
    const char* errDesc() const;
    void addElement(XmlElement*);
    void copyStream(const std::istream&);
    void popElement(const syskit::utf8_t*, const syskit::utf8_t*);
    void pushElement(XmlElement*);
    void skipSpaces();
    static bool validateElementName(void*, size_t, syskit::utf32_t);
    static size_t countInvalidBytes(const syskit::utf8_t*, size_t);

    // More XML scanner support.
    static XmlElement* mkElement(const syskit::utf8_t*, const syskit::utf8_t*, size_t, int);
    static XmlElement* normalizeSpace(const syskit::utf8_t*, size_t);
    static XmlElement* normalizeSpace8(const syskit::utf8_t*, size_t);
    static XmlProlog* mkProlog(const syskit::utf8_t*, const syskit::utf8_t*);
    static bool findAttrBound(attrBound_t&, const syskit::utf8_t*, const syskit::utf8_t*);
    static bool isDigit(syskit::utf32_t);
    static bool isEqOrSpaceZ(syskit::utf8_t);
    static bool isQuote1Z(syskit::utf8_t);
    static bool isQuote2Z(syskit::utf8_t);
    static bool isQuoteZ(syskit::utf8_t);
    static bool isSpace(syskit::utf8_t);
    static bool isSpaceZ(syskit::utf8_t);
    static bool isValidName(syskit::utf32_t);
    static bool isValidName0(syskit::utf32_t);
    static bool isXdigit(syskit::utf32_t);
    static void appendEsc(escKb_t&, char);
    static void appendEsc(escKb_t&, syskit::utf32_t);

    // Hard-coded finite state automaton to scan XML input.
    void scan0(syskit::utf32_t);
    void scanCdata(syskit::utf32_t);          //"<![CDATA["
    void scanCdataA0(syskit::utf32_t);        //"<!["
    void scanCdataA1(syskit::utf32_t);        //"<![C"
    void scanCdataA2(syskit::utf32_t);        //"<![CD"
    void scanCdataA3(syskit::utf32_t);        //"<![CDA"
    void scanCdataA4(syskit::utf32_t);        //"<![CDAT"
    void scanCdataA5(syskit::utf32_t);        //"<![CDATA"
    void scanCdataZ0(syskit::utf32_t);        //"<![CDATA[.*]"
    void scanCdataZ1(syskit::utf32_t);        //"<![CDATA[.*]]"
    void scanComment(syskit::utf32_t);        //"<!--"
    void scanCommentA0(syskit::utf32_t);      //"<!-"
    void scanCommentZ0(syskit::utf32_t);      //"<!--.*-"
    void scanCommentZ1(syskit::utf32_t);      //"<!--.*--"
    void scanContent(syskit::utf32_t);        //"<.>"
    void scanContent0(syskit::utf32_t);       //"<.>"
    void scanDecl(syskit::utf32_t);           //"<?xml "
    void scanDeclA0(syskit::utf32_t);         //"<?"
    void scanDeclA1(syskit::utf32_t);         //"<?."
    void scanDeclA2(syskit::utf32_t);         //"<?xm"
    void scanDeclA3(syskit::utf32_t);         //"<?xml"
    void scanDeclZ0(syskit::utf32_t);         //"<?xml .*?"
    void scanElement(syskit::utf32_t);        //"<.*[ \n\r\t]"
    void scanElementA0(syskit::utf32_t);      //"<."
    void scanElementEndZ0(syskit::utf32_t);   //"</"
    void scanEmptyElementZ0(syskit::utf32_t); //"<.*/"
    void scanEpilog(syskit::utf32_t);
    void scanLAngleBang(syskit::utf32_t);     //"<!"
    void scanTagStart(syskit::utf32_t);       //"<"

    // Hard-coded finite state automaton to support XML escape sequences.
    static void unescape0(escKb_t&, syskit::utf32_t);
    static void unescape1(escKb_t&, syskit::utf32_t);
    static void unescape9(escKb_t&, syskit::utf32_t);
    static void unescapeA(escKb_t&, syskit::utf32_t);   //"&a"
    static void unescapeAm(escKb_t&, syskit::utf32_t);  //"&am"
    static void unescapeAp(escKb_t&, syskit::utf32_t);  //"&ap"
    static void unescapeApo(escKb_t&, syskit::utf32_t); //"&apo"
    static void unescapeDec(escKb_t&, syskit::utf32_t); //"&#[0-9]+"
    static void unescapeG(escKb_t&, syskit::utf32_t);   //"&g"
    static void unescapeHex(escKb_t&, syskit::utf32_t); //"&#[xX][0-9A-Fa-f]+"
    static void unescapeL(escKb_t&, syskit::utf32_t);   //"&l"
    static void unescapeNum(escKb_t&, syskit::utf32_t); //"&#"
    static void unescapeQ(escKb_t&, syskit::utf32_t);   //"&q"
    static void unescapeQu(escKb_t&, syskit::utf32_t);  //"&qu"
    static void unescapeQuo(escKb_t&, syskit::utf32_t); //"&quo"
    static void unescapeX(escKb_t&, syskit::utf32_t);   //"&#[xX]"

};

END_NAMESPACE1

#include "appkit/S32.hpp"

BEGIN_NAMESPACE1(appkit)

// Return true if given character is a decimal digit ([0-9]).
inline bool XmlLexer::isDigit(syskit::utf32_t c)
{
    return (c <= MaxAscii) && S32::isDigit(c);
}

// Return true if given character is in [= \t\r\n].
// Also return true if given character is the terminating right angle.
inline bool XmlLexer::isEqOrSpaceZ(syskit::utf8_t c)
{
    return (mask_[c] & EqOrSpaceZM) != 0;
}

// Return true if given character is a single quote.
// Also return true if given character is the terminating right angle.
inline bool XmlLexer::isQuote1Z(syskit::utf8_t c)
{
    return (mask_[c] & Quote1ZM) != 0;
}

// Return true if given character is a double quote.
// Also return true if given character is the terminating right angle.
inline bool XmlLexer::isQuote2Z(syskit::utf8_t c)
{
    return (mask_[c] & Quote2ZM) != 0;
}

// Return true if given character is in ['"].
// Also return true if given character is the terminating right angle.
inline bool XmlLexer::isQuoteZ(syskit::utf8_t c)
{
    return (mask_[c] & QuoteZM) != 0;
}

// Return true if given character is considered an XML whitespace ([ \t\r\n]).
inline bool XmlLexer::isSpace(syskit::utf8_t c)
{
    return (mask_[c] & SpaceM) != 0;
}

// Return true if given character is considered an XML whitespace ([ \t\r\n]).
// Also return true if given character is the terminating right angle.
inline bool XmlLexer::isSpaceZ(syskit::utf8_t c)
{
    return (mask_[c] & SpaceZM) != 0;
}

// Return true if given character is a hexadecimal digit ([0-9a-fA-F]).
inline bool XmlLexer::isXdigit(syskit::utf32_t c)
{
    return (c <= MaxAscii) && S32::isXdigit(c);
}

inline const char* XmlLexer::errDesc() const
{
    return (kb_.scanErr == 0)?
        (((kb_.prolog == 0) && (kb_.root == 0))? "no prolog nor root element found": 0):
        (kb_.scanErr);
}

//! Return the analyzee.
//! Use lexeeByteSize() to obtain its byte size.
inline const syskit::utf8_t* XmlLexer::lexee() const
{
    return lexee_;
}

//! Return the analyzee.
//! Also return its byte size byteSize.
inline const syskit::utf8_t* XmlLexer::lexee(unsigned int& byteSize) const
{
    byteSize = numU8s_;
    return lexee_;
}

//! Return the analyzee's byte size.
inline unsigned int XmlLexer::lexeeByteSize() const
{
    return numU8s_;
}

inline void XmlLexer::lexKb_t::setLexee(const syskit::utf8_t* curByte, size_t remainingBytes)
{
    this->curByte = curByte;
    this->remainingBytes = remainingBytes;
}

END_NAMESPACE1

#endif
