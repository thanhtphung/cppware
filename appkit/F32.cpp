/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <math.h>
#include <string.h>
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/F32.hpp"
#include "appkit/S32.hpp"
#include "appkit/Tokenizer.hpp"
#include "appkit/crt.hpp"

BEGIN_NAMESPACE

appkit::String startWrap(char& wrapStringIn)
{
    switch (wrapStringIn)
    {
    case 0: break;
    case '(':
    case ')': wrapStringIn = ')'; return appkit::String(1, '(');
    case '[':
    case ']': wrapStringIn = ']'; return appkit::String(1, '[');
    case '{':
    case '}': wrapStringIn = '}'; return appkit::String(1, '{');
    case '<':
    case '>': wrapStringIn = '>'; return appkit::String(1, '<');
    default: return appkit::String(1, wrapStringIn); //should be one of ['"|] but not enforced
    }

    return appkit::String();
}

// Return 1 if wrapping seems valid.
// Return 0 if there seems to be no wrapping.
// Return -1 if wrapping seems invalid.
int parseWrapping(const appkit::String& s)
{
    int rc;
    const char* p = s.ascii();
    switch (p[0])
    {
    case '(': rc = (s[s.byteSize() - 2] == ')')? 1: -1; break;
    case '[': rc = (p[s.byteSize() - 2] == ']')? 1: -1; break;
    case '{': rc = (p[s.byteSize() - 2] == '}')? 1: -1; break;
    case '<': rc = (p[s.byteSize() - 2] == '>')? 1: -1; break;
    case '\'':
    case '"':
    case '|': rc = (p[s.byteSize() - 2] == p[0])? 1: -1; break;
    default: rc = 0; break;
    }

    return rc;
}

END_NAMESPACE

BEGIN_NAMESPACE1(appkit)

typedef union
{
    const void* opaque; //must be first
    F32::item_t item;
    unsigned int n;
} opaque_t;


F32::F32(const String* s, item_t defaultV)
{
    if ((s == 0) || s->empty())
    {
        v_ = defaultV;
    }
    else
    {
        char* pEnd;
        const char* p = s->ascii();
        double v = std::strtod(p, &pEnd);
        v_ = (pEnd != p)? static_cast<item_t>(v): defaultV;
    }
}


//!
//! Return instance as a string.
//!
String F32::toString() const
{
    String s;
    sprintf(s, "%.6g", v_);
    return s;
}


//!
//! Compare given arrays of 32-bit floats. Each array holds count numbers.
//! Allow a small epsilon difference when determining equality. That is,
//! two numbers are equal if they differ by epsilon or less. Behavior is
//! currently undefined for numbers with special values (NAN, +INF, etc.).
//!
bool F32::areEqual(const item_t* item0, const item_t* item1, size_t count, item_t epsilon)
{
    bool eq = true;
    for (size_t i = 0; i < count; ++i)
    {
        if (fabs(item0[i] - item1[i]) > epsilon)
        {
            eq = false;
            break;
        }
    }

    return eq;
}


//!
//! Compare given 32-bit floats. Allow a small epsilon difference when
//! determining equality. That is, two numbers are equal if they differ
//! by epsilon or less. Behavior is currently undefined for numbers with
//! special values (NAN, +INF, etc.).
//!
bool F32::areEqual(item_t item0, item_t item1, item_t epsilon)
{
    bool eq = (fabs(item0 - item1) <= epsilon);
    return eq;
}


F32::item_t F32::toF32(const char* s, size_t length, size_t* bytesUsed)
{
    const size_t LENGTH = 31;
    char* pEnd;
    double v;
    int c;
    if (length <= 0)
    {
        v = 0.0;
        *bytesUsed = 0;
    }

    else if (((c = s[length - 1]) == 0) || S32::isSpace(c))
    {
        v = std::strtod(s, &pEnd);
        *bytesUsed = pEnd - s;
    }

    // Use stack for temporary null-terminated string copy.
    else if (length <= LENGTH)
    {
        char buf[LENGTH + 1];
        memcpy(buf, s, length);
        buf[length] = 0;
        v = std::strtod(buf, &pEnd);
        *bytesUsed = pEnd - buf;
    }

    // Use heap for temporary null-terminated string copy.
    else
    {
        char* buf = new char[length + 1];
        memcpy(buf, s, length);
        buf[length] = 0;
        v = std::strtod(buf, &pEnd);
        *bytesUsed = pEnd - buf;
        delete[] buf;
    }

    return static_cast<float>(v);
}


//!
//! Compare two 32-bit floats. Return a negative value if item0 < item1.
//! Return 0 if item0 == item1. Return a positive value if item0 > item1.
//!
int F32::compareK(const void* item0, const void* item1)
{
    opaque_t k0 = {item0};
    opaque_t k1 = {item1};
    return (k0.item < k1.item)? (-1): ((k0.item>k1.item)? 1: 0);
}


