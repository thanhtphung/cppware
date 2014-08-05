/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_F32_HPP
#define APPKIT_F32_HPP

#include <cstdlib>
#include "appkit/String.hpp"
#include "syskit/F32Vec.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)


//! 32-bit real number
class F32
    //!
    //! A class representing a 32-bit float.
    //!
{

public:
    typedef float item_t;


    class Doublet
    {
    public:
        Doublet(const String& s);
        Doublet(item_t v0 = 0.0f, item_t v1 = 0.0f);
        operator String() const;
        item_t operator [](size_t index) const;
        item_t& operator [](size_t index);
        String toString(char wrapStringIn = '|') const;
    private:
        item_t v_[2];
        void parse(const String&);
        void toString(String&) const;
    };


    class Quartet
    {
    public:
        Quartet(const String& s);
        Quartet(item_t v0 = 0.0f, item_t v1 = 0.0f, item_t v2 = 0.0f, item_t v3 = 0.0f);
        operator String() const;
        item_t operator [](size_t index) const;
        item_t& operator [](size_t index);
        String toString(char wrapStringIn = '|') const;
    private:
        item_t v_[4];
        void parse(const String&);
        void toString(String&) const;
    };


    class Triplet
    {
    public:
        Triplet(const String& s);
        Triplet(item_t v0 = 0.0f, item_t v1 = 0.0f, item_t v2 = 0.0f);
        operator String() const;
        item_t operator [](size_t index) const;
        item_t& operator [](size_t index);
        String toString(char wrapStringIn = '|') const;
    private:
        item_t v_[3];
        void parse(const String&);
        void toString(String&) const;
    };


    class Vec: public syskit::F32Vec
    {
    public:
        Vec(const String& s);
        Vec(const Vec& vec);
        operator String() const;
        const Vec& operator =(const Vec& vec);
        String toString(char wrapStringIn = '|') const;
        virtual ~Vec();
    private:
        void parse(const String&);
        void toString(String&) const;
    };


    F32(const F32& item);
    F32(const String& s);
    F32(const String& s, size_t* bytesUsed);
    F32(const String* s, item_t defaultV = 0.0f);
    F32(const char* s);
    F32(const char* s, size_t* bytesUsed);
    F32(const char* s, size_t length);
    F32(const char* s, size_t length, size_t* bytesUsed);
    F32(item_t v = 0.0f);

    operator String() const;
    operator item_t() const;
    const F32& operator =(const F32& item);
    const F32& operator =(item_t v);

    String toString() const;
    item_t asReal() const;
    int round() const;

    static bool isValid(const char* s);

    static bool areEqual(const item_t* item0, const item_t* item1, size_t count = 1, item_t epsilon = 1.0e-7f);
    static bool areEqual(item_t item0, item_t item1, item_t epsilon = 1.0e-7f);
    static int compareK(const void* item0, const void* item1);
    static int compareKR(const void* item0, const void* item1);
    static int compareP(const void* item0, const void* item1);
    static int comparePR(const void* item0, const void* item1);
    static unsigned int hashK(const void* item, size_t numBuckets);
    static unsigned int hashP(const void* item, size_t numBuckets);

private:
    item_t v_;

    static item_t toF32(const char*, size_t, size_t*);

};

inline F32::F32(const F32& item)
{
    v_ = item.v_;
}

//! Construct instance from given valid string.
inline F32::F32(const String& s)
{
    double v = std::strtod(s.ascii(), 0);
    v_ = static_cast<item_t>(v);
}

//! Construct instance from given string. Also return the number of bytes
//! from given string utilized in the conversion process (zero indicates
//! given string could not be interpreted as a 32-bit float).
inline F32::F32(const String& s, size_t* bytesUsed)
{
    char* pEnd;
    const char* p = s.ascii();
    double v = std::strtod(p, &pEnd);
    v_ = static_cast<item_t>(v);
    *bytesUsed = pEnd - p;
}

