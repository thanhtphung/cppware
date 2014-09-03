#include "appkit/DelimitedTxt.hpp"
#include "appkit/StringVec.hpp"

#include "appkit-ut-pch.h"
#include "DelimitedTxtSuite.hpp"

using namespace appkit;

const char NEW_LINE = '\n';

// All lines have a line delim.
const char SAMPLE0[] = "1" "\n" "22" "\n" "333" "\n" "4444" "\n" "55555" "\n";

// First line has only a line delim.
// Last line does not have a line delim.
const char SAMPLE1[] = "\n" "1" "\n" "22" "\n" "333" "\n" "4444" "\n" "55555" "\n" "666666";

// No line delims.
const char SAMPLE2[] = "666666";

const char SEMICOLON = ';';


DelimitedTxtSuite::DelimitedTxtSuite()
{
}


DelimitedTxtSuite::~DelimitedTxtSuite()
{
}


bool DelimitedTxtSuite::cb0a(void* arg, const String& line)
{
    String& s = *static_cast<String*>(arg);
    s += line;
    return true;
}


bool DelimitedTxtSuite::cb0b(void* arg, const String& line)
{
    String& s = *static_cast<String*>(arg);
    String tmp(line);
    tmp += s;
    s = tmp;
    return true;
}


bool DelimitedTxtSuite::cb1a(void* arg, const char* line, size_t length)
{
    unsigned int& i = *static_cast<unsigned int*>(arg);
    --i;
    String expectedLine = formExpectedLine(i);
    bool ok = (String(line, length) == expectedLine);
    CPPUNIT_ASSERT(ok);

    return true;
}


bool
DelimitedTxtSuite::cb1b(void* arg, const char* line, size_t length)
{
    unsigned int& i = *static_cast<unsigned int*>(arg);
    --i;
    String expectedLine = formExpectedLine(i);
    bool ok = (String(line, length) == expectedLine);
    CPPUNIT_ASSERT(ok);

    return (i > 3);
}


bool DelimitedTxtSuite::cb1c(void* arg, const char* line, size_t length)
{
    unsigned int& i = *static_cast<unsigned int*>(arg);
    ++i;
    String expectedLine = formExpectedLine(i);
    bool ok = (String(line, length) == expectedLine);
    CPPUNIT_ASSERT(ok);

    return true;
}


bool DelimitedTxtSuite::cb1d(void* arg, const char* line, size_t length)
{
    unsigned int& i = *static_cast<unsigned int*>(arg);
    ++i;
    String expectedLine = formExpectedLine(i);
    bool ok = (String(line, length) == expectedLine);
    CPPUNIT_ASSERT(ok);

    return (i <= 3);
}


String DelimitedTxtSuite::formExpectedLine(size_t i)
{
    char c = static_cast<char>('0' + i);
    String expectedLine(i, c);
    if (i != 6)
    {
        expectedLine.append(1, NEW_LINE);
    }

    return expectedLine;
}


void DelimitedTxtSuite::cb3a(void* arg, const String& line)
{
    String& s = *static_cast<String*>(arg);
    s += line;
}


void DelimitedTxtSuite::cb3b(void* arg, const String& line)
{
    String& s = *static_cast<String*>(arg);
    String tmp(line);
    tmp += s;
    s = tmp;
}


void DelimitedTxtSuite::cb4a(void* arg, const char* line, size_t length)
{
    unsigned int& i = *static_cast<unsigned int*>(arg);
    --i;
    String expectedLine = formExpectedLine(i);
    bool ok = (String(line, length) == expectedLine);
    CPPUNIT_ASSERT(ok);
}


void DelimitedTxtSuite::cb4b(void* arg, const char* line, size_t length)
{
    unsigned int& i = *static_cast<unsigned int*>(arg);
    ++i;
    String expectedLine = formExpectedLine(i);
    bool ok = (String(line, length) == expectedLine);
    CPPUNIT_ASSERT(ok);
}


