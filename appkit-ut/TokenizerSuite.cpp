#include "appkit/StringVec.hpp"
#include "appkit/Tokenizer.hpp"
#include "syskit/Utf8Seq.hpp"

#include "appkit-ut-pch.h"
#include "TokenizerSuite.hpp"

using namespace appkit;
using namespace syskit;

const char* const DELIM0 = " ";
const char* const DELIM1 = "+-*/.";
const char* const MY_WHITE_SPACE = "\n\r \t\n\r\t \t \v\f";
const char* const TOKENIZEE0 = "1 22 333 4444 55555 666666 7777777 88888888 999999999";
const char* const TOKENIZEE1 = "999999999+88888888-7777777*666666/55555.4444.333.22.1";
const size_t NUM_CHARS = 4;

const utf32_t SAMPLE[NUM_CHARS] =
{
    0x0000abcdU, //3-byte value
    0x00000001U, //1-byte value
    0x000000abU, //2-byte value
    0x000abcdeU  //4-byte value
};


TokenizerSuite::TokenizerSuite()
{
}


TokenizerSuite::~TokenizerSuite()
{
}


bool TokenizerSuite::cb0a(void* arg, const String& token)
{
    unsigned int& i = *static_cast<unsigned int*>(arg);
    --i;
    bool ok = (token.length() == i);
    CPPUNIT_ASSERT(ok);

    ok = (token == String(i, static_cast<char>('0' + i)));
    CPPUNIT_ASSERT(ok);
    return true;
}


bool TokenizerSuite::cb0b(void* arg, const String& token)
{
    unsigned int& i = *static_cast<unsigned int*>(arg);
    --i;
    bool ok = (token.length() == i);
    CPPUNIT_ASSERT(ok);

    ok = (token == String(i, static_cast<char>('0' + i)));
    CPPUNIT_ASSERT(ok);
    return (i > 3);
}


bool TokenizerSuite::cb0c(void* arg, const String& token)
{
    unsigned int& i = *static_cast<unsigned int*>(arg);
    ++i;
    bool ok = (token.length() == i);
    CPPUNIT_ASSERT(ok);

    ok = (token == String(i, static_cast<char>('0' + i)));
    CPPUNIT_ASSERT(ok);
    return true;
}


bool TokenizerSuite::cb0d(void* arg, const String& token)
{
    unsigned int& i = *static_cast<unsigned int*>(arg);
    ++i;
    bool ok = (token.length() == i);
    CPPUNIT_ASSERT(ok);

    ok = (token == String(i, static_cast<char>('0' + i)));
    CPPUNIT_ASSERT(ok);
    return (i <= 3);
}


void TokenizerSuite::cb1a(void* arg, const String& token)
{
    unsigned int& i = *static_cast<unsigned int*>(arg);
    --i;
    bool ok = (token.length() == i);
    CPPUNIT_ASSERT(ok);

    ok = (token == String(i, static_cast<char>('0' + i)));
    CPPUNIT_ASSERT(ok);
}


void TokenizerSuite::cb1b(void* arg, const String& token)
{
    unsigned int& i = *static_cast<unsigned int*>(arg);
    ++i;
    bool ok = (token.length() == i);
    CPPUNIT_ASSERT(ok);

    ok = (token == String(i, static_cast<char>('0' + i)));
    CPPUNIT_ASSERT(ok);
}


//
// Apply callback to each token. Iterate left to right.
//
void TokenizerSuite::testApply00()
{
    Tokenizer tokenizer0(TOKENIZEE0, Tokenizer::WHITE_SPACE);
    Tokenizer tokenizer1(TOKENIZEE1);
    tokenizer1.setDelim(DELIM1);

    unsigned int i = 0;
    bool ok = tokenizer0.apply(cb0c, &i);
    CPPUNIT_ASSERT(ok);

    i = 0;
    ok = ((!tokenizer0.apply(cb0d, &i)) && (i == 4));
    CPPUNIT_ASSERT(ok);

    i = 0;
    tokenizer0.apply(cb1b, &i);

    i = 10;
    ok = tokenizer1.apply(cb0a, &i);
    CPPUNIT_ASSERT(ok);

    i = 10;
    ok = ((!tokenizer1.apply(cb0b, &i)) && (i == 3));
    CPPUNIT_ASSERT(ok);

    i = 10;
    tokenizer1.apply(cb1a, &i);
}


