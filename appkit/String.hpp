/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_STRING_HPP
#define APPKIT_STRING_HPP

#include "syskit/RefCounted.hpp"
#include "syskit/Utf8Seq.hpp"
#include "syskit/sys.hpp"

DECLARE_CLASS1(syskit, Bom)
DECLARE_CLASS1(syskit, Utf16Seq)
DECLARE_CLASS1(syskit, Utf8)
class StringSuite;

BEGIN_NAMESPACE1(appkit)

class QuotedString;


//! UTF-capable copy-on-write string
class String
    //!
    //! Yet another UTF-capable COW string class. Example:
    //!\code
    //! String path("./");
    //! path += childName;
    //! if (! path.isAscii())
    //! {
    //!   // Special handling for non-ASCII file paths.
    //! }
    //!\endcode
    //!
{

public:
    class Ascii8;
    class W;

    static const unsigned int INVALID_INDEX;

    // Constructors and destructor.
    String();
    String(const String& str);
    String(const String& str, size_t startAt, size_t charCount);
    String(const syskit::Utf16Seq& seq);
    String(const syskit::Utf8Seq& seq);
    String(const char* s);
    String(const char* s, size_t length);
    String(const wchar_t* s);
    String(const wchar_t* s, size_t numWchars);
    String(size_t count, char c);
    String(size_t count, wchar_t c);
    String(unsigned int capacity);
    ~String();

    // Operators.
    bool operator !=(const String& str) const;
    bool operator !=(const char* s) const;
    bool operator !=(const wchar_t* s) const;
    bool operator <(const String& str) const;
    bool operator <=(const String& str) const;
    bool operator >(const String& str) const;
    bool operator >=(const String& str) const;
    bool operator ==(const String& str) const;
    bool operator ==(const char* s) const;
    bool operator ==(const wchar_t* s) const;
    const String& operator +=(char c);
    const String& operator +=(const String& str);
    const String& operator +=(const syskit::Utf16Seq& seq);
    const String& operator +=(const syskit::Utf8Seq& seq);
    const String& operator +=(const syskit::Utf8& c);
    const String& operator +=(const char* s);
    const String& operator +=(const wchar_t* s);
    const String& operator +=(wchar_t c);
    const String& operator =(const String& str);
    const String& operator =(const syskit::Utf16Seq& seq);
    const String& operator =(const syskit::Utf8Seq& seq);
    const String& operator =(const char* s);
    const String& operator =(const wchar_t* s);
    syskit::utf32_t operator [](size_t index) const;
    static void operator delete(void* p, size_t size);
    static void operator delete(void* p, void* buf);
    static void* operator new(size_t size);
    static void* operator new(size_t size, void* buf);

    // Raw access.
    bool isAscii() const;
    const char* ascii() const;
    const syskit::utf8_t* raw() const;
    const syskit::utf8_t* raw(unsigned int& byteSize) const;
    const syskit::Utf8Seq& asUtf8Seq() const;
    syskit::utf8_t* detachRaw();
    syskit::utf8_t* detachRaw(unsigned int& byteSize);
    void attachRaw(syskit::utf8_t* s, size_t numU8s, size_t numChars);
    unsigned int capacity() const;
    wchar_t peek(size_t index) const;

    Ascii8 formAscii8(char defaultChar = '?', unsigned int* invalidCharCount = 0) const;
    syskit::Utf16Seq formUtf16Seq() const;
    W widen() const;
    unsigned char* formUtfX(unsigned int& byteSize, const syskit::Bom& bom, bool addBom = false) const;

    String formBox(size_t indent0, size_t indent, size_t width) const;
    String substr(size_t startAt, size_t charCount = static_cast<size_t>(0) - 1) const;
    bool contains(const String& str, bool searchBackward = false) const;
    bool contains(const syskit::utf32_t c, bool searchBackward = true) const;
    bool empty() const;
    bool endsWith(const String& suffix, bool ignoreCase = false) const;
    bool endsWith(const char* suffix, bool ignoreCase = false) const;
    bool startsWith(const String& prefix, bool ignoreCase = false) const;
    bool startsWith(const char* prefix, bool ignoreCase = false) const;
    unsigned int byteSize() const;
    unsigned int find(const String& str, size_t startAt = 0) const;
    unsigned int find(syskit::utf32_t c, size_t startAt = 0) const;
    unsigned int hash() const;
    unsigned int length() const;
    unsigned int rfind(const String& str, size_t startAt = INVALID_INDEX) const;
    unsigned int rfind(syskit::utf32_t c, size_t startAt = INVALID_INDEX) const;

    bool resize(unsigned int newCap);
    unsigned int reset8(const syskit::utf8_t* s, size_t numU8s);
    unsigned int reset16(const syskit::utf16_t* s, size_t numU16s);
    unsigned int resetX(const syskit::Bom& bom, const void* p, size_t byteSize);
    unsigned int resetX(const void* p, size_t byteSize);
    void reset();
    void reset(const String& str, size_t startAt, size_t charCount);
    void reset(const char* s, size_t length);
    void reset(const syskit::utf8_t* s, size_t numU8s, size_t numChars);
    void reset(const wchar_t* s, size_t numWchars);
    void reset(size_t count, char c);
    void reset(size_t count, wchar_t c);

    bool dequote(bool unescapeSingleQuotedString = true);
    bool truncate(size_t length);
    const String& replace(char oldC, char newC);
    void append(const Ascii8& s, size_t length);
    void append(const String& str, size_t startAt, size_t charCount);
    void append(const char* s, size_t length);
    void append(const syskit::utf8_t* s, size_t numU8s, size_t numChars);
    void append(const wchar_t* s, size_t numWchars);
    void append(size_t count, char c);
    void append(size_t count, wchar_t c);
    void setAscii(size_t index, char c);
    void trimSpace(bool trimL = true, bool trimR = true);

    static int compareKP(const void* item0, const void* item1);
    static int compareKPI(const void* item0, const void* item1);
    static int compareP(const void* item0, const void* item1);
    static int comparePI(const void* item0, const void* item1);
    static int comparePIR(const void* item0, const void* item1);
    static int comparePR(const void* item0, const void* item1);
    static unsigned int hashP(const void* item, size_t numBuckets);


    //! array of null-terminated 8-bit ASCII characters
    class Ascii8
        //!
        //! String view as an array of null-terminated 8-bit ASCII characters.
        //!
    {
    public:
        Ascii8(char* a8);
        Ascii8(const Ascii8& a8);
        Ascii8(unsigned char* a8);
        ~Ascii8();
        operator const char*() const;
        operator const unsigned char*() const;
        const Ascii8& operator =(const Ascii8& a8);
        char* detachS8();
        const char* asS8() const;
        const unsigned char* asU8() const;
        unsigned char* detachU8();
    private:
        char mutable* a8_;
    };


    //! array of null-terminated wide characters
    class W
        //!
        //! String view as an array of null-terminated wide characters.
        //!
    {
    public:
        W(const W& w);
        W(wchar_t* w);
        ~W();
        operator const wchar_t*() const;
        const W& operator =(const W& w);
        const wchar_t* raw() const;
        wchar_t* detachRaw();
    private:
        wchar_t mutable* w_;
    };

private:
    typedef unsigned char* (String::*formUtfX_t)(size_t& byteSize, const syskit::Bom& bom) const;

    class S: public syskit::RefCounted, public syskit::Utf8Seq
    {
    public:
        S(const S& s);
        S(const syskit::Utf8Seq& seq);
        S(const syskit::Utf8Seq& seq, size_t startAt, size_t charCount);
        S(unsigned int capacity = syskit::Utf8Seq::DefaultCap);
        S(syskit::utf8_t* s, size_t numU8s, size_t numChars);
        using syskit::Utf8Seq::addNull;
        using syskit::Utf8Seq::addNullIfNone;
        using syskit::Utf8Seq::resetWithNull;
        using syskit::Utf8Seq::rmNull;
        using syskit::Utf8Seq::seek;
        static void operator delete(void* p, size_t size);
        static void* operator new(size_t size);
    protected:
        virtual ~S();
    private:
        const S& operator =(const S&); //prohibit usage
    };

    S* s_;

    static const formUtfX_t formX_[][2];

    String(S*);
    unsigned char* form16(size_t&, const syskit::Bom&) const;
    unsigned char* form16WithBom(size_t&, const syskit::Bom&) const;
    unsigned char* form23(size_t&, const syskit::Bom&) const;
    unsigned char* form23WithBom(size_t&, const syskit::Bom&) const;
    unsigned char* form32(size_t&, const syskit::Bom&) const;
    unsigned char* form32WithBom(size_t&, const syskit::Bom&) const;
    unsigned char* form61(size_t&, const syskit::Bom&) const;
    unsigned char* form61WithBom(size_t&, const syskit::Bom&) const;
    unsigned char* form8(size_t&, const syskit::Bom&) const;
    unsigned char* form8WithBom(size_t&, const syskit::Bom&) const;
    void cow();
    void row();

    static S* emptyStringRef();
    static bool compareW(void*, size_t, syskit::utf32_t);
    static syskit::utf8_t* form8(size_t, syskit::utf32_t, size_t&);

    friend class QuotedString;
    friend class ::StringSuite;

};

