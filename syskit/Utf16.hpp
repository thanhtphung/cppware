/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_UTF16_HPP
#define SYSKIT_UTF16_HPP

#include <string.h>
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(syskit)

class Utf8;


//! utf16 character
class Utf16
    //!
    //! A class representing a UTF16 character. Natively, a UTF16 character has a
    //! 32-bit value with range 0x000000..0x00d7ff,0x00e000..0x10ffff. When encoded,
    //! a 16-bit value uses one unsigned short, and a larger value uses two unsigned
    //! shorts. Construct an instance to encode/decode a UTF16 character. Refer to
    //! RFC 2781 for more information. The www.unicode.org FAQs do mention these
    //! invalid 34 values: 0x00fdd0..0x00fdef,0x00fffe..0x00ffff. However, there
    //! seems to be discrepancies regarding these values in various RFCs and various
    //! implementations. Those 34 values are considered valid in this class. Where
    //! byte-ordering is a factor, data is assumed to have native-endian byte-ordering.
    //! Example:
    //!\code
    //! Utf16 c0(0x54321U);
    //! utf16_t seq[Utf16::MaxSeqLength];
    //! unsigned int seqLength = c0.encode(seq);
    //! Utf16 c1;
    //! if ((c1.decode(seq) != seqLength) || (c1.asU32() != c0.asU32()))
    //! {
    //!   assert("coding error" == 0);
    //! }
    //!\endcode
    //!
{

public:
    enum
    {
        DefaultChar = '?',
        MaxChar = 0x10ffff,
        MinReserved = 0x00d800,
        MaxReserved = 0x00dfff,

        HiHalfMin = 0x00d800,
        HiHalfMax = 0x00dbff,
        LoHalfMin = 0x00dc00,
        LoHalfMax = 0x00dfff,

        MinSeqLength = 1,
        MaxSeqLength = 2
    };

    // Constructors.
    Utf16(const Utf8& utf8);
    Utf16(unsigned char c = 0);
    Utf16(unsigned int c);
    Utf16(unsigned short c);

    // Operators.
    operator utf32_t() const;
    const Utf16& operator =(const Utf8& utf8);
    const Utf16& operator =(unsigned char c);
    const Utf16& operator =(unsigned int c);
    const Utf16& operator =(unsigned short c);

    unsigned int getSeqLength() const;
    utf32_t asU32() const;
    void resetWithValidChar(utf32_t validChar);

    // Encoding/Decoding operations.
    unsigned int decode(const utf16_t seq[MaxSeqLength]);
    unsigned int encode(utf16_t seq[MaxSeqLength]) const;

    // Utilities.
    static bool isValid(unsigned int value);
    static unsigned int decodeValidSeq(const utf16_t* validSeq, utf32_t& c);
    static utf32_t convertSurrogate(const utf16_t seq[MaxSeqLength]);
    static utf32_t convertSurrogate(utf16_t u16A, utf16_t u16B);
    static utf32_t convertValidSeq(const utf16_t* validSeq);
    static void convertSurrogate(utf32_t largeValidChar, utf16_t seq[MaxSeqLength]);

private:
    enum
    {
        Anchor = 0x10000
    };

    unsigned int c_;

};

//! Construct an instance with given character.
inline Utf16::Utf16(unsigned char c)
{
    c_ = c;
}

//! Construct an instance with given character. Given character must
//! not be larger than MaxChar and must not be one of the reserved
//! values in the MinReserved..MaxReserved range. If it is, DefaultChar
//! will be used instead.
inline Utf16::Utf16(unsigned int c)
{
    c_ = ((c > MaxChar) || ((c >= MinReserved) && (c <= MaxReserved)))? DefaultChar: c;
}

//! Construct an instance with given character. Given character must
//! not be one of the reserved values in the MinReserved..MaxReserved
//! range. If it is, DefaultChar will be used instead.
inline Utf16::Utf16(unsigned short c)
{
    c_ = ((c >= MinReserved) && (c <= MaxReserved))? DefaultChar: c;
}

//! Return the 32-bit character value.
inline Utf16::operator utf32_t() const
{
    return c_;
}

//! Reset instance with given character.
inline const Utf16& Utf16::operator =(unsigned char c)
{
    c_ = c;
    return *this;
}

//! Reset instance with given character. Given character must not be
//! larger than MaxChar and must not be one of the reserved values in
//! the MinReserved..MaxReserved range. If it is, DefaultChar will be
//! used instead.
inline const Utf16& Utf16::operator =(unsigned int c)
{
    c_ = ((c > MaxChar) || ((c >= MinReserved) && (c <= MaxReserved)))? DefaultChar: c;
    return *this;
}

//! Reset instance with given character. Given character must not be
//! one of the reserved values in the MinReserved..MaxReserved range.
//! If it is, DefaultChar will be used instead.
inline const Utf16& Utf16::operator =(unsigned short c)
{
    c_ = ((c >= MinReserved) && (c <= MaxReserved))? DefaultChar: c;
    return *this;
}

//! Return true if given value is valid for a UTF16 character (i.e., if
//! it is not larger than MaxChar and is not one of the reserved values
//! in the MinReserved..MaxReserved range).
inline bool Utf16::isValid(unsigned int value)
{
    return (value <= MaxChar) && ((value < MinReserved) || (value > MaxReserved));
}