void TokenizerSuite::testCountTokens00()
{
    Tokenizer tokenizer0(TOKENIZEE0, Tokenizer::WHITE_SPACE);
    Tokenizer tokenizer1(TOKENIZEE1);
    tokenizer1.setDelim(DELIM1);
    Tokenizer tokenizer2(TOKENIZEE0, DELIM0);

    Tokenizer tokenizer;
    bool ok = (tokenizer.countTokens() == 0);
    CPPUNIT_ASSERT(ok);
    tokenizer.setTokenizee(Tokenizer::WHITE_SPACE);
    ok = (tokenizer.countTokens() == 0);
    CPPUNIT_ASSERT(ok);

    String tokenizee("abc");
    tokenizer.setTokenizee(tokenizee);
    ok = (tokenizer.countTokens() == 1);
    CPPUNIT_ASSERT(ok);
    tokenizer.setTokenizee(" abc 12345", 5);
    ok = (tokenizer.countTokens() == 1);
    CPPUNIT_ASSERT(ok);
    tokenizer.setTokenizee("\t abc\n ");
    ok = (tokenizer.countTokens() == 1);
    CPPUNIT_ASSERT(ok);

    ok = (tokenizer0.countTokens() == 9);
    CPPUNIT_ASSERT(ok);
    ok = (tokenizer1.countTokens() == 9);
    CPPUNIT_ASSERT(ok);
    ok = (tokenizer2.countTokens() == 9);
    CPPUNIT_ASSERT(ok);
}


void TokenizerSuite::testCtor00()
{
    Tokenizer tokenizer;
    bool ok = (tokenizer.delim() == Tokenizer::WHITE_SPACE);
    CPPUNIT_ASSERT(ok);

    ok = (tokenizer.tokenizee().empty());
    CPPUNIT_ASSERT(ok);
}


void TokenizerSuite::testCtor01()
{
    Tokenizer tokenizer0(TOKENIZEE0, Tokenizer::WHITE_SPACE);

    String token;
    tokenizer0.next(token);
    tokenizer0.next(token);
    Tokenizer tokenizer(tokenizer0);

    bool ok = (tokenizer.next(token) && (token == "333"));
    CPPUNIT_ASSERT(ok);

    ok = (tokenizer.peek() && tokenizer.peek(token) && (token == "4444"));
    CPPUNIT_ASSERT(ok);
}


void TokenizerSuite::testCtor02()
{
    String tokenizee(TOKENIZEE0);
    Tokenizer tokenizer(tokenizee);
    bool ok = (tokenizer.tokenizee() == tokenizee);
    CPPUNIT_ASSERT(ok);
}


void TokenizerSuite::testCtor03()
{
    size_t length = strlen(TOKENIZEE1);
    Tokenizer tokenizer(TOKENIZEE1, length);
    bool ok = (tokenizer.tokenizee() == TOKENIZEE1);
    CPPUNIT_ASSERT(ok);
}


void TokenizerSuite::testCtor04()
{
    Utf8Seq seq;
    seq.shrink(SAMPLE, NUM_CHARS);
    String s8(seq);

    const char* delim[3] = {DELIM0, DELIM1, MY_WHITE_SPACE};
    for (int i = 0; i < 3; ++i)
    {
        String tokenizee(s8);
        tokenizee += delim[i];
        String s("token");
        tokenizee += s;
        tokenizee += delim[i];
        tokenizee += delim[i];
        tokenizee += delim[i];
        tokenizee += s8;

        Tokenizer tokenizer(tokenizee, delim[i]);
        String token;
        bool ok = tokenizer.next(token) && (token == s8);
        CPPUNIT_ASSERT(ok);
        ok = tokenizer.next(token) && (token == s);
        CPPUNIT_ASSERT(ok);
        ok = tokenizer.next(token) && (token == s8);
        CPPUNIT_ASSERT(ok);
        ok = (!tokenizer.next(token));
        CPPUNIT_ASSERT(ok);
        ok = (tokenizer.countTokens() == 3);
        CPPUNIT_ASSERT(ok);

        String cur;
        String old;
        tokenizer.reset();
        for (tokenizer.peek(old); tokenizer.next(cur); tokenizer.peek(old))
        {
            if (cur != old)
            {
                ok = false;
                break;
            }
        }
        CPPUNIT_ASSERT(ok);
    }
}