//
// Apply callback to each line. Use cb0_t.
//
void DelimitedTxtSuite::testApply00()
{
    DelimitedTxt txt0(SAMPLE0, sizeof(SAMPLE0) - 1, false /*makeCopy*/, NEW_LINE);

    String s;
    bool ok = txt0.applyLoToHi(cb0a, &s);
    CPPUNIT_ASSERT(ok);
    ok = (memcmp(s.raw(), txt0.txt(), txt0.txtSize()) == 0);
    CPPUNIT_ASSERT(ok);

    DelimitedTxt txt1(s, true, NEW_LINE);
    s.reset();
    ok = txt1.applyHiToLo(cb0b, &s);
    CPPUNIT_ASSERT(ok);
    ok = (memcmp(s.raw(), txt1.txt(), txt1.txtSize()) == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Apply callback to each line. Use cb1_t.
//
void DelimitedTxtSuite::testApply01()
{
    DelimitedTxt txt0(SAMPLE0, sizeof(SAMPLE0) - 1, false /*makeCopy*/, NEW_LINE);
    DelimitedTxt txt1(SAMPLE1, sizeof(SAMPLE1) - 1, true /*makeCopy*/, NEW_LINE);
    DelimitedTxt txt2(SAMPLE2, sizeof(SAMPLE2) - 1, false /*makeCopy*/, SEMICOLON);

    unsigned int i = 6;
    bool ok = txt0.applyHiToLo(cb1a, &i);
    CPPUNIT_ASSERT(ok);

    i = 6;
    ok = ((!txt0.applyHiToLo(cb1b, &i)) && (i == 3));
    CPPUNIT_ASSERT(ok);

    i = 0;
    ok = txt0.applyLoToHi(cb1c, &i);
    CPPUNIT_ASSERT(ok);

    i = 0;
    ok = ((!txt0.applyLoToHi(cb1d, &i)) && (i == 4));
    CPPUNIT_ASSERT(ok);

    i = 7;
    ok = txt1.applyHiToLo(cb1a, &i);
    CPPUNIT_ASSERT(ok);

    i = 7;
    ok = ((!txt1.applyHiToLo(cb1b, &i)) && (i == 3));
    CPPUNIT_ASSERT(ok);

    i = 0xffffffffU;
    ok = txt1.applyLoToHi(cb1c, &i);
    CPPUNIT_ASSERT(ok);

    i = 0xffffffffU;
    ok = ((!txt1.applyLoToHi(cb1d, &i)) && (i == 4));
    CPPUNIT_ASSERT(ok);

    i = 7;
    ok = txt2.applyHiToLo(cb1a, &i);
    CPPUNIT_ASSERT(ok);

    i = 7;
    ok = txt2.applyHiToLo(cb1b, &i);
    CPPUNIT_ASSERT(ok);

    i = 5;
    ok = txt2.applyLoToHi(cb1c, &i);
    CPPUNIT_ASSERT(ok);

    i = 5;
    ok = ((!txt2.applyLoToHi(cb1d, &i)) && (i == 6));
    CPPUNIT_ASSERT(ok);

    DelimitedTxt txt;
    DelimitedTxt::cb1_t cb = 0;
    ok = txt.applyHiToLo(cb, 0);
    CPPUNIT_ASSERT(ok);
}


//
// Apply callback to each line. Use cb3_t.
//
void DelimitedTxtSuite::testApply03()
{
    DelimitedTxt txt0(SAMPLE0, sizeof(SAMPLE0) - 1, false /*makeCopy*/, NEW_LINE);

    String s;
    txt0.applyLoToHi(cb3a, &s);
    bool ok = (memcmp(s.raw(), txt0.txt(), txt0.txtSize()) == 0);
    CPPUNIT_ASSERT(ok);

    s.reset();
    txt0.applyHiToLo(cb3b, &s);
    ok = (memcmp(s.raw(), txt0.txt(), txt0.txtSize()) == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Apply callback to each line. Use cb4_t.
//
void DelimitedTxtSuite::testApply04()
{
    DelimitedTxt txt0(SAMPLE0, sizeof(SAMPLE0) - 1, false /*makeCopy*/, NEW_LINE);

    unsigned int i = 6;
    txt0.applyHiToLo(cb4a, &i);

    DelimitedTxt txt;
    i = 999;
    txt.applyHiToLo(cb4a, &i);
    bool ok = (i == 999);
    CPPUNIT_ASSERT(ok);

    i = 0;
    txt0.applyLoToHi(cb4b, &i);

    const char* const TXT = "1" "\n" "22" "\n" "333" "\n" "4444" "\n" "55555" "\n" "666666";
    txt.setTxt(TXT, strlen(TXT), false /*makeCopy*/);
    i = 0;
    txt.applyLoToHi(cb4b, &i);
}


void DelimitedTxtSuite::testCount00()
{
    DelimitedTxt txt0(SAMPLE0, sizeof(SAMPLE0) - 1, false /*makeCopy*/, NEW_LINE);
    DelimitedTxt txt1(SAMPLE1, sizeof(SAMPLE1) - 1, true /*makeCopy*/, NEW_LINE);
    DelimitedTxt txt2(SAMPLE2, sizeof(SAMPLE2) - 1, false /*makeCopy*/, SEMICOLON);

    DelimitedTxt txt;
    bool ok = (txt.countLines() == 0);
    CPPUNIT_ASSERT(ok);

    txt.setTxt("", 0, false /*makeCopy*/);
    ok = (txt.countLines() == 0);
    CPPUNIT_ASSERT(ok);

    txt.setTxt("\n\n\n\n\n", 5, false /*makeCopy*/);
    ok = (txt.countLines() == 5);
    CPPUNIT_ASSERT(ok);

    ok = ((txt0.countLines() == 5) && (txt1.countLines() == 7) && (txt2.countLines() == 1));
    CPPUNIT_ASSERT(ok);
}


void DelimitedTxtSuite::testCtor00()
{
    DelimitedTxt txt;
    bool ok = (txt.delim() == NEW_LINE);
    CPPUNIT_ASSERT(ok);

    ok = ((txt.txt() == 0) && (txt.txtSize() == 0));
    CPPUNIT_ASSERT(ok);

    size_t length = 1234567890;
    ok = ((txt.txt(length) == 0) && (length == 0));
    CPPUNIT_ASSERT(ok);
}


void DelimitedTxtSuite::testCtor01()
{
    DelimitedTxt txt0(SAMPLE0, sizeof(SAMPLE0) - 1, false /*makeCopy*/, NEW_LINE);
    DelimitedTxt txt1(SAMPLE1, sizeof(SAMPLE1) - 1, true /*makeCopy*/, NEW_LINE);
    DelimitedTxt txt2(SAMPLE2, sizeof(SAMPLE2) - 1, false /*makeCopy*/, SEMICOLON);

    DelimitedTxt it0(txt0);
    size_t lenA = 12345;
    size_t lenB = 12345;
    bool ok = ((it0.delim() == txt0.delim()) && (it0.txt(lenA) == txt0.txt(lenB)) && (lenA == lenB));
    CPPUNIT_ASSERT(ok);

    DelimitedTxt it1(txt1);
    ok = ((it1.delim() == txt1.delim()) && (it1.txt(lenA) != txt1.txt(lenB)) && (lenA == lenB));
    CPPUNIT_ASSERT(ok);

    DelimitedTxt it2(txt2);
    ok = ((it2.delim() == txt2.delim()) && (it2.txt(lenA) == txt2.txt(lenB)) && (lenA == lenB));
    CPPUNIT_ASSERT(ok);
}


void DelimitedTxtSuite::testCtor02()
{
    const char* text = "55555.333.999999999.1.22.333.4444.88888888.55555.666666.7777777.";
    DelimitedTxt it0(text, strlen(text), false /*makeCopy*/, '.');
    String line;
    bool ok = (it0.next(line) && (line == "55555."));
    CPPUNIT_ASSERT(ok);

    DelimitedTxt it1(it0);
    ok = (it1.next(line) && (line == "333.") && (it0.countLines() == it1.countLines()));
    CPPUNIT_ASSERT(ok);

    const char* s;
    size_t length;
    it1.reset();
    while (it1.next(s, length))
    {
        unsigned int i = *s - '0';
        if (i + 1 != length)
        {
            ok = false;
            break;
        }
        char c = static_cast<char>('0' + i);
        String expectedLine(i, c);
        expectedLine.append(1, '.');
        if (String(s, length) != expectedLine)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


//
// Iterate low to high.
//
void DelimitedTxtSuite::testNext00()
{
    DelimitedTxt txt0(SAMPLE0, sizeof(SAMPLE0) - 1, false /*makeCopy*/, NEW_LINE);
    DelimitedTxt txt1(SAMPLE1, sizeof(SAMPLE1) - 1, true /*makeCopy*/, NEW_LINE);
    DelimitedTxt txt2(SAMPLE2, sizeof(SAMPLE2) - 1, false /*makeCopy*/, SEMICOLON);

    const char* line;
    size_t length;
    unsigned int i;
    bool ok = true;
    for (i = 0; txt0.next(line, length);)
    {
        ++i;
        String expectedLine = formExpectedLine(i);
        if (String(line, length) != expectedLine)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = ((i == 5) && (!txt0.next(line, length)) && (line == 0) && (length == 0));
    CPPUNIT_ASSERT(ok);

    for (i = 0xffffffffU; txt1.next(line, length);)
    {
        ++i;
        String expectedLine = formExpectedLine(i);
        if (String(line, length) != expectedLine)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = ((i == 6) && (!txt1.next(line, length)) && (line == 0) && (length == 0));
    CPPUNIT_ASSERT(ok);

    for (i = 5; txt2.next(line, length);)
    {
        ++i;
        String expectedLine = formExpectedLine(i);
        if (String(line, length) != expectedLine)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = ((i == 6) && (!txt2.next(line, length)) && (line == 0) && (length == 0));
    CPPUNIT_ASSERT(ok);
}


//
// Iterate low to high.
//
void DelimitedTxtSuite::testNext01()
{
    DelimitedTxt txt0(SAMPLE0, sizeof(SAMPLE0) - 1, false /*makeCopy*/, NEW_LINE);
    DelimitedTxt txt1(SAMPLE1, sizeof(SAMPLE1) - 1, true /*makeCopy*/, NEW_LINE);
    DelimitedTxt txt2(SAMPLE2, sizeof(SAMPLE2) - 1, false /*makeCopy*/, SEMICOLON);

    String line;
    unsigned int i;
    bool ok = true;
    for (i = 0; txt0.next(line);)
    {
        ++i;
        String expectedLine = formExpectedLine(i);
        if (line != expectedLine)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = ((i == 5) && (!txt0.next(line)) && line.empty());
    CPPUNIT_ASSERT(ok);

    for (i = 0xffffffffU; txt1.next(line);)
    {
        ++i;
        String expectedLine = formExpectedLine(i);
        if (line != expectedLine)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = ((i == 6) && (!txt1.next(line)) && line.empty());
    CPPUNIT_ASSERT(ok);

    for (i = 5; txt2.next(line);)
    {
        ++i;
        String expectedLine = formExpectedLine(i);
        if (line != expectedLine)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = ((i == 6) && (!txt2.next(line)) && line.empty());
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - bool DelimitedTxt::next(String&, bool);
//
void DelimitedTxtSuite::testNext02()
{
    DelimitedTxt txt0(SAMPLE0, sizeof(SAMPLE0) - 1, false /*makeCopy*/, NEW_LINE);

    String s;
    bool doTrimLine = false;
    for (String line; txt0.next(line, doTrimLine); s += line);
    bool ok = (memcmp(s.raw(), txt0.txt(), txt0.txtSize()) == 0);
    CPPUNIT_ASSERT(ok);

    doTrimLine = true;
    ok = (!txt0.next(s, doTrimLine));
    CPPUNIT_ASSERT(ok);

    txt0.reset();
    ok = (txt0.next(s, doTrimLine) && (s == "1"));
    CPPUNIT_ASSERT(ok);
}


void DelimitedTxtSuite::testOp00()
{
    DelimitedTxt txt0(SAMPLE0, sizeof(SAMPLE0) - 1, false /*makeCopy*/, NEW_LINE);
    DelimitedTxt txt1(SAMPLE1, sizeof(SAMPLE1) - 1, true /*makeCopy*/, NEW_LINE);

    String line;
    txt0.next(line);
    txt0.next(line);
    DelimitedTxt tmp0(txt1);
    txt1 = txt0;
    bool ok = ((tmp0.txt() != txt1.txt()) && tmp0.peekDown(line) && (line == "\n"));
    CPPUNIT_ASSERT(ok);

    ok = (txt1.next(line) && (line == "333\n"));
    CPPUNIT_ASSERT(ok);
    ok = (txt1.peekDown(line) && (line == "4444\n"));
    CPPUNIT_ASSERT(ok);
    ok = (txt1.peekUp(line) && (line == "22\n"));
    CPPUNIT_ASSERT(ok);

    bool makeCopy = false;
    DelimitedTxt tmp1(txt1, makeCopy);
    ok = ((tmp1.txt() == txt1.txt()) && tmp1.peekUp(line) && (line == "55555\n"));
    CPPUNIT_ASSERT(ok);
}


void DelimitedTxtSuite::testPeek00()
{
    DelimitedTxt txt0(SAMPLE0, sizeof(SAMPLE0) - 1, false /*makeCopy*/, NEW_LINE);

    DelimitedTxt txt;
    String line;
    bool ok = ((!txt.peekDown()) && (!txt.peekDown(line)) && line.empty());
    CPPUNIT_ASSERT(ok);

    ok = ((!txt.peekUp()) && (!txt.peekUp(line)) && line.empty());
    CPPUNIT_ASSERT(ok);

    ok = (txt0.peekDown() && txt0.peekDown(line) && (line == "1\n"));
    CPPUNIT_ASSERT(ok);

    ok = (txt0.peekUp() && txt0.peekUp(line) && (line == "55555\n"));
    CPPUNIT_ASSERT(ok);
}


//
// Iterate downward. Make sure we can look down.
//
void DelimitedTxtSuite::testPeek01()
{
    DelimitedTxt txt0(SAMPLE0, sizeof(SAMPLE0) - 1, false /*makeCopy*/, NEW_LINE);
    DelimitedTxt txt1(SAMPLE1, sizeof(SAMPLE1) - 1, true /*makeCopy*/, NEW_LINE);

    String curLine;
    String oldLine;
    bool ok = true;
    for (txt0.peekDown(oldLine); txt0.next(curLine); txt0.peekDown(oldLine))
    {
        if (curLine != oldLine)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (!txt0.peekDown());
    CPPUNIT_ASSERT(ok);

    for (txt1.peekDown(oldLine); txt1.next(curLine); txt1.peekDown(oldLine))
    {
        if (curLine != oldLine)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


//
// Iterate downward. Make sure we can look up.
//
void DelimitedTxtSuite::testPeek02()
{
    DelimitedTxt txt0(SAMPLE0, sizeof(SAMPLE0) - 1, false /*makeCopy*/, NEW_LINE);
    DelimitedTxt txt1(SAMPLE1, sizeof(SAMPLE1) - 1, true /*makeCopy*/, NEW_LINE);

    String curLine;
    String oldLine;
    String line;
    bool ok = true;
    for (; txt0.next(curLine); oldLine = curLine)
    {
        if ((txt0.peekUp(line) == oldLine.empty()) || (line != oldLine))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    for (oldLine.reset(); txt1.next(curLine); oldLine = curLine)
    {
        if ((txt1.peekUp(line) == oldLine.empty()) || (line != oldLine))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


//
// Iterate upward. Make sure we can look up.
//
void DelimitedTxtSuite::testPeek03()
{
    DelimitedTxt txt0(SAMPLE0, sizeof(SAMPLE0) - 1, false /*makeCopy*/, NEW_LINE);
    DelimitedTxt txt1(SAMPLE1, sizeof(SAMPLE1) - 1, true /*makeCopy*/, NEW_LINE);

    String curLine;
    String oldLine;
    bool ok = true;
    for (txt0.peekUp(oldLine); txt0.prev(curLine); txt0.peekUp(oldLine))
    {
        if (curLine != oldLine)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (!txt0.peekUp());
    CPPUNIT_ASSERT(ok);

    for (txt1.peekUp(oldLine); txt1.prev(curLine); txt1.peekUp(oldLine))
    {
        if (curLine != oldLine)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


//
// Iterate upward. Make sure we can look down.
//
void DelimitedTxtSuite::testPeek04()
{
    DelimitedTxt txt0(SAMPLE0, sizeof(SAMPLE0) - 1, false /*makeCopy*/, NEW_LINE);
    DelimitedTxt txt1(SAMPLE1, sizeof(SAMPLE1) - 1, true /*makeCopy*/, NEW_LINE);

    String curLine;
    String oldLine;
    String line;
    bool ok = true;
    for (; txt0.prev(curLine); oldLine = curLine)
    {
        if ((txt0.peekDown(line) == oldLine.empty()) || (line != oldLine))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    for (oldLine.reset(); txt1.prev(curLine); oldLine = curLine)
    {
        if ((txt1.peekDown(line) == oldLine.empty()) || (line != oldLine))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void DelimitedTxtSuite::testPeek05()
{
    DelimitedTxt txt0(SAMPLE0, sizeof(SAMPLE0) - 1, false /*makeCopy*/, NEW_LINE);

    String s;
    for (String line; txt0.peekDown(line); s += line, txt0.next(line));
    bool ok = (memcmp(s.raw(), txt0.txt(), txt0.txtSize()) == 0);
    CPPUNIT_ASSERT(ok);
    txt0.reset();

    s.reset();
    for (String line; txt0.peekUp(line); line += s, s = line, txt0.prev(line));
    ok = (memcmp(s.raw(), txt0.txt(), txt0.txtSize()) == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Iterate high to low.
//
void DelimitedTxtSuite::testPrev00()
{
    DelimitedTxt txt0(SAMPLE0, sizeof(SAMPLE0) - 1, false /*makeCopy*/, NEW_LINE);
    DelimitedTxt txt1(SAMPLE1, sizeof(SAMPLE1) - 1, true /*makeCopy*/, NEW_LINE);
    DelimitedTxt txt2(SAMPLE2, sizeof(SAMPLE2) - 1, false /*makeCopy*/, SEMICOLON);

    const char* line;
    size_t length;
    unsigned int i;
    bool ok = true;
    for (i = 5; txt0.prev(line, length); --i)
    {
        String expectedLine = formExpectedLine(i);
        if (String(line, length) != expectedLine)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = ((i == 0) && (!txt0.prev(line, length)) && (line == 0) && (length == 0));
    CPPUNIT_ASSERT(ok);

    for (i = 6; txt1.prev(line, length); --i)
    {
        String expectedLine = formExpectedLine(i);
        if (String(line, length) != expectedLine)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = ((i == 0xffffffffU) && (!txt1.prev(line, length)) && (line == 0) && (length == 0));
    CPPUNIT_ASSERT(ok);

    for (i = 6; txt2.prev(line, length); --i)
    {
        String expectedLine = formExpectedLine(i);
        if (String(line, length) != expectedLine)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = ((i == 5) && (!txt2.prev(line, length)) && (line == 0) && (length == 0));
    CPPUNIT_ASSERT(ok);
}


//
// Iterate high to low.
//
void DelimitedTxtSuite::testPrev01()
{
    DelimitedTxt txt0(SAMPLE0, sizeof(SAMPLE0) - 1, false /*makeCopy*/, NEW_LINE);
    DelimitedTxt txt1(SAMPLE1, sizeof(SAMPLE1) - 1, true /*makeCopy*/, NEW_LINE);
    DelimitedTxt txt2(SAMPLE2, sizeof(SAMPLE2) - 1, false /*makeCopy*/, SEMICOLON);

    String line;
    unsigned int i;
    bool ok = true;
    for (i = 5; txt0.prev(line); --i)
    {
        String expectedLine = formExpectedLine(i);
        if (line != expectedLine)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = ((i == 0) && (!txt0.prev(line)) && line.empty());
    CPPUNIT_ASSERT(ok);

    for (i = 6; txt1.prev(line); --i)
    {
        String expectedLine = formExpectedLine(i);
        if (line != expectedLine)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = ((i == 0xffffffffU) && (!txt1.prev(line)) && line.empty());
    CPPUNIT_ASSERT(ok);

    for (i = 6; txt2.prev(line); --i)
    {
        String expectedLine = formExpectedLine(i);
        if (line != expectedLine)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = ((i == 5) && (!txt2.prev(line)) && line.empty());
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - bool DelimitedTxt::prev(String&, bool);
//
void DelimitedTxtSuite::testPrev02()
{
    DelimitedTxt txt0(SAMPLE0, sizeof(SAMPLE0) - 1, false /*makeCopy*/, NEW_LINE);

    String s;
    bool doTrimLine = false;
    for (String line; txt0.prev(line, doTrimLine); line += s, s = line);
    bool ok = (memcmp(s.raw(), txt0.txt(), txt0.txtSize()) == 0);
    CPPUNIT_ASSERT(ok);

    doTrimLine = true;
    ok = (!txt0.prev(s, doTrimLine));
    CPPUNIT_ASSERT(ok);

    txt0.reset();
    ok = (txt0.prev(s, doTrimLine) && (s == "55555"));
    CPPUNIT_ASSERT(ok);
}


void DelimitedTxtSuite::testTrim00()
{
    DelimitedTxt txt(NEW_LINE);
    size_t trimmedLength = txt.trimLine("", 0);
    bool ok = (trimmedLength == 0);
    CPPUNIT_ASSERT(ok);

    trimmedLength = txt.trimLine("abc123", 6);
    ok = (trimmedLength == 6);
    CPPUNIT_ASSERT(ok);

    trimmedLength = txt.trimLine("abc\n", 4);
    ok = (trimmedLength == 3);
    CPPUNIT_ASSERT(ok);

    trimmedLength = txt.trimLine("abc123\r\n", 8);
    ok = (trimmedLength == 6);
    CPPUNIT_ASSERT(ok);

    txt.setDelim(',');
    trimmedLength = txt.trimLine("abc123\r\n", 8);
    ok = (trimmedLength == 8);
    CPPUNIT_ASSERT(ok);
}


void DelimitedTxtSuite::testTrim01()
{
    DelimitedTxt txt(NEW_LINE);
    String line0;
    bool ok = txt.trimLine(line0).empty();
    CPPUNIT_ASSERT(ok);

    line0 = "abc123";
    ok = (txt.trimLine(line0) == "abc123");
    CPPUNIT_ASSERT(ok);

    line0 = "abc\n";
    ok = (txt.trimLine(line0) == "abc");
    CPPUNIT_ASSERT(ok);

    String line1("abc\r\n");
    ok = (txt.trimLine(line1) == "abc");
    CPPUNIT_ASSERT(ok);
}


void DelimitedTxtSuite::testVectorize00()
{
    DelimitedTxt txt(SAMPLE0, sizeof(SAMPLE0) - 1, false /*makeCopy*/, NEW_LINE);
    StringVec vec;
    bool doTrimLine = false;
    bool ok = txt.vectorize(vec, doTrimLine) && (vec.numItems() == 5);
    CPPUNIT_ASSERT(ok);

    for (unsigned int i = 5; i > 0;)
    {
        --i;
        if (vec[i] != formExpectedLine(i + 1))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    const char* delim = 0;
    ok = (vec.stringify(delim) == SAMPLE0);
    CPPUNIT_ASSERT(ok);
}


void DelimitedTxtSuite::testVectorize01()
{
    DelimitedTxt txt(SAMPLE0, sizeof(SAMPLE0) - 1, false /*makeCopy*/, NEW_LINE);
    StringVec vec;
    bool doTrimLine = true;
    bool ok = txt.vectorize(vec, doTrimLine) && (vec.numItems() == 5);
    CPPUNIT_ASSERT(ok);

    for (unsigned int i = 5; i > 0;)
    {
        --i;
        if ((vec[i] + "\n") != formExpectedLine(i + 1))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    const char* delim = "\n";
    ok = ((vec.stringify(delim) + "\n") == SAMPLE0);
    CPPUNIT_ASSERT(ok);
}
