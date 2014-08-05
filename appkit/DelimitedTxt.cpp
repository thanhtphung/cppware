/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/DelimitedTxt.hpp"
#include "appkit/StringVec.hpp"

using namespace syskit;

const char CARRIAGE_RETURN = '\r';
const char LINE_FEED = '\n';
const utf32_t CARRIAGE_RETURN32 = '\r';

BEGIN_NAMESPACE1(appkit)


//!
//! Construct instance using empty text. Use given delimiter when parsing for lines.
//!
DelimitedTxt::DelimitedTxt(char delim)
{
    delim_ = delim;
    p1_ = 0;
    p2_ = 0;
    pEnd_ = 0;
    txt_ = 0;
    txtIsMine_ = true;
}


//!
//! Construct a duplicate instance of the given text. The attached text is a
//! shallow copy if delimitedTxt holds a shallow copy. The attached text is a
//! deep copy if delimitedTxt holds a deep copy.
//!
DelimitedTxt::DelimitedTxt(const DelimitedTxt& delimitedTxt)
{
    txt_ = 0;
    txtIsMine_ = false;
    copyFrom(delimitedTxt);
}


//!
//! Construct a duplicate instance of the given text. However, do not maintain
//! the iterating state.  The attached text is a deep copy if makeCopy is true.
//! 
DelimitedTxt::DelimitedTxt(const DelimitedTxt& delimitedTxt, bool makeCopy)
{
    delim_ = delimitedTxt.delim_;
    p1_ = 0;
    p2_ = 0;
    txt_ = 0;
    txtIsMine_ = false;
    size_t length = delimitedTxt.pEnd_ - delimitedTxt.txt_;
    setTxt(reinterpret_cast<const char*>(delimitedTxt.txt_), length, makeCopy);
}


//!
//! Construct instance using given text. A deep copy of the given text is made if
//! makeCopy is true.
//!
DelimitedTxt::DelimitedTxt(const String& txt, bool makeCopy, char delim)
{
    delim_ = delim;
    txt_ = 0;
    txtIsMine_ = true;
    setTxt(txt, makeCopy);
}


//!
//! Construct an instance with given text (length characters starting at txt). A
//! deep copy of the given text is made if makeCopy is true. Use given delimiter
//! when parsing for lines.
//!
DelimitedTxt::DelimitedTxt(const char* txt, size_t length, bool makeCopy, char delim)
{
    delim_ = delim;
    txt_ = 0;
    txtIsMine_ = true;
    setTxt(txt, length, makeCopy);
}


DelimitedTxt::~DelimitedTxt()
{
    if (txtIsMine_)
    {
        delete[] txt_;
    }
}


//!
//! Iterate the text from high to low (i.e., bottom to top). Invoke callback
//! at each line. The callback should return true to continue iterating and
//! should return false to abort iterating. Return false if the callback aborted
//! the iterating. Return true otherwise.
//!
bool DelimitedTxt::applyHiToLo(cb1_t cb, void* arg) const
{

    // Ignore if attached text is empty.
    if (txt_ == pEnd_)
    {
        return true;
    }

    // Iterate from bottom to top. Invoke callback at each line.
    // Return immediately if aborting.
    const char delim = delim_;
    const utf8_t* p;
    const utf8_t* p2;
    for (p = p2 = pEnd_ - 1; p > txt_;)
    {
        if (*--p == delim)
        {
            if (!cb(arg, reinterpret_cast<const char*>(p + 1), p2 - p))
            {
                return false;
            }
            p2 = p;
        }
    }

    // Still need to take care of the top line.
    return cb(arg, reinterpret_cast<const char*>(p), p2 - p + 1);
}


//!
//! Iterate the text from low to high (i.e., top to bottom). Invoke callback
//! at each line. The callback should return true to continue iterating and
//! should return false to abort iterating. Return false if the callback aborted
//! the iterating. Return true otherwise.
//!
bool DelimitedTxt::applyLoToHi(cb0_t cb, void* arg) const
{
    String line;
    arg0_t arg0 = {cb, arg, &line};
    return applyLoToHi(proxy0, &arg0);
}


//!
//! Iterate the text from low to high (i.e., top to bottom). Invoke callback
//! at each line. The callback should return true to continue iterating and
//! should return false to abort iterating. Return false if the callback aborted
//! the iterating. Return true otherwise.
//!
bool DelimitedTxt::applyLoToHi(cb1_t cb, void* arg) const
{

    // Iterate from top to bottom. Invoke callback at each line.
    // Return immediately if aborting.
    const char delim = delim_;
    const utf8_t* p;
    const utf8_t* p1;
    for (p = p1 = txt_; p < pEnd_; ++p)
    {
        if (*p == delim)
        {
            if (!cb(arg, reinterpret_cast<const char*>(p1), p - p1 + 1))
            {
                return false;
            }
            p1 = p + 1;
        }
    }

    // Still need to take care of the bottom line w/o a line delimiter.
    if (p1 < p)
    {
        return cb(arg, reinterpret_cast<const char*>(p1), p - p1);
    }

    // Return true to indicate the iterating was not aborted.
    return true;
}


