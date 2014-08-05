/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/Utf16.hpp"
#include "syskit/Utf8.hpp"
#include "syskit/Utf8Seq.hpp"
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(syskit)


//
// Transfer resource ownership.
// Nullify source.
//
Utf8Seq::Utf8Seq(Utf8Seq* seq):
UtfSeq(*seq)
{
    seq_ = seq->seq_;
    seq->seq_ = 0;
}


//!
//! Construct a duplicate instance of the given sequence.
//!
Utf8Seq::Utf8Seq(const Utf8Seq& seq):
UtfSeq(seq)
{
    seq_ = new utf8_t[Utf8Seq::capacity()];
    memcpy(seq_, seq.seq_, byteSize_);
}


//!
//! Construct instance with given subsequence (charCount characters starting at startAt).
//!
Utf8Seq::Utf8Seq(const Utf8Seq& seq, size_t startAt, size_t charCount, unsigned int capacity):
UtfSeq(capacity, 0, 0)
{
    seq_ = new utf8_t[Utf8Seq::capacity()];
    reset(seq, startAt, charCount);
}


//!
//! Construct instance from given ASCII string (length characters starting at s).
//!
Utf8Seq::Utf8Seq(const char* s, size_t length, unsigned int capacity):
UtfSeq(capacity, 0, 0)
{
    seq_ = new utf8_t[Utf8Seq::capacity()];
    reset(s, length);
}


//!
//! Construct an empty sequence. Sequence exponentially grows by doubling.
//! If given capacity is zero, then an initial capacity of one will be used
//! instead. A sequence can hold up to capacity bytes before growth occurs.
//!
Utf8Seq::Utf8Seq(unsigned int capacity):
UtfSeq(capacity, 0, 0)
{
    seq_ = new utf8_t[Utf8Seq::capacity()];
}


//!
//! Take over ownership of given UTF8 data (numU8s bytes starting at s).
//! Given data holds numChars characters suitable for this sequence.
//! Delete data using the delete[] operator when appropriate.
//!
Utf8Seq::Utf8Seq(utf8_t* s, size_t numU8s, size_t numChars):
UtfSeq(static_cast<unsigned int>(numU8s), numU8s, numChars)
{
    seq_ = s;
}


Utf8Seq::~Utf8Seq()
{
    delete[] seq_;
}


//!
//! Append given character to this sequence.
//!
const Utf8Seq& Utf8Seq::operator +=(const Utf8& c)
{
    unsigned int minCap = byteSize_ + Utf8::MaxSeqLength;
    if ((minCap <= capacity()) || growTo(minCap))
    {
        byteSize_ += c.encode(seq_ + byteSize_);
        ++numChars_;
    }

    return *this;
}


//!
//! Append given ASCII character to this sequence.
//!
const Utf8Seq& Utf8Seq::operator +=(char c)
{

    // 7-bit ASCII.
    if (c >= 0)
    {
        unsigned int minCap = byteSize_ + 1;
        if ((minCap <= capacity()) || growTo(minCap))
        {
            seq_[byteSize_] = c;
            ++numChars_;
            ++byteSize_;
        }
    }

    // 8-bit ASCII.
    else
    {
        unsigned int minCap = byteSize_ + 2;
        if ((minCap <= capacity()) || growTo(minCap))
        {
            bool skipValidation = true;
            Utf8 c8(static_cast<unsigned char>(c), skipValidation);
            byteSize_ += c8.encode(seq_ + byteSize_);
            ++numChars_;
        }
    }

    return *this;
}


//!
//! Append given sequence to this sequence.
//! Return reference to self.
//!
const Utf8Seq& Utf8Seq::operator +=(const Utf8Seq& seq)
{
    if (seq.numChars_ > 0)
    {
        unsigned int minCap = byteSize_ + seq.byteSize_;
        if ((minCap <= capacity()) || growTo(minCap))
        {
            memcpy(seq_ + byteSize_, seq.seq_, seq.byteSize_);
            numChars_ += seq.numChars_;
            byteSize_ += seq.byteSize_;
        }
    }

    return *this;
}


const Utf8Seq& Utf8Seq::operator =(const Utf8Seq& seq)
{

    // Prevent self assignment.
    if (this != &seq)
    {

        // Grow to accomodate source.
        if (seq.byteSize_ > capacity())
        {
            delete[] seq_;
            seq_ = new utf8_t[setNextCap(seq.byteSize_)];
        }

        // Copy from given sequence.
        UtfSeq::operator =(seq);
        memcpy(seq_, seq.seq_, byteSize_);
    }

    // Return reference to self.
    return *this;
}


//!
//! Return the character at given index. Behavior is unpredictable if
//! the index is invalid (i.e., if it's greater than or equal to the
//! number of characters in the sequence). This method is implemented
//! by decoding the raw sequence from its beginning/end until the desired
//! character is seen. It can quite inefficient if some character in the
//! sequence uses more than one byte. If most of the characters need to
//! be iterated, use expand() for best performance.
//!
utf32_t Utf8Seq::operator [](size_t index) const
{

    // No decoding required if each character uses just one byte.
    utf32_t c;
    if (numChars_ == byteSize_)
    {
        c = seq_[index];
    }

    // Some decoding required if some character uses more than one byte.
    else
    {
        const utf8_t* p = seek(index);
        Utf8 c8;
        c8.decode(p);
        c = c8.asU32();
    }

    // Return the character at given index.
    return c;
}