String operator +(const String& a, char b);
String operator +(const String& a, const String& b);
String operator +(const String& a, const char* b);
String operator +(const String& a, const wchar_t* b);
String operator +(const String& a, wchar_t b);

String operator +(char a, const String& b);
String operator +(const char* a, const String& b);
String operator +(const wchar_t* a, const String& b);
String operator +(wchar_t a, const String& b);

END_NAMESPACE1

#include <ostream>
#include "syskit/Bom.hpp"
#include "syskit/BufPool.hpp"
#include "syskit/Utf16Seq.hpp"

BEGIN_NAMESPACE1(appkit)

inline String::String(S* s)
{
    s_ = s;
}

//! Return true if this string does not equal given string.
inline bool String::operator !=(const String& str) const
{
    return (*s_ != *str.s_);
}

//! Return true if this string does not equal given string.
inline bool String::operator !=(const char* s) const
{
    return s_->isAscii()? (strcmp(reinterpret_cast<const char*>(s_->raw()), s) != 0): true;
}

//! Return true if this string does not equal given string
inline bool String::operator !=(const wchar_t* s) const
{
    bool isEq = s_->applyLoToHi(compareW, const_cast<wchar_t*>(s));
    return (!isEq);
}

//! Return true if this string is less than given string.
//! Ignore locale.
inline bool String::operator <(const String& str) const
{
    return (*s_ < *str.s_);
}

