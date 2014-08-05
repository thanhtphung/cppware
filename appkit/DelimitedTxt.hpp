/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_DELIMITED_TXT_HPP
#define APPKIT_DELIMITED_TXT_HPP

#include "appkit/String.hpp"
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(appkit)

class StringVec;


//! delimited text
class DelimitedTxt
    //!
    //! A class representing some delimited text. The text is viewed as a sequence
    //! of characters which need to be broken up into lines. A line is a sequence
    //! of characters terminated by a line delimiter. By default, the line
    //! delimiter is the new-line character ('\n'). A different delimiter can
    //! be specified at construction or afterwards using setDelim(). To
    //! iterate downward, use the next() and/or applyLoToHi() methods. To
    //! iterate upward, use the prev() and/or applyHiToLo() methods. Peeking
    //! is supported via the peekDown() and peekUp() methods. Example:
    //!\code
    //! const char* s = "1" "\n" "22" "\n" "333" "\n";
    //! DelimitedTxt txt(s, strlen(s));
    //! :
    //! unsigned int numLines = txt.countLines(); //3
    //! :
    //! String line;
    //! while (txt.next(line))
    //! {
    //!   //do something with each line
    //! }
    //!\endcode
    //!
{

public:
    typedef bool(*cb0_t)(void* arg, const String& line);
    typedef bool(*cb1_t)(void* arg, const char* line, size_t length);
    typedef void(*cb3_t)(void* arg, const String& line);
    typedef void(*cb4_t)(void* arg, const char* line, size_t length);

    // Constructors and destructor.
    DelimitedTxt(char delim = '\n');
    DelimitedTxt(const DelimitedTxt& delimitedTxt);
    DelimitedTxt(const DelimitedTxt& delimitedTxt, bool makeCopy);
    DelimitedTxt(const String& txt, bool makeCopy = false, char delim = '\n');
    DelimitedTxt(const char* txt, size_t length, bool makeCopy = false, char delim = '\n');
    ~DelimitedTxt();

    // Operators.
    const DelimitedTxt& operator =(const DelimitedTxt& delimitedTxt);

    // Iterator support.
    bool applyHiToLo(cb0_t cb, void* arg = 0) const;
    bool applyHiToLo(cb1_t cb, void* arg = 0) const;
    bool applyLoToHi(cb0_t cb, void* arg = 0) const;
    bool applyLoToHi(cb1_t cb, void* arg = 0) const;
    bool next(String& line, bool doTrimLine = false);
    bool next(const char*& line, size_t& length);
    bool peekDown() const;
    bool peekDown(String& line) const;
    bool peekDown(const char*& line, size_t& length) const;
    bool peekUp() const;
    bool peekUp(String& line) const;
    bool peekUp(const char*& line, size_t& length) const;
    bool prev(String& line, bool doTrimLine = false);
    bool prev(const char*& line, size_t& length);
    bool vectorize(StringVec& vec, bool doTrimLine = false, bool reverseOrder = false) const;
    void applyHiToLo(cb3_t cb, void* arg = 0) const;
    void applyHiToLo(cb4_t cb, void* arg = 0) const;
    void applyLoToHi(cb3_t cb, void* arg = 0) const;
    void applyLoToHi(cb4_t cb, void* arg = 0) const;
    void reset();

    // Utilities.
    char delim() const;
    const String& trimLine(String& line) const;
    const syskit::utf8_t* txt() const;
    const syskit::utf8_t* txt(size_t& size) const;
    size_t countLines() const;
    size_t trimLine(const char* line, size_t length) const;
    size_t txtSize() const;
    void setDelim(char delim);
    void setTxt(const String& txt, bool makeCopy = false);
    void setTxt(const char* txt, size_t length, bool makeCopy = false);

private:
    typedef struct
    {
        cb0_t cb;
        void* arg;
        String* line;
    } arg0_t;

    typedef struct
    {
        cb3_t cb;
        void* arg;
        String* line;
    } arg3_t;

    typedef struct
    {
        bool ok;
        const DelimitedTxt* txt;
        String* line;
        StringVec* vec;
    } saveArg_t;

    bool txtIsMine_;
    char delim_;
    const syskit::utf8_t* p1_;
    const syskit::utf8_t* p2_;
    const syskit::utf8_t* pEnd_;
    const syskit::utf8_t* txt_;

    bool getNext(String&);
    bool getPrev(String&);
    bool trimNext(String&);
    bool trimPrev(String&);
    void copyFrom(const DelimitedTxt&);

    static bool proxy0(void*, const char*, size_t);
    static bool proxy2(void*, const char*, size_t);
    static void proxy3(void*, const char*, size_t);
    static void proxy5(void*, const char*, size_t);
    static void save(void*, const char*, size_t);
    static void trimAndSave(void*, const char*, size_t);

};

