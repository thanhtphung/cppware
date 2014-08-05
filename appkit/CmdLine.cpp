/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <cstdlib>
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/CmdLine.hpp"
#include "appkit/Path.hpp"
#include "appkit/S32.hpp"
#include "appkit/Str.hpp"
#include "appkit/Tokenizer.hpp"

const bool IGNORE_CASE = true;
const char EQUALS_SIGN = '=';
const char HYPHEN = '-';
const char TOKEN_DELIM[] = "\001";
const int ARG_GROWTH = -1;
const unsigned int ARG_CAP = 8;

using namespace syskit;

BEGIN_NAMESPACE1(appkit)


//!
//! Construct a duplicate instance of the given command line.
//!
CmdLine::CmdLine(const CmdLine& cmdLine):
cmd_(cmdLine.cmd_),
optDic_(cmdLine.optDic_),
argVec_(cmdLine.argVec_)
{
    tag_ = cmdLine.tag_;
}


//!
//! Construct instance with given command line.
//!
CmdLine::CmdLine(const String& cmd):
cmd_(cmd),
optDic_(IGNORE_CASE),
argVec_(ARG_CAP, ARG_GROWTH)
{

    // Parse the command to form the argument vector and the option map.
    tag_ = 0;
    parseCmd();
}


//!
//! Construct instance with given command line.
//!
CmdLine::CmdLine(const char* cmd):
cmd_(cmd),
optDic_(IGNORE_CASE),
argVec_(ARG_CAP, ARG_GROWTH)
{

    // Parse the command to form the argument vector and the option map.
    tag_ = 0;
    parseCmd();
}


//!
//! Construct instance with length characters from given command line.
//!
CmdLine::CmdLine(const char* cmd, size_t length):
cmd_(cmd, length),
optDic_(IGNORE_CASE),
argVec_(ARG_CAP, ARG_GROWTH)
{

    // Parse the command to form the argument vector and the option map.
    tag_ = 0;
    parseCmd();
}


CmdLine::~CmdLine()
{
}


bool CmdLine::operator ==(const CmdLine& cmdLine) const
{
    bool eq = (cmd_ == cmdLine.cmd_);
    return eq;
}


const CmdLine& CmdLine::operator =(const CmdLine& cmdLine)
{

    // Prevent self assignment.
    if (this != &cmdLine)
    {
        argVec_ = cmdLine.argVec_;
        optDic_ = cmdLine.optDic_;
        cmd_ = cmdLine.cmd_;
        tag_ = cmdLine.tag_;
    }

    // Return reference to self.
    return *this;
}


//
// Given token might be an option to be added to the option map. Treat
// option value as a C++ string literal and removing surrounding quotes
// (if any) in addition to resolving escape sequences. If the option
// does not have a value, use an empty string as the value. Return true
// if given token is an option specification.
//
bool CmdLine::parseOpt(const String& nv)
{

    // An option must start with a hyphen.
    const char* opt = nv.ascii();
    if (opt[0] != HYPHEN)
    {
        bool ok = false;
        return ok;
    }

    // Skip single or double hyphen.
    const char* optN = opt + 1;
    if (opt[1] == HYPHEN)
    {
        ++optN;
    }

    // Option has no values.
    const char* pEqual = strchr(optN, EQUALS_SIGN);
    size_t numHyphens = optN - opt;
    String n;
    String v;
    if (pEqual == 0)
    {
        n = String(nv, numHyphens, nv.length() - numHyphens);
    }

    // Option has some value.
    // Treat option value as a C++ string literal.
    else
    {
        size_t nLength = pEqual - optN;
        const char* optV = pEqual + 1;
        size_t vLength = nv.byteSize() - nLength - numHyphens - 2;
        if (nv.isAscii())
        {
            n.reset(optN, nLength);
            v.reset(optV, vLength);
        }
        else
        {
            n.reset8(reinterpret_cast<const utf8_t*>(optN), nLength);
            v.reset8(reinterpret_cast<const utf8_t*>(optV), vLength);
        }
        bool unescapeSingleQuotedString = false;
        v.dequote(unescapeSingleQuotedString);
    }

    bool ok = n.empty()? (false): (optDic_.associate(n, v), true);
    return ok;
}


//
// Replace white space in given null-terminated string with the normalized
// delimiter unless they're inside some quotes. Besides single quotes and
// double quotes, various forms of parentheses (i.e., "()", "[]", "{}", "<>",
// and "||") are also considered quotes. That is, white space inside single
// quotes, double quotes, and parentheses are left alone. Nested quotes are
// not handled.
//
void CmdLine::markWhiteSpace(utf8_t* s)
{
    char quote = 0;
    for (; *s; ++s)
    {

        // In quotes.
        if (quote != 0)
        {
            if (*s == quote)
            {
                quote = 0;
            }
        }

        // Not in quotes.
        else
        {
            if (S32::isSpace(*s))
            {
                *s = TOKEN_DELIM[0];
            }
            else
            {
                switch (*s)
                {
                case '(': quote = ')'; break;
                case '[': quote = ']'; break;
                case '{': quote = '}'; break;
                case '<': quote = '>'; break;
                case '"':
                case '\'':
                case '|': quote = *s; break;
                default: break;
                }
            }
        }
    }
}


//
// Given token is an argument to be added to the argument vector. Treat
// argument as a C++ string literal and remove surrounding quotes (if
// any) in addition to resolving escape sequences.
//
void CmdLine::parseArg(const String& arg)
{
    String argV(arg);
    bool unescapeSingleQuotedString = false;
    argV.dequote(unescapeSingleQuotedString);
    argVec_.add(argV);
}


//
// Parse the command to form the argument vector and the option map.
//
void CmdLine::parseCmd()
{

    // Make copy of command line for parsing.
    size_t byteSize = cmd_.byteSize();
    utf8_t* s = new utf8_t[byteSize];
    memcpy(s, cmd_.raw(), byteSize);

    // Treat first token as argument 0 (aka the command name).
    markWhiteSpace(s);
    String tokenizee;
    tokenizee.reset(s, cmd_.byteSize() - 1, cmd_.length());
    Tokenizer tokenizer(tokenizee, TOKEN_DELIM);
    String token;
    if (tokenizer.next(token))
    {

        // Parse remaining tokens for options and arguments.
        Path arg0(token);
        parseArg(arg0.asString());
        while (tokenizer.next(token))
        {

            // Parse this option for name and value and update the option map.
            // As soon as argument 1 is found, there are no more options.
            if (!parseOpt(token))
            {
                do
                {
                    parseArg(token);
                } while (tokenizer.next(token));
                break;
            }
        }
    }

    // Destroy command line copy.
    delete[] s;
}


void CmdLine::refresh()
{
    argVec_.reset();
    optDic_.reset();
    parseCmd();
}

END_NAMESPACE1
