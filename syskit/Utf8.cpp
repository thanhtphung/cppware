/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/Utf8.hpp"
#include "syskit/macros.h"

// Decoding mask. Indexed by sequence length. On bits match high-order bits in first byte.
const unsigned int FIRST_BYTE_D_MASK[syskit::Utf8::MaxSeqLength + 1] =
{
    0x00U, 0x00U, 0x1fU, 0x0fU, 0x07U
};

// Encoding mask. Indexed by sequence length. Each mask holds minimal on bits in first byte.
const unsigned int FIRST_BYTE_E_MASK[syskit::Utf8::MaxSeqLength + 1] =
{
    0x00U, 0x00U, 0xc0U, 0xe0U, 0xf0U
};

const unsigned int MAX_VALUE_2 = 0x000007ffU; //11-bit, 2 UTF8 bytes
const unsigned int MAX_VALUE_3 = 0x0000ffffU; //16-bit, 3 UTF8 bytes

BEGIN_NAMESPACE1(syskit)

// Sequence length table. Indexed by first byte. If first byte is i,
// seqLength_[i] gives expected sequence length (between MinSeqLength
// and MaxSeqLength). Two special values are also used: InvalidByte0=0
// indicates "i" is not a valid first byte but probably is a subsequent
// byte in a multi-byte sequence, and InvalidByte=255 indicates "i" is
// not valid in any sequence.
const unsigned char Utf8::seqLength_[] =
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  //0x00-0x0f
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  //0x10-0x1f
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  //0x20-0x2f
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  //0x30-0x3f
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  //0x40-0x4f
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  //0x50-0x5f
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  //0x60-0x6f
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  //0x70-0x7f
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //0x80-0x8f
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //0x90-0x9f
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //0xa0-0xaf
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //0xb0-0xbf
    255, 255, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  //0xc0-0xcf
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  //0xd0-0xdf
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  //0xe0-0xef
    4, 4, 4, 4, 4, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 //0xf0-0xff
};


//
// Return true if given UTF8 multi-byte sequence (up to MaxSeqLength bytes
// starting at seq) is valid. A valid UTF8 byte sequence has between
// MinSeqLength and MaxSeqLength bytes, represents a value smaller
// than or equal to MaxValue, and is as short as possible.
//
bool Utf8::isValid234(const utf8_t seq[MaxSeqLength])
{

    // Basically, subsequent bytes must have range 0x80U..0xbfU. If
    // sequence length is 2, range check is not required since it's
    // already built-in in the sequence length table. If sequence
    // length is more than 2, the sequence is valid only if its value
    // cannot be encoded using fewer bytes. A 1-byte sequence is an
    // ASCII character. Also, check for reserved values in 3-byte
    // sequences and out-of-range values in 4-byte sequences.
    switch (seqLength_[seq[0]])
    {
    case 4:
        if ((seq[3] < 0x80U) || (seq[3] > 0xbfU))
        {
            return false;
        }
        // no break

    case 3:
        if ((seq[2] < 0x80U) || (seq[2] > 0xbfU))
        {
            return false;
        }
        // no break

    case 2:
        if (seq[1] > 0xbfU)
        {
            return false;
        }
        if (seq[0] == 0xe0U) return (seq[1] >= 0xa0U); //too many bytes in a 3-byte sequence?
        if (seq[0] == 0xedU) return (seq[1] <= 0x9fU); //reserved value in a 3-byte sequence?
        if (seq[0] == 0xf0U) return (seq[1] >= 0x90U); //too many bytes in a 4-byte sequence?
        if (seq[0] == 0xf4U) return (seq[1] <= 0x8fU); //value too big for a 4-byte sequence?
        return (seq[1] >= 0x80U);

    default:
        return false;
    }
}