//! Return true if this string is less than or equal to given string.
//! Ignore locale.
inline bool String::operator <=(const String& str) const
{
    return (*s_ <= *str.s_);
}

//! Return true if this string is greater than given string.
//! Ignore locale.
inline bool String::operator >(const String& str) const
{
    return (*s_ > *str.s_);
}

//! Return true if this string is greater than or equal to given string.
//! Ignore locale.
inline bool String::operator >=(const String& str) const
{
    return (*s_ >= *str.s_);
}

//! Return true if this string equals given string.
inline bool String::operator ==(const String& str) const
{
    return (*s_ == *str.s_);
}

//! Return true if this string equals given string.
inline bool String::operator ==(const char* s) const
{
    return s_->isAscii()? (strcmp(reinterpret_cast<const char*>(s_->raw()), s) == 0): false;
}

//! Return true if this string equals given string
inline bool String::operator ==(const wchar_t* s) const
{
    bool isEq = s_->applyLoToHi(compareW, const_cast<wchar_t*>(s));
    return isEq;
}

//! Reset instance with given UTF16 sequence.
inline const String& String::operator =(const syskit::Utf16Seq& seq)
{
    row();
    s_->reset(seq.raw(), seq.numU16s(), seq.numChars());
    s_->addNullIfNone();
    return *this;
}

//! Reset instance with given UTF8 sequence.
inline const String& String::operator =(const syskit::Utf8Seq& seq)
{
    row();
    s_->reset(seq.raw(), seq.byteSize(), seq.numChars());
    s_->addNullIfNone();
    return *this;
}

//! Return the character residing at given index.
inline syskit::utf32_t String::operator [](size_t index) const
{
    return (*s_)[index];
}

inline void String::operator delete(void* p, size_t size)
{
    syskit::BufPool::freeBuf(p, size);
}

inline void String::operator delete(void* /*p*/, void* /*buf*/)
{
}

