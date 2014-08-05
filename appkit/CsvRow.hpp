/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_CSV_ROW_HPP
#define APPKIT_CSV_ROW_HPP

#include "appkit/String.hpp"
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(appkit)


//! comma-separated-value row
class CsvRow
    //!
    //! A class representing a CSV (comma-separated-value) row. A row consists
    //! of one or more columns separated by a delimiter. By default, the column
    //! delimiter is the comma (','). A different delimiter can be specified at
    //! construction. To iterate, use the next() and/or apply() methods. Example:
    //!\code
    //! String s("1,22,333");
    //! CsvRow row(s);
    //! :
    //! String col;
    //! while (row.next(col))
    //! {
    //!   //do something with each column
    //! }
    //!\endcode
    //!
{

public:
    typedef bool(*cb0_t)(void* arg, const String& col);
    typedef bool(*cb1_t)(void* arg, const char* col, size_t length);
    typedef void(*cb3_t)(void* arg, const String& col);
    typedef void(*cb4_t)(void* arg, const char* col, size_t length);

    // Constructors and destructor.
    CsvRow(const String& row, char delim = ',');
    CsvRow(const char* row, size_t length, char delim = ',');
    ~CsvRow();

    // Iterator support.
    bool apply(cb0_t cb, void* arg = 0) const;
    bool apply(cb1_t cb, void* arg = 0) const;
    bool next(String& col, bool doDequoteCol = false, bool unescapeSingleQuotedString = true);
    bool next(const char*& col, size_t& length);
    void apply(cb3_t cb, void* arg = 0) const;
    void apply(cb4_t cb, void* arg = 0) const;
    void reset();

    // Utilities.
    char delim() const;
    const String& trimCol(String& col) const;
    const syskit::utf8_t* row() const;
    const syskit::utf8_t* row(size_t& size) const;
    size_t countCols() const;
    size_t rowSize() const;
    size_t trimCol(const char* col, size_t length) const;

    static bool dequoteCol(String& col, bool unescapeSingleQuotedString = true);

private:
    typedef struct
    {
        cb0_t cb;
        void* arg;
        String* col;
    } arg0_t;

    typedef struct
    {
        cb3_t cb;
        void* arg;
        String* col;
    } arg3_t;

    char delim_;
    char isAscii_;
    const syskit::utf8_t* p1_;
    const syskit::utf8_t* p2_;
    const syskit::utf8_t* pEnd_;
    const syskit::utf8_t* row_;

    CsvRow(const CsvRow&); //prohibit usage
    const CsvRow& operator =(const CsvRow&); //prohibit usage

    bool dequoteNext(String&, bool);
    bool getNext(String&);

    static bool proxy0(void*, const char*, size_t);
    static bool proxy0a(void*, const char*, size_t);
    static void proxy3(void*, const char*, size_t);
    static void proxy3a(void*, const char*, size_t);

};

inline CsvRow::~CsvRow()
{
}

//! Retrieve the next column. Return true if there's one. Otherwise, return
//! false and an empty column (if row is empty or if there's no more columns).
//! The first invocation after construction or reset() will return the first
//! column. If doDequoteCol is true, the returned column is also trimmed and
//! dequoted. For a single quoted column, escape sequence resolution can be
//! skipped if unescapeSingleQuotedString is false.
inline bool CsvRow::next(String& col, bool doDequoteCol, bool unescapeSingleQuotedString)
{
    bool ok = doDequoteCol? dequoteNext(col, unescapeSingleQuotedString): getNext(col);
    return ok;
}

//! Return the column delimiter.
inline char CsvRow::delim() const
{
    return delim_;
}

//! Trim trailing delimiter (if any) from given column. Return given (trimmed) column.
inline const String& CsvRow::trimCol(String& col) const
{
    size_t n = col.byteSize() - 1; //index of last byte in front of terminating null
    if ((n > 0) && (col.raw()[n - 1] == static_cast<syskit::utf8_t>(delim_))) col.truncate(col.length() - 1);
    return col;
}

//! Return the attached row. Use rowSize() to obtain its size in bytes if necessary.
inline const syskit::utf8_t* CsvRow::row() const
{
    return row_;
}

//! Return the attached row. Also return its size in bytes in size.
inline const syskit::utf8_t* CsvRow::row(size_t& size) const
{
    size = pEnd_ - row_;
    return row_;
}

//! Return the attached row size in bytes. Return zero if iterator is unattached.
inline size_t CsvRow::rowSize() const
{
    return pEnd_ - row_;
}

//! Trim trailing delimiter (if any) from given column. Return length of the trimmed column.
inline size_t CsvRow::trimCol(const char* col, size_t length) const
{
    return ((length > 0) && (col[length - 1] == delim_))? (length - 1): (length);
}

//! Reset the row iterator to its initial state.
//! That is, next() will return the first column.
inline void CsvRow::reset()
{
    p1_ = 0;
    p2_ = 0;
}

END_NAMESPACE1

#endif
