/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/CsvRow.hpp"
#include "appkit/S32.hpp"
#include "appkit/Str.hpp"

using namespace syskit;

const char BACKSLASH = '\\';
const char DOUBLE_QUOTE = '"';
const char SINGLE_QUOTE = '\'';

BEGIN_NAMESPACE1(appkit)


//!
//! Construct instance using given row. A shallow copy of the given row is made.
//!
CsvRow::CsvRow(const String& row, char delim)
{
    delim_ = delim;
    isAscii_ = row.isAscii()? 1: 0;
    row_ = row.raw();
    pEnd_ = row_ + row.byteSize() - 1;
    reset();
}


//!
//! Construct an instance with given row (length characters starting at row). A
//! shallow copy of the given row is made. Use given delimiter when parsing for
//! columns.
//!
CsvRow::CsvRow(const char* row, size_t length, char delim)
{
    delim_ = delim;
    isAscii_ = Str::isAscii(row, length)? 1: 0;
    row_ = reinterpret_cast<const utf8_t*>(row);
    pEnd_ = row_ + length;
    reset();
}


//!
//! Iterate the row from left to right. Invoke callback at each column. The callback
//! should return true to continue iterating and should return false to abort iterating.
//! Return false if the callback aborted the iterating. Return true otherwise.
//!
bool CsvRow::apply(cb0_t cb, void* arg) const
{
    String col;
    arg0_t arg0 = {cb, arg, &col};
    cb1_t proxy = (isAscii_ != 0)? proxy0a: proxy0;
    return apply(proxy, &arg0);
}


//!
//! Iterate the row from left to right. Invoke callback at each column. The callback
//! should return true to continue iterating and should return false to abort iterating.
//! Return false if the callback aborted the iterating. Return true otherwise.
//!
bool CsvRow::apply(cb1_t cb, void* arg) const
{

    // Iterate from left to right. Invoke callback at each column.
    // Return immediately if aborting.
    const char delim = delim_;
    const utf8_t* p;
    const utf8_t* p1;
    char quote = 0;
    for (p = p1 = row_; p < pEnd_; ++p)
    {
        if (quote == 0)
        {
            if (p[0] == delim)
            {
                if (!cb(arg, reinterpret_cast<const char*>(p1), p - p1 + 1))
                {
                    return false;
                }
                p1 = p + 1;
            }
            else if ((p[0] == DOUBLE_QUOTE) || (p[0] == SINGLE_QUOTE))
            {
                quote = p[0];
            }
        }
        else if ((p[0] == quote) && (p[-1] != BACKSLASH))
        {
            quote = 0;
        }
    }

    // Still need to take care of the last column w/o a column delimiter.
    if (p1 < p)
    {
        return cb(arg, reinterpret_cast<const char*>(p1), p - p1);
    }

    // Return true to indicate the iterating was not aborted.
    return true;
}


//!
//! Remove surrounding quotes from given column. Also resolve escape sequences
//! treating the column as a C++ string literal. As an exception, replace two
//! consecutive quotes, if any, with one. No-op if given column is not quoted.
//! Return true if column was modified (due to surrounding quote removal and/or
//! escape sequence resolution). For a single quoted column, escape sequence
//! resolution can be skipped if unescapeSingleQuotedString is false.
//!
bool CsvRow::dequoteCol(String& col, bool unescapeSingleQuotedString)
{

    // Quoted string.
    bool modified;
    const char* p = col.ascii();
    const char* pLast = p + col.byteSize() - 2;
    if ((col.length() >= 2) && ((*p == DOUBLE_QUOTE) || (*p == SINGLE_QUOTE)) && (p < pLast) && (*pLast == *p))
    {
        --pLast;
        for (char quote = *p++; p < pLast; ++p)
        {
            if ((p[0] == quote) && (p[1] == quote))
            {
                size_t index = p - col.ascii();
                col.setAscii(index, BACKSLASH); //"\"\"" --> "\\\"" and "\'\'" --> "\\\'"
                p = col.ascii() + index + 1;
                pLast = col.ascii() + col.byteSize() - 2 - 1;
            }
        }
        modified = col.dequote(unescapeSingleQuotedString);
    }

    // No surrounding quotes.
    else
    {
        modified = false;
    }

    // Return true if column was modified.
    return modified;
}


//
// Retrieve the next column. Return true if there's one. Otherwise, return
// false and an empty column (if row is empty or if there's no more columns).
// The first invocation after construction or reset() will return the first
// column. Also trim and dequote the returned column. For a single quoted column,
// escape sequence resolution can be skipped if unescapeSingleQuotedString is
// false.
//
bool CsvRow::dequoteNext(String& col, bool unescapeSingleQuotedString)
{
    bool ok = getNext(col);
    if (ok)
    {
        trimCol(col);
        dequoteCol(col, unescapeSingleQuotedString);
    }

    return ok;
}


//
// Retrieve the next column. Return true if there's one. Otherwise, return
// false and an empty column (if row is empty or if there's no more columns).
// The first invocation after construction or reset() will return the first
// column.
//
bool CsvRow::getNext(String& col)
{
    const char* s;
    size_t length;
    bool ok = next(s, length);
    if (!ok)
    {
        col.reset();
    }
    else if (isAscii_ != 0)
    {
        col.reset(s, length);
    }
    else
    {
        col.reset8(reinterpret_cast<const utf8_t*>(s), length);
    }

    return ok;
}


