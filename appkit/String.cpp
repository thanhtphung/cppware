/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/Bom.hpp"
#include "syskit/Utf16Seq.hpp"
#include "syskit/Utf8.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/QuotedString.hpp"
#include "appkit/S32.hpp"
#include "appkit/Str.hpp"
#include "appkit/String.hpp"
#include "appkit/U16.hpp"
#include "appkit/U32.hpp"
#include "appkit/U8.hpp"

using namespace syskit;

const unsigned int MAX_ASCII8 = 0xffU;

BEGIN_NAMESPACE

class EmptyString: public RefCounted, public appkit::String
{
public:
    virtual ~EmptyString();
    static const EmptyString* instance();
private:
    static bool destructed_;
    EmptyString();
    EmptyString(const EmptyString&); //prohibit usage
    const EmptyString& operator =(const EmptyString&); //prohibit usage
};

bool EmptyString::destructed_ = false;

EmptyString::EmptyString():
RefCounted(0U),
String(Utf8Seq::DefaultCap)
{
}

EmptyString::~EmptyString()
{
    destructed_ = true;
}

// Return the EmptyString reference-counted singleton constructed at first use.
// Return zero if the singleton has been destructed (via automatic static data
// destruction) or is being constructed.
const EmptyString* EmptyString::instance()
{
    static const EmptyString* s_emptyString = new EmptyString;
    return destructed_? 0: s_emptyString;
}

static RefCounted::Count s_emptyStringLock(*EmptyString::instance());

END_NAMESPACE

BEGIN_NAMESPACE1(appkit)

inline const utf8_t* findFirst(const utf8_t* s, utf32_t c)
{
    return reinterpret_cast<const utf8_t*>(strchr(reinterpret_cast<const char*>(s), static_cast<char>(c)));
}

inline const utf8_t* strstr(const utf8_t* s, const String& k)
{
    const char* str = reinterpret_cast<const char*>(s);
    const char* key = k.ascii();
    const utf8_t* p = reinterpret_cast<const utf8_t*>(::strstr(str, key));
    return p;
}

const String::formUtfX_t String::formX_[][2] =
{
    {&String::form8, &String::form8WithBom},  //Bom::None
    {&String::form8, &String::form8WithBom},  //Bom::Utf8
    {&String::form16, &String::form16WithBom}, //Bom::Utf16
    {&String::form61, &String::form61WithBom}, //Bom::Utf61
    {&String::form32, &String::form32WithBom}, //Bom::Utf32
    {&String::form23, &String::form23WithBom}  //Bom::Utf23
};

const unsigned int String::INVALID_INDEX = 0xffffffffU;


//!
//! Construct an empty string.
//!
String::String()
{
    s_ = emptyStringRef();
}


//!
//! Construct an instance initialized with given UTF16 sequence.
//!
String::String(const Utf16Seq& seq)
{
    if (seq.numU16s() > 0)
    {
        s_ = new S;
        s_->reset(seq.raw(), seq.numU16s(), seq.numChars());
        s_->addNullIfNone();
    }
    else
    {
        s_ = emptyStringRef();
    }
}


//!
//! Construct an instance initialized with given UTF8 sequence.
//!
String::String(const Utf8Seq& seq)
{
    if (seq.byteSize() > 0)
    {
        s_ = new S(seq);
        s_->addNullIfNone();
    }
    else
    {
        s_ = emptyStringRef();
    }
}


//!
//! Construct a duplicate instance of the given string.
//!
String::String(const String& str)
{
    S* s = str.s_;
    s->addRef();
    s_ = s;
}


//!
//! Construct instance with given substring (charCount characters starting at startAt).
//!
String::String(const String& str, size_t startAt, size_t charCount)
{
    if (charCount > 0)
    {
        s_ = new S(*str.s_, startAt, charCount);
        s_->addNullIfNone();
    }
    else
    {
        s_ = emptyStringRef();
    }
}


//!
//! Construct an instance initialized with given null-terminated ASCII string.
//!
String::String(const char* s)
{
    if (*s != 0)
    {
        s_ = new S;
        s_->reset(s, strlen(s) + 1);
    }
    else
    {
        s_ = emptyStringRef();
    }
}


//!
//! Construct an instance initialized with given ASCII string (length characters
//! starting at s).
//!
String::String(const char* s, size_t length)
{
    if (length > 0)
    {
        s_ = new S;
        s_->reset(s, length);
        s_->addNullIfNone();
    }
    else
    {
        s_ = emptyStringRef();
    }
}


//!
//! Construct an empty string. String exponentially grows by doubling its capacity.
//! If given capacity is zero, then an initial capacity of one will be used instead.
//! A string can hold up to about capacity bytes before growth occurs.
//!
String::String(unsigned int capacity)
{

    // All empty strings constructed via other constructors use the EmptyString
    // singleton when possible. This constructor creates its own empty string.
    s_ = new S(capacity);
    s_->resetWithNull();
}


//!
//! Construct an ASCII string with count characters (c).
//! For example, String(5,'c') == "ccccc".
//!
String::String(size_t count, char c)
{
    if (count > 0)
    {
        s_ = new S;
        s_->append(count, c);
        s_->addNull();
    }
    else
    {
        s_ = emptyStringRef();
    }
}


