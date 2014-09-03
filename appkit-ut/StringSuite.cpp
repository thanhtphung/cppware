#include "appkit/String.hpp"
#include "appkit/StringPair.hpp"
#include "appkit/StringVec.hpp"
#include "appkit/U16.hpp"
#include "syskit/Bom.hpp"
#include "syskit/Thread.hpp"
#include "syskit/Utf8.hpp"

#include "appkit-ut-pch.h"
#include "StringSuite.hpp"

using namespace appkit;
using namespace syskit;

const size_t BYTE_SIZE = 41;
const size_t NUM_CHARS = 16;

const unsigned int SAMPLE[NUM_CHARS] =
{
    0x000000abU, //2-byte value
    0x0000000aU, //1-byte value
    0x0000007fU, //1-byte value
    0x00000080U, //2-byte value
    0x000000abU, //2-byte value
    0x000007ffU, //2-byte value
    0x00000800U, //3-byte value
    0x0000abcdU, //3-byte value
    0x0000ffffU, //3-byte value
    0x00010000U, //4-byte value
    0x000abcdeU, //4-byte value
    0x0010ffffU, //4-byte value
    0x000abcdeU, //4-byte value
    0x0000abcdU, //3-byte value
    0x0000000aU, //1-byte value
    0x0000000bU  //1-byte value
};

BEGIN_NAMESPACE


class Sample0: public String
{
public:
    Sample0();
    ~Sample0();
private:
    Sample0(const Sample0&); //prohibit usage
    const Sample0& operator =(const Sample0&); //prohibit usage
};

Sample0::Sample0():
String()
{

    char buf[127 + 1];
    char* s = buf;
    for (char i = 127; i >= 0; *s++ = i--);
    String::operator =(buf);
}

Sample0::~Sample0()
{
}


class Sample1: public String
{
public:
    Sample1();
    ~Sample1();
private:
    Sample1(const Sample1&); //prohibit usage
    const Sample1& operator =(const Sample1&); //prohibit usage
};

Sample1::Sample1():
String()
{

    Utf8Seq seq;
    seq.shrink(SAMPLE, NUM_CHARS);
    String::operator =(seq);
}

Sample1::~Sample1()
{
}

END_NAMESPACE


StringSuite::StringSuite()
{
}


StringSuite::~StringSuite()
{
}


void StringSuite::testAppend00()
{
    String str0;
    str0 += 'x';
    bool ok = (str0 == "x");
    CPPUNIT_ASSERT(ok);
    char nullChar = 0;
    str0 += nullChar;
    ok = ((str0 == "x") && (str0.length() == 2) && (str0.byteSize() == 3));
    CPPUNIT_ASSERT(ok);

    str0.truncate(1);
    str0 += "yz";
    ok = (str0 == "xyz");
    CPPUNIT_ASSERT(ok);
    str0 += "";
    ok = (str0 == "xyz");
    CPPUNIT_ASSERT(ok);

    str0.append("123456", 0);
    ok = (str0 == "xyz");
    CPPUNIT_ASSERT(ok);
    str0.append("123456", 3);
    ok = (str0 == "xyz123");
    CPPUNIT_ASSERT(ok);

    String str1;
    str1.append("xyz123", 7);
    ok = (str1 == str0);
    CPPUNIT_ASSERT(ok);

    str1.append(0, '4');
    ok = (str1 == str0);
    CPPUNIT_ASSERT(ok);

    str1.append(4, '4');
    ok = (str1 == "xyz1234444");
    CPPUNIT_ASSERT(ok);
}


//
// String::append(const utf8_t*, size_t, size_t);
//
void StringSuite::testAppend01()
{
    Sample1 sample1;

    const Utf8Seq& seq = sample1.asUtf8Seq();
    String str;
    str.append(seq.raw(), seq.byteSize(), seq.numChars());
    bool ok = (str == sample1);
    CPPUNIT_ASSERT(ok);
}