//!
//! Retrieve the next column. Return true if there's one. Otherwise, return
//! false and zeroes in col and length (if row is empty or if there's no more
//! columns). The first invocation after construction or reset() will return
//! the first column.
//!
bool CsvRow::next(const char*& col, size_t& length)
{

    // Assume unsuccessful.
    bool ok = false;

    // See if first column is available.
    if (p1_ == 0)
    {
        if (row_ < pEnd_)
        {
            ok = true;
            p1_ = row_;
        }
    }

    // See if next column is available.
    else if ((p2_ + 1) < pEnd_)
    {
        ok = true;
        p1_ = p2_ + 1;
    }

    // Retrieve the next column.
    // If a column delimiter is in a quoted string, consider it part of the string.
    if (ok)
    {
        const utf8_t* p;
        const utf8_t* pLast;
        const char delim = delim_;
        char quote = 0;
        for (p = p1_, pLast = pEnd_ - 1; p < pLast; ++p)
        {
            if (quote == 0)
            {
                if (p[0] == delim) break;
                else if ((p[0] == DOUBLE_QUOTE) || (p[0] == SINGLE_QUOTE)) quote = p[0];
            }
            else if ((p[0] == quote) && (p[-1] != BACKSLASH))
            {
                quote = 0;
            }
        }
        p2_ = p;
        col = reinterpret_cast<const char*>(p1_);
        length = p - p1_ + 1;
    }
    else
    {
        col = 0;
        length = 0;
    }

    // Return true if successful.
    return ok;
}


bool CsvRow::proxy0(void* arg, const char* col, size_t length)
{
    const arg0_t& arg0 = *static_cast<const arg0_t*>(arg);
    arg0.col->reset8(reinterpret_cast<const utf8_t*>(col), length);
    return arg0.cb(arg0.arg, *arg0.col);
}


bool CsvRow::proxy0a(void* arg, const char* col, size_t length)
{
    const arg0_t& arg0 = *static_cast<const arg0_t*>(arg);
    arg0.col->reset(col, length);
    return arg0.cb(arg0.arg, *arg0.col);
}


//!
//! Return the total number of columns in the row. Return zero if the attached
//! row is empty. A column consists of consecutive characters terminated by the
//! column delimiter. The last column can but does not have to be delimited by
//! the column delimiter. That is, non-empty row consists of at least one column
//! even if the row has no column delimiters.
//!
size_t CsvRow::countCols() const
{

    // Iterate from left to right. Count each column.
    const char delim = delim_;
    size_t numCols = 0;
    char quote = 0;
    for (const utf8_t* p = row_; p < pEnd_; ++p)
    {
        if (quote == 0)
        {
            if (p[0] == delim) ++numCols;
            else if ((p[0] == DOUBLE_QUOTE) || (p[0] == SINGLE_QUOTE)) quote = p[0];
        }
        else if ((p[0] == quote) && (p[-1] != BACKSLASH))
        {
            quote = 0;
        }
    }

    // Still need to take care of the last column w/o a column delimiter.
    if ((row_ != pEnd_) && (*(pEnd_ - 1) != delim))
    {
        ++numCols;
    }

    // Return the total number of columns in the text.
    return numCols;
}


//!
//! Iterate the row from left to right. Invoke callback at each column.
//!
void CsvRow::apply(cb3_t cb, void* arg) const
{
    String col;
    arg3_t arg3 = {cb, arg, &col};
    cb4_t proxy = (isAscii_ != 0)? proxy3a: proxy3;
    apply(proxy, &arg3);
}


//!
//! Iterate the row from left to right. Invoke callback at each column.
//!
void CsvRow::apply(cb4_t cb, void* arg) const
{

    // Iterate from left to right. Invoke callback at each column.
    const char delim = delim_;
    const utf8_t* p;
    const utf8_t* p1;
    char quote = 0;
    for (p = p1 = row_; p < pEnd_; ++p)
    {
        if (quote == 0)
        {
            if (p[0] == delim)
            {
                cb(arg, reinterpret_cast<const char*>(p1), p - p1 + 1);
                p1 = p + 1;
            }
            else if ((p[0] == DOUBLE_QUOTE) || (p[0] == SINGLE_QUOTE))
            {
                quote = p[0];
            }
        }
        else if ((p[0] == quote) && (p[-1] != BACKSLASH))
        {
            quote = 0;
        }
    }

    // Still need to take care of the last column w/o a column delimiter.
    if (p1 < p)
    {
        cb(arg, reinterpret_cast<const char*>(p1), p - p1);
    }
}


void CsvRow::proxy3(void* arg, const char* col, size_t length)
{
    const arg3_t& arg3 = *static_cast<const arg3_t*>(arg);
    arg3.col->reset8(reinterpret_cast<const utf8_t*>(col), length);
    arg3.cb(arg3.arg, *arg3.col);
}


void CsvRow::proxy3a(void* arg, const char* col, size_t length)
{
    const arg3_t& arg3 = *static_cast<const arg3_t*>(arg);
    arg3.col->reset(col, length);
    arg3.cb(arg3.arg, *arg3.col);
}

END_NAMESPACE1