//! Decode given UTF16 byte sequence (either 1 or 2 unsigned shorts starting
//! at seq). If successful, update instance with decoded value and return
//! the number of unsigned shorts decoded. Otherwise (i.e., given UTF16 byte
//! sequence is malformed), return zero.
inline unsigned int Utf16::decode(const utf16_t seq[MaxSeqLength])
{
    if ((seq[0] < HiHalfMin) || (seq[0] > LoHalfMax))
    {
        c_ = seq[0];
        return 1;
    }
    if ((seq[0] <= HiHalfMax) && (seq[1] >= LoHalfMin) && (seq[1] <= LoHalfMax))
    {
        c_ = Anchor + (((seq[0] & 0x3ffU) << 10) | (seq[1] & 0x3ffU));
        return 2;
    }
    return 0;
}

//! Decode given valid UTF16 byte sequence (either 1 or 2 unsigned shorts
//! starting at validSeq). Save the effective value in c. Return the
//! number of unsigned shorts decoded.
inline unsigned int Utf16::decodeValidSeq(const utf16_t* validSeq, utf32_t& c)
{
    return ((validSeq[0] < HiHalfMin) || (validSeq[0] > LoHalfMax))?
        ((c = validSeq[0]), 1):
        ((c = Anchor + (((validSeq[0] & 0x3ffU) << 10) | (validSeq[1] & 0x3ffU))), 2);
}

//! Encode value into a UTF16 byte sequence. A 16-bit value uses one unsigned
//! short, and a larger value uses two unsigned shorts. Save results in seq.
//! Assume seq is a buffer large enough for this value. Return the number of
//! unsigned shorts utilized in the encoding.
inline unsigned int Utf16::encode(utf16_t seq[MaxSeqLength]) const
{
    if (c_ <= 0xffffU)
    {
        seq[0] = static_cast<unsigned short>(c_);
        return 1;
    }
    unsigned int delta = c_ - Anchor;
    seq[0] = static_cast<unsigned short>(HiHalfMin | (delta >> 10));
    seq[1] = static_cast<unsigned short>(LoHalfMin | (delta & 0x3ffU));
    return 2;
}

//! Return the sequence length in shorts for this instance if encoded.
//! A 16-bit value uses one unsigned short, and a larger value uses two
//! unsigned shorts.
inline unsigned int Utf16::getSeqLength() const
{
    return (c_ <= 0xffffU)? 1: 2;
}

//! Return the 32-bit character value.
inline utf32_t Utf16::asU32() const
{
    return c_;
}

//! Convert given surrogate pair into its effective value. The surrogate pair
//! consists of two unsigned shorts: the first unsigned short holds the high
//! half zone, and the second unsigned short holds the low half zone. Return
//! the effective value.
inline utf32_t Utf16::convertSurrogate(const utf16_t seq[MaxSeqLength])
{
    return Anchor + (((seq[0] & 0x3ffU) << 10) | (seq[1] & 0x3ffU));
}

//! Convert given surrogate pair into its effective value. The surrogate pair
//! consists of two unsigned shorts: the first unsigned short u16A holds the high
//! half zone, and the second unsigned short u16B holds the low half zone. Return
//! the effective value.
inline utf32_t Utf16::convertSurrogate(utf16_t u16A, utf16_t u16B)
{
    return Anchor + (((u16A & 0x3ffU) << 10) | (u16B & 0x3ffU));
}

//! Convert given valid UTF16 byte sequence (either 1 or 2 unsigned shorts
//! starting at validSeq) into its effective value. Return the effective
//! value. Behavior is unpredictable if given sequence is invalid.
inline utf32_t Utf16::convertValidSeq(const utf16_t* validSeq)
{
    return ((validSeq[0] < HiHalfMin) || (validSeq[0] > LoHalfMax))?
        (validSeq[0]):
        (Anchor + (((validSeq[0] & 0x3ffU) << 10) | (validSeq[1] & 0x3ffU)));
}

//! Convert given large and valid value (0x010000..0x10ffff) into a surrogate pair.
//! The surrogate pair consists of two unsigned shorts: the first unsigned short
//! holds the high half zone, and the second unsigned short holds the low half zone.
inline void Utf16::convertSurrogate(utf32_t largeValidChar, utf16_t seq[MaxSeqLength])
{
    unsigned int delta = largeValidChar - Anchor;
    seq[0] = static_cast<unsigned short>(HiHalfMin | (delta >> 10));
    seq[1] = static_cast<unsigned short>(LoHalfMin | (delta & 0x3ffU));
}

//! Reset instance with given value. Given value is guaranteed valid
//! by user, and no validation is performed. Behavior is unpredictable
//! if given value is invalid (i.e., larger than MaxChar or in the
//! MinReserved..MaxReserved range).
inline void Utf16::resetWithValidChar(utf32_t validChar)
{
    c_ = validChar;
}

END_NAMESPACE1

#include "syskit/Utf8.hpp"

BEGIN_NAMESPACE1(syskit)

//! Construct from a Utf16 instance.
inline Utf16::Utf16(const Utf8& utf8)
{
    c_ = utf8.asU32();
}

//! Reset instance with given value.
inline const Utf16& Utf16::operator =(const Utf8& utf8)
{
    c_ = utf8.asU32();
    return *this;
}

END_NAMESPACE1

#endif