//!
//! Clone sequence. Return cloned sequence. The cloned sequence is
//! allocated using the heap and is to be freed by the user using
//! the delete operator.
//!
UtfSeq* Utf8Seq::clone() const
{
    return new Utf8Seq(*this);
}


//!
//! Iterate the sequence from high to low (i.e., right to left). Invoke
//! callback at each character. The callback should return true to continue
//! iterating and should return false to abort iterating. Return false if
//! the callback aborted the iterating. Return true otherwise.
//!
bool Utf8Seq::applyHiToLo(cb0_t cb, void* arg) const
{

    // Assume normal iterating.
    bool ok = true;

    // No decoding required if each character uses just one byte.
    if (numChars_ == byteSize_)
    {
        for (size_t i = numChars_; i-- > 0;)
        {
            if (!cb(arg, i, seq_[i]))
            {
                ok = false;
                break;
            }
        }
    }

    // Some decoding required if some character uses more than one byte.
    else
    {
        Utf8 c;
        const utf8_t* p = seq_ + byteSize_;
        for (size_t i = numChars_; i-- > 0;)
        {
            while (Utf8::getSeqLength(*--p) == Utf8::InvalidByte0);
            c.decode(p);
            if (!cb(arg, i, c.asU32()))
            {
                ok = false;
                break;
            }
        }
    }

    // Return true if the iterating was not aborted.
    return ok;
}


//!
//! Iterate the sequence from low to high (i.e., left to right). Invoke
//! callback at each character. The callback should return true to continue
//! iterating and should return false to abort iterating. Return false if
//! the callback aborted the iterating. Return true otherwise.
//!
bool Utf8Seq::applyLoToHi(cb0_t cb, void* arg) const
{

    // Assume normal iterating.
    bool ok = true;

    // No decoding required if each character uses just one byte.
    if (numChars_ == byteSize_)
    {
        for (size_t i = 0; i < numChars_; ++i)
        {
            if (!cb(arg, i, seq_[i]))
            {
                ok = false;
                break;
            }
        }
    }

    // Some decoding required if some character uses more than one byte.
    else
    {
        Utf8 c;
        const utf8_t* p = seq_;
        for (size_t i = 0; i < numChars_; ++i)
        {
            p += c.decode(p);
            if (!cb(arg, i, c.asU32()))
            {
                ok = false;
                break;
            }
        }
    }

    // Return true if the iterating was not aborted.
    return ok;
}


//!
//! Count the number of UTF8 characters in given UTF8 byte sequence (numU8s
//! bytes starting at s). The counting is done by scanning the byte sequence
//! left to right until all is scanned or until some invalid data is seen.
//! Return true if successful. Return false otherwise (given UTF8 byte sequence
//! is invalid). In either case, set numChars to the number of valid UTF8
//! characters seen in the counting process.
//!
bool Utf8Seq::countChars(const utf8_t* s, size_t numU8s, unsigned int& numChars)
{
    bool ok = true;
    const utf8_t* p = s;
    size_t count = 0;
    for (size_t remaining = numU8s; remaining > 0; ++count)
    {
        if (!Utf8::isValid(p, remaining))
        {
            ok = false;
            break;
        }
        size_t charLength = Utf8::getSeqLength(*p);
        p += charLength;
        remaining -= charLength;
    }

    numChars = static_cast<unsigned int>(count);
    return ok;
}


//!
//! Return true if given UTF8 byte sequence (numU8s bytes starting at
//! s) is valid. The validation is done by scanning the byte sequence
//! left to right until all is scanned or until some invalid data is seen.
//! If user needs to know exactly where validation fails, use a non-zero
//! badSeq value. Upon validation failure, if badSeq is non-zero, *badSeq
//! will contain the location where invalid data is seen. Otherwise,
//! badSeq is not used.
//!
bool Utf8Seq::isValid(const utf8_t* s, size_t numU8s, const utf8_t** badSeq)
{
    bool ok = true;
    const utf8_t* p = s;
    for (size_t remaining = numU8s; remaining > 0;)
    {
        if (!Utf8::isValid(p, remaining))
        {
            ok = false;
            if (badSeq != 0)
            {
                *badSeq = p;
            }
            break;
        }
        size_t charLength = Utf8::getSeqLength(*p);
        p += charLength;
        remaining -= charLength;
    }

    return ok;
}


//!
//! Resize sequence. Given new capacity must be non-zero and must not be less
//! than the current sequence length in bytes. Return true if successful.
//!
bool Utf8Seq::resize(unsigned int newCap)
{
    bool ok;
    if ((newCap == 0) || (newCap < byteSize_))
    {
        ok = false;
    }

    else
    {
        ok = true;
        if (newCap != capacity())
        {
            utf8_t* raw8 = new utf8_t[newCap];
            memcpy(raw8, seq_, byteSize_);
            delete[] seq_;
            seq_ = raw8;
            setCapacity(newCap);
        }
    }

    return ok;
}


//!
//! Truncate sequence if it has more than numChars characters.
//! Return true if truncation occurred.
//!
bool Utf8Seq::truncate(size_t numChars)
{
    bool truncated;
    if (numChars_ <= numChars)
    {
        truncated = false;
    }
    else
    {
        truncated = true;
        if (numChars_ == byteSize_)
        {
            setLength(numChars, numChars);
        }
        else
        {
            const utf8_t* p = seq_ + byteSize_;
            do
            {
                for (--p; Utf8::getSeqLength(*p) == Utf8::InvalidByte0; --p);
            } while (--numChars_ > numChars);
            setByteSize(p - seq_);
        }
    }

    return truncated;
}


