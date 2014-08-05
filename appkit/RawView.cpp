/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/sys.hpp"

#include "appkit-pch.h"
#include "appkit/RawView.hpp"
#include "appkit/S32.hpp"
#include "appkit/U32.hpp"
#include "appkit/U8.hpp"

using namespace syskit;

const char LINE_FEED = '\n';

BEGIN_NAMESPACE1(appkit)


//!
//! Construct a default instance with no binary data. Its ASCII view is configured
//! with these values: DefaultBpr, DefaultFirstIndent, DefaultIndent, DefaultOffset,
//! DefaultRowsPerPage, DefaultSpacing. The offset part and ASCII part are enabled.
//! Use reset() to associate with binary data. Sample ASCII view with the above
//! configuration:
//!\code
//! [00000073]  2f2f2120 4120636c 61737320 72657072  //! A class repr
//! [00000083]  6573656e 74696e67 2062696e 61727920  esenting binary
//! [00000093]  64617461 20776974 6820616e 20415343  data with an ASC
//! [000000a3]  49492076 6965772e 20546865 20415343  II view. The ASC
//! [000000b3]  49492076 6965770d 0a2f2f21 2063      II view..//! c..
//!\endcode
//!
RawView::RawView()
{
    raw_ = 0;
    rawLength_ = 0;

    useDefaults();
}


RawView::RawView(const RawView& rawView)
{
    raw_ = rawView.raw_;
    rawLength_ = rawView.rawLength_;

    copyConfig(rawView);
}


//!
//! Construct instance using the given the ASCII view configuration. Also associate
//! with given binary data.
//!
RawView::RawView(bool showAscii,
    bool showOffset,
    unsigned int bytesPerRow,
    unsigned int firstIndent,
    unsigned int indent,
    unsigned int offset,
    unsigned int rowsPerPage,
    unsigned int spacing,
    const void* raw,
    size_t length)
{
    raw_ = static_cast<const unsigned char*>(raw);
    rawLength_ = length;

    firstIndent_ = firstIndent;
    indent_ = indent;
    offset_ = offset;
    showAscii_ = showAscii;
    showOffset_ = showOffset;
    spacing_ = spacing;
    setRowsPerPage(rowsPerPage);
    setBytesPerRow(bytesPerRow); //must be set after spacing
}


//!
//! Construct an instance associated with given binary data. Its ASCII view is configured
//! with these values: DefaultBpr, DefaultFirstIndent, DefaultIndent, DefaultOffset,
//! DefaultRowsPerPage, DefaultSpacing. The offset part and ASCII part are enabled.
//! Sample ASCII view with the above configuration:
//!\code
//! [00000073]  2f2f2120 4120636c 61737320 72657072  //! A class repr
//! [00000083]  6573656e 74696e67 2062696e 61727920  esenting binary
//! [00000093]  64617461 20776974 6820616e 20415343  data with an ASC
//! [000000a3]  49492076 6965772e 20546865 20415343  II view. The ASC
//! [000000b3]  49492076 6965770d 0a2f2f21 2063      II view..//! c..
//!\endcode
//!
RawView::RawView(const void* raw, size_t length)
{
    raw_ = static_cast<const unsigned char*>(raw);
    rawLength_ = length;

    useDefaults();
}


const RawView& RawView::operator =(const RawView& rawView)
{

    // Prevent self assignment.
    if (this != &rawView)
    {
        raw_ = rawView.raw_;
        rawLength_ = rawView.rawLength_;
        copyConfig(rawView);
    }

    // Return reference to self.
    return *this;
}


String RawView::toString() const
{
    size_t size = computeCharsPerString();
    char* buf = new char[size];
    size_t length;
    dumpView(buf, length);

    String s;
    size_t numU8s = length + 1;
    s.attachRaw(reinterpret_cast<utf8_t*>(buf), numU8s, numU8s);
    return s;
}


//
// Form the ASCII view as one page from the given binary data and
// save the ASCII view into the given string buffer. The buffer must
// be big enough.
//
char* RawView::dumpPage(char* s, const unsigned char* p, size_t n) const
{

    // Start subsequent pages w/ two newlines.
    if (p != raw_)
    {
        s[0] = LINE_FEED;
        s[1] = LINE_FEED;
        s += 2;
    }

    // Dump all rows except last.
    size_t rowN = (n - 1) / bytesPerRow_;
    for (size_t row = 0; row < rowN; ++row)
    {
        s = dumpRow(s, p, bytesPerRow_);
        *s++ = LINE_FEED;
        p += bytesPerRow_;
        n -= bytesPerRow_;
    }

    // Dump last row.
    return dumpRow(s, p, n);
}


//
// Form the ASCII view as one row from the given binary data and
// save the ASCII view into the given string buffer. The buffer must
// be big enough.
//
char* RawView::dumpRow(char* s, const unsigned char* p, size_t n) const
{

    // Indent.
    unsigned int indent = (p == raw_)? firstIndent_: indent_;
    if (indent > 0)
    {
        memset(s, ' ', indent);
        s += indent;
    }

    // Show hexadecimal offset.
    if (showOffset_)
    {
        unsigned int offset = static_cast<unsigned int>(p - raw_) + offset_;
        s[0] = '[';
        U32::toXdigits(offset, s + 1);
        s[9] = ']';
        s[10] = ' ';
        s[11] = ' ';
        s += 12;
    }

    // Show hexadecimal contents.
    size_t i;
    for (i = 0; i < n;)
    {
        U8::toXdigits(p[i++], s);
        s += 2;
        if (spacing_ && ((i%spacing_) == 0))
        {
            *s++ = ' ';
        }
    }

    // Pad hexadecimal contents with blanks.
    size_t numPaddedBytes = bytesPerRow_ - i;
    if (numPaddedBytes)
    {
        unsigned int numBlanks = 0;
        while (i < bytesPerRow_)
        {
            numBlanks += (spacing_ && ((i%spacing_) == 0))? 3: 2;
            ++i;
        }
        memset(s, ' ', numBlanks);
        s += numBlanks;
    }
    if ((spacing_ == 0) || (n % spacing_))
    {
        *s++ = ' ';
    }

    // Show ASCII part.
    if (showAscii_)
    {
        *s++ = ' ';
        const unsigned char* p1 = p;
        for (const unsigned char* pEnd = p + n; p1 < pEnd; ++p1)
        {
            *s++ = S32::isPrint(*p1)? *p1: '.';
        }
        memset(s, '.', numPaddedBytes);
        s += numPaddedBytes;
    }

    // Return pointer to next character in string buffer.
    return s;
}