//!
//! Construct a string with count characters (c).
//! For example, String(5, L'c') == L"ccccc".
//!
String::String(size_t count, wchar_t c)
{
    if (count > 0)
    {
        size_t seqLength;
        utf8_t* s = form8(count, static_cast<utf32_t>(c), seqLength);
        s_ = new S(s, seqLength, count + 1);
    }
    else
    {
        s_ = emptyStringRef();
    }
}


String::~String()
{
    s_->rmRef();
}


//!
//! Append given ASCII character to this string.
//!
const String& String::operator +=(char c)
{
    cow();
    s_->rmNull();
    char s[1 + 1] = {c, 0};
    s_->append(s, 1 + 1);

    return *this;
}


//!
//! Append given string to this string.
//!
const String& String::operator +=(const String& str)
{
    if (str.s_->numChars() > 1)
    {
        cow();
        s_->rmNull();
        *s_ += *str.s_;
    }

    return *this;
}


//!
//! Append given UTF16 sequence to this string.
//!
const String& String::operator +=(const Utf16Seq& seq)
{
    if (seq.numChars() > 0)
    {
        Utf8Seq tail;
        tail.reset(seq.raw(), seq.numU16s(), seq.numChars());
        cow();
        s_->rmNull();
        *s_ += tail;
        s_->addNullIfNone();
    }

    return *this;
}


//!
//! Append given character to this string.
//!
const String& String::operator +=(const Utf8& c)
{
    cow();
    s_->rmNull();
    *s_ += c;
    s_->addNullIfNone();

    return *this;
}


//!
//! Append given UTF8 sequence to this string.
//!
const String& String::operator +=(const Utf8Seq& seq)
{
    if (seq.numChars() > 0)
    {
        cow();
        s_->rmNull();
        *s_ += seq;
        s_->addNullIfNone();
    }

    return *this;
}


//!
//! Append given null-terminated ASCII string to this string.
//!
const String& String::operator +=(const char* s)
{
    if (*s != 0)
    {
        cow();
        s_->rmNull();
        s_->append(s, strlen(s) + 1);
    }

    return *this;
}


//!
//! Append given null-terminated wide-character string to this string.
//!
const String& String::operator +=(const wchar_t* s)
{
    if (*s != 0)
    {
        String tail(s);
        *this += tail;
    }

    return *this;
}


//!
//! Append given wide character to this string.
//!
const String& String::operator +=(wchar_t c)
{
    cow();
    s_->rmNull();
    utf8_t seq8[Utf8::MaxSeqLength + 1];
    Utf8 c8(static_cast<unsigned int>(c));
    size_t length8 = c8.encode(seq8);
    seq8[length8] = 0;
    s_->append(seq8, length8 + 1, 1 + 1);

    return *this;
}


const String& String::operator =(const String& str)
{
    if (this != &str)
    {
        S* s = str.s_;
        s->addRef();
        s_->rmRef();
        s_ = s;
    }

    return *this;
}


//!
//! Reset instance with given null-terminated ASCII string.
//!
const String& String::operator =(const char* s)
{
    if (*s != 0)
    {
        row();
        s_->reset(s, strlen(s) + 1);
    }
    else
    {
        s_->rmRef();
        s_ = emptyStringRef();
    }

    return *this;
}


//!
//! Some long strings, when wrapped, don't look nice with the rest of the output.
//! This method offers a way to wrap a long string in an imaginary box. Input is
//! the box characteristics, and return value is the boxed string.
//!
String String::formBox(size_t indent0, size_t indent, size_t width) const
{

    // Simple case. No need for indentation. Fits in a one-line box.
    size_t newLength = length();
    if ((indent0 == 0) && (newLength <= width))
    {
        String box(*this);
        return box;
    }

    // Another simple case. Need some indentation. Fits in a one-line box.
    newLength += indent0;
    if (newLength <= width)
    {
        String box(indent0, ' ');
        box += *this;
        return box;
    }

    // Form box. Each line except the last holds exactly width characters. The
    // last line holds the remaining characters. Indent first line by indent0.
    // Indent subsequent lines by indent.
    if (isAscii())
    {
        size_t height = length() / width + 1;
        newLength += (indent + 1) * (height - 1);
        utf8_t* buf = new utf8_t[newLength + 1];
        utf8_t* dst = buf;
        const char* src = ascii();
        memset(dst, ' ', indent0);
        dst += indent0;
        for (size_t i = 1; i < height; ++i)
        {
            memcpy(dst, src, width);
            dst += width;
            src += width;
            *dst++ = '\n';
            if (indent > 0)
            {
                memset(dst, ' ', indent);
                dst += indent;
            }
        }
        size_t lengthN = length() % width;
        if (lengthN > 0)
        {
            memcpy(dst, src, lengthN);
            dst += lengthN;
        }
        else
        {
            newLength -= indent + 1;
            dst -= indent + 1;
        }
        *dst = 0;
        String box;
        box.attachRaw(buf, newLength + 1, newLength + 1);
        return box;
    }

    // TODO: add non-ASCII support.
    return *this;
}