//!
//! Seek and return the address of the character residing at given index.
//! Behavior is unpredictable if the index is invalid (i.e., if it's greater
//! than or equal to the number of characters in the sequence). This method
//! can be quite inefficient for non-ASCII data. If most of the characters
//! need to be iterated, use expand() for best performance.
//!
const utf8_t* Utf8Seq::seek(size_t index) const
{

    // No decoding required if each character uses just one byte.
    const utf8_t* p = seq_;
    if (numChars_ == byteSize_)
    {
        p += index;
    }

    // Some decoding required if some character uses more than one byte.
    else if (index > 0)
    {
        if (index <= (numChars_ >> 1))
        {
            for (size_t i = 0; i++ < index; p += Utf8::getSeqLength(*p));
        }
        else
        {
            p += byteSize_;
            size_t i = numChars_;
            do
            {
                while (Utf8::getSeqLength(*--p) == Utf8::InvalidByte0);
            } while (--i > index);
        }
    }

    // Return the address of the character residing at given index.
    return p;
}


//!
//! Compare two Utf8Seq instances given their addresses. Ignore locale. Return
//! a negative value if *item0 < *item1. Return 0 if *item0 == *item1. Return a
//! positive value if *item0 > *item1.
//!
int Utf8Seq::compareP(const void* item0, const void* item1)
{
    const Utf8Seq& k0 = *static_cast<const Utf8Seq*>(item0);
    const Utf8Seq& k1 = *static_cast<const Utf8Seq*>(item1);
    int rc;
    if (k0.byteSize_ < k1.byteSize_)
    {
        rc = memcmp(k0.seq_, k1.seq_, k0.byteSize_);
        if (rc == 0)
        {
            rc = -1;
        }
    }
    else if (k0.byteSize_ > k1.byteSize_)
    {
        rc = memcmp(k0.seq_, k1.seq_, k1.byteSize_);
        if (rc == 0)
        {
            rc = 1;
        }
    }
    else
    {
        rc = memcmp(k0.seq_, k1.seq_, k0.byteSize_);
    }

    return rc;
}


//!
//! Compare two Utf8Seq instances given their addresses. Ignore locale. Reverse
//! the normal sense of comparison. Return a positive value if *item0 < *item1.
//! Return 0 if *item0 == *item1. Return a negative value if *item0 > *item1.
//!
int Utf8Seq::comparePR(const void* item0, const void* item1)
{
    int rc = compareP(item0, item1);
    return -rc;
}


//!
//! Convert from given UTF16 data (numU16s shorts starting at s). Save
//! result in this instance. Given data may contain some invalid byte
//! sequence. For each invalid character seen, use defaultChar as its
//! replacement. Return the number of invalid characters seen.
//!
unsigned int Utf8Seq::convert16(const utf16_t* s, size_t numU16s, utf32_t defaultChar)
{
    reset();
    if (numU16s == 0)
    {
        return 0;
    }

    // Prepare worst-case buffer.
    unsigned int minCap = static_cast<unsigned int>(numU16s)* 2 * Utf8::MaxSeqLength;
    unsigned int oldCap = growBuf(minCap);
    unsigned int invalidCharCount = 0;
    size_t charCount = numU16s;
    const utf16_t* p16 = s;
    const utf16_t* p16End = p16 + charCount;

    // Iterate through each UTF16 character.
    // Plenty of room in existing buffer for encoding.
    Utf8 default8(defaultChar);
    utf8_t* p8 = seq_;
    for (Utf8 c; p16 < p16End; p8 += c.encode(p8))
    {
        if ((*p16 >= Utf16::HiHalfMin) && (*p16 <= Utf16::LoHalfMax))
        {
            if ((p16 + 1 < p16End) && (*p16 <= Utf16::HiHalfMax) &&
                (p16[1] >= Utf16::LoHalfMin) && (p16[1] <= Utf16::LoHalfMax))
            {
                utf32_t value = Utf16::convertSurrogate(p16);
                c.resetWithValidChar(value);
                --charCount;
            }
            else
            {
                c = default8;
                ++invalidCharCount;
            }
            p16 += 2;
        }
        else
        {
            c.resetWithValidChar(*p16++);
        }
    }

    // Summarize result.
    // Return number of invalid characters seen.
    setLength(p8 - seq_, charCount);
    shrinkBuf(oldCap);
    return invalidCharCount;
}