//
// Retrieve the next line (i.e., the next line iterating from top to
// bottom). Return true if there's one. Otherwise, return false and
// an empty line (if text is empty or if there's no more lines). The
// first invocation after construction or reset() will return the top
// line.
//
bool DelimitedTxt::getNext(String& line)
{
    const char* s;
    size_t length;
    bool ok = next(s, length)?
        (line.reset8(reinterpret_cast<const utf8_t*>(s), length), true):
        (line.reset(), false);
    return ok;
}


//
// Retrieve the previous line (i.e., the next line iterating from bottom
// to top). Return true if there's one. Otherwise, return false and an
// empty line (if text is empty or if there's no more lines). The first
// invocation after construction or reset() will return the bottom line.
//
bool DelimitedTxt::getPrev(String& line)
{
    const char* s;
    size_t length;
    bool ok = prev(s, length)?
        (line.reset8(reinterpret_cast<const utf8_t*>(s), length), true):
        (line.reset(), false);
    return ok;
}


//!
//! Retrieve the next line (i.e., the next line iterating from top to
//! bottom). Return true if there's one. Otherwise, return false and
//! zeroes in line and length (if text is empty or if there's no more
//! lines). The first invocation after construction or reset() will
//! return the top line.
//!
bool DelimitedTxt::next(const char*& line, size_t& length)
{

    // Assume unsuccessful.
    bool ok = false;

    // See if top line is available.
    if (p1_ == 0)
    {
        if (txt_ < pEnd_)
        {
            ok = true;
            p1_ = txt_;
        }
    }

    // See if next line is available.
    else if ((p2_ + 1) < pEnd_)
    {
        ok = true;
        p1_ = p2_ + 1;
    }

    // Retrieve the next line.
    if (ok)
    {
        const char delim = delim_;
        const utf8_t* p;
        const utf8_t* pLast;
        for (p = p1_, pLast = pEnd_ - 1; p < pLast && *p != delim; ++p);
        p2_ = p;
        line = reinterpret_cast<const char*>(p1_);
        length = p - p1_ + 1;
    }
    else
    {
        line = 0;
        length = 0;
    }

    // Return true if successful.
    return ok;
}


//!
//! Peek at the line below. Return true if there's one. Otherwise, return
//! false and zeroes in line and length (if text is empty or if there's no
//! more lines). The first invocation after construction or reset will
//! return the top line.
//!
bool DelimitedTxt::peekDown(const char*& line, size_t& length) const
{

    // See if top line is available.
    const utf8_t* p1 = 0;
    if (p1_ == 0)
    {
        if (txt_ < pEnd_)
        {
            p1 = txt_;
        }
    }

    // See if next line is available.
    else if ((p2_ + 1) < pEnd_)
    {
        p1 = p2_ + 1;
    }

    // Retrieve the next line.
    bool ok;
    if (p1 != 0)
    {
        const char delim = delim_;
        const utf8_t* p;
        const utf8_t* pLast;
        for (p = p1, pLast = pEnd_ - 1; p < pLast && *p != delim; ++p);
        line = reinterpret_cast<const char*>(p1);
        length = p - p1 + 1;
        ok = true;
    }
    else
    {
        line = 0;
        length = 0;
        ok = false;
    }

    // Return true if successful.
    return ok;
}


//!
//! Peek at the line above. Return true if there's one. Otherwise, return
//! false and zeroes in line and length (if text is empty or if there's no
//! more lines). The first invocation after construction or reset will
//! return the bottom line.
//!
bool DelimitedTxt::peekUp(const char*& line, size_t& length) const
{

    // See if bottom line is available.
    const utf8_t* p2 = 0;
    if (p2_ == 0)
    {
        if (txt_ < pEnd_)
        {
            p2 = pEnd_ - 1;
        }
    }

    // See if previous line is available.
    else if (p1_ > txt_)
    {
        p2 = p1_ - 1;
    }

    // Retrieve the previous line.
    bool ok;
    if (p2 != 0)
    {
        const char delim = delim_;
        const utf8_t* p;
        for (p = p2 - 1; p >= txt_ && *p != delim; --p);
        line = reinterpret_cast<const char*>(p)+1;
        length = p2 - p;
        ok = true;
    }
    else
    {
        line = 0;
        length = 0;
        ok = false;
    }

    // Return true if successful.
    return ok;
}


