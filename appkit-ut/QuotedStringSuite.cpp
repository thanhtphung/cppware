#include "appkit/QuotedString.hpp"
#include "appkit/String.hpp"

#include "appkit-ut-pch.h"
#include "QuotedStringSuite.hpp"

using namespace appkit;


QuotedStringSuite::QuotedStringSuite()
{
}


QuotedStringSuite::~QuotedStringSuite()
{
}


void QuotedStringSuite::testDequote00()
{

    // Empty string.
    String str;
    bool ok = ((!str.dequote()) && str.empty());
    CPPUNIT_ASSERT(ok);

    // Empty quoted string.
    str = "\'\'";
    ok = (str.dequote() && str.empty());
    CPPUNIT_ASSERT(ok);
    str = "\"\"";
    ok = (str.dequote() && str.empty());
    CPPUNIT_ASSERT(ok);

    str = "\"double-quoted-string no-escape-sequences\"";
    ok = (str.dequote() && (str == "double-quoted-string no-escape-sequences"));
    CPPUNIT_ASSERT(ok);
    str = "\'single-quoted-string no-escape-sequences\'";
    ok = (str.dequote() && (str == "single-quoted-string no-escape-sequences"));
    CPPUNIT_ASSERT(ok);
    str = "\'no-terminating-quote no-escape-sequences";
    ok = (!str.dequote());
    CPPUNIT_ASSERT(ok);
    str = "\"no-terminating-quote no-escape-sequences\'";
    ok = (!str.dequote());
    CPPUNIT_ASSERT(ok);

    str = "no-quotes no-escape-sequences";
    ok = ((!str.dequote()) && (str == "no-quotes no-escape-sequences"));
    CPPUNIT_ASSERT(ok);
}


void QuotedStringSuite::testDequote01()
{

    // Valid escape sequences.
    String str("\\\?" "\\\'" "\\\"" "\\\\" "abc123" "\\a" "\\b" "\\f" "\\n" "\\r" "\\t" "\\v");
    bool ok = (str.dequote() && (str == "\?" "\'" "\"" "\\" "abc123" "\a" "\b" "\f" "\n" "\r" "\t" "\v"));
    CPPUNIT_ASSERT(ok);

    // Invalid escape sequence.
    str = "\\Z";
    ok = (str.dequote() && (str == "?"));
    CPPUNIT_ASSERT(ok);
    str = "\\x";
    ok = (str.dequote() && (str == "?"));
    CPPUNIT_ASSERT(ok);

    // Unfinished escape sequence.
    str = "Z\\";
    ok = (str.dequote() && (str == "Z"));
    CPPUNIT_ASSERT(ok);

    // Octal escape sequence.
    str = "\\1\\77\\177";
    ok = (str.dequote() && (str == "\1\77\177"));
    CPPUNIT_ASSERT(ok);
    str = "\\377";
    ok = (str.dequote() && (str.length() == 1) && (str[0] == 0xffU));
    CPPUNIT_ASSERT(ok);

    // Hexadecimal escape sequence.
    str = "\\x1\\xf\\xF\\x22\\x7f\\x7F";
    ok = (str.dequote() && (str == "\x1\xf\xF\x22\x7f\x7F"));
    CPPUNIT_ASSERT(ok);
    str = "\\xff\\xFF";
    ok = (str.dequote() && (str.length() == 2) && (str[0] == 0xffU) && (str[1] == 0xffU));
    CPPUNIT_ASSERT(ok);
}


void QuotedStringSuite::testDequote02()
{
    String str("\\u");
    bool ok = (str.dequote() && (str == "?"));
    CPPUNIT_ASSERT(ok);
    str = "\\uabcx";
    ok = (str.dequote() && (str == "?abcx"));
    CPPUNIT_ASSERT(ok);
    str = "\\ufedcba90";
    ok = (str.dequote() && (str == "?"));
    CPPUNIT_ASSERT(ok);

    str = "\\u0000";
    ok = (str.dequote() && str.empty());
    CPPUNIT_ASSERT(ok);

    str = "\\uD7FF";
    ok = (str.dequote() && (str.length() == 1) && (str[0] == 0xd7ffU));
    CPPUNIT_ASSERT(ok);

    str = "\\ue000";
    ok = (str.dequote() && (str.length() == 1) && (str[0] == 0xe000U));
    CPPUNIT_ASSERT(ok);

    str = "\"\\u0010ffff\"";
    ok = (str.dequote() && (str.length() == 1) && (str[0] == 0x0010ffffU));
    CPPUNIT_ASSERT(ok);
}