//!
//! Convert from given UTF16 data (numU16s non-native-endian shorts starting at
//! s). Save result in this instance. Given data may contain some invalid byte
//! sequence. For each invalid character seen, use defaultChar as its replacement.
//! Return the number of invalid characters seen.
//!
unsigned int Utf8Seq::convert61(const utf16_t* s, size_t numU16s, utf32_t defaultChar)
{
    reset();
    if (numU16s == 0)
    {
        return 0;
    }

    // Prepare worst-case buffer.
    unsigned int minCap = static_cast<unsigned int>(numU16s)* 2 * Utf8::MaxSeqLength;
    unsigned int oldCap = growBuf(minCap);
    unsigned int invalidCharCount = 0;
    size_t charCount = numU16s;
    const utf16_t* p61 = s;
    const utf16_t* p61End = p61 + charCount;

    // Iterate through each UTF16 character.
    // Plenty of room in existing buffer for encoding.
    Utf8 default8(defaultChar);
    utf8_t* p8 = seq_;
    for (Utf8 c; p61 < p61End; p8 += c.encode(p8))
    {
        unsigned short u16A = bswap16(*p61);
        if ((u16A >= Utf16::HiHalfMin) && (u16A <= Utf16::LoHalfMax))
        {
            if ((p61 + 1 < p61End) && (u16A <= Utf16::HiHalfMax))
            {
                unsigned short u16B = bswap16(p61[1]);
                if ((u16B >= Utf16::LoHalfMin) && (u16B <= Utf16::LoHalfMax))
                {
                    utf32_t value = Utf16::convertSurrogate(u16A, u16B);
                    c.resetWithValidChar(value);
                    --charCount;
                    p61 += 2;
                    continue;
                }
            }
            c = default8;
            ++invalidCharCount;
            p61 += 2;
        }
        else
        {
            c.resetWithValidChar(u16A);
            ++p61;
        }
    }

    // Summarize result.
    // Return number of invalid characters seen.
    setLength(p8 - seq_, charCount);
    shrinkBuf(oldCap);
    return invalidCharCount;
}


//!
//! Convert from given UTF8 data (numU8s bytes starting at s). Save
//! result in this instance. Given data may contain some invalid byte
//! sequence. For each invalid character seen, use defaultChar as its
//! replacement. Return the number of invalid characters seen.
//!
unsigned int Utf8Seq::convert8(const utf8_t* s, size_t numU8s, utf32_t defaultChar)
{
    reset();
    if (numU8s == 0)
    {
        return 0;
    }

    // Assume no errors (most likely scenario).
    unsigned int minCap = static_cast<unsigned int>(numU8s);
    if (countChars(s, numU8s, numChars_))
    {
        if ((minCap <= capacity()) || growTo(minCap))
        {
            memcpy(seq_, s, numU8s);
            setByteSize(numU8s);
        }
        return 0;
    }

    // Prepare worst-case buffer.
    minCap *= Utf8::MaxSeqLength;
    unsigned int oldCap = growBuf(minCap);
    unsigned int invalidCharCount = 0;
    size_t charCount = 0;
    size_t remaining = numU8s;
    const utf8_t* p8 = s;

    // Iterate through each UTF8 character.
    // Plenty of room in existing buffer for encoding.
    Utf8 default8(defaultChar);
    utf8_t* p = seq_;
    for (Utf8 c; remaining > 0; ++charCount)
    {
        size_t length = c.decode(p8, remaining);
        switch (length)
        {
        case 4: *p++ = *p8++; --remaining; //no break
        case 3: *p++ = *p8++; --remaining; //no break
        case 2: *p++ = *p8++; --remaining; //no break
        case 1: *p++ = *p8++; --remaining; break;
        default: //0
            ++invalidCharCount;
            p += default8.encode(p);
            do
            {
                length = Utf8::getSeqLength(*++p8);
                --remaining;
            } while (((length < Utf8::MinSeqLength) || (length > Utf8::MaxSeqLength)) && (remaining > 0));
            break;
        }
    }

    // Summarize result.
    // Return number of invalid characters seen.
    setLength(p - seq_, charCount);
    shrinkBuf(oldCap);
    return invalidCharCount;
}


//!
//! Shrink given array into this sequence. Given array holds numChars
//! characters, some of which might be unsuitable for this sequence.
//! For each invalid character seen, use defaultChar as its replacement.
//! Return the number of invalid characters seen.
//!
unsigned int Utf8Seq::shrink(const utf32_t* s, size_t numChars, utf32_t defaultChar)
{
    reset();
    if (numChars == 0)
    {
        return 0;
    }

    // Prepare worst-case buffer.
    unsigned int minCap = static_cast<unsigned int>(numChars)* Utf8::MaxSeqLength;
    unsigned int oldCap = growBuf(minCap);
    unsigned int invalidCharCount = 0;
    const utf32_t* p32 = s;
    const utf32_t* p32End = p32 + numChars;

    // Iterate through each UTF32 character.
    // Plenty of room in existing buffer for encoding.
    Utf8 default8(defaultChar);
    utf8_t* p8 = seq_;
    for (Utf8 c; p32 < p32End; ++p32, p8 += c.encode(p8))
    {
        if (Utf8::isValid(*p32))
        {
            c.resetWithValidChar(*p32);
        }
        else
        {
            ++invalidCharCount;
            c = default8;
        }
    }

    // Summarize result.
    // Return number of invalid characters seen.
    setLength(p8 - seq_, numChars);
    shrinkBuf(oldCap);
    return invalidCharCount;
}


//!
//! Detach raw UTF8 sequence from instance. The return sequence is allocated
//! from the heap and is to be freed by the caller using the delete[] operator
//! when done. Instance is now disabled and must not be used further.
//!
utf8_t* Utf8Seq::detachRaw()
{
    utf8_t* raw = seq_;
    seq_ = 0;
    reset();
    setCapacity(0xffffffffUL);

    return raw;
}


//!
//! Expand into an array of 32-bit values. Return the resulting array.
//! The resulting array is allocated using the heap and is to be freed
//! by the user using the delete[] operator. The resulting array has
//! numChars() 32-bit values.
//!
utf32_t* Utf8Seq::expand() const
{
    utf32_t* s = new utf32_t[numChars_];
    return expand(s);
}