//!
//! Return given substring (charCount characters starting at startAt).
//! For example, String("abc123").substr(3, 2) == "12". If startAt is
//! beyond the string's end, an empty substring is returned (e.g.,
//! String("abc123").substr(999, 2) == ""). Given substring may extend
//! beyond the string's end (e.g., String("abc123").substr(3, 999) == "123").
//!
String String::substr(size_t startAt, size_t charCount) const
{
    size_t maxIndex = s_->numChars() - 1;
    size_t maxCharCount = maxIndex - startAt;
    return (startAt < maxIndex)?
        String(*this, startAt, (charCount > maxCharCount)? maxCharCount: charCount):
        String(emptyStringRef());
}


//!
//! Return string as an array of null-terminated 8-bit ASCII characters.
//!
String::Ascii8 String::formAscii8(char defaultChar, unsigned int* invalidCharCount) const
{

    // 7-bit ASCII.
    char* a8;
    if (s_->isAscii())
    {
        size_t byteSize = s_->byteSize();
        char* a8 = new char[byteSize];
        memcpy(a8, s_->raw(), byteSize);
        return a8;
    }

    // 8-bit ASCII.
    a8 = new char[s_->numChars()];
    const utf8_t* p = s_->raw();
    const utf8_t* pEnd = p + s_->byteSize();
    unsigned int nonAscii8s = 0;
    unsigned char* dst = reinterpret_cast<unsigned char*>(a8);
    for (Utf8 c; p < pEnd;)
    {
        p += c.decode(p);
        if (c <= MAX_ASCII8)
        {
            *dst++ = static_cast<unsigned char>(c.asU32());
        }
        else
        {
            *dst++ = defaultChar;
            ++nonAscii8s;
        }
    }

    if (invalidCharCount != 0)
    {
        *invalidCharCount = nonAscii8s;
    }

    return a8;
}


//
// If the EmptyString singleton is available (the norm), increment the reference
// count and return its gut. Otherwise (e.g., the singleton has been destructed),
// return a new empty string.
//
String::S* String::emptyStringRef()
{
    String::S* s;
    const EmptyString* emptyString = EmptyString::instance();
    if (emptyString == 0)
    {
        s = new S(Utf8Seq::DefaultCap);
        s->resetWithNull();
    }
    else
    {
        s = emptyString->s_;
        s->addRef();
    }

    return s;
}


//!
//! Return the string as a UTF16 sequence.
//!
Utf16Seq String::formUtf16Seq() const
{
    Utf16Seq seq16;
    seq16.reset(s_->raw(), s_->byteSize(), s_->numChars());
    return &seq16; //move guts from seq16 to returned sequence
}


bool String::compareW(void* arg, size_t index, utf32_t c)
{
    const wchar_t* s = static_cast<wchar_t*>(arg);
    bool keepGoing = (c == static_cast<utf32_t>(s[index]));
    return keepGoing;
}


//!
//! Treat instance as a C++ string literal and remove surrounding quotes (if any) in
//! addition to resolving escape sequences. For example, <"abc\'\"123"> --> <abc'"123>.
//! Return true if instance was modified (due to surrounding quote removal and/or
//! escape sequence resolution). For a single quoted string, escape sequence resolution
//! can be skipped if unescapeSingleQuotedString is false.
//!
bool String::dequote(bool unescapeSingleQuotedString)
{
    bool modified = QuotedString::dequote(*this, unescapeSingleQuotedString);
    return modified;
}


//!
//! Return true if this string ends with given suffix.
//! Ignore case and locale if ignoreCase is true.
//!
bool String::endsWith(const String& suffix, bool ignoreCase) const
{
    bool matched;
    size_t byteSize = suffix.s_->byteSize(); //>0
    if (byteSize <= s_->byteSize())
    {
        if (ignoreCase)
        {
            matched = (Str::compareKI(suffix.s_->raw(), s_->raw() + s_->byteSize() - byteSize) == 0);
        }
        else
        {
            matched = (memcmp(suffix.s_->raw(), s_->raw() + s_->byteSize() - byteSize, byteSize - 1) == 0);
        }
    }
    else
    {
        matched = false;
    }

    return matched;
}


//!
//! Return true if this string ends with given suffix.
//! Ignore case if ignoreCase is true.
//!
bool String::endsWith(const char* suffix, bool ignoreCase) const
{
    bool matched;
    size_t byteSize = strlen(suffix) + 1;
    if (byteSize <= s_->byteSize())
    {
        if (ignoreCase)
        {
            matched = (Str::compareKI(suffix, s_->raw() + s_->byteSize() - byteSize) == 0);
        }
        else
        {
            matched = (memcmp(suffix, s_->raw() + s_->byteSize() - byteSize, byteSize - 1) == 0);
        }
    }
    else
    {
        matched = false;
    }

    return matched;
}


//!
//! Resize string. Return true if successful. Given new capacity must be non-zero and
//! must be large enough to hold current string. A string can hold up to about capacity
//! bytes before growth occurs.
//!
bool String::resize(unsigned int newCap)
{
    bool ok;
    if (newCap != s_->capacity())
    {
        cow();
        ok = s_->resize(newCap);
    }
    else
    {
        ok = true;
    }

    return ok;
}