//! Assignment operator. The attached text is a shallow copy if delimitedTxt
//! holds a shallow copy. The attached text is a deep copy if delimitedTxt
//! holds a deep copy.
inline const DelimitedTxt& DelimitedTxt::operator =(const DelimitedTxt& delimitedTxt)
{
    if (this != &delimitedTxt) copyFrom(delimitedTxt);
    return *this;
}

//! Iterate the text from high to low (i.e., bottom to top). Invoke callback
//! at each line. The callback should return true to continue iterating and
//! should return false to abort iterating. Return false if the callback aborted
//! the iterating. Return true otherwise. Ignore if iterator is unattached.
inline bool DelimitedTxt::applyHiToLo(cb0_t cb, void* arg) const
{
    String line;
    arg0_t arg0 = {cb, arg, &line};
    return applyHiToLo(proxy0, &arg0);
}

//! Retrieve the next line (i.e., the next line iterating from top to
//! bottom). Return true if there's one. Otherwise, return false and
//! an empty line (if text is empty or if there's no more lines). The
//! first invocation after construction or reset() will return the top
//! line. If doTrimLine is true, the returned line is also trimmed.
inline bool DelimitedTxt::next(String& line, bool doTrimLine)
{
    bool ok = doTrimLine? trimNext(line): getNext(line);
    return ok;
}

//! Return true if there's a line below.
inline bool DelimitedTxt::peekDown() const
{
    return (p1_ == 0)? (txt_ < pEnd_): ((p2_ + 1) < pEnd_);
}

//! Peek at the line below. Return true if there's one. Otherwise, return
//! false and an empty line (if text is empty or if there's no more lines).
//! The first invocation after construction or reset will return the top
//! line.
inline bool DelimitedTxt::peekDown(String& line) const
{
    const char* s;
    size_t length;
    return peekDown(s, length)?
        (line.reset8(reinterpret_cast<const syskit::utf8_t*>(s), length), true):
        (line.reset(), false);
}

//! Return true if there's a line above.
inline bool DelimitedTxt::peekUp() const
{
    return (p2_ == 0)? (txt_ < pEnd_): (p1_ > txt_);
}

//! Peek at the line above. Return true if there's one. Otherwise, return
//! false and an empty line (if text is empty or if there's no more lines).
//! The first invocation after construction or reset will return the bottom
//! line.
inline bool DelimitedTxt::peekUp(String& line) const
{
    const char* s;
    size_t length;
    return peekUp(s, length)?
        (line.reset8(reinterpret_cast<const syskit::utf8_t*>(s), length), true):
        (line.reset(), false);
}

//! Retrieve the previous line (i.e., the next line iterating from bottom
//! to top). Return true if there's one. Otherwise, return false and an
//! empty line (if text is empty or if there's no more lines). The first
//! invocation after construction or reset() will return the bottom line.
//! If doTrimLine is true, the returned line is also trimmed.
inline bool DelimitedTxt::prev(String& line, bool doTrimLine)
{
    bool ok = doTrimLine? trimPrev(line): getPrev(line);
    return ok;
}

//! Return the line delimiter.
inline char DelimitedTxt::delim() const
{
    return delim_;
}

//! Return the attached text. Use txtSize() to obtain its size in bytes if necessary.
inline const syskit::utf8_t* DelimitedTxt::txt() const
{
    return txt_;
}

//! Return the attached text. Also return its size in bytes in size.
inline const syskit::utf8_t* DelimitedTxt::txt(size_t& size) const
{
    size = pEnd_ - txt_;
    return txt_;
}

//! Return the attached text size in bytes.
inline size_t DelimitedTxt::txtSize() const
{
    return pEnd_ - txt_;
}

//! Reset the iterator to its initial state. That is, next() will
//! return the top line, and prev() will return bottom line.
inline void DelimitedTxt::reset()
{
    p1_ = 0;
    p2_ = 0;
}

//! Use given delimiter when parsing for lines.
inline void DelimitedTxt::setDelim(char delim)
{
    delim_ = delim;
}

//! Attach iterator to given text. Also reset the iterator. A deep copy
//! of the given text is made if makeCopy is true.
inline void DelimitedTxt::setTxt(const String& txt, bool makeCopy)
{
    setTxt(txt.ascii(), txt.byteSize() - 1, makeCopy);
}

END_NAMESPACE1

#endif