void TokenizerSuite::testNext00()
{
    Tokenizer tokenizer0(TOKENIZEE0, Tokenizer::WHITE_SPACE);

    unsigned int i = 0;
    String token;
    bool ok = true;
    while (tokenizer0.next(token))
    {
        ++i;
        if ((token.length() != i) || (token != String(i, static_cast<char>('0' + i))))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = (token.empty() && (i == 9) && (!tokenizer0.next(token)));
    CPPUNIT_ASSERT(ok);
}


void TokenizerSuite::testNext01()
{
    Tokenizer tokenizer1(TOKENIZEE1);
    tokenizer1.setDelim(DELIM1);

    unsigned int i = 10;
    String token;
    bool ok = true;
    while (tokenizer1.next(token))
    {
        --i;
        if ((token.length() != i) || (token != String(i, static_cast<char>('0' + i))))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = (token.empty() && (i == 1) && (!tokenizer1.next(token)));
    CPPUNIT_ASSERT(ok);
}


void TokenizerSuite::testOp00()
{
    Tokenizer tokenizer0(TOKENIZEE0, Tokenizer::WHITE_SPACE);
    Tokenizer tokenizer1(TOKENIZEE1);
    tokenizer1.setDelim(DELIM1);

    String token;
    tokenizer0.next(token);
    tokenizer0.next(token);
    tokenizer1 = tokenizer0;

    bool ok = (tokenizer1.next(token) && (token == "333"));
    CPPUNIT_ASSERT(ok);

    ok = (tokenizer1.peek(token) && (token == "4444"));
    CPPUNIT_ASSERT(ok);
}


void TokenizerSuite::testPeek00()
{
    Tokenizer tokenizer0(TOKENIZEE0, Tokenizer::WHITE_SPACE);
    Tokenizer tokenizer1(TOKENIZEE1);
    tokenizer1.setDelim(DELIM1);
    Tokenizer tokenizer2(TOKENIZEE0, DELIM0);

    String token;
    bool ok = (tokenizer0.peek() && tokenizer0.peek(token) && (token == "1"));
    CPPUNIT_ASSERT(ok);

    ok = (tokenizer1.peek() && tokenizer1.peek(token) && (token == "999999999"));
    CPPUNIT_ASSERT(ok);

    ok = (tokenizer2.peek() && tokenizer2.peek(token) && (token == "1"));
    CPPUNIT_ASSERT(ok);
}


//
// Iterate left to right. Make sure we can look ahead to the right.
//
void TokenizerSuite::testPeek01()
{
    Tokenizer tokenizer1(TOKENIZEE1);
    tokenizer1.setDelim(DELIM1);

    String cur;
    String old;
    bool ok = true;
    for (tokenizer1.peek(old); tokenizer1.next(cur); tokenizer1.peek(old))
    {
        if (cur != old)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    Tokenizer tokenizer(TOKENIZEE0);
    ok = (tokenizer.remainingTokens() == TOKENIZEE0);
    CPPUNIT_ASSERT(ok);
    tokenizer.next(cur);
    tokenizer.next(cur);
    tokenizer.next(cur);
    ok = (tokenizer.remainingTokens() == " 4444 55555 666666 7777777 88888888 999999999");
    CPPUNIT_ASSERT(ok);
}


void TokenizerSuite::testSetDelim00()
{
    Tokenizer tokenizer;
    tokenizer.setDelim(0);
    bool ok = (tokenizer.delim() == Tokenizer::WHITE_SPACE);
    CPPUNIT_ASSERT(ok);

    tokenizer.setDelim("");
    ok = (tokenizer.delim() == Tokenizer::WHITE_SPACE);
    CPPUNIT_ASSERT(ok);

    tokenizer.setDelim(MY_WHITE_SPACE);
    ok = (tokenizer.delim() == Tokenizer::WHITE_SPACE);
    CPPUNIT_ASSERT(ok);
}


void TokenizerSuite::testVectorize00()
{
    Tokenizer tokenizer(TOKENIZEE0, Tokenizer::WHITE_SPACE);
    StringVec tokens(1 /*capacity*/, 0 /*growBy*/);
    bool ok = (!tokenizer.vectorize(tokens)) && (tokens[0] == "1") && (tokens.numItems() == 1);
    CPPUNIT_ASSERT(ok);

    int growBy = -1;
    tokens.setGrowth(growBy);
    ok = tokenizer.vectorize(tokens) && (tokens.stringify(" ") == tokenizer.tokenizee());
    CPPUNIT_ASSERT(ok);
}