inline void* String::operator new(size_t size)
{
    void* buf = syskit::BufPool::allocateBuf(size);
    return buf;
}

inline void* String::operator new(size_t /*size*/, void* buf)
{
    return buf;
}

//! Return true if this contains given substring. If searchBackward
//! is true, look right to left. Otherwise, look left to right.
inline bool String::contains(const String& str, bool searchBackward) const
{
    unsigned int foundAt = searchBackward? rfind(str): find(str);
    bool found = (foundAt != INVALID_INDEX);
    return found;
}

//! Return true if this contains given character. If searchBackward
//! is true, look right to left. Otherwise, look left to right.
inline bool String::contains(const syskit::utf32_t c, bool searchBackward) const
{
    unsigned int foundAt = searchBackward? rfind(c): find(c);
    bool found = (foundAt != INVALID_INDEX);
    return found;
}

//! Return true if this is an empty string.
inline bool String::empty() const
{
    return (s_->numChars() == 1);
}

//! Return true if this is a 7-bit ASCII string.
inline bool String::isAscii() const
{
    return s_->isAscii();
}

//! Return the string as a UTF8-sequence.
inline const syskit::Utf8Seq& String::asUtf8Seq() const
{
    return *s_;
}

//! Return the null-terminated ASCII string assuming instance is a 7-bit ASCII string.
inline const char* String::ascii() const
{
    return s_->ascii();
}

//! Return the raw UTF8 sequence. Use byteSize() to obtain its
//! length in bytes. Use length() to obtain its character count.
inline const syskit::utf8_t* String::raw() const
{
    return s_->raw();
}

//! Return the raw UTF8 sequence. Also return its length in bytes
//! in byteSize. Use length() to obtain its character count.
inline const syskit::utf8_t* String::raw(unsigned int& byteSize) const
{
    return s_->raw(byteSize);
}

//! Return the string size in bytes including the terminating null.
inline unsigned int String::byteSize() const
{
    return s_->byteSize();
}

//! Return the string length (number of characters) excluding the terminating null.
inline unsigned int String::length() const
{
    return (s_->numChars() - 1);
}

//! Reset instance with given UTF16 sequence (numU16s shorts starting at s).
//! Return the number of invalid characters seen in the conversion process.
inline unsigned int String::reset16(const syskit::utf16_t* s, size_t numU16s)
{
    row();
    unsigned int invalidCharCount = s_->convert16(s, numU16s);
    s_->addNullIfNone();
    return invalidCharCount;
}

//! Reset instance with given UTF8 sequence (numU8s bytes starting at s).
//! Return the number of invalid characters seen in the conversion process.
inline unsigned int String::reset8(const syskit::utf8_t* s, size_t numU8s)
{
    row();
    unsigned int invalidCharCount = s_->convert8(s, numU8s);
    s_->addNullIfNone();
    return invalidCharCount;
}

//! Form UTFx sequence. Prepend generated sequence with byte-order-mark if
//! indicated so. The returned sequence is allocated using the heap and is
//! to be freed by caller using the delete[] operator when done. Return the
//! generated sequence. Also return the generated sequence size in byteSize.
inline unsigned char* String::formUtfX(unsigned int& byteSize, const syskit::Bom& bom, bool addBom) const
{
    size_t size;
    formUtfX_t f = formX_[bom.type()][addBom? 1: 0];
    unsigned char* p = (this->*f)(size, bom);
    byteSize = static_cast<unsigned int>(size);
    return p;
}

//! Return the string capacity. A string can hold up to about capacity bytes
//! before growth occurs.
inline unsigned int String::capacity() const
{
    return s_->capacity();
}

//! Detach raw UTF8 sequence from instance. The return sequence is allocated
//! from the heap and is to be freed by the caller using the delete[] operator
//! when done. Instance is now disabled and must not be used further.
inline syskit::utf8_t* String::detachRaw()
{
    cow();
    return s_->detachRaw();
}

//! Detach raw UTF8 sequence from instance. The return sequence is allocated
//! from the heap and is to be freed by the caller using the delete[] operator
//! when done. Instance is now disabled and must not be used further.
inline syskit::utf8_t* String::detachRaw(unsigned int& byteSize)
{
    cow();
    return s_->detachRaw(byteSize);
}