//!
//! Expand into given array. Return the given array. The given array s
//! is allocated by the user and must be big enough to hold numChars()
//! 32-bit values.
//!
utf32_t* Utf8Seq::expand(utf32_t* s) const
{

    // No decoding required if each character uses just one byte.
    const utf8_t* p = seq_;
    const utf8_t* pEnd = p + byteSize_;
    if (numChars_ == byteSize_)
    {
        for (; p < pEnd; *s++ = *p++);
    }

    // Some decoding required if some character uses more than one byte.
    else
    {
        for (Utf8 c; p < pEnd; *s++ = c.asU32())
        {
            p += c.decode(p);
        }
    }

    // Return the given array.
    return s - numChars_;
}


//!
//! Append given subsequence (charCount characters starting at startAt) to this sequence.
//!
void Utf8Seq::append(const Utf8Seq& seq, size_t startAt, size_t charCount)
{
    if (charCount > 0)
    {
        unsigned int size0;
        unsigned int size1;
        seq.getByteSizes(startAt, charCount, size0, size1);
        unsigned int minCap = byteSize_ + size1;
        if ((minCap <= capacity()) || growTo(minCap))
        {
            memcpy(seq_ + byteSize_, seq.seq_ + size0, size1);
            setLength(byteSize_ + size1, numChars_ + charCount);
        }
    }
}


//!
//! Append given ASCII string (length characters starting at s) to this sequence.
//!
void Utf8Seq::append(const char* s, size_t length)
{
    if (length == 0)
    {
        return;
    }

    // Assume given string has only 7-bit ASCII characters.
    unsigned int minCap = static_cast<unsigned int>(byteSize_ + length);
    if ((minCap > capacity()) && (!growTo(minCap)))
    {
        return;
    }

    const unsigned char* p = reinterpret_cast<const unsigned char*>(s);
    const unsigned char* pEnd = p + length;
    utf8_t* p8 = seq_ + byteSize_;
    do
    {
        if (*p > Utf8::MaxAscii) //but given string has at least one 8-bit ASCII character...
        {
            size_t delta = p8 - seq_ - byteSize_;
            setLength(byteSize_ + delta, numChars_ + delta);
            appendAscii8(p, length - delta);
            return;
        }
        *p8++ = *p;
    } while (++p < pEnd);
    setLength(byteSize_ + length, numChars_ + length);
}


//!
//! Append given UTF8 data (numU8s bytes starting at s) to this sequence.
//! Given data holds numChars characters suitable for this sequence.
//!
void Utf8Seq::append(const utf8_t* s, size_t numU8s, size_t numChars)
{
    if (numChars > 0)
    {
        unsigned int minCap = static_cast<unsigned int>(byteSize_ + numU8s);
        if ((minCap <= capacity()) || growTo(minCap))
        {
            memcpy(seq_ + byteSize_, s, numU8s);
            setLength(byteSize_ + numU8s, numChars_ + numChars);
        }
    }
}


//!
//! Append given ASCII character to this sequence count times.
//!
void Utf8Seq::append(size_t count, char c)
{
    if (count == 0)
    {
        return;
    }

    // 7-bit ASCII.
    if (c >= 0)
    {
        unsigned int minCap = static_cast<unsigned int>(byteSize_ + count);
        if ((minCap <= capacity()) || growTo(minCap))
        {
            memset(seq_ + byteSize_, c, count);
            setLength(byteSize_ + count, numChars_ + count);
        }
    }

    // 8-bit ASCII.
    else
    {
        unsigned int minCap = static_cast<unsigned int>(byteSize_ + count * 2);
        if ((minCap <= capacity()) || growTo(minCap))
        {
            bool skipValidation = true;
            Utf8 c8(static_cast<unsigned char>(c), skipValidation);
            utf8_t a8[2]; //8-bit ASCII character uses 2 bytes
            c8.encode(a8);
            utf8_t* p8 = seq_ + byteSize_;
            for (const utf8_t* p8End = p8 + count * 2; p8 < p8End; p8 += 2)
            {
                p8[0] = a8[0];
                p8[1] = a8[1];
            }
            setLength(minCap, numChars_ + count);
        }
    }
}


//
// Append given 8-bit ASCII string (non-zero length characters starting at s) to this sequence.
//
void Utf8Seq::appendAscii8(const unsigned char* s, size_t length)
{
    unsigned int minCap = static_cast<unsigned int>(byteSize_ + length * 2); //assume worst-case (2 bytes per 8-bit ASCII character)
    if ((minCap <= capacity()) || growTo(minCap))
    {
        utf8_t* p8 = seq_ + byteSize_;
        const unsigned char* p = s;
        const unsigned char* pEnd = p + length;
        for (Utf8 c; p < pEnd; p8 += c.encode(p8))
        {
            c.resetWithValidChar(*p++);
        }
        setLength(p8 - seq_, numChars_ + length);
    }
}


//!
//! Iterate the sequence from high to low (i.e., right to left).
//! Invoke callback at each character.
//!
void Utf8Seq::applyHiToLo(cb1_t cb, void* arg) const
{

    // No decoding required if each character uses just one byte.
    if (numChars_ == byteSize_)
    {
        for (size_t i = numChars_; i-- > 0;)
        {
            cb(arg, i, seq_[i]);
        }
    }

    // Some decoding required if some character uses more than one byte.
    else
    {
        Utf8 c;
        const utf8_t* p = seq_ + byteSize_;
        for (size_t i = numChars_; i-- > 0;)
        {
            while (Utf8::getSeqLength(*--p) == Utf8::InvalidByte0);
            c.decode(p);
            cb(arg, i, c.asU32());
        }
    }
}


