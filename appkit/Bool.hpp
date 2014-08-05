/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_BOOL_HPP
#define APPKIT_BOOL_HPP

#include "appkit/String.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)


//! boolean value
class Bool
    //!
    //! A class representing a boolean value.
    //!
{

public:
    typedef bool item_t;

    Bool(const Bool& item);
    Bool(const String& s);
    Bool(const String* s, item_t defaultV = false);
    Bool(const char* s);
    Bool(item_t v = false);

    operator String() const;
    operator item_t() const;
    const Bool& operator =(const Bool& item);
    const Bool& operator =(item_t v);

    String toString() const;
    item_t asBool() const;

private:
    item_t v_;

    static item_t toBool(const char*);

};

inline Bool::Bool(const Bool& item)
{
    v_ = item.v_;
}

//! Construct instance from given valid string.
inline Bool::Bool(const String& s)
{
    v_ = toBool(s.ascii());
}

inline Bool::Bool(const String* s, item_t defaultV)
{
    // An empty string is considered "true".
    v_ = (s == 0)? defaultV: toBool(s->ascii());
}

//! Construct instance from given valid string.
inline Bool::Bool(const char* s)
{
    v_ = toBool(s);
}

inline Bool::Bool(item_t v)
{
    v_ = v;
}

//! Return instance as a string ("true" or "false").
inline Bool::operator String() const
{
    const char* s = v_? "true": "false";
    return String(s);
}

//! Return instance as a boolean value.
inline Bool::operator item_t() const
{
    return v_;
}

inline const Bool& Bool::operator =(const Bool& item)
{
    v_ = item.v_;
    return *this;
}

//! Reset instance with given boolean value.
inline const Bool& Bool::operator =(item_t v)
{
    v_ = v;
    return *this;
}

//! Return instance as a boolean value.
inline Bool::item_t Bool::asBool() const
{
    return v_;
}

//! Return instance as a string ("true" or "false").
inline String Bool::toString() const
{
    const char* s = v_? "true": "false";
    return s;
}

END_NAMESPACE1

#endif