//! Reset instance by making it an empty string.
inline void String::reset()
{
    s_->rmRef();
    s_ = emptyStringRef();
}

//! Construct instance with charCount characters starting at startAt from
//! given string.
inline void String::reset(const String& str, size_t startAt, size_t charCount)
{
    row();
    s_->reset(*str.s_, startAt, charCount);
    s_->addNullIfNone();
}

//! Reset instance with given UTF8 string.
inline void String::reset(const syskit::utf8_t* s, size_t numU8s, size_t numChars)
{
    row();
    s_->reset(s, numU8s, numChars);
    s_->addNullIfNone();
}

//! Treat string as a null-terminated 7-bit ASCII string and update the given character.
//! Don't do any error checking.
inline void String::setAscii(size_t index, char c)
{
    cow();
    s_->setU8(index, c);
}

//! Copy constructor. Transfer resource ownership. Nullify source.
inline String::Ascii8::Ascii8(const Ascii8& a8)
{
    a8_ = a8.a8_;
    a8.a8_ = 0;
}

//! Take over ownership of given pointer.
//! Delete pointer using the delete[] operator when destructed.
inline String::Ascii8::Ascii8(char* a8)
{
    a8_ = a8;
}

//! Take over ownership of given pointer.
//! Delete pointer using the delete[] operator when destructed.
inline String::Ascii8::Ascii8(unsigned char* a8)
{
    a8_ = reinterpret_cast<char*>(a8);
}

inline String::Ascii8::operator const char*() const
{
    return a8_;
}

inline String::Ascii8::operator const unsigned char*() const
{
    return reinterpret_cast<const unsigned char*>(a8_);
}

//! Detach raw string from instance. The returned string is allocated using 
//! the heap and is to be freed by the caller using the delete[] operator
//! when done.
inline char* String::Ascii8::detachS8()
{
    char* a8 = a8_;
    a8_ = 0;
    return a8;
}

inline const char* String::Ascii8::asS8() const
{
    return a8_;
}

inline const unsigned char* String::Ascii8::asU8() const
{
    return reinterpret_cast<const unsigned char*>(a8_);
}

//! Detach raw string from instance. The returned string is allocated using 
//! the heap and is to be freed by the caller using the delete[] operator
//! when done.
inline unsigned char* String::Ascii8::detachU8()
{
    char* a8 = a8_;
    a8_ = 0;
    return reinterpret_cast<unsigned char*>(a8);
}

inline void String::S::operator delete(void* p, size_t size)
{
    syskit::BufPool::freeBuf(p, size);
}

inline void* String::S::operator new(size_t size)
{
    void* buf = syskit::BufPool::allocateBuf(size);
    return buf;
}

//! Copy constructor. Transfer resource ownership. Nullify source.
inline String::W::W(const W& w)
{
    w_ = w.w_;
    w.w_ = 0;
}

//! Take over ownership of given pointer.
//! Delete pointer using the delete[] operator when destructed.
inline String::W::W(wchar_t* w)
{
    w_ = w;
}

inline String::W::operator const wchar_t*() const
{
    return w_;
}

inline const wchar_t* String::W::raw() const
{
    return w_;
}

//! Detach raw string from instance. The returned string is allocated using 
//! the heap and is to be freed by the caller using the delete[] operator
//! when done.
inline wchar_t* String::W::detachRaw()
{
    wchar_t* w = w_;
    w_ = 0;
    return w;
}

inline bool operator !=(const char* a, const String& b)
{
    return (b != a);
}

inline bool operator !=(const wchar_t* a, const String& b)
{
    return (b != a);
}

inline bool operator ==(const char* a, const String& b)
{
    return (b == a);
}

inline bool operator ==(const wchar_t* a, const String& b)
{
    return (b == a);
}

//! Append the UTF8 form of the string to the given output stream.
//! Return the resulting output stream.
inline std::ostream& operator <<(std::ostream& os, const String& str)
{
    return os.write(str.ascii(), static_cast<std::streamsize>(str.byteSize()));
}

END_NAMESPACE1

#if __linux || __CYGWIN__
#include "appkit/linux/String-linux.hpp"

#elif _WIN32
#include "appkit/win/String-win.hpp"

#endif
#endif