//!
//! Iterate the sequence from low to high (i.e., left to right).
//! Invoke callback at each character.
//!
void Utf8Seq::applyLoToHi(cb1_t cb, void* arg) const
{

    // No decoding required if each character uses just one byte.
    if (numChars_ == byteSize_)
    {
        for (size_t i = 0; i < numChars_; ++i)
        {
            cb(arg, i, seq_[i]);
        }
    }

    // Some decoding required if some character uses more than one byte.
    else
    {
        Utf8 c;
        const utf8_t* p = seq_;
        for (size_t i = 0; i < numChars_; ++i)
        {
            p += c.decode(p);
            cb(arg, i, c.asU32());
        }
    }
}


//!
//! Take over ownership of given UTF8 data (numU8s bytes starting at s).
//! Given data holds numChars characters suitable for this sequence.
//! Delete data using the delete[] operator when appropriate.
//!
void Utf8Seq::attachRaw(utf8_t* s, size_t numU8s, size_t numChars)
{
    delete[] seq_;
    seq_ = s;
    setLength(numU8s, numChars);
    setCapacity(byteSize_);
}


//!
//! Compute the byte sizes identifying the given subsequence. Save the
//! byte size of characters up to startAt in size0. Save the byte size
//! of charCount characters starting at startAt in size1.
//!
void Utf8Seq::getByteSizes(size_t startAt, size_t charCount, unsigned int& size0, unsigned int& size1) const
{
    if ((startAt < numChars_) && (charCount <= (numChars_ - startAt)))
    {
        if (numChars_ == byteSize_)
        {
            size0 = static_cast<unsigned int>(startAt);
            size1 = static_cast<unsigned int>(charCount);
        }
        else
        {
            const utf8_t* p1 = seq_;
            if (startAt <= (numChars_ >> 1))
            {
                for (size_t i = 0; i++ < startAt; p1 += Utf8::getSeqLength(*p1));
            }
            else
            {
                p1 += byteSize_;
                size_t i = numChars_;
                do
                {
                    while (Utf8::getSeqLength(*--p1) == Utf8::InvalidByte0);
                } while (--i > startAt);
            }
            const utf8_t* p2 = p1;
            for (size_t i = 0; i++ < charCount; p2 += Utf8::getSeqLength(*p2));
            size0 = static_cast<unsigned int>(p1 - seq_);
            size1 = static_cast<unsigned int>(p2 - p1);
        }
    }

    else
    {
        size0 = 0;
        size1 = 0;
    }
}


//!
//! Reset instance with given subsequence (charCount characters starting at startAt).
//!
void Utf8Seq::reset(const Utf8Seq& seq, size_t startAt, size_t charCount)
{

    // Same as reset.
    if (charCount == 0)
    {
        reset();
    }

    // Different instance.
    else if (this != &seq)
    {
        unsigned int size0;
        unsigned int size1;
        seq.getByteSizes(startAt, charCount, size0, size1);
        reset();
        if ((size1 > 0) && ((size1 <= capacity()) || growTo(size1)))
        {
            memcpy(seq_, seq.seq_ + size0, size1);
            setLength(size1, charCount);
        }
    }

    // Same instance. Subsequence.
    else if ((startAt > 0) || (charCount != numChars_))
    {
        unsigned int size0;
        unsigned int size1;
        seq.getByteSizes(startAt, charCount, size0, size1);
        reset();
        if (size1 > 0)
        {
            memmove(seq_, seq.seq_ + size0, size1);
            setLength(size1, charCount);
        }
    }
}


//!
//! Reset instance with given ASCII string (length characters starting at s).
//!
void Utf8Seq::reset(const char* s, size_t length)
{
    reset();
    if (length == 0)
    {
        return;
    }

    // Assume given string has only 7-bit ASCII characters.
    unsigned int minCap = static_cast<unsigned int>(length);
    if ((minCap > capacity()) && (!growTo(minCap)))
    {
        return;
    }

    const unsigned char* p = reinterpret_cast<const unsigned char*>(s);
    const unsigned char* pEnd = p + length;
    utf8_t* p8 = seq_;
    do
    {
        if (*p > Utf8::MaxAscii) //but given string has at least one 8-bit ASCII character...
        {
            size_t n = p8 - seq_;
            setLength(n, n);
            appendAscii8(p, length - numChars_);
            return;
        }
        *p8++ = *p;
    } while (++p < pEnd);
    setLength(length, length);
}


//!
//! Reset instance with UTF8 data (numU8s bytes starting at s).
//! Given data holds numChars characters suitable for this sequence.
//!
void Utf8Seq::reset(const utf8_t* s, size_t numU8s, size_t numChars)
{
    reset();
    unsigned int minCap = static_cast<unsigned int>(numU8s);
    if ((numChars > 0) && ((minCap <= capacity()) || growTo(minCap)))
    {
        memcpy(seq_, s, numU8s);
        setLength(numU8s, numChars);
    }
}