//!
//! Return true if this string starts with given prefix.
//! Ignore case and locale if ignoreCase is true.
//!
bool String::startsWith(const String& prefix, bool ignoreCase) const
{
    bool matched;
    size_t byteSize = prefix.s_->byteSize(); //>0
    if (byteSize <= s_->byteSize())
    {
        if (ignoreCase)
        {
            matched = (Str::compareKIN(prefix.ascii(), ascii(), byteSize - 1) == 0);
        }
        else
        {
            matched = (memcmp(prefix.s_->raw(), s_->raw(), byteSize - 1) == 0);
        }
    }
    else
    {
        matched = false;
    }

    return matched;
}


//!
//! Return true if this string starts with given prefix.
//! Ignore case if ignoreCase is true.
//!
bool String::startsWith(const char* prefix, bool ignoreCase) const
{
    bool matched = true;
    const char* p0 = prefix;
    const char* p1 = ascii();

    if (ignoreCase)
    {
        while (*p0)
        {
            if (S32::toLower(*p0++) != S32::toLower(*p1++))
            {
                matched = false;
                break;
            }
        }
    }

    else
    {
        while (*p0)
        {
            if (*p0++ != *p1++)
            {
                matched = false;
                break;
            }
        }
    }

    return matched;
}


//!
//! Truncate string if it has more than length characters.
//! Return true if truncation occurred.
//!
bool String::truncate(size_t length)
{
    bool truncated;
    if ((s_->numChars() - 1) <= length)
    {
        truncated = false;
    }
    else
    {
        truncated = true;
        cow();
        s_->truncate(length);
        s_->addNullIfNone();
    }

    return truncated;
}


//!
//! Replace each occurrence of oldC with newC. Return updated string.
//!
const String& String::replace(char oldC, char newC)
{

    // TODO: note that this works for 7-bit ASCII only.
    const utf8_t* p = findFirst(s_->raw(), oldC);
    if (p != 0)
    {
        size_t i = p - s_->raw();
        cow();
        p = s_->raw();
        s_->setU8(i++, newC);
        for (size_t byteSize = s_->byteSize(); i < byteSize; ++i)
        {
            if (p[i] == oldC)
            {
                s_->setU8(i, newC);
            }
        }
    }

    return *this;
}


//!
//! Compare two opaques. Look at the first opaque as an ASCII string and the
//! second opaque as a String pointer. Ignore locale. Return a negative value
//! if *item0 < *item1. Return 0 if *item0 == *item1. Return a positive value
//! if *item0 > *item1.
//!
int String::compareKP(const void* item0, const void* item1)
{
    const char* k0 = static_cast<const char*>(item0);
    const char* k1 = static_cast<const String*>(item1)->ascii();
    int rc = strcmp(k0, k1);
    return rc;
}


//!
//! Compare two opaques. Look at the first opaque as an ASCII string and the
//! second opaque as a String pointer. Ignore locale. Ignore case. Return a
//! negative value if *item0 < *item1. Return 0 if *item0 == *item1. Return a
//! positive value if *item0 > *item1.
//!
int String::compareKPI(const void* item0, const void* item1)
{
    const char* k0 = static_cast<const char*>(item0);
    const char* k1 = static_cast<const String*>(item1)->ascii();
    int rc = Str::compareKI(k0, k1);
    return rc;
}


//!
//! Compare two String instances given their addresses. Ignore locale. Return
//! a negative value if *item0 < *item1. Return 0 if *item0 == *item1. Return
//! a positive value if *item0 > *item1.
//!
int String::compareP(const void* item0, const void* item1)
{
    const char* k0 = static_cast<const String*>(item0)->ascii();
    const char* k1 = static_cast<const String*>(item1)->ascii();
    int rc = strcmp(k0, k1);
    return rc;
}


//!
//! Compare two String instances given their addresses. Ignore locale. Ignore
//! case. Return a negative value if *item0 < *item1. Return 0 if *item0 == *item1.
//! Return a positive value if *item0 > *item1.
//!
int String::comparePI(const void* item0, const void* item1)
{
    const char* k0 = static_cast<const String*>(item0)->ascii();
    const char* k1 = static_cast<const String*>(item1)->ascii();
    int rc = Str::compareKI(k0, k1);
    return rc;
}


//!
//! Compare two String instances given their addresses. Ignore locale. Ignore
//! case. Reverse the normal sense of comparison. Return a positive value if
//! *item0 < *item1. Return 0 if *item0 == *item1. Return a negative value if
//! *item0 > *item1.
//!
int String::comparePIR(const void* item0, const void* item1)
{
    const char* k0 = static_cast<const String*>(item0)->ascii();
    const char* k1 = static_cast<const String*>(item1)->ascii();
    int rc = Str::compareKI(k1, k0);
    return rc;
}


//!
//! Compare two String instances given their addresses. Ignore locale. Reverse
//! the normal sense of comparison. Return a positive value if *item0 < *item1.
//! Return 0 if *item0 == *item1. Return a negative value if *item0 > *item1.
//!
int String::comparePR(const void* item0, const void* item1)
{
    const char* k0 = static_cast<const String*>(item0)->ascii();
    const char* k1 = static_cast<const String*>(item1)->ascii();
    int rc = strcmp(k1, k0);
    return rc;
}


