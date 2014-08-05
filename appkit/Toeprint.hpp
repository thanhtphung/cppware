/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_TOEPRINT_HPP
#define APPKIT_TOEPRINT_HPP

#include "appkit/String.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)


//! string toeprint
class Toeprint
    //!
    //! A class representing a toeprint of arbitrary strings.
    //!
{

public:
    enum
    {
        Length = 6
    };

    Toeprint(const String& key0, const String& key1);
    Toeprint(const String& key0, const unsigned char* key1, size_t key1Length);

    operator const char*() const;
    bool operator !=(const Toeprint& toeprint) const;
    bool operator ==(const Toeprint& toeprint) const;

    const char* asString() const;
    const char* reset(const String& key0, const String& key1);
    const char* reset(const String& key0, const unsigned char* key1, size_t key1Length);

private:
    char code_[Length + 1];

    Toeprint(const Toeprint&); //prohibit usage
    const Toeprint& operator =(const Toeprint&); //prohibit usage

    void mkCode(unsigned int);

    static unsigned int checksum(const char*);

};

//! Construct a toeprint for given strings.
inline Toeprint::Toeprint(const String& key0, const String& key1)
{
    reset(key0, key1);
}

//! Construct a toeprint for given strings.
inline Toeprint::Toeprint(const String& key0, const unsigned char* key1, size_t key1Length)
{
    reset(key0, key1, key1Length);
}

//! Return the toeprint as a null-terminated string.
//! Its length is Toeprint::Length.
inline Toeprint::operator const char*() const
{
    return code_;
}

//! Return true if the two toeprints are different.
inline bool Toeprint::operator !=(const Toeprint& toeprint) const
{
    bool notEq = (memcmp(code_, toeprint.code_, Length + 1) != 0);
    return notEq;
}

//! Return true if the two toeprints are identical.
inline bool Toeprint::operator ==(const Toeprint& toeprint) const
{
    bool isEq = (memcmp(code_, toeprint.code_, Length + 1) == 0);
    return isEq;
}

//! Return the toeprint as a null-terminated string.
//! Its length is Toeprint::Length.
inline const char* Toeprint::asString() const
{
    return code_;
}

END_NAMESPACE1

#endif
