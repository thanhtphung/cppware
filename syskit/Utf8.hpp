/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_UTF8_HPP
#define SYSKIT_UTF8_HPP

#include <string.h>
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(syskit)

class Utf16;


//! utf8 character
class Utf8
    //!
    //! A class representing a UTF8 character. Natively, a UTF8 character
    //! has a 32-bit value with range 0x000000..0x00d7ff,0x00e000..0x10ffff.
    //! When encoded, depending on its value, up to 4 bytes are required.
    //! Construct an instance to encode/decode a UTF8 character. This
    //! implementation matches the newer UTF8 definition as described
    //! in RFC 3629. The older UTF8 definition is less restrictive and
    //! is described in RFC 2279. Example:
    //!\code
    //! Utf8 c0(0x54321U);
    //! utf8_t seq[Utf8::MaxSeqLength];
    //! unsigned int seqLength = c0.encode(seq);
    //! Utf8 c1;
    //! if ((c1.decode(seq, seqLength) != seqLength) || (c1.asU32() != c0.asU32()))
    //! {
    //!   assert("coding error" == 0);
    //! }
    //!\endcode
    //!
{

public:
    enum
    {
        InvalidByte = 255,
        InvalidByte0 = 0,

        DefaultChar = '?',
        MaxAscii = 0x7f,
        MaxChar = 0x10ffff,
        MinReserved = 0x00d800,
        MaxReserved = 0x00dfff,

        MinSeqLength = 1,
        MaxSeqLength = 4
    };

    // Constructors.
    Utf8(const Utf16& utf16);
    Utf8(unsigned char c = 0);
    Utf8(unsigned int c);
    Utf8(unsigned short c);
    Utf8(utf32_t validChar, bool skipValidation);

    // Operators.
    operator utf32_t() const;
    const Utf8& operator =(const Utf16& utf16);
    const Utf8& operator =(unsigned char c);
    const Utf8& operator =(unsigned int c);
    const Utf8& operator =(unsigned short c);

    unsigned int getSeqLength() const;
    utf32_t asU32() const;
    void resetWithValidChar(utf32_t validChar);

    // Encoding/Decoding operations.
    unsigned int decode(const utf8_t seq[MaxSeqLength]);
    unsigned int decode(const utf8_t* seq, size_t maxSeqLength);
    unsigned int encode(utf8_t seq[MaxSeqLength]) const;
    unsigned int encode(utf8_t* seq, size_t maxSeqLength) const;

    // Utilities.
    static bool isValid(const utf8_t seq[MaxSeqLength]);
    static bool isValid(const utf8_t* seq, size_t maxSeqLength);
    static bool isValid(unsigned int value);
    static unsigned int getSeqLength(unsigned char byte0);

private:
    unsigned int c_;

    static const unsigned char seqLength_[];

    unsigned int decode234(const utf8_t[MaxSeqLength]);
    unsigned int decode234(const utf8_t*, size_t);
    unsigned int encode234(utf8_t[MaxSeqLength]) const;
    unsigned int encode234(utf8_t*, size_t) const;

    static bool isValid234(const utf8_t[MaxSeqLength]);
    static bool isValid234(const utf8_t*, size_t);

};

//! Construct an instance with given character.
inline Utf8::Utf8(unsigned char c)
{
    c_ = c;
}

//! Construct an instance with given character. Given character must
//! not be larger than MaxChar and must not be one of the reserved
//! values in the MinReserved..MaxReserved range. If it is, DefaultChar
//! will be used instead.
inline Utf8::Utf8(unsigned int c)
{
    c_ = ((c > MaxChar) || ((c >= MinReserved) && (c <= MaxReserved)))? DefaultChar: c;
}

//! Construct an instance with given character. Given character must
//! not be one of the reserved values in the MinReserved..MaxReserved
//! range. If it is, DefaultChar will be used instead.
inline Utf8::Utf8(unsigned short c)
{
    c_ = ((c >= MinReserved) && (c <= MaxReserved))? DefaultChar: c;
}

//! Construct an instance with given character. Given character is guaranteed
//! valid by caller, and no validation is performed. Behavior is unpredictable
//! if given character is invalid (i.e., larger than MaxChar or in the
//! MinReserved..MaxReserved range).
inline Utf8::Utf8(utf32_t validChar, bool /*skipValidation*/)
{
    c_ = validChar;
}

//! Return the 32-bit character value.
inline Utf8::operator utf32_t() const
{
    return c_;
}

//! Reset instance with given value.
inline const Utf8& Utf8::operator =(unsigned char value)
{
    c_ = value;
    return *this;
}

//! Reset instance with given character. Given character must not be
//! larger than MaxChar and must not be one of the reserved values in
//! the MinReserved..MaxReserved range. If it is, DefaultChar will be
//! used instead.
inline const Utf8& Utf8::operator =(unsigned int c)
{
    c_ = ((c > MaxChar) || ((c >= MinReserved) && (c <= MaxReserved)))? DefaultChar: c;
    return *this;
}

//! Reset instance with given character. Given character must not be
//! one of the reserved values in the MinReserved..MaxReserved range.
//! If it is, DefaultChar will be used instead.
inline const Utf8& Utf8::operator =(unsigned short c)
{
    c_ = ((c >= MinReserved) && (c <= MaxReserved))? DefaultChar: c;
    return *this;
}

