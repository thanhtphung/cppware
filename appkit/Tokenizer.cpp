/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/Utf8.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/S32.hpp"
#include "appkit/StringVec.hpp"
#include "appkit/Tokenizer.hpp"

using namespace syskit;

BEGIN_NAMESPACE1(appkit)

const char Tokenizer::WHITE_SPACE[] = " \t\n\r\v\f"; //isspace()


//!
//! Construct a tokenizer with an empty tokenizee. Use WHITE_SPACE
//! when tokenizing.
//!
Tokenizer::Tokenizer():
tokenizee_()
{
    refresh();
    setDelim(WHITE_SPACE);
}


//!
//! Construct a tokenizer with the given tokenizee. Make a
//! shallow copy of the given token delimiters and use them
//! when tokenizing.
//!
Tokenizer::Tokenizer(const String& tokenizee, const char* delim):
tokenizee_(tokenizee)
{
    refresh();
    setDelim(delim);
}


//!
//! Construct a duplicate instance of the given tokenizer.
//!
Tokenizer::Tokenizer(const Tokenizer& tokenizer):
tokenizee_(tokenizer.tokenizee_)
{
    refresh();
    delim_ = tokenizer.delim_;
    get_ = tokenizer.get_;
    move_ = tokenizer.move_;
    peek_ = tokenizer.peek_;
    if (tokenizer.pCur_ != 0)
    {
        pCur_ = pStart_ + (tokenizer.pCur_ - tokenizer.pStart_);
    }
}


//!
//! Construct a tokenizer with the given tokenizee. Make a
//! shallow copy of the given token delimiters and use them
//! when tokenizing. If the length of the tokenizee is known,
//! the Tokenizer(const char*, size_t, const char*) constructor
//! should be used.
//!
Tokenizer::Tokenizer(const char* tokenizee, const char* delim):
tokenizee_(tokenizee)
{
    refresh();
    setDelim(delim);
}


//!
//! Construct a tokenizer with length characters from given
//! tokenizee. Make a shallow copy of the given token delimiters
//! and use them when tokenizing.
//!
Tokenizer::Tokenizer(const char* tokenizee, size_t length, const char* delim):
tokenizee_(tokenizee, length)
{
    refresh();
    setDelim(delim);
}


Tokenizer::~Tokenizer()
{
}


const Tokenizer& Tokenizer::operator =(const Tokenizer& tokenizer)
{

    // Prevent self assignment.
    if (this != &tokenizer)
    {
        tokenizee_ = tokenizer.tokenizee_;
        refresh();
        delim_ = tokenizer.delim_;
        get_ = tokenizer.get_;
        move_ = tokenizer.move_;
        peek_ = tokenizer.peek_;
        if (tokenizer.pCur_ != 0)
        {
            pCur_ = pStart_ + (tokenizer.pCur_ - tokenizer.pStart_);
        }
    }

    // Return reference to self.
    return *this;
}


//!
//! Iterate the tokens left to right. Invoke callback at each token.
//! The callback should return true to continue iterating and should
//! return false to abort iterating. Return false if the callback
//! aborted the iterating. Return true otherwise.
//!
bool Tokenizer::apply(cb0_t cb, void* arg) const
{

    // Iterate the tokens from left to right.
    bool ok = true;
    if (!tokenizee_.empty())
    {
        const char* p = pStart_;
        for (String token; (this->*get_)(p, token);)
        {
            if (!cb(arg, token))
            {
                ok = false;
                break;
            }
        }
    }

    // Return true if the iterating was not aborted.
    return ok;
}


//
// If there are no tokens on the right starting at p, return false and make token
// an empty string. Otherwise, return true with the right token in token and set
// p to the next character beyond the token. White-space delimiters. ASCII
// tokenizee.
//
bool Tokenizer::doGet(const char*& p, String& token) const
{
    bool ok;
    const char* p1;
    for (p1 = p; S32::isSpace(*p1); ++p1);
    if (*p1 != 0)
    {
        ok = true;
        const char* p2 = p1;
        for (++p2; (*p2 != 0) && (!S32::isSpace(*p2)); ++p2);
        token.reset(p1, p2 - p1);
        p = p2;
    }
    else
    {
        ok = false;
        token.reset();
    }

    return ok;
}


//
// If there are no tokens on the right starting at p, return false and make token
// an empty string. Otherwise, return true with the right token in token and set
// p to the next character beyond the token. Single-character delimiters. ASCII
// tokenizee.
//
bool Tokenizer::doGet1(const char*& p, String& token) const
{
    bool ok;
    const char delim = delim_[0];
    const char* p1;
    for (p1 = p; *p1 == delim; ++p1);
    if (*p1 != 0)
    {
        ok = true;
        const char* p2 = p1;
        for (++p2; (*p2 != 0) && (*p2 != delim); ++p2);
        token.reset(p1, p2 - p1);
        p = p2;
    }
    else
    {
        ok = false;
        token.reset();
    }

    return ok;
}