//
// Form UTF16 sequence without BOM.
//
unsigned char* String::form16(size_t& byteSize, const Bom& /*bom*/) const
{
    Utf16Seq seq16;
    seq16.reset(s_->raw(), s_->byteSize(), s_->numChars());
    byteSize = seq16.byteSize();
    unsigned char* seq = new unsigned char[byteSize];
    memcpy(seq, seq16.raw(), byteSize);
    return seq;
}


//
// Form UTF16 sequence with BOM.
//
unsigned char* String::form16WithBom(size_t& byteSize, const Bom& bom) const
{
    Utf16Seq seq16;
    seq16.reset(s_->raw(), s_->byteSize(), s_->numChars());
    size_t bodySize = seq16.byteSize();
    byteSize = bom.byteSize() + bodySize;
    unsigned char* seq = new unsigned char[byteSize];
    unsigned char* p = seq + bom.encode(seq);
    memcpy(p, seq16.raw(), bodySize);
    return seq;
}


//
// Form non-native-endian UTF32 sequence without BOM.
//
unsigned char* String::form23(size_t& byteSize, const Bom& bom) const
{
    unsigned char* seq = form32(byteSize, bom);
    unsigned int* item = reinterpret_cast<unsigned int*>(seq);
    size_t numItems = byteSize >> 2;
    U32::bswap(item, numItems);
    return seq;
}


//
// Form non-native-endian UTF32 sequence with BOM.
//
unsigned char* String::form23WithBom(size_t& byteSize, const Bom& bom) const
{
    size_t bomSize = bom.byteSize();
    unsigned char* seq = form32WithBom(byteSize, bom);
    unsigned int* item = reinterpret_cast<unsigned int*>(seq + bomSize);
    size_t numItems = (byteSize - bomSize) >> 2;
    U32::bswap(item, numItems);
    return seq;
}


//
// Form UTF32 sequence without BOM.
//
unsigned char* String::form32(size_t& byteSize, const Bom& /*bom*/) const
{
    byteSize = s_->numChars() * sizeof(utf32_t);
    unsigned char* seq = new unsigned char[byteSize];
    s_->expand(reinterpret_cast<utf32_t*>(seq));
    return seq;
}


//
// Form UTF32 sequence with BOM.
//
unsigned char* String::form32WithBom(size_t& byteSize, const Bom& bom) const
{
    size_t bodySize = s_->numChars() * sizeof(utf32_t);
    byteSize = bom.byteSize() + bodySize;
    unsigned char* seq = new unsigned char[byteSize];
    unsigned char* p = seq + bom.encode(seq);
    s_->expand(reinterpret_cast<utf32_t*>(p));
    return seq;
}


//
// Form non-native-endian UTF16 sequence without BOM.
//
unsigned char* String::form61(size_t& byteSize, const Bom& bom) const
{
    unsigned char* seq = form16(byteSize, bom);
    unsigned short* item = reinterpret_cast<unsigned short*>(seq);
    size_t numItems = byteSize >> 1;
    U16::bswap(item, numItems);
    return seq;
}


//
// Form non-native-endian UTF16 sequence with BOM.
//
unsigned char* String::form61WithBom(size_t& byteSize, const Bom& bom) const
{
    size_t bomSize = bom.byteSize();
    unsigned char* seq = form16WithBom(byteSize, bom);
    unsigned short* item = reinterpret_cast<unsigned short*>(seq + bomSize);
    size_t numItems = (byteSize - bomSize) >> 1;
    U16::bswap(item, numItems);
    return seq;
}


//
// Form UTF8 sequence without BOM.
//
unsigned char* String::form8(size_t& byteSize, const Bom& /*bom*/) const
{
    byteSize = s_->byteSize();
    unsigned char* seq = new unsigned char[byteSize];
    memcpy(seq, s_->raw(), byteSize);
    return seq;
}


//
// Form UTF8 sequence with BOM.
//
unsigned char* String::form8WithBom(size_t& byteSize, const Bom& bom) const
{
    size_t bodySize = s_->byteSize();
    byteSize = bom.byteSize() + bodySize;
    unsigned char* seq = new unsigned char[byteSize];
    unsigned char* p = seq + bom.encode(seq);
    memcpy(p, s_->raw(), bodySize);
    return seq;
}


//!
//! Find given substring. Look left to right starting at character residing
//! at startAt. Return the character index where the substring is found (e.g.,
//! String("xxx-key-xxx").find("key") returns 4. Return INVALID_INDEX if not
//! found.
//!
unsigned int String::find(const String& str, size_t startAt) const
{
    const utf8_t* pLo = s_->raw();
    const utf8_t* pHi = pLo + byteSize() - str.byteSize();
    size_t foundAt = INVALID_INDEX;
    if ((pLo <= pHi) && (startAt < s_->numChars()))
    {

        if (s_->isAscii())
        {
            const utf8_t* p = strstr(pLo + startAt, str);
            if (p != 0) foundAt = p - pLo;
        }

        else
        {
            pLo = s_->seek(startAt);
            const utf8_t* p = strstr(pLo, str);
            if (p != 0)
            {
                unsigned int numChars;
                Utf8Seq::countChars(pLo, p - pLo, numChars);
                foundAt = startAt + numChars;
            }
        }
    }

    return static_cast<unsigned int>(foundAt);
}