//
// Return true if given UTF8 multi-byte sequence (up to maxSeqLength bytes
// starting at seq) is valid. A valid UTF8 byte sequence has between
// MinSeqLength and MaxSeqLength bytes, represents a value smaller
// than or equal to MaxValue, and is as short as possible.
//
bool Utf8::isValid234(const utf8_t* seq, size_t maxSeqLength)
{

    // Return immediately if given byte sequence is too short.
    size_t seqLength;
    if ((maxSeqLength == 0) || (maxSeqLength < (seqLength = seqLength_[seq[0]])))
    {
        return false;
    }

    // Basically, subsequent bytes must have range 0x80U..0xbfU. If
    // sequence length is 2, range check is not required since it's
    // already built-in in the sequence length table. If sequence
    // length is more than 2, the sequence is valid only if its value
    // cannot be encoded using fewer bytes. A 1-byte sequence is an
    // ASCII character. Also, check for reserved values in 3-byte
    // sequences and out-of-range values in 4-byte sequences.
    switch (seqLength)
    {
    case 4:
        if ((seq[3] < 0x80U) || (seq[3]>0xbfU))
        {
            return false;
        }
        // no break

    case 3:
        if ((seq[2] < 0x80U) || (seq[2]>0xbfU))
        {
            return false;
        }
        // no break

    case 2:
        if (seq[1] > 0xbfU)
        {
            return false;
        }
        if (seq[0] == 0xe0U) return (seq[1] >= 0xa0U); //too many bytes in a 3-byte sequence?
        if (seq[0] == 0xedU) return (seq[1] <= 0x9fU); //reserved value in a 3-byte sequence?
        if (seq[0] == 0xf0U) return (seq[1] >= 0x90U); //too many bytes in a 4-byte sequence?
        if (seq[0] == 0xf4U) return (seq[1] <= 0x8fU); //too-big values in a 4-byte sequence?
        return (seq[1] >= 0x80U);

    default:
        return false;
    }
}


//
// Decode given UTF8 multi-byte sequence (up to MaxSeqLength bytes starting at
// seq). Assume the sequence has been validated, so minimize error checking.
// If successful, update instance with decoded value and return the number
// of bytes decoded (2, 3, or 4). Otherwise (i.e., given UTF8 byte sequence
// is malformed), return zero.
//
unsigned int Utf8::decode234(const utf8_t seq[MaxSeqLength])
{

    // Each subsequent byte holds 6 bits. The first byte holds up
    // to 5 high-order bits (seqLength=n --> numHighOrderBits=7-n).
    unsigned int seqLength = seqLength_[seq[0]];
    unsigned int value = 0;
    unsigned int delta = 0;
    switch (seqLength)
    {
    case 4:
        value = seq[3] & 0x3fU;
        delta = 6;
        // no break

    case 3:
        value |= (seq[2] & 0x3fU) << delta;
        delta += 6;
        // no break

    case 2:
        value |= (seq[1] & 0x3fU) << delta;
        delta += 6;
        value |= (seq[0] & FIRST_BYTE_D_MASK[seqLength]) << delta;
        c_ = value;
        break;

    default:
        seqLength = 0;
        break;
    }

    return seqLength;
}


//
// Decode given UTF8 multi-byte sequence (up to maxSeqLength bytes starting at seq).
// If successful, update instance with decoded value and return the number
// of bytes decoded (2, 3, or 4). Otherwise (i.e., given UTF8 byte sequence
// is either too short or malformed), return zero.
//
unsigned int Utf8::decode234(const utf8_t* seq, size_t maxSeqLength)
{

    // Return immediately if given byte sequence is too short.
    unsigned int seqLength;
    if ((maxSeqLength == 0) || (maxSeqLength < (seqLength = seqLength_[seq[0]])))
    {
        return 0;
    }

    // Basically, subsequent bytes must have range 0x80U..0xbfU. If
    // sequence length is 2, range check is not required since it's
    // already built-in in the sequence length table. If sequence
    // length is more than 2, the sequence is valid only if its value
    // cannot be encoded using fewer bytes. Also, check for reserved
    // values in 3-byte sequences and out-of-range values in 4-byte
    // sequences. Each subsequent byte holds 6 bits. The first byte
    // holds up to 5 high-order bits (seqLength=n --> numHighOrderBits=7-n).
    unsigned int value = 0;
    unsigned int delta = 0;
    switch (seqLength)
    {
    case 4:
        if ((seq[3] < 0x80U) || (seq[3]>0xbfU))
        {
            return 0;
        }
        value = seq[3] & 0x3fU;
        delta = 6;
        // no break

    case 3:
        if ((seq[2] < 0x80U) || (seq[2]>0xbfU))
        {
            return 0;
        }
        value |= (seq[2] & 0x3fU) << delta;
        delta += 6;
        // no break

    case 2:
        if (seq[1] > 0xbfU)
        {
            return 0;
        }
        value |= (seq[1] & 0x3fU) << delta;
        delta += 6;
        value |= (seq[0] & FIRST_BYTE_D_MASK[seqLength]) << delta;
        // Too many bytes in a 3-byte sequence?
        // Reserved value in a 3-byte sequence?
        // Too many bytes in a 4-byte sequence?
        // Value too big for a 4-byte sequence?
        if (seq[0] == 0xe0U) return (seq[1] >= 0xa0U)? (c_ = value, seqLength): (0);
        if (seq[0] == 0xedU) return (seq[1] <= 0x9fU)? (c_ = value, seqLength): (0);
        if (seq[0] == 0xf0U) return (seq[1] >= 0x90U)? (c_ = value, seqLength): (0);
        if (seq[0] == 0xf4U) return (seq[1] <= 0x8fU)? (c_ = value, seqLength): (0);
        return (seq[1] >= 0x80U)? (c_ = value, seqLength): (0);

    default:
        return 0;
    }
}


