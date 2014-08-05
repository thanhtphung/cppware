/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_TOKENIZER_HPP
#define APPKIT_TOKENIZER_HPP

#include "appkit/String.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)

class StringVec;


//! string tokenizer
class Tokenizer
    //!
    //! A class representing a string tokenizer. A tokenizee is a string
    //! of tokens to be tokenized. Tokens are delimited by delimiters.
    //! Given a tokenizee and the delimiter(s), a tokenizer can identify
    //! the tokens. To iterate, use the next() and/or apply() methods.
    //! Peeking is supported. Example:
    //!\code
    //! Tokenizer tokenizer("a tokenizee is a string of tokens to be tokenized");
    //! String token;
    //! while (tokenizer.next(token))
    //! {
    //!   //do something with each token
    //! }
    //!\endcode
    //!
{

public:
    typedef bool(*cb0_t)(void* arg, const String& token);
    typedef void(*cb1_t)(void* arg, const String& token);

    static const char WHITE_SPACE[];

    // Constructors and destructor.
    Tokenizer();
    Tokenizer(const String& tokenizee, const char* delim = WHITE_SPACE);
    Tokenizer(const Tokenizer& tokenizer);
    Tokenizer(const char* tokenizee, const char* delim = WHITE_SPACE);
    Tokenizer(const char* tokenizee, size_t length, const char* delim = WHITE_SPACE);
    ~Tokenizer();

    // Operators.
    const Tokenizer& operator =(const Tokenizer& tokenizer);

    // Iterator support.
    String remainingTokens() const;
    bool apply(cb0_t cb, void* arg = 0) const;
    bool next(String& token);
    bool peek() const;
    bool peek(String& token) const;
    bool vectorize(StringVec& vec) const;
    void apply(cb1_t cb, void* arg = 0) const;
    void reset();

    // Utilities.
    const String& tokenizee() const;
    const char* delim() const;
    unsigned int countTokens() const;
    void setDelim(const char* delim);
    void setTokenizee(const String& tokenizee);
    void setTokenizee(const char* tokenizee);
    void setTokenizee(const char* tokenizee, size_t length);

private:
    typedef bool (Tokenizer::*get_t)(const char*&, String&) const;
    typedef bool (Tokenizer::*move_t)(const char*&) const;
    typedef bool (Tokenizer::*peek_t)() const;

    String tokenizee_;
    const char* delim_;
    const char* pCur_;
    const char* pEnd_;
    const char* pStart_;
    get_t get_;
    move_t move_;
    peek_t peek_;

    bool doGet(const char*&, String&) const;
    bool doGet1(const char*&, String&) const;
    bool doGet1U(const char*&, String&) const;
    bool doGetU(const char*&, String&) const;
    bool doGetX(const char*&, String&) const;
    bool doMove(const char*&) const;
    bool doMove1(const char*&) const;
    bool doMoveU(const char*&) const;
    bool doMoveX(const char*&) const;
    bool doPeek() const;
    bool doPeek1() const;
    bool doPeekX() const;
    void refresh();

    static bool sameAsWhiteSpace(const char*);

};

//! Peek at remaining tokens. Return the remaining unparsed part of the
//! tokenizee. The first invocation after construction or reset will return
//! the tokenizee.
inline String Tokenizer::remainingTokens() const
{
    const char* p = (pCur_ == 0)? pStart_: pCur_;
    return p;
}

//! Retrieve the next token. Return true if there's one. Otherwise,
//! return false and an empty token value (if tokenizee is empty or
//! if there's no more tokens). The first invocation after construction
//! or reset will return the left-most token.
inline bool Tokenizer::next(String& token)
{
    if (pCur_ == 0) pCur_ = pStart_;
    bool ok = (this->*get_)(pCur_, token);
    return ok;
}

//! Peek for more tokens and return true if there's one.
inline bool Tokenizer::peek() const
{
    return (this->*peek_)();
}

//! Peek at the next token. Return true if there's one. Otherwise, return
//! false and an empty token value (if tokenizee is empty or if there's no
//! more tokens). The first invocation after construction or reset will
//! return the left-most token.
inline bool Tokenizer::peek(String& token) const
{
    const char* p = (pCur_ == 0)? pStart_: pCur_;
    bool ok = (this->*get_)(p, token);
    return ok;
}

//! Return the tokenizee.
inline const String& Tokenizer::tokenizee() const
{
    return tokenizee_;
}

//! Return the token delimiters.
inline const char* Tokenizer::delim() const
{
    return delim_;
}

//! Reset tokenizer using the same tokenizee. That is, next() will return
//! the left-most token.
inline void Tokenizer::reset()
{
    pCur_ = 0;
}

//! Reset tokenizer with given tokenizee.
inline void Tokenizer::setTokenizee(const String& tokenizee)
{
    tokenizee_ = tokenizee;
    refresh();
}

//! Reset tokenizer with given tokenizee. If the length of the tokenizee
//! is known, the setTokenizee(const char*, size_t) method should be used.
inline void Tokenizer::setTokenizee(const char* tokenizee)
{
    tokenizee_ = tokenizee;
    refresh();
}

//! Reset tokenizer with length characters from given tokenizee.
inline void Tokenizer::setTokenizee(const char* tokenizee, size_t length)
{
    tokenizee_.reset(tokenizee, length);
    refresh();
}

END_NAMESPACE1

#endif