//!
//! Retrieve the previous line (i.e., the next line iterating from bottom
//! to top). Return true if there's one. Otherwise, return false and zeroes
//! in line and length (if text is empty or if there's no more lines). The
//! first invocation after construction or reset() will return the bottom line.
//!
bool DelimitedTxt::prev(const char*& line, size_t& length)
{

    // Assume unsuccessful.
    bool ok = false;

    // See if bottom line is available.
    if (p2_ == 0)
    {
        if (txt_ < pEnd_)
        {
            ok = true;
            p2_ = pEnd_ - 1;
        }
    }

    // See if previous line is available.
    else if (p1_ > txt_)
    {
        ok = true;
        p2_ = p1_ - 1;
    }

    // Retrieve the previous line.
    if (ok)
    {
        const char delim = delim_;
        const utf8_t* p;
        for (p = p2_ - 1; p >= txt_ && *p != delim; --p);
        p1_ = p + 1;
        line = reinterpret_cast<const char*>(p1_);
        length = p2_ - p;
    }
    else
    {
        line = 0;
        length = 0;
    }

    // Return true if successful.
    return ok;
}


bool DelimitedTxt::proxy0(void* arg, const char* line, size_t length)
{
    const arg0_t& arg0 = *static_cast<const arg0_t*>(arg);
    arg0.line->reset8(reinterpret_cast<const utf8_t*>(line), length);
    return arg0.cb(arg0.arg, *arg0.line);
}


//
// Retrieve the next line (i.e., the next line iterating from top to
// bottom). Return true if there's one. Otherwise, return false and
// an empty line (if text is empty or if there's no more lines). The
// first invocation after construction or reset() will return the top
// line. Also trim the returned line.
//
bool DelimitedTxt::trimNext(String& line)
{
    bool ok = getNext(line);
    if (ok)
    {
        trimLine(line);
    }

    return ok;
}


//
// Retrieve the previous line (i.e., the next line iterating from bottom
// to top). Return true if there's one. Otherwise, return false and an
// empty line (if text is empty or if there's no more lines). The first
// invocation after construction or reset() will return the bottom line.
// Also trim the returned line.
//
bool DelimitedTxt::trimPrev(String& line)
{
    bool ok = getPrev(line);
    if (ok)
    {
        trimLine(line);
    }

    return ok;
}


//!
//! Look at text as a vector of lines. Return true if successful. Return
//! false otherwise (result is partial due to insufficient capacity in
//! provided vector). If doTrimLine is true, the returned lines are also
//! trimmed.
//!
bool DelimitedTxt::vectorize(StringVec& vec, bool doTrimLine, bool reverseOrder) const
{
    String line;
    vec.reset();
    saveArg_t arg = {true /*ok*/, this, &line, &vec};
    cb4_t cb = doTrimLine? trimAndSave: save;
    reverseOrder? applyHiToLo(cb, &arg): applyLoToHi(cb, &arg);
    return arg.ok;
}


//!
//! Trim trailing delimiter (if any) from given line.
//! Return given (trimmed) line.
//!
const String& DelimitedTxt::trimLine(String& line) const
{
    const utf32_t delim32 = delim_;
    size_t n = line.length();
    if ((n > 0) && (line[n - 1] == delim32))
    {
        size_t trimmedLength = ((delim_ == LINE_FEED) && (n > 1) && (line[n - 2] == CARRIAGE_RETURN32))? (n - 2): (n - 1);
        line.truncate(trimmedLength);
    }

    return line;
}


//!
//! Return the total number of lines in the text. Return zero if the attached
//! text is empty. A line consists of consecutive characters terminated by the
//! line delimiter. The bottom line can but does not have to be delimited by
//! the line delimiter. That is, non-empty text consists of at least one line
//! even if the text has no line delimiters.
//!
size_t DelimitedTxt::countLines() const
{

    // Iterate from top to bottom. Count each line.
    const char delim = delim_;
    size_t numLines = 0;
    for (const utf8_t* p = txt_; p < pEnd_; ++p)
    {
        if (*p == delim)
        {
            ++numLines;
        }
    }

    // Still need to take care of the bottom line w/o a line delimiter.
    if ((txt_ != pEnd_) && (*(pEnd_ - 1) != delim))
    {
        ++numLines;
    }

    // Return the total number of lines in the text.
    return numLines;
}


//!
//! Trim trailing delimiter (if any) from given line. Return length of the trimmed line.
//!
size_t DelimitedTxt::trimLine(const char* line, size_t length) const
{
    if ((length > 0) && (line[length - 1] == delim_))
    {
        ((delim_ == LINE_FEED) && (length > 1) && (line[length - 2] == CARRIAGE_RETURN))? (length -= 2): (--length);
    }

    return length;
}