//!
//! Find given character. Look left to right starting at character
//! residing at startAt. Return the located character index. Return
//! INVALID_INDEX if not found.
//!
unsigned int String::find(utf32_t c, size_t startAt) const
{
    size_t foundAt = INVALID_INDEX;
    if (startAt < s_->numChars())
    {

        if (s_->isAscii())
        {
            if (c <= Utf8::MaxAscii)
            {
                const utf8_t* pLo = s_->raw();
                const utf8_t* p = findFirst(pLo + startAt, c);
                if (p != 0) foundAt = p - pLo;
            }
        }

        else if (Utf8::isValid(c))
        {
            Utf8 c8;
            const utf8_t* p = s_->seek(startAt);
            for (size_t i = startAt; *p; ++i)
            {
                p += c8.decode(p);
                if (c8 == c)
                {
                    foundAt = i;
                    break;
                }
            }
        }
    }

    return static_cast<unsigned int>(foundAt);
}


//!
//! Return string as a 32-bit value.
//!
unsigned int String::hash() const
{

    // DJB hash function?
    unsigned int code = 5381;
    for (const utf8_t* p = s_->raw(); *p != 0; ++p)
    {
        code += (code << 5) + *p;
    }

    return code;
}


//!
//! Modular hash function for a string given its address.
//! Return a non-negative number less than numBuckets.
//!
unsigned int String::hashP(const void* item, size_t numBuckets)
{
    const String& s = *static_cast<const String*>(item);
    unsigned int i = (s.hash() % static_cast<unsigned int>(numBuckets));
    return i;
}


//!
//! Reset instance with given UTFx string. Given BOM guides how it's decoded.
//! With a BOM of None, it's assumed to be UTF8. Return the number of invalid
//! characters seen in the conversion process.
//!
unsigned int String::resetX(const Bom& bom, const void* p, size_t byteSize)
{
    row();
    unsigned int invalidCharCount;
    const unsigned char* p8 = static_cast<const unsigned char*>(p);

    switch (bom.type())
    {

    case Bom::Utf16:
    {
        const utf16_t* p16 = reinterpret_cast<const utf16_t*>(p8);
        size_t numU16s = byteSize >> 1;
        invalidCharCount = s_->convert16(p16, numU16s);
        break;
    }

    case Bom::Utf61:
    {
        const utf16_t* p61 = reinterpret_cast<const utf16_t*>(p8);
        size_t numU61s = byteSize >> 1;
        invalidCharCount = s_->convert61(p61, numU61s);
        break;
    }

    case Bom::Utf32:
    {
        const utf32_t* p32 = reinterpret_cast<const utf32_t*>(p8);
        size_t numChars = byteSize >> 2;
        utf32_t defaultChar = '?';
        invalidCharCount = s_->shrink(p32, numChars, defaultChar);
        break;
    }

    case Bom::Utf23:
    {
        const utf32_t* p23 = reinterpret_cast<const utf32_t*>(p8);
        size_t numChars = byteSize >> 2;
        utf32_t* p32 = new utf32_t[numChars];
        U32::bswap(p23, numChars, p32);
        utf32_t defaultChar = '?';
        invalidCharCount = s_->shrink(p32, numChars, defaultChar);
        delete[] p32;
        break;
    }

    default: //Bom::None, Bom::Utf8
        invalidCharCount = s_->convert8(p8, byteSize);
        break;

    }

    s_->addNullIfNone();
    return invalidCharCount;
}


//!
//! Reset instance with given UTFx string. Given string's BOM guides
//! how it's decoded. Without any BOM, it's assumed to be UTF8. Return
//! the number of invalid characters seen in the conversion process.
//!
unsigned int String::resetX(const void* p, size_t byteSize)
{
    Bom bom(Bom::decode(p, byteSize));
    size_t bomSize = bom.byteSize();
    const unsigned char* p8 = static_cast<const unsigned char*>(p)+bomSize;
    unsigned int invalidCharCount = resetX(bom, p8, byteSize - bomSize);
    return invalidCharCount;
}


//!
//! Find given substring. Look right to left starting at character residing
//! at startAt. Return the character index where the substring is found (e.g.,
//! String("xxx-key-xxx").rfind("key") returns 4. Return INVALID_INDEX if not
//! found.
//!
unsigned int String::rfind(const String& str, size_t startAt) const
{
    const utf8_t* pLo = s_->raw();
    const utf8_t* pHi = pLo + byteSize() - str.byteSize();
    unsigned int foundAt = INVALID_INDEX;
    if (pLo <= pHi)
    {

        const utf8_t* p = pHi;
        if (startAt < s_->numChars())
        {
            p = s_->isAscii()? (pLo + startAt): (s_->seek(startAt));
            if (p > pHi) p = pHi;
        }

        size_t n = str.byteSize() - 1;
        for (const utf8_t* key = str.raw(); p >= pLo; --p)
        {
            if (memcmp(p, key, n) == 0)
            {
                if (s_->isAscii()) foundAt = static_cast<unsigned int>(p - pLo);
                else Utf8Seq::countChars(pLo, p - pLo, foundAt);
                break;
            }
        }
    }

    return foundAt;
}