//
// doGet1() for non-ASCII tokenizee.
//
bool Tokenizer::doGet1U(const char*& p, String& token) const
{
    Utf8 c;
    const utf32_t delim = delim_[0];
    const utf8_t* p1 = reinterpret_cast<const utf8_t*>(p);
    size_t seqLength = 0;
    do
    {
        p1 += seqLength;
        seqLength = c.decode(p1);
    } while (c.asU32() == delim);

    bool ok;
    if (c.asU32() != 0)
    {
        ok = true;
        const utf8_t* p2 = p1;
        size_t numChars = 0U - 1;
        seqLength = 0;
        do
        {
            ++numChars;
            p2 += seqLength;
            seqLength = c.decode(p2);
        } while ((c.asU32() != 0) && (c.asU32() != delim));
        token.reset(p1, p2 - p1, numChars);
        p = reinterpret_cast<const char*>(p2);
    }

    else
    {
        ok = false;
        token.reset();
    }

    return ok;
}


//
// doGet() for non-ASCII tokenizee.
//
bool Tokenizer::doGetU(const char*& p, String& token) const
{
    Utf8 c;
    const utf8_t* p1 = reinterpret_cast<const utf8_t*>(p);
    size_t seqLength = 0;
    do
    {
        p1 += seqLength;
        seqLength = c.decode(p1);
    } while ((seqLength == 1) && S32::isSpace(c.asU32()));

    bool ok;
    if (c.asU32() != 0)
    {
        ok = true;
        const utf8_t* p2 = p1;
        size_t numChars = 0U - 1;
        seqLength = 0;
        do
        {
            ++numChars;
            p2 += seqLength;
            seqLength = c.decode(p2);
        } while ((c.asU32() != 0) && ((seqLength > 1) || (!S32::isSpace(c.asU32()))));
        token.reset(p1, p2 - p1, numChars);
        p = reinterpret_cast<const char*>(p2);
    }

    else
    {
        ok = false;
        token.reset();
    }

    return ok;
}


//
// If there are no tokens on the right starting at p, return false and make token
// an empty string. Otherwise, return true with the right token in token and set
// p to the next character beyond the token. Flexible delimiters.
//
bool Tokenizer::doGetX(const char*& p, String& token) const
{
    bool ok;
    p += strspn(p, delim_);
    if (p < pEnd_)
    {
        ok = true;
        const char* p2 = p + strcspn(p, delim_);
        if (tokenizee_.isAscii())
        {
            token.reset(p, p2 - p);
        }
        else
        {
            token.reset8(reinterpret_cast<const utf8_t*>(p), p2 - p);
        }
        p = p2;
    }
    else
    {
        ok = false;
        token.reset();
    }

    return ok;
}


//
// If there are no tokens on the right starting at p, return false.
// Otherwise, return true and set p to the next character beyond
// the token. White-space delimiters. ASCII tokenizee.
//
bool Tokenizer::doMove(const char*& p) const
{
    bool ok;
    const char* p1;
    for (p1 = p; S32::isSpace(*p1); ++p1);
    if (*p1 != 0)
    {
        ok = true;
        for (++p1; (*p1 != 0) && (!S32::isSpace(*p1)); ++p1);
        p = p1;
    }
    else
    {
        ok = false;
    }

    return ok;
}


//
// If there are no tokens on the right starting at p, return false.
// Otherwise, return true and set p to the next character beyond
// the token. Single-character delimiters.
//
bool Tokenizer::doMove1(const char*& p) const
{
    bool ok;
    const char delim = delim_[0];
    const char* p1;
    for (p1 = p; *p1 == delim; ++p1);
    if (*p1 != 0)
    {
        ok = true;
        for (++p1; (*p1 != 0) && (*p1 != delim); ++p1);
        p = p1;
    }
    else
    {
        ok = false;
    }

    return ok;
}


//
// doMove() for non-ASCII tokenizee.
//
bool Tokenizer::doMoveU(const char*& p) const
{
    Utf8 c;
    const utf8_t* p1 = reinterpret_cast<const utf8_t*>(p);
    size_t seqLength = 0;
    do
    {
        p1 += seqLength;
        seqLength = c.decode(p1);
    } while ((seqLength == 1) && S32::isSpace(c.asU32()));

    bool ok;
    if (c.asU32() != 0)
    {
        ok = true;
        const utf8_t* p2 = p1;
        seqLength = 0;
        do
        {
            p2 += seqLength;
            seqLength = c.decode(p2);
        } while ((c.asU32() != 0) && ((seqLength > 1) || (!S32::isSpace(c.asU32()))));
        p = reinterpret_cast<const char*>(p2);
    }

    else
    {
        ok = false;
    }

    return ok;
}


