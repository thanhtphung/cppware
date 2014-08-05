/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_BOM_HPP
#define SYSKIT_BOM_HPP

#include <string.h>
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! UTFx byte-order-mark
class Bom
    //!
    //! A class representing a UTFx byte-order-mark.
    //!\code
    //! Bom bom = Bom::decode("\xef\xbb\xbfabc123", 9);
    //! assert(bom.type() == Bom::Utf8);
    //!\endcode
    //!
{

public:
    enum type_e
    {
        None = 0,
        Utf8,
        Utf16, //native-endian
        Utf61, //non-native-endian
        Utf32, //native-endian
        Utf23  //non-native-endian
    };

    enum
    {
        MaxSeqLength = 4
    };

    Bom(const Bom& bom);
    Bom(unsigned int /*type_e*/ type = Utf8);

    bool operator !=(const Bom& bom) const;
    bool operator ==(const Bom& bom) const;
    const Bom& operator =(const Bom& bom);

    unsigned int byteSize() const;
    unsigned int encode(unsigned char seq[MaxSeqLength]) const;
    unsigned int /*type_e*/ type() const;
    void reset(unsigned int /*type_e*/ type);

    static Bom decode(const void* p, size_t byteSize);

private:
    typedef struct
    {
        const unsigned char* seq;
        unsigned int byteSize;
    } bom_t;

    unsigned int /*type_e*/ type_;

    static const bom_t bom_[];

};

inline Bom::Bom(const Bom& bom)
{
    type_ = bom.type_;
}

inline Bom::Bom(unsigned int /*type_e*/ type)
{
    type_ = type;
}

inline bool Bom::operator !=(const Bom& bom) const
{
    return type_ != bom.type_;
}

inline bool Bom::operator ==(const Bom& bom) const
{
    return type_ == bom.type_;
}

inline const Bom& Bom::operator =(const Bom& bom)
{
    type_ = bom.type_;
    return *this;
}

//! Return the byte size of the byte-order-mark when encoded.
inline unsigned int Bom::byteSize() const
{
    return bom_[type_].byteSize;
}

//! Encode the byte-order-mark into given byte sequence.
//! Return the byte size of the byte-order-mark.
inline unsigned int Bom::encode(unsigned char seq[MaxSeqLength]) const
{
    const bom_t& r = bom_[type_];
    memcpy(seq, r.seq, r.byteSize);
    return r.byteSize;
}

inline unsigned int /*type_e*/ Bom::type() const
{
    return type_;
}

inline void Bom::reset(unsigned int /*type_e*/ type)
{
    type_ = type;
}

END_NAMESPACE1

#endif
