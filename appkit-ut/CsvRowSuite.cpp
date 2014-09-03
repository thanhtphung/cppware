#include "appkit/CsvRow.hpp"
#include "appkit/Directory.hpp"

#include "appkit-ut-pch.h"
#include "CsvRowSuite.hpp"

using namespace appkit;
using namespace syskit;

const char COMMA = ',';

// All columns have a column delim.
const char SAMPLE0[] = "1" "," "22" "," "333" "," "4444" "," "55555" ",";

// First column has only a column delim.
// Last column does not have a column delim.
const char SAMPLE1[] = "," "1" "," "22" "," "333" "," "4444" "," "55555" "," "666666";

// No column delims.
const char SAMPLE2[] = "666666";


CsvRowSuite::CsvRowSuite()
{
}


CsvRowSuite::~CsvRowSuite()
{
}


String CsvRowSuite::formExpectedCol(size_t i)
{
    char c = static_cast<char>('0' + i);
    String expectedCol(i, c);
    if (i != 6)
    {
        expectedCol.append(1, COMMA);
    }

    return expectedCol;
}


bool CsvRowSuite::cb0(void* arg, const String& col)
{
    String& s = *static_cast<String*>(arg);
    s += col;
    return true;
}


bool CsvRowSuite::cb1a(void* arg, const char* col, size_t length)
{
    unsigned int& i = *static_cast<unsigned int*>(arg);
    ++i;
    String expectedCol = formExpectedCol(i);
    bool ok = (String(col, length) == expectedCol);
    CPPUNIT_ASSERT(ok);

    return true;
}


bool CsvRowSuite::cb1b(void* arg, const char* col, size_t length)
{
    unsigned int& i = *static_cast<unsigned int*>(arg);
    ++i;
    String expectedCol = formExpectedCol(i);
    bool ok = (String(col, length) == expectedCol);
    CPPUNIT_ASSERT(ok);

    return (i <= 3);
}


bool CsvRowSuite::cb1c(void* arg, const char* col, size_t length)
{
    String& s = *static_cast<String*>(arg);
    s.append(col, length);
    return true;
}


void CsvRowSuite::cb3(void* arg, const String& col)
{
    String& s = *static_cast<String*>(arg);
    s += col;
}


void CsvRowSuite::cb4a(void* arg, const char* col, size_t length)
{
    unsigned int& i = *static_cast<unsigned int*>(arg);
    ++i;
    String expectedCol = formExpectedCol(i);
    bool ok = (String(col, length) == expectedCol);
    CPPUNIT_ASSERT(ok);
}


void CsvRowSuite::cb4b(void* arg, const char* col, size_t length)
{
    String& s = *static_cast<String*>(arg);
    s.append(col, length);
}


//
// Apply callback to each col. Use cb0_t.
//
void CsvRowSuite::testApply00()
{
    Directory dir("../etc/");
    String files;
    String subdirs;
    dir.list(files, subdirs);

    String s;
    CsvRow csv0(subdirs, Directory::DELIM);
    bool ok = (csv0.apply(cb0, &s) && (s == subdirs));
    CPPUNIT_ASSERT(ok);

    s.reset();
    CsvRow csv1(files, Directory::DELIM);
    ok = (csv1.apply(cb0, &s) && (s == files));
    CPPUNIT_ASSERT(ok);
}


//
// Apply callback to each col. Use cb1_t.
//
void CsvRowSuite::testApply01()
{
    CsvRow row0(SAMPLE0, sizeof(SAMPLE0) - 1, COMMA);
    CsvRow row1(SAMPLE1, sizeof(SAMPLE1) - 1, COMMA);
    CsvRow row2(SAMPLE2, sizeof(SAMPLE2) - 1, COMMA);

    unsigned int i = 0;
    bool ok = row0.apply(cb1a, &i);
    CPPUNIT_ASSERT(ok);

    i = 0;
    ok = ((!row0.apply(cb1b, &i)) && (i == 4));
    CPPUNIT_ASSERT(ok);

    i = 0xffffffffU;
    ok = row1.apply(cb1a, &i);
    CPPUNIT_ASSERT(ok);

    i = 0xffffffffU;
    ok = ((!row1.apply(cb1b, &i)) && (i == 4));
    CPPUNIT_ASSERT(ok);

    i = 5;
    ok = row2.apply(cb1a, &i);
    CPPUNIT_ASSERT(ok);

    i = 5;
    ok = ((!row2.apply(cb1b, &i)) && (i == 6));
    CPPUNIT_ASSERT(ok);

    const char CSV[] = "1,\"22\\\"\",',333,',\"4444,\",55555,666666";
    String s;
    CsvRow csv(CSV, sizeof(CSV) - 1);
    ok = (csv.apply(cb1c, &s) && (s == CSV));
    CPPUNIT_ASSERT(ok);
}