//
// String::append(const wchar_t*, size_t);
//
void StringSuite::testAppend02()
{
    const wchar_t* s = L"abc123\u7777";
    size_t numWchars = 0;
    String str;
    str.append(s, numWchars);
    bool ok = str.empty();
    CPPUNIT_ASSERT(ok);

    numWchars = 7;
    str.append(s, numWchars);
    ok = (str == L"abc123\u7777");
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - void String::attachRaw(utf8_t*, size_t, size_t);
//
void StringSuite::testAttachRaw00()
{
    String str0;
    utf8_t* s = new utf8_t[3 + 1];
    memcpy(s, "xyz", 3 + 1);
    str0.attachRaw(s, 3 + 1, 3 + 1);
    bool ok = (str0 == "xyz");
    CPPUNIT_ASSERT(ok);

    String str1(str0);
    s = new utf8_t[6 + 1];
    memcpy(s, "abc123", 6 + 1);
    str0.attachRaw(s, 6 + 1, 6 + 1);
    ok = ((str0 == "abc123") && (str1 == "xyz"));
    CPPUNIT_ASSERT(ok);
}


void StringSuite::testCompare00()
{
    String str0("abc123");
    String str1(str0);
    String str2("ABC123");
    bool ok = ((String::compareP(&str0, &str1) == 0) && (String::comparePR(&str0, &str1) == 0));
    CPPUNIT_ASSERT(ok);
    ok = ((String::comparePI(&str0, &str2) == 0) && (String::comparePIR(&str0, &str2) == 0));
    CPPUNIT_ASSERT(ok);

    str1 = "abc0";
    ok = ((String::compareP(&str0, &str1) > 0) && (String::comparePR(&str0, &str1) < 0));
    CPPUNIT_ASSERT(ok);
    ok = ((String::compareP(&str1, &str0) < 0) && (String::comparePR(&str1, &str0) > 0));
    CPPUNIT_ASSERT(ok);

    str2 = "ABC0";
    ok = ((String::comparePI(&str0, &str2) > 0) && (String::comparePIR(&str0, &str2) < 0));
    CPPUNIT_ASSERT(ok);
    ok = ((String::comparePI(&str2, &str0) < 0) && (String::comparePIR(&str2, &str0) > 0));
    CPPUNIT_ASSERT(ok);
}


void StringSuite::testCopy00()
{
    String s0a(1023, 'a');
    String s1a[3];
    bool ok = true;
    for (int i = 999; i > 0; --i)
    {
        s1a[0] += s0a;
        s1a[1] = s0a + s0a;
        s1a[2] = s0a + 'b';
        s1a[2] = "bb" + s0a;
        //s1a[3] = s0a;
        //ok = (s1a[3] == s0a.ascii()) && (s1a[3] == s0a);
    }

    CPPUNIT_ASSERT(ok);
}


void StringSuite::testCopy01()
{
    std::string s0b(1023, 'a');
    std::string s1b[3];
    bool ok = true;
    for (int i = 999; i > 0; --i)
    {
        s1b[0] += s0b;
        s1b[1] = s0b + s0b;
        s1b[2] = s0b + 'b';
        s1b[2] = "bb" + s0b;
        //s1b[3] = s0b;
        //ok = (s1b[3] == s0b.c_str()) && (s1b[3] == s0b);
    }

    CPPUNIT_ASSERT(ok);
}


#if 0
#include <atlstr.h>

void
StringSuite::testCopy02()
{
    CStringA s0c('a', 1023);
    CStringA s1c[3];
    bool ok = true;
    for (int i = 999; i > 0; --i)
    {
        s1c[0] += s0c;
        s1c[1] = s0c + s0c;
        s1c[2] = s0c + 'b';
        s1c[2] = "bb" + s0c;
        //s1c[3] = s0c;
        //ok = (s1c[3] == (const char*)s0c) && (s1c[3] == s0c);
    }

    CPPUNIT_ASSERT(ok);
}
#endif


void StringSuite::testCow00()
{
    Sample0 sample0;
    String str0(sample0);
    unsigned int byteSize = 123;
    bool ok = ((str0.raw(byteSize) == sample0.raw()) && (byteSize == sample0.byteSize()));
    CPPUNIT_ASSERT(ok);

    str0 = sample0.asUtf8Seq();
    ok = ((str0 == sample0) && (str0.raw() != sample0.raw()));
    CPPUNIT_ASSERT(ok);

    String str1(str0);
    str1 += str0;
    ok = (str1.truncate(str0.length()) && (str1 == str0));
    CPPUNIT_ASSERT(ok);
    ok = (!str1.truncate(str0.length()) && (str1 == str0));
    CPPUNIT_ASSERT(ok);
}


void StringSuite::testCow01()
{
    String s0a("String has guts.");
    String s1a(s0a);
    bool ok = (s0a.raw() == s1a.raw());
    CPPUNIT_ASSERT(ok);

    std::string s0b("std::string doesn\'t.");
    std::string s1b(s0b);
    ok = (s0b.c_str() != s1b.c_str());
    CPPUNIT_ASSERT(ok);

    std::wstring s0c(L"std::wstring doesn\'t.");
    std::wstring s1c(s0c);
    ok = (s0c.c_str() != s1c.c_str());
    CPPUNIT_ASSERT(ok);

#if 0
    CStringA s0d("CStringA has guts.");
    CStringA s1d(s0d);
    ok = ((const char*)s0d == (const char*)s1d);
    CPPUNIT_ASSERT(ok);

    CStringW s0e("CStringW has guts.");
    CStringW s1e(s0e);
    ok = ((const wchar_t*)s0e == (const wchar_t*)s1e);
    CPPUNIT_ASSERT(ok);
#endif
}


//
// Default constructor.
//
void StringSuite::testCtor00()
{
    String str;
    bool ok = ((str.length() == 0) && str.isAscii() && str.empty());
    CPPUNIT_ASSERT(ok);
}


//
// Construct from ASCII strings.
//
void StringSuite::testCtor01()
{
    String str("abc123", 3);
    bool ok = ((str == "abc") && str.isAscii() && (strcmp(str.ascii(), "abc") == 0));
    CPPUNIT_ASSERT(ok);

    str.reset("abc123", 6);
    ok = ((str == "abc123") && str.isAscii() && (strcmp(str.ascii(), "abc123") == 0));
    CPPUNIT_ASSERT(ok);

    Sample0 sample0;
    ok = ((sample0.length() == 127) && (sample0.byteSize() == 127 + 1) && sample0.isAscii());
    CPPUNIT_ASSERT(ok);
}


//
// Wide-character strings.
//
void StringSuite::testCtor02()
{
    Sample1 sample1;
    String str0(sample1.widen());
    String str1(sample1.widen(), sample1.length());
    bool ok = (str0 == sample1) && (str1 == sample1);
    CPPUNIT_ASSERT(ok);

    str0 = L"abc123";
    ok = ((str0 == "abc123") && str0.isAscii() && (strcmp(str0.ascii(), "abc123") == 0));
    CPPUNIT_ASSERT(ok);

    str0 += L"xyz";
    ok = ((str0 == "abc123xyz") && str0.isAscii() && (strcmp(str0.ascii(), "abc123xyz") == 0));
    CPPUNIT_ASSERT(ok);

    str0 = sample1.widen();
    ok = (str0 == sample1);
    CPPUNIT_ASSERT(ok);
}


//
// Construct from UTF8 sequence.
//
void StringSuite::testCtor03()
{
    Sample0 sample0;
    Sample1 sample1;

    const Utf8Seq& seq0 = sample0.asUtf8Seq();
    String str0(seq0);
    bool ok = (str0 == sample0);
    CPPUNIT_ASSERT(ok);

    const Utf8Seq& seq1 = sample1.asUtf8Seq();
    String str1(seq1);
    ok = (str1 == sample1);
    CPPUNIT_ASSERT(ok);
    str1 = seq0;
    ok = (str1 == sample0);
    CPPUNIT_ASSERT(ok);

    Utf8Seq seq2;
    str1 += seq2;
    ok = (str1 == sample0);
    CPPUNIT_ASSERT(ok);

    str1.reset();
    str1 += seq1;
    ok = (str1 == sample1);
    CPPUNIT_ASSERT(ok);

    ok = ((sample1.length() == NUM_CHARS) && (sample1.byteSize() == BYTE_SIZE) && (!sample1.isAscii()));
    CPPUNIT_ASSERT(ok);
}


//
// Construct from UTF16 sequence.
//
void StringSuite::testCtor04()
{
    Sample0 sample0;
    Sample1 sample1;

    Utf16Seq seq0(sample0.formUtf16Seq());
    String str(seq0);
    bool ok = (str == sample0);
    CPPUNIT_ASSERT(ok);

    seq0 = sample1.formUtf16Seq();
    str = seq0;
    ok = (str == sample1);
    CPPUNIT_ASSERT(ok);

    Utf16Seq seq1;
    str += seq1;
    ok = (str == sample1);
    CPPUNIT_ASSERT(ok);

    str.reset();
    str += seq0;
    ok = (str == sample1);
    CPPUNIT_ASSERT(ok);

    utf16_t* u61 = new utf16_t[seq0.numU16s()];
    U16::bswap(seq0.raw(), seq0.numU16s(), u61);
    str.resetX(Bom(Bom::Utf61), u61, seq0.byteSize());
    ok = (str == sample0);
    delete[] u61;
}


//
// Construct from substrings.
//
void StringSuite::testCtor05()
{
    Sample0 sample0;
    Sample1 sample1;

    String str(sample0, 0, sample0.length() + 1);
    bool ok = (str == sample0);
    CPPUNIT_ASSERT(ok);

    str.reset(sample0, 0, sample0.length());
    ok = (str == sample0);
    CPPUNIT_ASSERT(ok);

    str.reset();
    str.append(sample1, 0, sample1.length());
    ok = (str == sample1);
    CPPUNIT_ASSERT(ok);
    str.append(sample1, 5, 0);
    ok = (str == sample1);
    CPPUNIT_ASSERT(ok);
}


void StringSuite::testCtor06()
{
    Utf16Seq seq0;
    String str0(seq0);
    bool ok = str0.empty();
    CPPUNIT_ASSERT(ok);

    Utf8Seq seq1;
    String str1(seq1);
    ok = str1.empty();
    CPPUNIT_ASSERT(ok);

    str1 = "123";
    String str2(str1, 1, 0);
    ok = str2.empty();
    CPPUNIT_ASSERT(ok);

    String str3("");
    ok = str3.empty();
    CPPUNIT_ASSERT(ok);

    String str4("abc", 0);
    ok = str4.empty();
    CPPUNIT_ASSERT(ok);
}


//
// String(size_t count, char);
// String(size_t count, wchar_t);
//
void StringSuite::testCtor07()
{
    size_t count = 0;
    char c = 'c';
    String str0(count, c);
    bool ok = str0.empty();
    CPPUNIT_ASSERT(ok);

    wchar_t w[3] = {0x77, 0x777, 0x7777};
    String str1(count, w[0]);
    ok = str0.empty();
    CPPUNIT_ASSERT(ok);

    count = 5;
    String str2(count, c);
    ok = (str2 == "ccccc");
    CPPUNIT_ASSERT(ok);

    for (size_t i = 0; i < 3; ++i)
    {
        String str3(count, w[i]);
        for (size_t j = 0; j < count; ++j)
        {
            if ((str3.length() != count) || (str3[j] != static_cast<utf32_t>(w[i])))
            {
                ok = false;
                i = 2; //terminate outer loop
                break;
            }
        }
    }
    CPPUNIT_ASSERT(ok);
}


//
// String(const wchar_t*)
// String(const wchar_t*, size_t)
//
void StringSuite::testCtor08()
{
    const wchar_t* s = L"";
    String str0(s);
    bool ok = str0.empty();
    CPPUNIT_ASSERT(ok);

    s = L"abc";
    String str1(s);
    ok = (str1 == "abc");
    CPPUNIT_ASSERT(ok);

    size_t numWchars = 0;
    String str2(s, numWchars);
    ok = str2.empty();
    CPPUNIT_ASSERT(ok);

    numWchars = 3;
    String str3(s, numWchars);
    ok = (str3 == "abc");
    CPPUNIT_ASSERT(ok);
}


//
// String::find(utf32_t, size_t);
//
void StringSuite::testFind00()
{
    String str("abcdefghijabcdefghij");
    bool ok = (str.find('x') == String::INVALID_INDEX);
    CPPUNIT_ASSERT(ok);
    ok = (str.find('a') == 0);
    CPPUNIT_ASSERT(ok);
    ok = (str.find('f') == 5);
    CPPUNIT_ASSERT(ok);
    ok = (str.find('f', 5) == 5);
    CPPUNIT_ASSERT(ok);
    ok = (str.find('f', 6) == 15);
    CPPUNIT_ASSERT(ok);
    ok = (str.find('f', 16) == String::INVALID_INDEX);
    CPPUNIT_ASSERT(ok);
    ok = (str.find(0xabcdeU) == String::INVALID_INDEX);
    CPPUNIT_ASSERT(ok);
    ok = (str.find(0xabcdeU, 999) == String::INVALID_INDEX);
    CPPUNIT_ASSERT(ok);
}


void StringSuite::testFind01()
{
    Sample1 sample1;

    utf32_t invalidChar = 0xffffffffU;
    bool ok = (sample1.find(invalidChar) == String::INVALID_INDEX);
    CPPUNIT_ASSERT(ok);
    ok = (sample1.find('a', 999) == String::INVALID_INDEX);
    CPPUNIT_ASSERT(ok);
    ok = (sample1.find(0x0aU) == 1);
    CPPUNIT_ASSERT(ok);
    ok = (sample1.find(0x10000U, 10) == String::INVALID_INDEX);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - bool String::contains(const String&, bool) const;
// - size_t String::find(const String&, size_t) const;
// - size_t String::rfind(const String&, size_t) const;
//
void StringSuite::testFind02()
{
    Sample0 sample0;
    Sample1 sample1;

    bool ok = true;
    for (size_t i = 0, n0 = sample0.length(), n1 = sample1.length(); i < 4; ++i, n0 -= 2, --n1)
    {
        String key0(sample0, i, n0);
        String key1(sample1, i, n1);
        if ((sample0.find(key0) != i) || (sample0.rfind(key0) != i) ||
            (sample0.find(key0, i) != i) || (sample0.rfind(key0, n0) != i) ||
            (sample0.find(key0, i + 999) != String::INVALID_INDEX) ||
            (key0.contains(sample0, false /*searchBackward*/) != (i == 0)) ||
            (key0.contains(sample0, true /*searchBackward*/) != (i == 0)) ||
            (sample1.find(key1) != i) || (sample1.rfind(key1) != i) ||
            (sample1.find(key1, i) != i) || (sample1.rfind(key1, n1) != i) ||
            (key1.contains(sample1, false /*searchBackward*/) != (i == 0)) ||
            (key1.contains(sample1, true /*searchBackward*/) != (i == 0)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = (!sample0.contains(sample1)) && (!sample1.contains(sample0));
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - String::Ascii8 String::formAscii8(char, size_t);
//
void StringSuite::testFormAscii8a()
{
    Sample0 sample0;
    String::Ascii8 a0(sample0.formAscii8());
    bool ok = (strcmp(a0, sample0.ascii()) == 0);
    CPPUNIT_ASSERT(ok);

    Sample1 sample1;
    char defaultChar = 'x';
    unsigned int invalidCharCount = 0;
    String::Ascii8 a1(sample1.formAscii8(defaultChar, &invalidCharCount));
    ok = (strcmp(a1, "\xab\x0a\x7f\x80\xabxxxxxxxxx\x0a\x0b") == 0) && (invalidCharCount == 9);
    CPPUNIT_ASSERT(ok);

    String sample2;
    a1 = sample2.formAscii8();
    ok = (strcmp(a1, "") == 0);
    CPPUNIT_ASSERT(ok);
}


void StringSuite::testFormBox00()
{
    String s("123");
    bool ok = (s.formBox(0, 0, 99) == s);
    CPPUNIT_ASSERT(ok);
    ok = (s.formBox(0, 0, 3) == s);
    CPPUNIT_ASSERT(ok);

    ok = (s.formBox(2, 0, 99) == "  123");
    CPPUNIT_ASSERT(ok);
    ok = (s.formBox(2, 0, 5) == "  123");
    CPPUNIT_ASSERT(ok);

    s = "abc123xy";
    ok = (s.formBox(0, 0, 3) == "abc\n123\nxy");
    CPPUNIT_ASSERT(ok);
    s = "abc123xyz";
    ok = (s.formBox(0, 0, 3) == "abc\n123\nxyz");
    CPPUNIT_ASSERT(ok);
    ok = (s.formBox(1, 1, 4) == " abc1\n 23xy\n z");
    CPPUNIT_ASSERT(ok);
    s = "abc123xyz123";
    ok = (s.formBox(1, 1, 4) == " abc1\n 23xy\n z123");
    CPPUNIT_ASSERT(ok);
}


//
// String::formUtfX(size_t, const Bom&, bool);
// UTF8.
//
void StringSuite::testFormUtfX00()
{
    Sample1 sample1;

    Bom bom(Bom::Utf8);
    bool addBom = false;
    unsigned int byteSize = 123;
    unsigned char* p = sample1.formUtfX(byteSize, bom, addBom);
    String str;
    size_t numInvalidChars = str.reset8(p, byteSize);
    bool ok = ((numInvalidChars == 0) && (str == sample1));
    delete[] p;
    CPPUNIT_ASSERT(ok);

    addBom = true;
    p = sample1.formUtfX(byteSize, bom, addBom);
    numInvalidChars = str.resetX(p, byteSize);
    ok = ((numInvalidChars == 0) && (str == sample1));
    delete[] p;
    CPPUNIT_ASSERT(ok);
}


//
// String::formUtfX(size_t, const Bom&, bool);
// UTF16.
//
void StringSuite::testFormUtfX01()
{
    Sample1 sample1;

    Bom bom(Bom::Utf16);
    bool addBom = false;
    unsigned int byteSize = 123;
    unsigned char* p = sample1.formUtfX(byteSize, bom, addBom);
    String str;
    size_t numInvalidChars = str.reset16(reinterpret_cast<const utf16_t*>(p), byteSize >> 1);
    bool ok = ((numInvalidChars == 0) && (str == sample1));
    delete[] p;
    CPPUNIT_ASSERT(ok);

    addBom = true;
    p = sample1.formUtfX(byteSize, bom, addBom);
    numInvalidChars = str.resetX(p, byteSize);
    ok = ((numInvalidChars == 0) && (str == sample1));
    delete[] p;
    CPPUNIT_ASSERT(ok);
}


//
// String::formUtfX(size_t, const Bom&, bool);
// UTF61.
//
void StringSuite::testFormUtfX02()
{
    Sample1 sample1;

    Bom bom(Bom::Utf61);
    bool addBom = false;
    unsigned int byteSize = 123;
    unsigned char* p = sample1.formUtfX(byteSize, bom, addBom);
    String str;
    size_t numInvalidChars = str.resetX(bom, reinterpret_cast<const utf16_t*>(p), byteSize);
    bool ok = ((numInvalidChars == 0) && (str == sample1));
    delete[] p;
    CPPUNIT_ASSERT(ok);

    addBom = true;
    p = sample1.formUtfX(byteSize, bom, addBom);
    numInvalidChars = str.resetX(p, byteSize);
    ok = ((numInvalidChars == 0) && (str == sample1));
    delete[] p;
    CPPUNIT_ASSERT(ok);
}


//
// String::formUtfX(size_t, const Bom&, bool);
// UTF32.
//
void StringSuite::testFormUtfX03()
{
    Sample1 sample1;

    Bom bom(Bom::Utf32);
    bool addBom = false;
    unsigned int byteSize = 123;
    unsigned char* p = sample1.formUtfX(byteSize, bom, addBom);
    String str;
    size_t numInvalidChars = str.resetX(bom, reinterpret_cast<const utf32_t*>(p), byteSize);
    bool ok = ((numInvalidChars == 0) && (str == sample1));
    delete[] p;
    CPPUNIT_ASSERT(ok);

    addBom = true;
    p = sample1.formUtfX(byteSize, bom, addBom);
    numInvalidChars = str.resetX(p, byteSize);
    ok = ((numInvalidChars == 0) && (str == sample1));
    delete[] p;
    CPPUNIT_ASSERT(ok);
}


//
// String::formUtfX(size_t, const Bom&, bool);
// UTF23.
//
void StringSuite::testFormUtfX04()
{
    Sample1 sample1;

    Bom bom(Bom::Utf23);
    bool addBom = false;
    unsigned int byteSize = 123;
    unsigned char* p = sample1.formUtfX(byteSize, bom, addBom);
    String str;
    size_t numInvalidChars = str.resetX(bom, reinterpret_cast<const utf32_t*>(p), byteSize);
    bool ok = ((numInvalidChars == 0) && (str == sample1));
    delete[] p;
    CPPUNIT_ASSERT(ok);

    addBom = true;
    p = sample1.formUtfX(byteSize, bom, addBom);
    numInvalidChars = str.resetX(p, byteSize);
    ok = ((numInvalidChars == 0) && (str == sample1));
    delete[] p;
    CPPUNIT_ASSERT(ok);
}


void StringSuite::testHash00()
{
    String str("aRandomStringUsedForHash!!!");
    bool ok = (str.hash() == 93520317);
    CPPUNIT_ASSERT(ok);

    size_t numBuckets = 131;
    unsigned int bucket = String::hashP(&str, numBuckets);
    ok = (bucket == 72);
    CPPUNIT_ASSERT(ok);
}


void StringSuite::testNew00()
{
    String* s = new String;
    bool ok = (s != 0);
    CPPUNIT_ASSERT(ok);

    StringPair* nv = new StringPair(*s);
    ok = (nv != 0);
    CPPUNIT_ASSERT(ok);
    delete nv;
    unsigned char buf0[sizeof(*nv)];
    nv = new(buf0)StringPair(*s);
    ok = (reinterpret_cast<unsigned char*>(nv) == buf0);
    CPPUNIT_ASSERT(ok);
    nv->StringPair::~StringPair();
    delete s;

    unsigned char buf1[sizeof(*s)];
    s = new(buf1)String;
    ok = (reinterpret_cast<unsigned char*>(s) == buf1);
    CPPUNIT_ASSERT(ok);
    s->String::~String();
}


void StringSuite::testOp00()
{
    String str0("123");
    str0 = str0;
    bool ok = (str0 == "123");
    CPPUNIT_ASSERT(ok);

    String str1;
    str1 = "abc";
    str1 += str0;
    ok = (str1 == "abc123");
    CPPUNIT_ASSERT(ok);

    str1 = str0;
    ok = (str1 == str0);
    CPPUNIT_ASSERT(ok);

    Sample0 sample0;
    Sample1 sample1;
    ok = ((!(sample1 == "abc123")) && (sample0 != sample1) && (!(sample1 == sample0)));
    CPPUNIT_ASSERT(ok);
}


#if 0
void StringSuite::testPerf00()
{
    String s0a(1023, 'a');
    String s1a[4];
    std::string s0b(1023, 'a');
    std::string s1b[4];
    CStringA s0c('a', 1023);
    CStringA s1c[4];
    CStringW s0d(L'a', 1023);
    CStringW s1d[4];
    for (int i = 9999; i > 0; --i)
    {
        s1a[0] += s0a;
        s1a[1] = s0a + s0a;
        s1a[2] = s0a + 'b';
        s1a[2] += 'b';
        s1a[2] = "bb" + s0a;
        s1a[3] = s0a;

        s1b[0] += s0b;
        s1b[1] = s0b + s0b;
        s1b[2] = s0b + 'b';
        s1b[2] += 'b';
        s1b[2] = "bb" + s0b;
        s1b[3] = s0b;

        s1c[0] += s0c;
        s1c[1] = s0c + s0c;
        s1c[2] = s0c + 'b';
        s1c[2] += 'b';
        s1c[2] = "bb" + s0c;
        s1c[3] = s0c;

        s1d[0] += s0d;
        s1d[1] = s0d + s0d;
        s1d[2] = s0d + L'b';
        s1d[2] += L'b';
        s1d[2] = L"bb" + s0d;
        s1d[3] = s0d;
    }

    bool ok = true;
    CPPUNIT_ASSERT(ok);
}


void StringSuite::testPerf01()
{
    String s0a[4];
    s0a[1] = String(31, 'a');
    s0a[2] = String(1023, 'a');
    s0a[3] = String(65535, 'a');
    String s1a[5];

    std::string s0b[4];
    s0b[1] = std::string(31, 'a');
    s0b[2] = std::string(1023, 'a');
    s0b[3] = std::string(65535, 'a');
    std::string s1b[5];

    std::wstring s0c[4];
    s0c[1] = std::wstring(31, 'a');
    s0c[2] = std::wstring(1023, 'a');
    s0c[3] = std::wstring(65535, 'a');
    std::wstring s1c[5];

    CStringA s0d[4];
    s0d[1] = CStringA('a', 31);
    s0d[2] = CStringA('a', 1023);
    s0d[3] = CStringA('a', 65535);
    CStringA s1d[5];

    CStringW s0e[4];
    s0e[1] = CStringW(L'a', 31);
    s0e[2] = CStringW(L'a', 1023);
    s0e[3] = CStringW(L'a', 65535);
    CStringW s1e[5];

    for (int i = 9999; i > 0; --i)
    {
        s1a[0] = s0a[0];
        s1a[1] = s0a[1];
        s1a[2] = s0a[2];
        s1a[2].reset();
        s1a[2] = s0a[2];
        s1a[3] = s0a[3];
        s1a[3].reset();
        s1a[3] = s0a[3];
        s1a[4] += s0a[1];

        s1b[0] = s0b[0];
        s1b[1] = s0b[1];
        s1b[2] = s0b[2];
        s1b[2].clear();
        s1b[2] = s0b[2];
        s1b[3] = s0b[3];
        s1b[3].clear();
        s1b[3] = s0b[3];
        s1b[4] += s0b[1];

        s1c[0] = s0c[0];
        s1c[1] = s0c[1];
        s1c[2] = s0c[2];
        s1c[2].clear();
        s1c[2] = s0c[2];
        s1c[3] = s0c[3];
        s1c[3].clear();
        s1c[3] = s0c[3];
        s1c[4] += s0c[1];

        s1d[0] = s0d[0];
        s1d[1] = s0d[1];
        s1d[2] = s0d[2];
        s1d[2].Empty();
        s1d[2] = s0d[2];
        s1d[3] = s0d[3];
        s1d[3].Empty();
        s1d[3] = s0d[3];
        s1d[4] += s0d[1];

        s1e[0] = s0e[0];
        s1e[1] = s0e[1];
        s1e[2] = s0e[2];
        s1e[2].Empty();
        s1e[2] = s0e[2];
        s1e[3] = s0e[3];
        s1e[3].Empty();
        s1e[3] = s0e[3];
        s1e[4] += s0e[1];
    }

    bool ok = true;
    CPPUNIT_ASSERT(ok);
}


void StringSuite::testPerf02()
{
    void* arg = this;
    unsigned int stackSizeInBytes = 0;
    bool startSuspended = true;
    Thread t0(entry00, arg, stackSizeInBytes, startSuspended);
    Thread t1(entry00, arg, stackSizeInBytes, startSuspended);
    Thread t2(entry00, arg, stackSizeInBytes, startSuspended);
    t0.resume();
    t1.resume();
    t2.resume();
    t0.waitTilDone();
    t1.waitTilDone();
    t2.waitTilDone();
}
#endif


void StringSuite::testOp01()
{
    Sample0 sample0;
    Sample1 sample1;

    // ASCII.
    String str0;
    String str1;
    bool ok = ((!(str0 < str1)) && (str0 < sample0) && (!(sample0 < str0)));
    CPPUNIT_ASSERT(ok);

    // Non-ASCII.
    String str2("abc");
    ok = ((sample0 < sample1) && (!(sample1 < sample0)));
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - const String& String::operator +=(const Utf8&);
// - const String& String::operator +=(const wchar_t*);
// - const String& String::operator +=(wchar_t);
//
void StringSuite::testOp02()
{
    String str;
    unsigned char c = 0xabU;
    Utf8 c8(c);
    str += c8;
    bool ok = ((str.length() == 1) && (str[0] == c8));
    CPPUNIT_ASSERT(ok);

    str += L'\xba';
    ok = ((str.length() == 2) && (str[1] == L'\xba'));
    CPPUNIT_ASSERT(ok);

    Sample1 sample1;
    String s0(sample1, 0, 10);
    String s1(sample1, 10, 6);
    str = s0.widen();
    str += s1.widen();
    ok = (str == sample1);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - String operator +(const String&, char);
// - String operator +(const String&, const String&);
// - String operator +(const String&, const char*);
// - String operator +(const String&, const wchar_t*);
// - String operator +(const String&, wchar_t);
// - String operator +(char, const String&);
// - String operator +(const char*, const String&);
// - String operator +(const wchar_t*, const String&);
// - String operator +(wchar_t, const String&);
//
void StringSuite::testOp03()
{
    String a("a");
    String ab(a + 'b');
    String ba('b' + a);
    bool ok = ((ab == "ab") && (ba == "ba"));
    CPPUNIT_ASSERT(ok);

    a = "aa";
    String b("bb");
    ab = a + b;
    ok = (ab == "aabb");
    CPPUNIT_ASSERT(ok);

    a = "aaa";
    ab = a + "bbb";
    ba = "bbb" + a;
    ok = ((ab == "aaabbb") && (ba == "bbbaaa"));
    CPPUNIT_ASSERT(ok);

    a = L"a\uaaaaa";
    ab = a + L"b\ubbbbb";
    ba = L"b\ubbbbb" + a;
    ok = ((ab == L"a\uaaaaab\ubbbbb") && (ba == L"b\ubbbbba\uaaaaa"));
    CPPUNIT_ASSERT(ok);

    a = L"\uaaaa";
    ab = a + L'\ubbbb';
    ba = L'\ubbbb' + a;
    ok = ((ab == L"\uaaaa\ubbbb") && (ba == L"\ubbbb\uaaaa"));
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - void String::reset();
// - void String::reset(const char*, size_t);
// - void String::reset(const wchar_t*, size_t);
// - void String::reset(size_t, wchar_t);
//
void StringSuite::testReset00()
{
    Sample1 sample1;

    String str0;
    String str1(sample1);
    str1.reset();
    bool ok = (str1 == str0);
    CPPUNIT_ASSERT(ok);

    str1.reset("abc123", 1);
    ok = ((str1 == "a") && (str1[0] == 'a') && (str1[1] == 0));
    CPPUNIT_ASSERT(ok);

    str1.reset(3, L'\u9abc');
    ok = (str1 == L"\u9abc\u9abc\u9abc");
    CPPUNIT_ASSERT(ok);

    str1.reset(sample1.widen(), sample1.length());
    ok = (str1 == sample1);
    CPPUNIT_ASSERT(ok);
}


void StringSuite::testResize00()
{
    String s0("to-be-resized");
    String s1(s0);
    unsigned int oldCap = s1.capacity();
    unsigned int newCap = s1.capacity() * 2;
    bool ok = s1.resize(oldCap) && (s1.capacity() == oldCap) && (s1 == s0);
    CPPUNIT_ASSERT(ok);

    ok = s1.resize(newCap) && (s1.capacity() == newCap) && (s1 == s0);
    CPPUNIT_ASSERT(ok);
}


//
// String::rfind(utf32_t, size_t);
//
void StringSuite::testRfind00()
{
    String str("abcdefghijabcdefghij");
    bool ok = (str.rfind('x') == String::INVALID_INDEX);
    CPPUNIT_ASSERT(ok);
    ok = (str.rfind('a') == 10);
    CPPUNIT_ASSERT(ok);
    ok = (str.rfind('j') == 19);
    CPPUNIT_ASSERT(ok);
    ok = (str.rfind('a', 10) == 10);
    CPPUNIT_ASSERT(ok);
    ok = (str.rfind('a', 9) == 0);
    CPPUNIT_ASSERT(ok);
    ok = (str.rfind('j', 5) == String::INVALID_INDEX);
    CPPUNIT_ASSERT(ok);
    ok = (str.rfind(0xabcdeU) == String::INVALID_INDEX);
    CPPUNIT_ASSERT(ok);
    ok = (str.rfind(0xabcdeU, 999) == String::INVALID_INDEX);
    CPPUNIT_ASSERT(ok);
}


void StringSuite::testRfind01()
{
    Sample1 sample1;

    utf32_t invalidChar = 0xffffffffU;
    bool ok = (sample1.rfind(invalidChar) == String::INVALID_INDEX);
    CPPUNIT_ASSERT(ok);
    ok = (sample1.rfind('a', 999) == String::INVALID_INDEX);
    CPPUNIT_ASSERT(ok);
    ok = (sample1.rfind(0x0aU) == 14);
    CPPUNIT_ASSERT(ok);
    ok = (sample1.rfind(0x10000U, 8) == String::INVALID_INDEX);
    CPPUNIT_ASSERT(ok);
}


void StringSuite::testSize00()
{
    bool ok = (sizeof(String) == sizeof(void*)) && //Win32:4 x64:8
        (sizeof(String::S) == sizeof(void*) * 5 + 16) && //Win32:36 x64:56
        (sizeof(StringPair) == sizeof(void*) * 2); //Win32:8 x64:16
    CPPUNIT_ASSERT(ok);
}


void StringSuite::testSubstr()
{
    const String str0("abcdefghij");
    size_t startAt = 3;
    size_t charCount = 4;
    const String str1(str0.substr(startAt, charCount));
    bool ok = (str1 == "defg");
    CPPUNIT_ASSERT(ok);

    startAt = 5;
    ok = (str0.substr(startAt) == "fghij");
    CPPUNIT_ASSERT(ok);

    startAt = 0;
    ok = (str0.substr(startAt) == str0);
    CPPUNIT_ASSERT(ok);
    charCount = 0;
    ok = str0.substr(startAt, charCount).empty();
    CPPUNIT_ASSERT(ok);
    charCount = 1;
    ok = (str0.substr(startAt, charCount) == "a");
    CPPUNIT_ASSERT(ok);

    startAt = 20;
    charCount = 20;
    ok = str0.substr(startAt, charCount).empty();
    CPPUNIT_ASSERT(ok);
    startAt = 0;
    ok = (str0.substr(startAt, charCount) == str0);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - void String::trimSpace(bool, bool);
//
void StringSuite::testTrimSpace00()
{
    String str("");
    str.trimSpace(true /*trimL*/, true /*trimR*/);
    bool ok = str.empty();
    CPPUNIT_ASSERT(ok);

    Sample1 sample;
    sample.truncate(sample.length() - 2);
    str = " \t";
    str += sample;
    str += "  \n";
    str.trimSpace(true /*trimL*/, true /*trimR*/);
    ok = (str == sample);
    CPPUNIT_ASSERT(ok);

    str = "abc 123\n\n\n\n";
    str.trimSpace(false /*trimL*/, true /*trimR*/);
    ok = (str == "abc 123");
    CPPUNIT_ASSERT(ok);

    str = "   \t \r\n";
    str.trimSpace();
    ok = str.empty();
    CPPUNIT_ASSERT(ok);
}


void StringSuite::testVec00()
{
    String v[2];
    v[0] = "abc";
    v[1] = "123";

    StringVec vec;
    vec.add(v, 2);
    bool ok = (vec.numItems() == 2) && (vec[0] == v[0]) && (vec[1] == v[1]);
    CPPUNIT_ASSERT(ok);

    vec.add(2, v[1]);
    vec.add(3, v[0]);
    const char* delim = 0;
    size_t maxItems = 0xffffffffU;
    String s(vec.stringify(delim, maxItems));
    ok = (s == "abc123123123abcabcabc");
    CPPUNIT_ASSERT(ok);

    vec.add(vec);
    vec.setItem(1, "x");
    s = vec.stringify(delim, maxItems);
    ok = (s == "abcx123123abcabcabcabc123123123abcabcabc");
    CPPUNIT_ASSERT(ok);

    delim = ", ";
    maxItems = 3;
    s = vec.stringify(delim, maxItems);
    ok = (s == "abc, x, 123...");
    CPPUNIT_ASSERT(ok);

    StringVec vec1(vec);
    vec1.reset(vec, 0, 3);
    s = vec1.stringify(delim, maxItems);
    ok = (s == "abc, x, 123");
    CPPUNIT_ASSERT(ok);

    bool reverseOrder = false;
    vec1.sort(vec, reverseOrder);
    s = vec.stringify(delim, maxItems);
    ok = (s == "123, abc, x");
    CPPUNIT_ASSERT(ok);

    StringVec vec2(StringVec::DefaultCap, -1, true /*ignoreCase*/);
    vec2 = vec;
    size_t i0 = 123;
    size_t i1 = 123;
    ok = vec1.find("abc") && vec2.find("abc", i0) && (i0 == 1) && vec2.find("ABC", i1) && (i1 == 1);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - String::W String::widen() const;
//
void StringSuite::testWiden00()
{
    String str0("abc");
    String::W w0 = str0.widen();
    bool ok = (memcmp(w0, L"abc", sizeof(L"abc")) == 0);
    CPPUNIT_ASSERT(ok);

    const wchar_t* p = w0;
    String::W w1(w0);
    ok = ((w0 == 0) && (w1 == p));
    CPPUNIT_ASSERT(ok);

    w0 = str0.widen();
    p = w0;
    w1 = w0;
    ok = ((w0 == 0) && (w1 == p));
    CPPUNIT_ASSERT(ok);
}


void StringSuite::testWith00()
{
    String str0("abc");
    String str1("a");
    String str2("abcd");
    String str3("A");

    bool ok = str0.startsWith(str1);
    CPPUNIT_ASSERT(ok);
    ok = str0.startsWith(str1, false);
    CPPUNIT_ASSERT(ok);
    ok = str0.startsWith(str3, true);
    CPPUNIT_ASSERT(ok);
    ok = !str0.startsWith(str3, false);
    CPPUNIT_ASSERT(ok);
    ok = !str0.startsWith(str2);
    CPPUNIT_ASSERT(ok);

    ok = str0.startsWith("a");
    CPPUNIT_ASSERT(ok);
    ok = str0.startsWith("a", false);
    CPPUNIT_ASSERT(ok);
    ok = str0.startsWith("A", true);
    CPPUNIT_ASSERT(ok);
    ok = !str0.startsWith("A", false);
    CPPUNIT_ASSERT(ok);
    ok = !str0.startsWith("abcd");
    CPPUNIT_ASSERT(ok);

    str1.reset("c", 1);
    str3.reset("C", 1);

    ok = str0.endsWith(str1);
    CPPUNIT_ASSERT(ok);
    ok = str0.endsWith(str1, false);
    CPPUNIT_ASSERT(ok);
    ok = str0.endsWith(str3, true);
    CPPUNIT_ASSERT(ok);
    ok = !str0.endsWith(str3, false);
    CPPUNIT_ASSERT(ok);
    ok = !str0.endsWith(str2);
    CPPUNIT_ASSERT(ok);

    ok = str0.endsWith("c");
    CPPUNIT_ASSERT(ok);
    ok = str0.endsWith("c", false);
    CPPUNIT_ASSERT(ok);
    ok = str0.endsWith("C", true);
    CPPUNIT_ASSERT(ok);
    ok = !str0.endsWith("C", false);
    CPPUNIT_ASSERT(ok);
    ok = !str0.endsWith("abcd");
    CPPUNIT_ASSERT(ok);
}


#if 0
void* StringSuite::entry00(void* arg)
{
    StringSuite* p = static_cast<StringSuite*>(arg);
    p->testPerf01();
    return 0;
}
#endif