//!
//! Iterate the text from high to low (i.e., bottom to top). Invoke callback at each line.
//!
void DelimitedTxt::applyHiToLo(cb3_t cb, void* arg) const
{
    String line;
    arg3_t arg3 = {cb, arg, &line};
    applyHiToLo(proxy3, &arg3);
}


//!
//! Iterate the text from high to low (i.e., bottom to top). Invoke callback at each line.
//!
void DelimitedTxt::applyHiToLo(cb4_t cb, void* arg) const
{

    // Ignore if attached text is empty.
    if (txt_ == pEnd_)
    {
        return;
    }

    // Iterate from bottom to top. Invoke callback at each line.
    const char delim = delim_;
    const utf8_t* p;
    const utf8_t* p2;
    for (p = p2 = pEnd_ - 1; p > txt_;)
    {
        if (*--p == delim)
        {
            cb(arg, reinterpret_cast<const char*>(p + 1), p2 - p);
            p2 = p;
        }
    }

    // Still need to take care of the top line.
    cb(arg, reinterpret_cast<const char*>(p), p2 - p + 1);
}


//!
//! Iterate the text from low to high (i.e., top to bottom). Invoke callback at each line.
//!
void DelimitedTxt::applyLoToHi(cb3_t cb, void* arg) const
{
    String line;
    arg3_t arg3 = {cb, arg, &line};
    applyLoToHi(proxy3, &arg3);
}


//!
//! Iterate the text from low to high (i.e., top to bottom). Invoke callback at each line.
//!
void DelimitedTxt::applyLoToHi(cb4_t cb, void* arg) const
{

    // Iterate from top to bottom. Invoke callback at each line.
    const char delim = delim_;
    const utf8_t* p;
    const utf8_t* p1;
    for (p = p1 = txt_; p < pEnd_; ++p)
    {
        if (*p == delim)
        {
            cb(arg, reinterpret_cast<const char*>(p1), p - p1 + 1);
            p1 = p + 1;
        }
    }

    // Still need to take care of the bottom line w/o a line delimiter.
    if (p1 < p)
    {
        cb(arg, reinterpret_cast<const char*>(p1), p - p1);
    }
}


void DelimitedTxt::copyFrom(const DelimitedTxt& delimitedTxt)
{
    delim_ = delimitedTxt.delim_;
    size_t length = delimitedTxt.pEnd_ - delimitedTxt.txt_;
    bool makeCopy = delimitedTxt.txtIsMine_;
    setTxt(reinterpret_cast<const char*>(delimitedTxt.txt_), length, makeCopy);
    if (delimitedTxt.p1_ != 0)
    {
        p1_ = txt_ + (delimitedTxt.p1_ - delimitedTxt.txt_);
        p2_ = txt_ + (delimitedTxt.p2_ - delimitedTxt.txt_);
    }
}


void DelimitedTxt::proxy3(void* arg, const char* line, size_t length)
{
    const arg3_t& arg3 = *static_cast<const arg3_t*>(arg);
    arg3.line->reset8(reinterpret_cast<const utf8_t*>(line), length);
    arg3.cb(arg3.arg, *arg3.line);
}


//!
//! Attach to given text (length characters starting at txt). Also reset the
//! iterator. A deep copy of the given text is made if makeCopy is true.
//!
void DelimitedTxt::setTxt(const char* txt, size_t length, bool makeCopy)
{

    // Destroy old copy.
    if (txtIsMine_)
    {
        delete[] txt_;
    }

    // Make private copy of given text if necessary.
    if (makeCopy)
    {
        utf8_t* p = new utf8_t[length];
        memcpy(p, txt, length);
        txt_ = p;
    }
    else
    {
        txt_ = reinterpret_cast<const utf8_t*>(txt);
    }
    txtIsMine_ = makeCopy;
    pEnd_ = txt_ + length;

    // Reset iterator.
    reset();
}


void DelimitedTxt::save(void* arg, const char* line, size_t length)
{
    saveArg_t& r = *static_cast<saveArg_t*>(arg);
    r.line->reset8(reinterpret_cast<const utf8_t*>(line), length);
    bool ok = r.vec->add(*r.line);
    if (!ok)
    {
        r.ok = false;
    }
}


void DelimitedTxt::trimAndSave(void* arg, const char* line, size_t length)
{
    saveArg_t& r = *static_cast<saveArg_t*>(arg);
    length = r.txt->trimLine(line, length);
    r.line->reset8(reinterpret_cast<const utf8_t*>(line), length);
    bool ok = r.vec->add(*r.line);
    if (!ok)
    {
        r.ok = false;
    }
}

END_NAMESPACE1