//
// Apply callback to each col. Use cb3_t.
//
void CsvRowSuite::testApply03()
{
    Directory dir("../etc/");
    String files;
    String subdirs;
    dir.list(files, subdirs);

    String s;
    CsvRow csv0(subdirs, Directory::DELIM);
    csv0.apply(cb3, &s);
    bool ok = (s == subdirs);
    CPPUNIT_ASSERT(ok);

    s.reset();
    CsvRow csv1(files, Directory::DELIM);
    csv1.apply(cb3, &s);
    ok = (s == files);
    CPPUNIT_ASSERT(ok);
}


//
// Apply callback to each col. Use cb4_t.
//
void CsvRowSuite::testApply04()
{
    CsvRow row0(SAMPLE0, sizeof(SAMPLE0) - 1, COMMA);

    unsigned i = 0;
    row0.apply(cb4a, &i);

    String s0("1" "," "22" "," "333" "," "4444" "," "55555" "," "666666");
    CsvRow csv0(s0);
    i = 0;
    csv0.apply(cb4a, &i);

    const char CSV[] = "1,\"22\\\"\",',333,',\"4444,\",55555,666666";
    String s1;
    CsvRow csv1(CSV, sizeof(CSV) - 1);
    csv1.apply(cb4b, &s1);
    bool ok = (s1 == CSV);
    CPPUNIT_ASSERT(ok);
}


void CsvRowSuite::testCount00()
{
    CsvRow row0(SAMPLE0, sizeof(SAMPLE0) - 1, COMMA);
    CsvRow row1(SAMPLE1, sizeof(SAMPLE1) - 1, COMMA);
    CsvRow row2(SAMPLE2, sizeof(SAMPLE2) - 1, COMMA);

    const char CSV[] = "'',,',abc\\'123,',\"\\\",\",,";
    CsvRow csv(CSV, sizeof(CSV) - 1);
    bool ok = (csv.countCols() == 5);
    CPPUNIT_ASSERT(ok);

    ok = ((row0.countCols() == 5) && (row1.countCols() == 7) && (row2.countCols() == 1));
    CPPUNIT_ASSERT(ok);
}