//
// Encode non-ASCII value into a UTF8 byte sequence. Save results in seq.
// Assume seq is a buffer large enough for this value. Return the resulting
// sequence length (2, 3, or 4).
//
unsigned int Utf8::encode234(utf8_t seq[MaxSeqLength]) const
{

    // Determine sequence length for a multi-byte sequence.
    unsigned int seqLength;
    if (c_ <= MAX_VALUE_2) seqLength = 2;
    else if (c_ <= MAX_VALUE_3) seqLength = 3;
    else seqLength = 4;

    // Each subsequent byte holds 6 bits. The first byte holds up
    // to 5 high-order bits (seqLength=n --> numHighOrderBits=7-n).
    unsigned int value = c_;
    switch (seqLength)
    {
    case 4:
        seq[3] = static_cast<unsigned char>((value & 0x3fU) | 0x80U);
        value >>= 6;
        // no break

    case 3:
        seq[2] = static_cast<unsigned char>((value & 0x3fU) | 0x80U);
        value >>= 6;
        // no break

    default:
        seq[1] = static_cast<unsigned char>((value & 0x3fU) | 0x80U);
        seq[0] = static_cast<unsigned char>((value >> 6) | FIRST_BYTE_E_MASK[seqLength]);
        break;

    }

    return seqLength;
}


//
// Encode non-ASCII value into a UTF8 byte sequence. Save results in seq. Assume
// seq is a buffer large enough for maxSeqLength bytes. Return the resulting
// sequence length (2, 3, or 4) if successful. Return zero otherwise (i.e., given
// buffer is not large enough).
//
unsigned int Utf8::encode234(utf8_t* seq, size_t maxSeqLength) const
{

    // Determine sequence length for a multi-byte sequence.
    unsigned int seqLength;
    if (c_ <= MAX_VALUE_2) seqLength = 2;
    else if (c_ <= MAX_VALUE_3) seqLength = 3;
    else seqLength = 4;

    // Make sure given buffer is large enough.
    if (maxSeqLength < seqLength)
    {
        return 0;
    }

    // Each subsequent byte holds 6 bits. The first byte holds up
    // to 5 high-order bits (seqLength=n --> numHighOrderBits=7-n).
    unsigned int value = c_;
    switch (seqLength)
    {
    case 4:
        seq[3] = static_cast<unsigned char>((value & 0x3fU) | 0x80U);
        value >>= 6;
        // no break

    case 3:
        seq[2] = static_cast<unsigned char>((value & 0x3fU) | 0x80U);
        value >>= 6;
        // no break

    default:
        seq[1] = static_cast<unsigned char>((value & 0x3fU) | 0x80U);
        seq[0] = static_cast<unsigned char>((value >> 6) | FIRST_BYTE_E_MASK[seqLength]);
        break;

    }

    return seqLength;
}

END_NAMESPACE1