//!
//! Find given character. Look right to left starting at character
//! residing at startAt. Starting position can be INVALID_INDEX to
//! indicate starting at the right end. Return the located character
//! index. Return INVALID_INDEX if not found.
//!
unsigned int String::rfind(utf32_t c, size_t startAt) const
{
    if (startAt >= s_->numChars())
    {
        startAt = s_->numChars() - 1;
    }

    size_t foundAt = INVALID_INDEX;
    if (s_->isAscii())
    {
        if (c <= Utf8::MaxAscii)
        {
            const utf8_t* p0 = s_->raw();
            const utf8_t* p = p0 + startAt;
            do
            {
                if (*p == c)
                {
                    foundAt = p - p0;
                    break;
                }
            } while (--p >= p0);
        }
    }

    else if (Utf8::isValid(c))
    {
        Utf8 c8;
        const utf8_t* p = s_->seek(startAt);
        for (size_t i = startAt;; --i)
        {
            c8.decode(p);
            if (c8 == c)
            {
                foundAt = i;
                break;
            }
            if (i == 0) break;
            while (Utf8::getSeqLength(*--p) == Utf8::InvalidByte0);
        }
    }

    return static_cast<unsigned int>(foundAt);
}


//
// Form null-terminated UTF8 sequence.
//
utf8_t* String::form8(size_t count, utf32_t c, size_t& byteSize)
{
    utf8_t seq8[Utf8::MaxSeqLength];
    Utf8 c8(c);
    size_t length8 = c8.encode(seq8);
    size_t seqLength = length8 * count + 1;
    utf8_t* p = new utf8_t[seqLength];
    const utf8_t* pEnd = p + seqLength - 1;

    switch (length8)
    {
    case 1:
        for (; p < pEnd; p[0] = seq8[0], ++p);
        break;
    case 2:
        for (; p < pEnd; p[0] = seq8[0], p[1] = seq8[1], p += 2);
        break;
    case 3:
        for (; p < pEnd; p[0] = seq8[0], p[1] = seq8[1], p[2] = seq8[2], p += 3);
        break;
    default: //4 (applicable only where sizeof(wchar_t) == 4)
        for (; p < pEnd; p[0] = seq8[0], p[1] = seq8[1], p[2] = seq8[2], p[3] = seq8[3], p += 4);
        break;
    }

    p[0] = 0;
    byteSize = seqLength;
    return p - seqLength + 1;
}


//!
//! Append given substring (charCount characters starting at startAt) to this string.
//!
void String::append(const String& str, size_t startAt, size_t charCount)
{
    if (charCount > 0)
    {
        cow();
        s_->rmNull();
        s_->append(*str.s_, startAt, charCount);
        s_->addNullIfNone();
    }
}


//!
//! Append given ASCII string (length characters starting at s) to this string.
//!
void String::append(const char* s, size_t length)
{
    if (length > 0)
    {
        cow();
        s_->rmNull();
        s_->append(s, length);
        s_->addNullIfNone();
    }
}


//!
//! Append given UTF8 string to this string.
//!
void String::append(const utf8_t* s, size_t numU8s, size_t numChars)
{
    if (numChars > 0)
    {
        cow();
        s_->rmNull();
        s_->append(s, numU8s, numChars);
        s_->addNullIfNone();
    }
}


//!
//! Append given wide-character string (numWchars wide-characters starting at s)
//! to this string.
//!
void String::append(const wchar_t* s, size_t numWchars)
{
    if (numWchars > 0)
    {
        String tail(s, numWchars);
        *this += tail;
    }
}


//!
//! Append given ASCII character to this string count times.
//!
void String::append(size_t count, char c)
{
    if (count > 0)
    {
        cow();
        s_->rmNull();
        s_->append(count, c);
        s_->addNullIfNone();
    }
}


//!
//! Take over ownership of given UTF8 data (numU8s bytes starting at s).
//! Given data holds numChars characters. Delete data using the delete[]
//! operator when appropriate.
//!
void String::attachRaw(utf8_t* s, size_t numU8s, size_t numChars)
{
    if (s_->refCount() > 1U)
    {
        s_->rmRef();
        s_ = new S(s, numU8s, numChars);
    }
    else
    {
        s_->attachRaw(s, numU8s, numChars);
    }
}


//
// Copy-on-write. If guts are shared, make private copy. Otherwise, no-op.
//
void String::cow()
{
    if (s_->refCount() > 1U)
    {
        S* clone = new S(*s_);
        s_->rmRef();
        s_ = clone;
    }
}


//!
//! Reset instance with given ASCII string (length characters starting at s).
//!
void String::reset(const char* s, size_t length)
{
    if (length > 0)
    {
        row();
        s_->reset(s, length);
        s_->addNullIfNone();
    }
    else
    {
        reset();
    }
}