void CsvRowSuite::testCtor00()
{
    const char CSV[] = "55555.333.999999999.1.22.333.4444.88888888.55555.666666.7777777.";
    const char DOT = '.';
    CsvRow csv(CSV, sizeof(CSV) - 1, DOT);
    String col;
    bool ok = (csv.next(col) && (col == "55555."));
    CPPUNIT_ASSERT(ok);

    const char* s;
    size_t length;
    csv.reset();
    while (csv.next(s, length))
    {
        unsigned int i = *s - '0';
        if (i + 1 != length)
        {
            ok = false;
            break;
        }
        char c = static_cast<char>('0' + i);
        String expectedCol(i, c);
        expectedCol.append(1, DOT);
        if (String(s, length) != expectedCol)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void CsvRowSuite::testCtor01()
{
    CsvRow row0(SAMPLE0, sizeof(SAMPLE0) - 1, COMMA);
    CsvRow row1(SAMPLE1, sizeof(SAMPLE1) - 1, COMMA);
    CsvRow row2(SAMPLE2, sizeof(SAMPLE2) - 1, COMMA);

    const utf8_t* row = row2.row();
    bool ok = ((reinterpret_cast<const char*>(row) == SAMPLE2) && (row2.rowSize() == sizeof(SAMPLE2) - 1));
    CPPUNIT_ASSERT(ok);

    size_t size = 123;
    row = row1.row(size);
    ok = ((reinterpret_cast<const char*>(row) == SAMPLE1) && (size == sizeof(SAMPLE1) - 1));
    CPPUNIT_ASSERT(ok);

    ok = (row0.delim() == COMMA);
    CPPUNIT_ASSERT(ok);
}


void CsvRowSuite::testDequoteCol00()
{
    String col("no quotes");
    bool ok = (!CsvRow::dequoteCol(col)) && (col == "no quotes");
    CPPUNIT_ASSERT(ok);
    col = "\'no-terminating-quote";
    ok = (!CsvRow::dequoteCol(col)) && (col == "\'no-terminating-quote");
    CPPUNIT_ASSERT(ok);
    col = "\"no-terminating-quote\'";
    ok = (!CsvRow::dequoteCol(col)) && (col == "\"no-terminating-quote\'");
    CPPUNIT_ASSERT(ok);

    col = "\'excel quote: \'\'\'";
    ok = CsvRow::dequoteCol(col) && (col == "excel quote: \'");
    CPPUNIT_ASSERT(ok);
    col = "\'excel quote: \'\'\'";
    ok = CsvRow::dequoteCol(col, false /*unescapeSingleQuotedStrings*/) && (col == "excel quote: \\'");
    CPPUNIT_ASSERT(ok);
    col = "\"excel quote: \"\"\"";
    ok = CsvRow::dequoteCol(col) && (col == "excel quote: \"");
    CPPUNIT_ASSERT(ok);
}


void CsvRowSuite::testNext00()
{
    CsvRow row0(SAMPLE0, sizeof(SAMPLE0) - 1, COMMA);
    CsvRow row1(SAMPLE1, sizeof(SAMPLE1) - 1, COMMA);
    CsvRow row2(SAMPLE2, sizeof(SAMPLE2) - 1, COMMA);

    const char* col;
    size_t length;
    unsigned int i;
    bool ok = true;
    for (i = 0; row0.next(col, length);)
    {
        ++i;
        String expectedCol = formExpectedCol(i);
        if (String(col, length) != expectedCol)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = ((i == 5) && (!row0.next(col, length)) && (col == 0) && (length == 0));
    CPPUNIT_ASSERT(ok);

    for (i = 0xffffffffU; row1.next(col, length);)
    {
        ++i;
        String expectedCol = formExpectedCol(i);
        if (String(col, length) != expectedCol)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = ((i == 6) && (!row1.next(col, length)) && (col == 0) && (length == 0));
    CPPUNIT_ASSERT(ok);

    for (i = 5; row2.next(col, length);)
    {
        ++i;
        String expectedCol = formExpectedCol(i);
        if (String(col, length) != expectedCol)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = ((i == 6) && (!row2.next(col, length)) && (col == 0) && (length == 0));
    CPPUNIT_ASSERT(ok);
}


void CsvRowSuite::testNext01()
{
    CsvRow row0(SAMPLE0, sizeof(SAMPLE0) - 1, COMMA);
    CsvRow row1(SAMPLE1, sizeof(SAMPLE1) - 1, COMMA);
    CsvRow row2(SAMPLE2, sizeof(SAMPLE2) - 1, COMMA);

    String col;
    unsigned int i;
    bool ok = true;
    for (i = 0; row0.next(col);)
    {
        ++i;
        String expectedCol = formExpectedCol(i);
        if (expectedCol != col)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = ((i == 5) && (!row0.next(col)) && col.empty());
    CPPUNIT_ASSERT(ok);

    for (i = 0xffffffffU; row1.next(col);)
    {
        ++i;
        String expectedCol = formExpectedCol(i);
        if (expectedCol != col)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = ((i == 6) && (!row1.next(col)) && col.empty());
    CPPUNIT_ASSERT(ok);

    for (i = 5; row2.next(col);)
    {
        ++i;
        String expectedCol = formExpectedCol(i);
        if (expectedCol != col)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = ((i == 6) && (!row2.next(col)) && col.empty());
    CPPUNIT_ASSERT(ok);
}


void CsvRowSuite::testNext02()
{
    Directory dir("../etc/");
    String files;
    String subdirs;
    dir.list(files, subdirs);

    String s;
    CsvRow csv0(subdirs, Directory::DELIM);
    for (String col; csv0.next(col); s += col);
    bool ok = (s == subdirs);
    CPPUNIT_ASSERT(ok);

    s.reset();
    CsvRow csv1(files, Directory::DELIM);
    for (String col; csv1.next(col); s += col);
    ok = (s == files);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - bool CsvRow::next(String&, bool);
//
void CsvRowSuite::testNext03()
{
    const char CSV[] = "1;\"22\\\"\";';333;';\"4444;\";55555;666666;";
    const char SEMICOLON = ';';
    CsvRow csv0(CSV, sizeof(CSV) - 1, SEMICOLON);

    String s;
    bool doDequoteCol = false;
    for (String col; csv0.next(col, doDequoteCol); s += col);
    bool ok = (s == CSV);
    CPPUNIT_ASSERT(ok);

    doDequoteCol = true;
    ok = (!csv0.next(s, doDequoteCol));
    CPPUNIT_ASSERT(ok);

    csv0.reset();
    ok = (csv0.next(s, doDequoteCol) && (s == "1"));
    CPPUNIT_ASSERT(ok);
    ok = (csv0.next(s, doDequoteCol) && csv0.next(s, doDequoteCol) && (s == ";333;"));
    CPPUNIT_ASSERT(ok);

    CsvRow csv1("\xff", strlen("\xff"), SEMICOLON);
    csv1.next(s);
    ok = (s == "?");
    CPPUNIT_ASSERT(ok);
}


void CsvRowSuite::testTrim00()
{
    const char VERT_BAR = '|';
    CsvRow csv("", 0, VERT_BAR);
    String col0;
    const char* col2 = "";
    bool ok = (csv.trimCol(col0).empty() && (csv.trimCol(col2, 0) == 0));
    CPPUNIT_ASSERT(ok);

    col0 = "col";
    col2 = "col";
    ok = ((csv.trimCol(col0) == "col") && (csv.trimCol(col2, 3) == 3));
    CPPUNIT_ASSERT(ok);

    col0 = "col|";
    col2 = "col|";
    ok = ((csv.trimCol(col0) == "col") && (csv.trimCol(col2, 4) == 3));
    CPPUNIT_ASSERT(ok);

    // Non-ASCII.
    const utf32_t CENT = 0xA2U;
    Utf8Seq seq;
    utf32_t col32[2] = {CENT, VERT_BAR};
    seq.shrink(col32, 1);
    col0 = seq;
    seq.shrink(col32, 2);
    String col3(seq);
    csv.trimCol(col0);
    csv.trimCol(col3);
    ok = ((col0.length() == 1) && (col0[0] == CENT) && (col0 == col3));
    CPPUNIT_ASSERT(ok);
}