//
// Reset instance with UTF16 data (numChars shorts starting at s).
// Given data holds numChars characters suitable for this sequence.
//
void Utf8Seq::reset16(const utf16_t* s, size_t numChars)
{
    reset();
    if (numChars == 0)
    {
        return;
    }

    // Prepare worst-case buffer.
    unsigned int minCap = static_cast<unsigned int>(numChars)* Utf8::MaxSeqLength;
    unsigned int oldCap = growBuf(minCap);
    const utf16_t* p16 = s;
    const utf16_t* p16End = p16 + numChars;

    // Iterate through each UTF16 character.
    // Plenty of room in existing buffer for encoding.
    utf8_t* p8 = seq_;
    for (Utf8 c; p16 < p16End; p8 += c.encode(p8))
    {
        c.resetWithValidChar(*p16++);
    }

    // Summarize result.
    setLength(p8 - seq_, numChars);
    shrinkBuf(oldCap);
}


//
// Reset instance with UTF16 data (numU16s shorts starting at s).
// Given data holds numChars characters suitable for this sequence.
// Assume given data is non-empty and is valid. If either size is unknown,
// zero is allowed, but at least one size must be non-zero. That is,
// behavior is unpredictable if both numU16s and numChars are zeroes.
//
void Utf8Seq::reset16(const utf16_t* s, size_t numU16s, size_t numChars)
{
    reset();
    const utf16_t* p16 = s;

    // Given data has numChars characters.
    // Prepare worst-case buffer.
    // Iterate through each UTF16 character.
    unsigned int oldCap;
    utf8_t* p8;
    if (numU16s == 0)
    {
        unsigned int minCap = static_cast<unsigned int>(numChars)* Utf8::MaxSeqLength;
        oldCap = growBuf(minCap);
        p8 = seq_;
        size_t i = 0;
        for (Utf8 c; i < numChars; ++i, p8 += c.encode(p8))
        {
            if ((*p16 >= Utf16::HiHalfMin) && (*p16 <= Utf16::LoHalfMax))
            {
                utf32_t value = Utf16::convertSurrogate(p16);
                c.resetWithValidChar(value);
                p16 += 2;
            }
            else
            {
                c.resetWithValidChar(*p16++);
            }
        }
    }

    // Given data has numU16s shorts.
    // Prepare worst-case buffer.
    // Iterate through each UTF16 character.
    else
    {
        unsigned int minCap = static_cast<unsigned int>(numU16s)* Utf8::MaxSeqLength;
        oldCap = growBuf(minCap);
        p8 = seq_;
        numChars = 0;
        const utf16_t* p16End = p16 + numU16s;
        for (Utf8 c; p16 < p16End; ++numChars, p8 += c.encode(p8))
        {
            if ((*p16 >= Utf16::HiHalfMin) && (*p16 <= Utf16::LoHalfMax))
            {
                utf32_t value = Utf16::convertSurrogate(p16);
                c.resetWithValidChar(value);
                p16 += 2;
            }
            else
            {
                c.resetWithValidChar(*p16++);
            }
        }
    }

    // Summarize result.
    setLength(p8 - seq_, numChars);
    shrinkBuf(oldCap);
}


//!
//! Shrink given array into this sequence. Given array holds numChars
//! characters suitable for this sequence. Behavior is unpredictable
//! if it contains unsuitable characters. Use the other shrink() method
//! if the given array might have unsuitable characters.
//!
void Utf8Seq::shrink(const utf32_t* s, size_t numChars)
{
    reset();
    if (numChars == 0)
    {
        return;
    }

    // Prepare worst-case buffer.
    unsigned int minCap = static_cast<unsigned int>(numChars)* Utf8::MaxSeqLength;
    unsigned int oldCap = growBuf(minCap);
    const utf32_t* p32 = s;
    const utf32_t* p32End = p32 + numChars;

    // Iterate through each UTF32 character.
    // Plenty of room in existing buffer for encoding.
    utf8_t* p8 = seq_;
    for (Utf8 c; p32 < p32End; p8 += c.encode(p8))
    {
        c.resetWithValidChar(*p32++);
    }

    // Summarize result.
    setLength(p8 - seq_, numChars);
    shrinkBuf(oldCap);
}


void Utf8Seq::shrinkBuf(unsigned int oldCap)
{
    for (unsigned int cap = oldCap; cap > 0; cap <<= 1)
    {
        if (cap >= byteSize_)
        {
            resize(cap);
            break;
        }
    }
}


//!
//! Construct an unattached Utf8Seq iterator.
//!
Utf8Seq::Itor::Itor()
{
    copyMade_ = false;
    curIndex_ = INVALID_INDEX;
    p1_ = 0;
    p2_ = 0;
    seq_ = 0;
}


//!
//! Construct a Utf8Seq iterator. Attach iterator to given sequence. A
//! deep copy of the given sequence is made if makeCopy is true. Normally,
//! a deep copy should be made only if the given sequence can change during
//! iterating.
//!
Utf8Seq::Itor::Itor(const Utf8Seq& seq, bool makeCopy)
{
    copyMade_ = false;
    p1_ = 0;
    p2_ = 0;
    seq_ = 0;

    // attach() is also responsible for resetting the iterator and
    // that's why curIndex_ does not have to be initialized here.
    attach(seq, makeCopy);
}


Utf8Seq::Itor::~Itor()
{
    detach();
}