//
// Save ASCII form of the raw data into the given string buffer. The
// buffer must be big enough (size >= computeCharsPerString() bytes).
// Return the given string buffer. Also return the resulting string
// length in length.
//
char* RawView::dumpView(char* stringBuf, size_t& length) const
{

    // No data.
    char* s = stringBuf;
    if ((raw_ == 0) || (rawLength_ == 0))
    {
        s[0] = 0;
        length = 0;
        return stringBuf;
    }

    // Dump all pages except last.
    const unsigned char* p = raw_;
    size_t n = bytesPerRow_ * rowsPerPage_;
    for (size_t page = 0, pageN = (rawLength_ - 1) / n; page < pageN; ++page)
    {
        s = dumpPage(s, p, n);
        p += n;
    }

    // Dump last page.
    n = rawLength_ - (p - raw_);
    if (n > 0)
    {
        s = dumpPage(s, p, n);
    }

    // Return the given string buffer.
    s[0] = 0;
    length = s - stringBuf;
    return stringBuf;
}


//
// Return the default bytes per row given the spacing.
//
unsigned int RawView::defaultBpr(unsigned int spacing) const
{
    unsigned int bpr;
    switch (spacing)
    {
    case 1:
        bpr = DefaultBprForSpacing1;
        break;
    case 2:
    case 3:
        bpr = DefaultBprForSpacing2Or3;
        break;
    default:
        bpr = DefaultBpr;
        break;
    }

    return bpr;
};


//!
//! Return the string size required for the ASCII form.
//!
unsigned int RawView::computeCharsPerString() const
{

    // Start with two characters per byte, some indentation, and a
    // newline. The offset part uses 12 characters. The ASCII part 
    // uses one character per byte plus a space. Make sure the spaces
    // in the hexadecimal contents are accounted for.
    unsigned int charsPerRow = indent_ + (bytesPerRow_ << 1) + 1;
    if (showOffset_)
    {
        charsPerRow += 12;
    }
    if (spacing_)
    {
        for (unsigned int i = 1; i <= bytesPerRow_; ++i)
        {
            if ((i%spacing_) == 0)
            {
                ++charsPerRow;
            }
        }
    }
    else
    {
        ++charsPerRow;
    }
    if (showAscii_)
    {
        if ((spacing_ == 0) || (bytesPerRow_ % spacing_))
        {
            ++charsPerRow;
        }
        charsPerRow += bytesPerRow_ + 1;
    }

    // Compute characters per page. The last row does not have the newline.
    unsigned int charsPerPage = charsPerRow * rowsPerPage_ - 1;

    // Compute characters per string. Make sure the page separators are
    // accounted for. Make sure the indentation difference is accounted for.
    unsigned int bytesPerPage = bytesPerRow_ * rowsPerPage_;
    unsigned int numFullPages = static_cast<unsigned int>(rawLength_) / bytesPerPage;
    unsigned int bytesInPartialPage = static_cast<unsigned int>(rawLength_) % bytesPerPage;
    int delta = firstIndent_ - indent_;
    unsigned int rowsInPartialPage;
    if (bytesInPartialPage)
    {
        --delta; //adjustment for missing newline in last row of partial page
        rowsInPartialPage = (bytesInPartialPage - 1) / bytesPerRow_ + 1;
    }
    else
    {
        rowsInPartialPage = 0;
    }
    if (numFullPages)
    {
        delta += (numFullPages - 1) << 1; //page separators
        if (rowsInPartialPage)
        {
            delta += 2;
        }
    }
    unsigned int charsPerString = numFullPages * charsPerPage + //full pages
        rowsInPartialPage * charsPerRow + //partial page
        delta + //miscellaneous adjustments (page separators, indentation)
        1; //terminating null

    // Return the number of characters in this ASCII view.
    return charsPerString;
}


//
// Copy the ASCII view configuration from the given raw data.
//
void RawView::copyConfig(const RawView& rawView)
{
    bytesPerRow_ = rawView.bytesPerRow_;
    firstIndent_ = rawView.firstIndent_;
    indent_ = rawView.indent_;
    offset_ = rawView.offset_;
    rowsPerPage_ = rawView.rowsPerPage_;
    showAscii_ = rawView.showAscii_;
    showOffset_ = rawView.showOffset_;
    spacing_ = rawView.spacing_;
}


//
// Use default values for the ASCII view.
//
void RawView::useDefaults()
{
    firstIndent_ = DefaultFirstIndent;
    indent_ = DefaultIndent;
    offset_ = DefaultOffset;
    rowsPerPage_ = DefaultRowsPerPage;
    showAscii_ = true;
    showOffset_ = true;
    spacing_ = DefaultSpacing;
    setBytesPerRow(0); //must be set after spacing
}

END_NAMESPACE1