//!
//! Compare two 32-bit floats. Reverse the normal sense of comparison.
//! Return a positive value if item0 < item1. Return 0 if item0 == item1.
//! Return a negative value if item0 > item1.
//!
int F32::compareKR(const void* item0, const void* item1)
{
    opaque_t k0 = {item0};
    opaque_t k1 = {item1};
    return (k0.item < k1.item)? (1): ((k0.item>k1.item)? -1: 0);
}


//!
//! Compare two 32-bit floats given their addresses. Return a negative
//! value if *item0 < *item1. Return 0 if *item0 == *item1. Return a
//! positive value if *item0 > *item1.
//!
int F32::compareP(const void* item0, const void* item1)
{
    item_t k0 = *static_cast<const item_t*>(item0);
    item_t k1 = *static_cast<const item_t*>(item1);
    return (k0 < k1)? (-1): ((k0>k1)? 1: 0);
}


//!
//! Compare two 32-bit floats given their addresses. Reverse the normal
//! sense of comparison. Return a positive value if *item0 < *item1. Return
//! 0 if *item0 == *item1. Return a negative value if *item0 > *item1.
//!
int F32::comparePR(const void* item0, const void* item1)
{
    item_t k0 = *static_cast<const item_t*>(item0);
    item_t k1 = *static_cast<const item_t*>(item1);
    return (k0 < k1)? (1): ((k0>k1)? -1: 0);
}


//!
//! Modular hash function for a 32-bit float.
//! Return a non-negative number less than numBuckets.
//!
unsigned int F32::hashK(const void* item, size_t numBuckets)
{
    opaque_t k = {item};
    unsigned int i = static_cast<unsigned int>(k.n % numBuckets);
    return i;
}


//!
//! Modular hash function for a 32-bit float given its address.
//! Return a non-negative number less than numBuckets.
//!
unsigned int F32::hashP(const void* item, size_t numBuckets)
{
    unsigned long long k = *static_cast<const unsigned long long*>(item);
    unsigned int i = static_cast<unsigned int>(k % numBuckets);
    return i;
}


//!
//! Construct doublet from its string form similar to the format used in toString().
//! Zeroes are used if given string cannot be interpreted.
//!
F32::Doublet::Doublet(const String& s)
{
    v_[0] = 0.0f;
    v_[1] = 0.0f;

    int rc;
    String trimmed(s);
    trimmed.trimSpace();
    if ((!trimmed.isAscii()) || (trimmed.byteSize() <= 3) || ((rc = parseWrapping(trimmed)) < 0))
    {
        return;
    }

    // Remove wrapping.
    if (rc > 0)
    {
        trimmed = trimmed.substr(1, trimmed.length() - 2);
        trimmed.trimSpace();
    }

    parse(trimmed);
}


//!
//! Default format with vertical bars wrapping the doublet: "|v0 v1|".
//! Example: "|-0.1 22.345|". Use zero to omit the wrapping. Some wrap
//! pairing is automatic. For example, either '(' or ')' will result in
//! "(v0 v1)".
//!
String F32::Doublet::toString(char wrapStringIn) const
{
    String s(startWrap(wrapStringIn));
    toString(s);
    if (wrapStringIn != 0)
    {
        s += wrapStringIn;
    }

    return s;
}


//
// Parse doublet.
// Don't worry about the wrapping as it, if any, has been removed.
//
void F32::Doublet::parse(const String& s)
{
    Tokenizer tokenizer(s);
    String token;
    tokenizer.next(token);
    v_[0] = F32(token);
    tokenizer.next(token);
    v_[1] = F32(token);
}


//
// Serialize doublet.
// Don't worry about the wrapping.
//
void F32::Doublet::toString(String& s) const
{
    s += F32(v_[0]).toString();
    s += ' ';
    s += F32(v_[1]).toString();
}


//!
//! Construct quartet from its string form similar to the format used in toString().
//! Zeroes are used if given string cannot be interpreted.
//!
F32::Quartet::Quartet(const String& s)
{
    v_[0] = 0.0f;
    v_[1] = 0.0f;
    v_[2] = 0.0f;
    v_[3] = 0.0f;

    int rc;
    String trimmed(s);
    trimmed.trimSpace();
    if ((!trimmed.isAscii()) || (trimmed.byteSize() <= 7) || ((rc = parseWrapping(trimmed)) < 0))
    {
        return;
    }

    // Remove wrapping.
    if (rc > 0)
    {
        trimmed = trimmed.substr(1, trimmed.length() - 2);
        trimmed.trimSpace();
    }

    parse(trimmed);
}


//!
//! Default format with vertical bars wrapping the quartet: "|v0 v1 v2 v3|".
//! Example: "|678.9 -0.1 22.345 0|". Use zero to omit the wrapping. Some wrap
//! pairing is automatic. For example, either '<' or '>' will result in
//! "<v0 v1 v2 v3>".
//!
String F32::Quartet::toString(char wrapStringIn) const
{
    String s(startWrap(wrapStringIn));
    toString(s);
    if (wrapStringIn != 0)
    {
        s += wrapStringIn;
    }

    return s;
}