//! Return true if given UTF8 byte sequence (up to MaxSeqLength bytes
//! starting at seq) is valid. A valid UTF8 byte sequence has between
//! MinSeqLength and MaxSeqLength bytes, represents a value smaller
//! than or equal to MaxValue, and is as short as possible.
inline bool Utf8::isValid(const utf8_t seq[MaxSeqLength])
{
    return (seq[0] <= MaxAscii)? true: isValid234(seq);
}

//! Return true if given UTF8 byte sequence (up to maxSeqLength bytes
//! starting at seq) is valid. A valid UTF8 byte sequence has between
//! MinSeqLength and MaxSeqLength bytes, represents a value smaller
//! than or equal to MaxValue, and is as short as possible.
inline bool Utf8::isValid(const utf8_t* seq, size_t maxSeqLength)
{
    return ((maxSeqLength > 0) && (seq[0] <= MaxAscii))? true: isValid234(seq, maxSeqLength);
}

//! Return true if given value is valid for a UTF8 character (i.e., if
//! it is not larger than MaxChar and is not one of the reserved values
//! in the MinReserved..MaxReserved range).
inline bool Utf8::isValid(unsigned int value)
{
    return (value <= MaxChar) && ((value < MinReserved) || (value > MaxReserved));
}

//! Decode given UTF8 byte sequence (up to MaxSeqLength bytes starting at
//! seq). Assume the sequence has been validated, so minimize error checking.
//! If successful, update instance with decoded value and return the number
//! of bytes decoded (MinSeqLength..MaxSeqLength). Otherwise (i.e., given
//! UTF8 byte sequence is malformed), return zero.
inline unsigned int Utf8::decode(const utf8_t seq[MaxSeqLength])
{
    return (seq[0] <= MaxAscii)? ((c_ = seq[0]), 1): decode234(seq);
}

//! Decode given UTF8 byte sequence (maxSeqLength bytes starting at seq).
//! If successful, update instance with decoded value and return the number
//! of bytes decoded (MinSeqLength..MaxSeqLength). Otherwise (i.e., given
//! UTF8 byte sequence is either too short or malformed), return zero.
inline unsigned int Utf8::decode(const utf8_t* seq, size_t maxSeqLength)
{
    return ((maxSeqLength > 0) && (seq[0] <= MaxAscii))?
        ((c_ = seq[0]), 1):
        decode234(seq, maxSeqLength);
}

//! Encode value into a UTF8 byte sequence. Save results in seq. Assume
//! seq is a buffer large enough for this value. Return the resulting
//! sequence length (MinSeqLength..MaxSeqLength).
inline unsigned int Utf8::encode(utf8_t seq[MaxSeqLength]) const
{
    return (c_ <= MaxAscii)? ((seq[0] = static_cast<unsigned char>(c_)), 1): encode234(seq);
}

//! Encode value into a UTF8 byte sequence. Save results in seq. Assume
//! seq is a buffer large enough for maxSeqLength bytes. Return the resulting
//! sequence length (MinSeqLength..MaxSeqLength) if successful. Return zero
//! otherwise (i.e., given buffer is not large enough).
inline unsigned int Utf8::encode(utf8_t* seq, size_t maxSeqLength) const
{
    return ((c_ <= MaxAscii) && (maxSeqLength > 0))?
        ((seq[0] = static_cast<unsigned char>(c_)), 1):
        encode234(seq, maxSeqLength);
}

//! Return the actual UTF8 byte sequence length for this instance if encoded.
//! A UTF8 character requires between MinSeqLength and MaxSeqLength bytes.
//! Basically, more bytes are required for larger values.
inline unsigned int Utf8::getSeqLength() const
{
    const unsigned int MAX_VALUE_2 = 0x000007ffU; //11-bit, 2 UTF8 bytes
    const unsigned int MAX_VALUE_3 = 0x0000ffffU; //16-bit, 3 UTF8 bytes
    if (c_ <= MaxAscii) return 1;
    if (c_ <= MAX_VALUE_2) return 2;
    if (c_ <= MAX_VALUE_3) return 3;
    return 4;
}

//! A UTF8 byte sequence length can be determined by just looking at the first
//! byte. This method will return the sequence length given the first byte. A
//! valid UTF8 byte sequence has between MinSeqLength and MaxSeqLength bytes.
//! This method can return two values outside the MinSeqLength..MaxSeqLength
//! range: InvalidByte0 indicates given byte is not a valid first byte but
//! probably is a subsequent byte in a multi-byte sequence, and InvalidByte
//! indicates given byte is not valid in any sequence.
inline unsigned int Utf8::getSeqLength(unsigned char byte0)
{
    return seqLength_[byte0];
}

//! Return the 32-bit character value.
inline utf32_t Utf8::asU32() const
{
    return c_;
}

//! Reset instance with given character. Given character is guaranteed
//! valid by caller, and no validation is performed. Behavior is
//! unpredictable if given character is invalid (i.e., larger than
//! MaxChar or in the MinReserved..MaxReserved range).
inline void Utf8::resetWithValidChar(utf32_t validChar)
{
    c_ = validChar;
}

END_NAMESPACE1

#include "syskit/Utf16.hpp"

BEGIN_NAMESPACE1(syskit)

//! Construct from a Utf16 instance.
inline Utf8::Utf8(const Utf16& utf16)
{
    c_ = utf16.asU32();
}

//! Reset instance with given value.
inline const Utf8& Utf8::operator =(const Utf16& utf16)
{
    c_ = utf16.asU32();
    return *this;
}

END_NAMESPACE1

#endif