//!
//! Retrieve the next character iterating left to right. Return true and
//! the result in c if there's one. Otherwise, return false and zero in c
//! (if sequence is empty or if there's no more characters). Behavior is
//! unpredictable if the iterator is unattached. The first invocation after
//! construction or reset() will return the left-most character.
//!
bool Utf8Seq::Itor::next(utf32_t& c)
{

    // Assume unsuccessful.
    bool ok = false;
    c = 0;

    // Retrieve left-most character.
    if (curIndex_ == INVALID_INDEX)
    {
        if (seq_->numChars_ > 0)
        {
            ok = true;
            curIndex_ = 0;
            p1_ = seq_->seq_;
            Utf8 c8;
            p2_ = p1_ + c8.decode(p1_) - 1;
            c = c8.asU32();
        }
    }

    // Retrieve right character.
    else if (++curIndex_ < seq_->numChars_)
    {
        ok = true;
        p1_ = p2_ + 1;
        Utf8 c8;
        p2_ = p1_ + c8.decode(p1_) - 1;
        c = c8.asU32();
    }

    // There's no more characters.
    else
    {
        curIndex_ = seq_->numChars_ - 1;
    }

    // Return true if successful.
    return ok;
}


//!
//! Peek at the left character. Return true and the result in c if there's one.
//! Return false and zero in c otherwise (if string is empty or if peeking from
//! the left-most character). Behavior is unpredictable if the iterator is
//! unattached. The first invocation after construction or reset() will return
//! the right-most character.
//!
bool Utf8Seq::Itor::peekLeft(utf32_t& c) const
{

    // Assume unsuccessful.
    bool ok = false;
    c = 0;

    // Peek at right-most character.
    if (curIndex_ == INVALID_INDEX)
    {
        if (seq_->numChars_ > 0)
        {
            ok = true;
            const utf8_t* p = seq_->seq_ + seq_->byteSize_ - 1;
            for (; Utf8::getSeqLength(*p) == Utf8::InvalidByte0; --p);
            Utf8 c8;
            c8.decode(p);
            c = c8.asU32();
        }
    }

    // Peek at left character.
    else if ((curIndex_ - 1) < seq_->numChars_)
    {
        ok = true;
        const utf8_t* p = p1_ - 1;
        for (; Utf8::getSeqLength(*p) == Utf8::InvalidByte0; --p);
        Utf8 c8;
        c8.decode(p);
        c = c8.asU32();
    }

    // Return true if successful.
    return ok;
}


//!
//! Peek at the right character. Return true and the result in c if there's one.
//! Return false and zero in c otherwise (if string is empty or if peeking from
//! the right-most character). Behavior is unpredictable if the iterator is
//! unattached. The first invocation after construction or reset() will return
//! the left-most character.
//!
bool Utf8Seq::Itor::peekRight(utf32_t& c) const
{

    // Assume unsuccessful.
    bool ok = false;
    c = 0;

    // Peek at left-most character.
    if (curIndex_ == INVALID_INDEX)
    {
        if (seq_->numChars_ > 0)
        {
            ok = true;
            Utf8 c8;
            c8.decode(seq_->seq_);
            c = c8.asU32();
        }
    }

    // Peek at right character.
    else if ((curIndex_ + 1) < seq_->numChars_)
    {
        ok = true;
        Utf8 c8;
        c8.decode(p2_ + 1);
        c = c8.asU32();
    }

    // Return true if successful.
    return ok;
}


//!
//! Retrieve the previous character iterating right to left. Return true
//! and the result in c if there's one. Otherwise, return false and zero
//! in c (if sequence is empty or if there's no more characters). Behavior
//! is unpredictable if the iterator is unattached. The first invocation
//! after construction or reset() will return the right-most character.
//!
bool Utf8Seq::Itor::prev(utf32_t& c)
{

    // Assume unsuccessful.
    bool ok = false;
    c = 0;

    // Retrieve right-most character.
    if (curIndex_ == INVALID_INDEX)
    {
        if (seq_->numChars_ > 0)
        {
            ok = true;
            curIndex_ = seq_->numChars_ - 1;
            p2_ = seq_->seq_ + seq_->byteSize_ - 1;
            for (p1_ = p2_; Utf8::getSeqLength(*p1_) == Utf8::InvalidByte0; --p1_);
            Utf8 c8;
            c8.decode(p1_);
            c = c8.asU32();
        }
    }

    // Retrieve left character.
    else if (--curIndex_ < seq_->numChars_)
    {
        ok = true;
        p2_ = p1_ - 1;
        for (p1_ = p2_; Utf8::getSeqLength(*p1_) == Utf8::InvalidByte0; --p1_);
        Utf8 c8;
        c8.decode(p1_);
        c = c8.asU32();
    }

    // There's no more characters.
    else
    {
        curIndex_ = 0;
    }

    // Return true if successful.
    return ok;
}


//!
//! Attach iterator to given sequence. A deep copy of the given sequence
//! is made if makeCopy is true. Normally, a deep copy should be made only
//! if the given sequence can change during iterating.
//!
void Utf8Seq::Itor::attach(const Utf8Seq& seq, bool makeCopy)
{

    // Destroy old copy.
    if (copyMade_)
    {
        delete seq_;
    }

    // Make private copy of given sequence if necessary.
    seq_ = makeCopy? new Utf8Seq(seq): &seq;
    copyMade_ = makeCopy;

    // Reset iterator.
    reset();
}


//!
//! Detach from any attached sequence. Ignore if iterator is unattached.
//!
void Utf8Seq::Itor::detach()
{
    if (copyMade_)
    {
        delete seq_;
        copyMade_ = false;
    }

    seq_ = 0;
}

END_NAMESPACE1