//
// If there are no tokens on the right starting at p, return false.
// Otherwise, return true and set p to the next character beyond
// the token. Flexible delimiters.
//
bool Tokenizer::doMoveX(const char*& p) const
{
    bool ok;
    p += strspn(p, delim_);
    if (p < pEnd_)
    {
        ok = true;
        p += strcspn(p, delim_);
    }
    else
    {
        ok = false;
    }

    return ok;
}


//
// Return true if there's a token to the right.
// White-space delimiters.
//
bool Tokenizer::doPeek() const
{
    const char* p = (pCur_ == 0)? pStart_: pCur_;
    for (; S32::isSpace(*p); ++p);
    return (*p != 0);
}


//
// Return true if there's a token to the right.
// Single-character delimiters.
//
bool Tokenizer::doPeek1() const
{
    const char* p = (pCur_ == 0)? pStart_: pCur_;
    const char delim = delim_[0];
    for (; *p == delim; ++p);
    return (*p != 0);
}


//
// Return true if there's a token to the right.
// Flexible delimiters.
//
bool Tokenizer::doPeekX() const
{
    const char* p = (pCur_ == 0)? pStart_: pCur_;
    return ((p + strspn(p, delim_)) < pEnd_);
}


bool Tokenizer::sameAsWhiteSpace(const char* delim)
{

    // See if delimiters contain white space characters only.
    bool answer = false;
    const char* p;
    for (p = delim; S32::isSpace(*p); ++p);

    // See if delimiters contain all white space characters.
    if (*p == 0)
    {
        p = WHITE_SPACE;
        while (strchr(delim, *p) != 0)
        {
            ++p;
            if (*p == 0)
            {
                answer = true;
                break;
            }
        }
    }

    // Return true if delimiters cover white space and only white space.
    return answer;
}


//!
//! Look at tokenizee as a vector of tokens. Return true if successful. Return
//! false otherwise (result is partial due to insufficient capacity in provided
//! vector).
//!
bool Tokenizer::vectorize(StringVec& vec) const
{

    // Iterate the tokens from left to right.
    vec.reset();
    bool ok = true;
    if (!tokenizee_.empty())
    {
        const char* p = pStart_;
        for (String token; (this->*get_)(p, token);)
        {
            if (!vec.add(token))
            {
                ok = false;
                break;
            }
        }
    }

    // Return true if successful.
    return ok;
}


//!
//! Return the total number of tokens in the tokenizee.
//!
unsigned int Tokenizer::countTokens() const
{
    unsigned int numTokens = 0;
    for (const char* p = pStart_; (this->*move_)(p); ++numTokens);
    return numTokens;
}


//!
//! Iterate the tokens from left to right.
//! Invoke callback at each token.
//!
void Tokenizer::apply(cb1_t cb, void* arg) const
{

    // Iterate the tokens from left to right.
    if (!tokenizee_.empty())
    {
        const char* p = pStart_;
        for (String token; (this->*get_)(p, token);)
        {
            cb(arg, token);
        }
    }
}


void Tokenizer::refresh()
{
    pCur_ = 0;
    pStart_ = tokenizee_.ascii();
    pEnd_ = pStart_ + tokenizee_.byteSize() - 1;
}


//!
//! Make a shallow copy of the given token delimiters and use them when tokenizing.
//!
void Tokenizer::setDelim(const char* delim)
{

    // Assume white-space delimiters.
    delim_ = WHITE_SPACE;
    if (tokenizee_.isAscii())
    {
        get_ = &Tokenizer::doGet;
        move_ = &Tokenizer::doMove;
    }
    else
    {
        get_ = &Tokenizer::doGetU;
        move_ = &Tokenizer::doMoveU;
    }
    peek_ = &Tokenizer::doPeek;

    // Check most likely cases first.
    // Give the white-space delimiters one more chance at the end.
    if ((delim == WHITE_SPACE) || (delim == 0) || (delim[0] == 0))
    {
    }
    else if (delim[1] == 0)
    {
        delim_ = delim;
        get_ = tokenizee_.isAscii()? &Tokenizer::doGet1: &Tokenizer::doGet1U;
        move_ = &Tokenizer::doMove1;
        peek_ = &Tokenizer::doPeek1;
    }
    else if (!sameAsWhiteSpace(delim))
    {
        delim_ = delim;
        get_ = &Tokenizer::doGetX;
        move_ = &Tokenizer::doMoveX;
        peek_ = &Tokenizer::doPeekX;
    }
}

END_NAMESPACE1