void QuotedStringSuite::testEnquote00()
{
    String str;
    char quote = 0;
    QuotedString quoted(str, quote);
    bool ok = (quoted.asString() == str);
    CPPUNIT_ASSERT(ok);
    quote = '"';
    quoted = QuotedString(str, quote);
    ok = (quoted.asString() == "\"\"");
    CPPUNIT_ASSERT(ok);
    quote = '\'';
    quoted = QuotedString(str, quote);
    ok = (quoted.asString() == "\'\'");
    CPPUNIT_ASSERT(ok);

    str = "\\";
    quote = 0;
    quoted = QuotedString(str, quote);
    ok = (quoted.asString() == "\\\\");
    CPPUNIT_ASSERT(ok);
    quote = '"';
    quoted = QuotedString(str, quote);
    ok = (quoted.asString() == "\"\\\\\"");
    CPPUNIT_ASSERT(ok);
    quote = '\'';
    quoted = QuotedString(str, quote);
    ok = (quoted.asString() == "\'\\\\\'");
    CPPUNIT_ASSERT(ok);

    str = "no-escape-sequences";
    quote = 0;
    quoted = QuotedString(str, quote);
    ok = (quoted.asString() == str);
    CPPUNIT_ASSERT(ok);
    quote = '\"';
    quoted = QuotedString(str, quote);
    ok = (quoted.asString() == ('\"' + str + '\"'));
    CPPUNIT_ASSERT(ok);
    quote = '\'';
    quoted = QuotedString(str, quote);
    ok = (quoted.asString() == ('\'' + str + '\''));
    CPPUNIT_ASSERT(ok);
}


void QuotedStringSuite::testEnquote01()
{
    String str = "C:\\a b c\\123.xyz";
    char quote = 0;
    QuotedString quoted(str, quote);
    bool ok = (quoted.asString() == "C:\\\\a b c\\\\123.xyz");
    CPPUNIT_ASSERT(ok);
    quote = '\"';
    quoted = QuotedString(str, quote);
    ok = (quoted.asString() == "\"" "C:\\\\a b c\\\\123.xyz" "\"");
    CPPUNIT_ASSERT(ok);
    quote = '\'';
    quoted = QuotedString(str, quote);
    ok = (quoted.asString() == "\'" "C:\\\\a b c\\\\123.xyz" "\'");
    CPPUNIT_ASSERT(ok);

    str = "\?" "\'" "\"" "\\" "abc123" "\a" "\b" "\f" "\n" "\r" "\t" "\v";
    quote = 0;
    quoted = QuotedString(str, quote);
    ok = (quoted.asString() == "\\\?" "\\\'" "\\\"" "\\\\" "abc123" "\\a" "\\b" "\\f" "\\n" "\\r" "\\t" "\\v");
    CPPUNIT_ASSERT(ok);
    ok = (quoted.dequote() == str);
    CPPUNIT_ASSERT(ok);

    str = "C:\\a b c\\123.xyz";
    quote = '\'';
    bool escapeSingleQuotedString = false;
    quoted = QuotedString(str, quote, escapeSingleQuotedString);
    ok = (quoted.asString() == "'C:\\a b c\\123.xyz'");
    CPPUNIT_ASSERT(ok);
    bool unescapeSingleQuotedString = false;
    ok = (quoted.dequote(unescapeSingleQuotedString) == str);
    CPPUNIT_ASSERT(ok);

    str = "C:\\a b c\\123.xyz";
    quote = '\"';
    quoted = QuotedString(str, quote, escapeSingleQuotedString);
    ok = (quoted.asString() == "\"" "C:\\\\a b c\\\\123.xyz" "\"");
    CPPUNIT_ASSERT(ok);
    ok = (quoted.dequote(unescapeSingleQuotedString) == "C:\\a b c\\123.xyz");
    CPPUNIT_ASSERT(ok);
}