//!
//! Reset instance with count characters (c).
//! For example, (s.reset(5,'c'), s) == "ccccc".
//!
void String::reset(size_t count, char c)
{
    if (count > 0)
    {
        row();
        s_->reset();
        s_->append(count, c);
        s_->addNull();
    }
    else
    {
        reset();
    }
}


//!
//! Reset instance with count characters (c).
//! For example, (s.reset(5, L'c'), s) == L"ccccc".
//!
void String::reset(size_t count, wchar_t c)
{
    if (count > 0)
    {
        size_t seqLength;
        utf8_t* s = form8(count, static_cast<utf32_t>(c), seqLength);
        s_->rmRef();
        s_ = new S(s, seqLength, count + 1);
    }
    else
    {
        reset();
    }
}


//
// Reset-on-write. If guts are shared, make private copy of empty string using
// current capacity. Otherwise, no-op.
//
void String::row()
{
    if (s_->refCount() > 1U)
    {
        unsigned int capacity = s_->capacity();
        s_->rmRef();
        s_ = new S(capacity);
    }
}


//!
//! Trim leading and/or trailing white space.
//!
void String::trimSpace(bool trimL, bool trimR)
{

    // Skip leading white space.
    const unsigned char* p1 = s_->raw();
    const unsigned char* p2 = p1 + s_->byteSize() - 1 - 1;
    if (trimL)
    {
        for (; S32::isSpace(*p1); ++p1);
    }

    // Skip trailing white space.
    size_t trailingSpaces = 0;
    if (trimR)
    {
        for (; (p2 >= p1) && S32::isSpace(*p2); --p2, ++trailingSpaces);
    }

    // Form result.
    if ((p1 > s_->raw()) || (trailingSpaces > 0))
    {
        if (p1 <= p2)
        {
            if (p1 == s_->raw())
            {
                cow();
                s_->truncate(s_->byteSize() - 1 - trailingSpaces);
                s_->addNullIfNone();
            }
            else
            {
                size_t startAt = p1 - s_->raw();
                size_t charCount = s_->numChars() - 1 - trailingSpaces - startAt;
                *this = String(*this, startAt, charCount);
            }
        }
        else
        {
            reset();
        }
    }
}


//!
//! Assignment operator. Transfer resource ownership. Nullify source.
//!
const String::Ascii8& String::Ascii8::operator =(const Ascii8& a8)
{
    if (this != &a8)
    {
        char* toBeDeleted = a8_;
        a8_ = a8.a8_;
        a8.a8_ = 0;
        delete[] toBeDeleted;
    }

    return *this;
}


String::Ascii8::~Ascii8()
{
    delete[] a8_;
}


String::S::S(const S& s):
RefCounted(1U),
Utf8Seq(s)
{
}


String::S::S(const Utf8Seq& seq):
RefCounted(1U),
Utf8Seq(seq)
{
}


String::S::S(const Utf8Seq& seq, size_t startAt, size_t charCount):
RefCounted(1U),
Utf8Seq(seq, startAt, charCount)
{
}


String::S::S(unsigned int capacity):
RefCounted(1U),
Utf8Seq(capacity)
{
}


String::S::S(utf8_t* s, size_t numU8s, size_t numChars):
RefCounted(1U),
Utf8Seq(s, numU8s, numChars)
{
}


String::S::~S()
{
}


//!
//! Assignment operator. Transfer resource ownership. Nullify source.
//!
const String::W& String::W::operator =(const W& w)
{
    if (this != &w)
    {
        wchar_t* toBeDeleted = w_;
        w_ = w.w_;
        w.w_ = 0;
        delete[] toBeDeleted;
    }

    return *this;
}


String::W::~W()
{
    delete[] w_;
}


//!
//! Concatenate a and b. Return result.
//!
String operator +(const String& a, char b)
{
    String s(a);
    s += b;
    return s;
}


//!
//! Concatenate a and b. Return result.
//!
String operator +(const String& a, const String& b)
{
    String s(a);
    s += b;
    return s;
}


//!
//! Concatenate a and b. Return result.
//!
String operator +(const String& a, const char* b)
{
    String s(a);
    s += b;
    return s;
}


//!
//! Concatenate a and b. Return result.
//!
String operator +(const String& a, const wchar_t* b)
{
    String s(a);
    s += b;
    return s;
}


//!
//! Concatenate a and b. Return result.
//!
String operator +(const String& a, wchar_t b)
{
    String s(a);
    s += b;
    return s;
}


//!
//! Concatenate a and b. Return result.
//!
String operator +(char a, const String& b)
{
    String s(1, a);
    s += b;
    return s;
}


//!
//! Concatenate a and b. Return result.
//!
String operator +(const char* a, const String& b)
{
    String s(a);
    s += b;
    return s;
}


//!
//! Concatenate a and b. Return result.
//!
String operator +(const wchar_t* a, const String& b)
{
    String s(a);
    s += b;
    return s;
}


//!
//! Concatenate a and b. Return result.
//!
String operator +(wchar_t a, const String& b)
{
    String s(1, a);
    s += b;
    return s;
}

END_NAMESPACE1