//
// Parse quartet.
// Don't worry about the wrapping as it, if any, has been removed.
//
void F32::Quartet::parse(const String& s)
{
    Tokenizer tokenizer(s);
    String token;
    tokenizer.next(token);
    v_[0] = F32(token);
    tokenizer.next(token);
    v_[1] = F32(token);
    tokenizer.next(token);
    v_[2] = F32(token);
    tokenizer.next(token);
    v_[3] = F32(token);
}


//
// Serialize quartet.
// Don't worry about the wrapping.
//
void F32::Quartet::toString(String& s) const
{
    s += F32(v_[0]).toString();
    s += ' ';
    s += F32(v_[1]).toString();
    s += ' ';
    s += F32(v_[2]).toString();
    s += ' ';
    s += F32(v_[3]).toString();
}


//!
//! Construct triplet from its string form similar to the format used in toString().
//! Zeroes are used if given string cannot be interpreted.
//!
F32::Triplet::Triplet(const String& s)
{
    v_[0] = 0.0f;
    v_[1] = 0.0f;
    v_[2] = 0.0f;

    int rc;
    String trimmed(s);
    trimmed.trimSpace();
    if ((!trimmed.isAscii()) || (trimmed.byteSize() <= 5) || ((rc = parseWrapping(trimmed)) < 0))
    {
        return;
    }

    // Remove wrapping.
    if (rc > 0)
    {
        trimmed = trimmed.substr(1, trimmed.length() - 2);
        trimmed.trimSpace();
    }

    parse(trimmed);
}


//!
//! Default format with vertical bars wrapping the triplet: "|v0 v1 v2|".
//! Example: "|6789 -0.1 22.345|". Use zero to omit the wrapping. Some wrap
//! pairing is automatic. For example, either '[' or ']' will result in
//! "[v0 v1 v2]".
//!
String F32::Triplet::toString(char wrapStringIn) const
{
    String s(startWrap(wrapStringIn));
    toString(s);
    if (wrapStringIn != 0)
    {
        s += wrapStringIn;
    }

    return s;
}


//
// Parse triplet.
// Don't worry about the wrapping as it, if any, has been removed.
//
void F32::Triplet::parse(const String& s)
{
    Tokenizer tokenizer(s);
    String token;
    tokenizer.next(token);
    v_[0] = F32(token);
    tokenizer.next(token);
    v_[1] = F32(token);
    tokenizer.next(token);
    v_[2] = F32(token);
}


//
// Serialize triplet.
// Don't worry about the wrapping.
//
void F32::Triplet::toString(String& s) const
{
    s += F32(v_[0]).toString();
    s += ' ';
    s += F32(v_[1]).toString();
    s += ' ';
    s += F32(v_[2]).toString();
}


//!
//! Construct vector from its string form similar to the format used in toString().
//!
F32::Vec::Vec(const String& s):
F32Vec(8 /*capacity*/, -1 /*growBy*/)
{
    int rc;
    String trimmed(s);
    trimmed.trimSpace();
    if ((!trimmed.isAscii()) || (trimmed.byteSize() == 0) || ((rc = parseWrapping(trimmed)) < 0))
    {
        return;
    }

    // Remove wrapping.
    if (rc > 0)
    {
        trimmed = trimmed.substr(1, trimmed.length() - 2);
        trimmed.trimSpace();
    }

    parse(trimmed);
}


F32::Vec::Vec(const Vec& vec):
F32Vec(vec)
{
}


F32::Vec::~Vec()
{
}


const F32::Vec& F32::Vec::operator =(const Vec& vec)
{
    F32Vec::operator =(vec);
    return *this;
}


//!
//! Default format with vertical bars wrapping the vector: "|v0 v1 v2|".
//! Example: "|6789 -0.1 22.345|". Use zero to omit the wrapping. Some wrap
//! pairing is automatic. For example, either '[' or ']' will result in
//! "[v0 v1 v2]".
//!
String F32::Vec::toString(char wrapStringIn) const
{
    String s(startWrap(wrapStringIn));
    toString(s);
    if (wrapStringIn != 0)
    {
        s += wrapStringIn;
    }

    return s;
}


//
// Parse vector.
// Don't worry about the wrapping as it, if any, has been removed.
//
void F32::Vec::parse(const String& s)
{
    Tokenizer tokenizer(s);
    String token;
    while (tokenizer.next(token))
    {
        item_t item = F32(token);
        add(item);
    }
}


//
// Serialize vector.
// Don't worry about the wrapping.
//
void F32::Vec::toString(String& s) const
{
    size_t numItems = F32Vec::numItems();
    if (numItems == 0)
    {
        return;
    }

    const item_t* p = raw();
    for (size_t i = 0; i < numItems; ++i)
    {
        s += F32(p[i]).toString();
        s += ' ';
    }

    s.truncate(s.length() - 1);
}

END_NAMESPACE1
