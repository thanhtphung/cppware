/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_D64_HPP
#define APPKIT_D64_HPP

#include <cstdlib>
#include "appkit/String.hpp"
#include "syskit/D64Vec.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)


//! 64-bit real number
class D64
    //!
    //! A class representing a 64-bit double.
    //!
{

public:
    typedef double item_t;


    class Doublet
    {
    public:
        Doublet(const String& s);
        Doublet(item_t v0 = 0.0, item_t v1 = 0.0);
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
        Quartet(item_t v0 = 0.0, item_t v1 = 0.0, item_t v2 = 0.0, item_t v3 = 0.0);
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
        Triplet(item_t v0 = 0.0, item_t v1 = 0.0, item_t v2 = 0.0);
        operator String() const;
        item_t operator [](size_t index) const;
        item_t& operator [](size_t index);
        String toString(char wrapStringIn = '|') const;
    private:
        item_t v_[3];
        void parse(const String&);
        void toString(String&) const;
    };


    class Vec: public syskit::D64Vec
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


    D64(const D64& item);
    D64(const String& s);
    D64(const String& s, size_t* bytesUsed);
    D64(const String* s, item_t defaultV = 0.0);
    D64(const char* s);
    D64(const char* s, size_t* bytesUsed);
    D64(const char* s, size_t length);
    D64(const char* s, size_t length, size_t* bytesUsed);
    D64(item_t v = 0.0);

    operator String() const;
    operator item_t() const;
    const D64& operator =(const D64& item);
    const D64& operator =(item_t v);

    String toString() const;
    item_t asReal() const;

    static bool isValid(const char* s);

    static bool areEqual(const item_t* item0, const item_t* item1, size_t count = 1, item_t epsilon = 1.0e-15);
    static bool areEqual(item_t item0, item_t item1, item_t epsilon = 1.0e-15);
    static int compareK(const void* item0, const void* item1);
    static int compareKR(const void* item0, const void* item1);
    static int compareP(const void* item0, const void* item1);
    static int comparePR(const void* item0, const void* item1);
    static unsigned int hashK(const void* item, size_t numBuckets);
    static unsigned int hashP(const void* item, size_t numBuckets);

private:
    item_t v_;

    static item_t toD64(const char*, size_t, size_t*);

};

inline D64::D64(const D64& item)
{
    v_ = item.v_;
}

//! Construct instance from given valid string.
inline D64::D64(const String& s)
{
    v_ = std::strtod(s.ascii(), 0);
}

//! Construct instance from given string. Also return the number of bytes
//! from given string utilized in the conversion process (zero indicates
//! given string could not be interpreted as a 64-bit double).
inline D64::D64(const String& s, size_t* bytesUsed)
{
    char* pEnd;
    const char* p = s.ascii();
    v_ = std::strtod(p, &pEnd);
    *bytesUsed = pEnd - p;
}

//! Construct instance from given valid string.
inline D64::D64(const char* s)
{
    v_ = std::strtod(s, 0);
}

//! Construct instance from given string. Also return the number of bytes
//! from given string utilized in the conversion process (zero indicates
//! given string could not be interpreted as a 64-bit double).
inline D64::D64(const char* s, size_t* bytesUsed)
{
    char* pEnd;
    v_ = std::strtod(s, &pEnd);
    *bytesUsed = pEnd - s;
}

//! Construct instance from given valid string (length bytes starting at s).
inline D64::D64(const char* s, size_t length)
{
    size_t bytesUsed;
    v_ = toD64(s, length, &bytesUsed);
}

//! Construct instance from given string (length bytes starting at s).
//! Also return the number of bytes from given string utilized in the
//! conversion process (zero indicates given string could not be
//! interpreted as a 64-bit double).
inline D64::D64(const char* s, size_t length, size_t* bytesUsed)
{
    v_ = toD64(s, length, bytesUsed);
}

inline D64::D64(item_t v)
{
    v_ = v;
}

//! Return instance as a string.
inline D64::operator String() const
{
    return toString();
}

//! Return instance as a real number.
inline D64::operator item_t() const
{
    return v_;
}

inline const D64& D64::operator =(const D64& item)
{
    v_ = item.v_;
    return *this;
}

//! Reset instance with given double.
inline const D64& D64::operator =(item_t v)
{
    v_ = v;
    return *this;
}

//! Return instance as a real number.
inline D64::item_t D64::asReal() const
{
    return v_;
}

//! Return true if given string holds a 64-bit double and nothing else.
inline bool D64::isValid(const char* s)
{
    char* pEnd;
    std::strtod(s, &pEnd);
    return ((pEnd != s) && (*pEnd == 0));
}

inline D64::Doublet::Doublet(item_t v0, item_t v1)
{
    v_[0] = v0;
    v_[1] = v1;
}

//! Return instance as a string.
inline D64::Doublet::operator String() const
{
    char wrapStringIn = '|';
    return toString(wrapStringIn);
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline D64::item_t D64::Doublet::operator [](size_t index) const
{
    return v_[index];
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline D64::item_t& D64::Doublet::operator [](size_t index)
{
    return v_[index];
}

inline D64::Quartet::Quartet(item_t v0, item_t v1, item_t v2, item_t v3)
{
    v_[0] = v0;
    v_[1] = v1;
    v_[2] = v2;
    v_[3] = v3;
}

//! Return instance as a string.
inline D64::Quartet::operator String() const
{
    char wrapStringIn = '|';
    return toString(wrapStringIn);
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline D64::item_t D64::Quartet::operator [](size_t index) const
{
    return v_[index];
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline D64::item_t& D64::Quartet::operator [](size_t index)
{
    return v_[index];
}

inline D64::Triplet::Triplet(item_t v0, item_t v1, item_t v2)
{
    v_[0] = v0;
    v_[1] = v1;
    v_[2] = v2;
}

//! Return instance as a string.
inline D64::Triplet::operator String() const
{
    char wrapStringIn = '|';
    return toString(wrapStringIn);
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline D64::item_t D64::Triplet::operator [](size_t index) const
{
    return v_[index];
}

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid.
inline D64::item_t& D64::Triplet::operator [](size_t index)
{
    return v_[index];
}

//! Return instance as a string.
inline D64::Vec::operator String() const
{
    char wrapStringIn = '|';
    return toString(wrapStringIn);
}

END_NAMESPACE1

#endif