//! Construct instance from given valid string.
inline F32::F32(const char* s)
{
    double v = std::strtod(s, 0);
    v_ = static_cast<item_t>(v);
}

//! Construct instance from given string. Also return the number of bytes
//! from given string utilized in the conversion process (zero indicates
//! given string could not be interpreted as a 32-bit float).
inline F32::F32(const char* s, size_t* bytesUsed)
{
    char* pEnd;
    double v = std::strtod(s, &pEnd);
    v_ = static_cast<item_t>(v);
    *bytesUsed = pEnd - s;
}

//! Construct instance from given valid string (length bytes starting at s).
inline F32::F32(const char* s, size_t length)
{
    size_t bytesUsed;
    v_ = toF32(s, length, &bytesUsed);
}

//! Construct instance from given string (length bytes starting at s).
//! Also return the number of bytes from given string utilized in the
//! conversion process (zero indicates given string could not be
//! interpreted as a 32-bit float).
inline F32::F32(const char* s, size_t length, size_t* bytesUsed)
{
    v_ = toF32(s, length, bytesUsed);
}

inline F32::F32(item_t v)
{
    v_ = v;
}

//! Return instance as a string.
inline F32::operator String() const
{
    return toString();
}

//! Return instance as a real number.
inline F32::operator item_t() const
{
    return v_;
}

inline const F32& F32::operator =(const F32& item)
{
    v_ = item.v_;
    return *this;
}

//! Reset instance with given float.
inline const F32& F32::operator =(item_t v)
{
    v_ = v;
    return *this;
}

//! Return instance as a real number.
inline F32::item_t F32::asReal() const
{
    return v_;
}

//! Return true if given string holds a 32-bit float and nothing else.
inline bool F32::isValid(const char* s)
{
    char* pEnd;
    std::strtod(s, &pEnd);
    return ((pEnd != s) && (*pEnd == 0));
}

inline F32::Doublet::Doublet(item_t v0, item_t v1)
{
    v_[0] = v0;
    v_[1] = v1;
}

//! Return instance as a string.
inline F32::Doublet::operator String() const
{
    char wrapStringIn = '|';
    return toString(wrapStringIn);
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline F32::item_t F32::Doublet::operator [](size_t index) const
{
    return v_[index];
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline F32::item_t& F32::Doublet::operator [](size_t index)
{
    return v_[index];
}

inline F32::Quartet::Quartet(item_t v0, item_t v1, item_t v2, item_t v3)
{
    v_[0] = v0;
    v_[1] = v1;
    v_[2] = v2;
    v_[3] = v3;
}

//! Return instance as a string.
inline F32::Quartet::operator String() const
{
    char wrapStringIn = '|';
    return toString(wrapStringIn);
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline F32::item_t F32::Quartet::operator [](size_t index) const
{
    return v_[index];
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline F32::item_t& F32::Quartet::operator [](size_t index)
{
    return v_[index];
}

inline F32::Triplet::Triplet(item_t v0, item_t v1, item_t v2)
{
    v_[0] = v0;
    v_[1] = v1;
    v_[2] = v2;
}

//! Return instance as a string.
inline F32::Triplet::operator String() const
{
    char wrapStringIn = '|';
    return toString(wrapStringIn);
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline F32::item_t F32::Triplet::operator [](size_t index) const
{
    return v_[index];
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline F32::item_t& F32::Triplet::operator [](size_t index)
{
    return v_[index];
}

//! Return instance as a string.
inline F32::Vec::operator String() const
{
    char wrapStringIn = '|';
    return toString(wrapStringIn);
}

END_NAMESPACE1

#if __linux || __CYGWIN__ || __FREERTOS__
#include "appkit/linux/F32-linux.hpp"

#elif _WIN32
#if _M_X64
#include "appkit/x64/F32-x64.hpp"
#else
#include "appkit/win32/F32-win32.hpp"
#endif

#else
#error "unsupported architecture"

#endif
#endif
