/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_RAW_VIEW_H
#define APPKIT_RAW_VIEW_H

#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)

class String;


//! raw data view
class RawView
    //!
    //! A class representing binary data with an ASCII view. The ASCII view
    //! consists of three parts: optional hexadecimal offsets, hexadecimal
    //! contents, and optional ASCII contents. Trivial methods are available
    //! to configure the ASCII view (e.g., bytesPerRow(), rowsPerPage(),
    //! showAscii(), showOffset(), spacing(), etc.). Sample default ASCII
    //! view:
    //!\code
    //! [00000073]  2f2f2120 4120636c 61737320 72657072  //! A class repr
    //! [00000083]  6573656e 74696e67 2062696e 61727920  esenting binary
    //! [00000093]  64617461 20776974 6820616e 20415343  data with an ASC
    //! [000000a3]  49492076 6965772e 20546865 20415343  II view. The ASC
    //! [000000b3]  49492076 6965770d 0a2f2f21 2063      II view..//! c..
    //!\endcode
    //!
{

public:
    enum
    {

        //! bytes per row when spacing is 0 or greater than 3
        DefaultBpr = 16,

        //! bytes per row when spacing is 1
        DefaultBprForSpacing1 = 16,

        //! bytes per row when spacing is 2 or 3
        DefaultBprForSpacing2Or3 = 18,

        DefaultFirstIndent = 0,
        DefaultIndent = 0,
        DefaultOffset = 0,
        DefaultRowsPerPage = 20,
        DefaultSpacing = 4
    };

    // Constructors and destructor.
    RawView();
    RawView(bool showAscii,
        bool showOffset,
        unsigned int bytesPerRow,
        unsigned int firstIndent,
        unsigned int indent,
        unsigned int offset,
        unsigned int rowsPerPage,
        unsigned int spacing,
        const void* raw,
        size_t length);
    RawView(const RawView& rawView);
    RawView(const void* raw, size_t length);

    // Operators.
    const RawView& operator =(const RawView& rawView);

    // ASCII view configuration.
    bool asciiIsOn() const;
    bool offsetIsOn() const;
    unsigned int bytesPerRow() const;
    unsigned int firstIndent() const;
    unsigned int indent() const;
    unsigned int offset() const;
    unsigned int rowsPerPage() const;
    unsigned int spacing() const;
    void setAsciiMode(bool showAscii);
    void setBytesPerRow(size_t bytesPerRow);
    void setFirstIndent(size_t firstIndent);
    void setIndent(size_t firstIndent, size_t indent);
    void setIndent(size_t indent);
    void setOffset(size_t offset);
    void setOffsetMode(bool showOffset);
    void setRowsPerPage(size_t rowsPerPage);
    void setSpacing(size_t spacing, size_t bytesPerRow = 0);

    // Raw access.
    const unsigned char* raw() const;
    const unsigned char* raw(size_t& length) const;
    size_t rawLength() const;
    void reset(const void* raw, size_t length);

    String toString() const;

private:
    const unsigned char* raw_;
    size_t rawLength_;

    bool showAscii_;
    bool showOffset_;
    unsigned int bytesPerRow_;
    unsigned int firstIndent_;
    unsigned int indent_;
    unsigned int offset_;
    unsigned int rowsPerPage_;
    unsigned int spacing_;

    char* dumpPage(char*, const unsigned char*, size_t) const;
    char* dumpRow(char*, const unsigned char*, size_t) const;
    char* dumpView(char*, size_t& length) const;
    unsigned int computeCharsPerString() const;
    unsigned int defaultBpr(unsigned int) const;
    void copyConfig(const RawView&);
    void useDefaults();

};

END_NAMESPACE1

#include "appkit/String.hpp"

BEGIN_NAMESPACE1(appkit)

//! Return true if the optional ASCII part is enabled.
inline bool RawView::asciiIsOn() const
{
    return showAscii_;
}

//! Return true if the optional offset part is enabled.
inline bool RawView::offsetIsOn() const
{
    return showOffset_;
}

inline const unsigned char* RawView::raw() const
{
    return raw_;
}

inline const unsigned char* RawView::raw(size_t& length) const
{
    length = rawLength_;
    return raw_;
}

inline size_t RawView::rawLength() const
{
    return rawLength_;
}

//! Return the number of bytes per row.
inline unsigned int RawView::bytesPerRow() const
{
    return bytesPerRow_;
}

//! Return the number of spaces before the first indent.
inline unsigned int RawView::firstIndent() const
{
    return firstIndent_;
}

//! Return the number of spaces for subsequent indents.
inline unsigned int RawView::indent() const
{
    return indent_;
}

//! Return the offset of the first byte.
inline unsigned int RawView::offset() const
{
    return offset_;
}

//! Return the number of rows per page.
inline unsigned int RawView::rowsPerPage() const
{
    return rowsPerPage_;
}

//! Return the number of bytes per column.
inline unsigned int RawView::spacing() const
{
    return spacing_;
}

inline void RawView::reset(const void* raw, size_t length)
{
    raw_ = static_cast<const unsigned char*>(raw);
    rawLength_ = length;
}

//! Configure the optional ASCII part.
inline void RawView::setAsciiMode(bool showAscii)
{
    showAscii_ = showAscii;
}

//! Configure the number of bytes per row. If the given value is zero,
//! a default bytes per row will be used.
inline void RawView::setBytesPerRow(size_t bytesPerRow)
{
    bytesPerRow_ = (bytesPerRow > 0)? static_cast<unsigned int>(bytesPerRow): defaultBpr(spacing_);
}

//! Configure the number of spaces for the first indent.
inline void RawView::setFirstIndent(size_t firstIndent)
{
    firstIndent_ = static_cast<unsigned int>(firstIndent);
}

//! Configure the number of spaces for both the first indent and subsequent
//! indents.
inline void RawView::setIndent(size_t indent)
{
    firstIndent_ = static_cast<unsigned int>(indent);
    indent_ = firstIndent_;
}

//! Configure the number of spaces for the first indent and the number of
//! spaces for subsequent indents.
inline void RawView::setIndent(size_t firstIndent, size_t indent)
{
    firstIndent_ = static_cast<unsigned int>(firstIndent);
    indent_ = static_cast<unsigned int>(indent);
}

//! Configure the offset of the first byte.
inline void RawView::setOffset(size_t offset)
{
    offset_ = static_cast<unsigned int>(offset);
}

//! Configure the optional hexadecimal offsets.
inline void RawView::setOffsetMode(bool showOffset)
{
    showOffset_ = showOffset;
}

//! Configure the number of rows per page.
inline void RawView::setRowsPerPage(size_t rowsPerPage)
{
    rowsPerPage_ = (rowsPerPage > 0)? static_cast<unsigned int>(rowsPerPage): DefaultRowsPerPage;
}

//! Configure the number of bytes per column and the number of bytes per row.
//! If the number of bytes per row is zero, a default bytes per row will be
//! used.
inline void RawView::setSpacing(size_t spacing, size_t bytesPerRow)
{
    spacing_ = static_cast<unsigned int>(spacing);
    bytesPerRow_ = (bytesPerRow > 0)? static_cast<unsigned int>(bytesPerRow): defaultBpr(spacing_);
}

//! Append ASCII form of given raw data into given output stream.
//! Return given output stream.
inline std::ostream& operator <<(std::ostream& os, const RawView& rawView)
{
    String s(rawView.toString());
    return os.write(s.ascii(), static_cast<std::streamsize>(s.byteSize()));
}

END_NAMESPACE1

#endif
