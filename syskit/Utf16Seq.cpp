/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/Utf16.hpp"
#include "syskit/Utf16Seq.hpp"
#include "syskit/Utf8.hpp"
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(syskit)


//
// Transfer resource ownership. Nullify source.
//
Utf16Seq::Utf16Seq(Utf16Seq* seq):
UtfSeq(*seq)
{
    numU16s_ = seq->numU16s_;
    seq_ = seq->seq_;
    seq->seq_ = 0;
}


//!
//! Construct a duplicate instance of the given sequence.
//!
Utf16Seq::Utf16Seq(const Utf16Seq& seq):
UtfSeq(seq)
{
    numU16s_ = seq.numU16s_;
    seq_ = new utf16_t[capacity()];
    memcpy(seq_, seq.seq_, byteSize_);
}


//!
//! Construct an empty sequence. Sequence exponentially grows by doubling.
//! If given capacity is zero, then an initial capacity of one will be used
//! instead.
//!
Utf16Seq::Utf16Seq(unsigned int capacity):
UtfSeq(capacity, 0, 0)
{
    numU16s_ = 0;
    seq_ = new utf16_t[Utf16Seq::capacity()];
}


//!
//! Take over ownership of given UTF16 data (numU16s shorts starting at s).
//! Given data holds numChars characters suitable for this sequence.
//! Delete data using the delete[] operator when appropriate.
//!
Utf16Seq::Utf16Seq(utf16_t* s, size_t numU16s, size_t numChars):
UtfSeq(static_cast<unsigned int>(numU16s), numU16s << 1, numChars)
{
    numU16s_ = static_cast<unsigned int>(numU16s);
    seq_ = s;
}


Utf16Seq::~Utf16Seq()
{
    delete[] seq_;
}


//!
//! Append given sequence to this one.
//! Return reference to self.
//!
const Utf16Seq& Utf16Seq::operator +=(const Utf16Seq& seq)
{
    if (seq.numChars_ > 0)
    {
        unsigned int minCap = numU16s_ + seq.numU16s_;
        if ((minCap <= capacity()) || growTo(minCap))
        {
            memcpy(seq_ + numU16s_, seq.seq_, seq.byteSize_);
            byteSize_ += seq.byteSize_;
            numChars_ += seq.numChars_;
            numU16s_ += seq.numU16s_;
        }
    }

    return *this;
}


//!
//! Assignment operator.
//!
const Utf16Seq& Utf16Seq::operator =(const Utf16Seq& seq)
{

    // Prevent self assignment.
    if (this != &seq)
    {

        // Grow to accomodate source.
        if (seq.numU16s_ > capacity())
        {
            delete[] seq_;
            seq_ = new utf16_t[setNextCap(seq.numU16s_)];
        }

        // Copy from given sequence.
        UtfSeq::operator =(seq);
        numU16s_ = seq.numU16s_;
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
//! sequence uses more than one short. If most of the characters need to
//! be iterated, use expand() for best performance.
//!
utf32_t Utf16Seq::operator [](size_t index) const
{

    // No decoding required if each character uses just one short.
    utf32_t c;
    if (numChars_ == numU16s_)
    {
        c = seq_[index];
    }

    // Some decoding required if some character uses more than one short.
    else
    {
        const utf16_t* p;
        if (index <= (numChars_ >> 1))
        {
            for (p = seq_; index > 0; --index, p += ((*p >= Utf16::HiHalfMin) && (*p <= Utf16::LoHalfMax))? 2: 1);
        }
        else
        {
            p = seq_ + numU16s_;
            size_t i = numChars_;
            do
            {
                p -= ((p[-1] >= Utf16::LoHalfMin) && (p[-1] <= Utf16::LoHalfMax))? 2: 1;
            } while (--i > index);
        }
        c = Utf16::convertValidSeq(p);
    }

    // Return the character at given index.
    return c;
}


//!
//! Clone sequence. Return cloned sequence. The cloned sequence is
//! allocated using the heap and is to be freed by the user using
//! the delete operator.
//!
UtfSeq* Utf16Seq::clone() const
{
    return new Utf16Seq(*this);
}


//!
//! Iterate the sequence from high to low (i.e., right to left). Invoke
//! callback at each character. The callback should return true to continue
//! iterating and should return false to abort iterating. Return false if
//! the callback aborted the iterating. Return true otherwise.
//!
bool Utf16Seq::applyHiToLo(cb0_t cb, void* arg) const
{

    // Assume normal iterating.
    bool ok = true;

    // No decoding required if each character uses just one short.
    if (numChars_ == numU16s_)
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
        const utf16_t* p = seq_ + numU16s_;
        for (size_t i = numChars_; i-- > 0;)
        {
            p -= ((p[-1] >= Utf16::LoHalfMin) && (p[-1] <= Utf16::LoHalfMax))? 2: 1;
            utf32_t c = Utf16::convertValidSeq(p);
            if (!cb(arg, i, c))
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
bool Utf16Seq::applyLoToHi(cb0_t cb, void* arg) const
{

    // Assume normal iterating.
    bool ok = true;

    // No decoding required if each character uses just one short.
    if (numChars_ == numU16s_)
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

    // Some decoding required if some character uses more than one short.
    else
    {
        const utf16_t* p = seq_;
        for (size_t i = 0; i < numChars_; ++i)
        {
            utf32_t c;
            p += Utf16::decodeValidSeq(p, c);
            if (!cb(arg, i, c))
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
//! Count the number of UTF16 characters in given UTF16 byte sequence (numU16s
//! shorts starting at s). The counting is done by scanning the byte sequence
//! left to right until all is scanned or until some invalid data is seen. Return
//! true if successful. Return false otherwise (given UTF16 byte sequence is
//! invalid). In either case, set numChars to the number of valid UTF16
//! characters seen in the counting process.
//!
bool Utf16Seq::countChars(const utf16_t* s, size_t numU16s, unsigned int& numChars)
{
    bool ok = true;
    const utf16_t* p16 = s;
    size_t count = 0;
    for (const utf16_t* p16End = p16 + numU16s; p16 < p16End; ++count)
    {
        if ((*p16 >= Utf16::HiHalfMin) && (*p16 <= Utf16::LoHalfMax))
        {
            if ((p16 + 1 < p16End) && (*p16 <= Utf16::HiHalfMax) &&
                (p16[1] >= Utf16::LoHalfMin) && (p16[1] <= Utf16::LoHalfMax))
            {
                p16 += 2;
            }
            else
            {
                ok = false;
                break;
            }
        }
        else
        {
            ++p16;
        }
    }

    numChars = static_cast<unsigned int>(count);
    return ok;
}


//!
//! Return true if given UTF16 byte sequence (numU16s shorts starting at
//! s) is valid. The validation is done by scanning the byte sequence
//! left to right until all is scanned or until some invalid data is seen.
//! If user needs to know exactly where validation fails, use a non-zero
//! badSeq value. Upon validation failure, if badSeq is non-zero, *badSeq
//! will contain the location where invalid data is seen. Otherwise,
//! badSeq is not used.
//!
bool Utf16Seq::isValid(const utf16_t* s, size_t numU16s, const utf16_t** badSeq)
{
    bool ok = true;
    const utf16_t* p16 = s;
    for (const utf16_t* p16End = p16 + numU16s; p16 < p16End;)
    {
        if ((*p16 >= Utf16::HiHalfMin) && (*p16 <= Utf16::LoHalfMax))
        {
            if ((p16 + 1 < p16End) && (*p16 <= Utf16::HiHalfMax) &&
                (p16[1] >= Utf16::LoHalfMin) && (p16[1] <= Utf16::LoHalfMax))
            {
                p16 += 2;
            }
            else
            {
                ok = false;
                if (badSeq != 0)
                {
                    *badSeq = p16;
                }
                break;
            }
        }
        else
        {
            ++p16;
        }
    }

    return ok;
}


//!
//! Resize sequence. Given new capacity must be non-zero and must not be less
//! than the current sequence length in shorts. Return true if successful.
//!
bool Utf16Seq::resize(unsigned int newCap)
{
    bool ok;
    if ((newCap == 0) || (newCap < numU16s_))
    {
        ok = false;
    }

    else
    {
        ok = true;
        if (newCap != capacity())
        {
            utf16_t* raw16 = new utf16_t[newCap];
            memcpy(raw16, seq_, byteSize_);
            delete[] seq_;
            seq_ = raw16;
            setCapacity(newCap);
        }
    }

    return ok;
}


//!
//! Convert from given UTF16 data (numU16s shorts starting at s). Save
//! result in this instance. Given data may contain some invalid byte
//! sequence. For each invalid character seen, use defaultChar as its
//! replacement. Return the number of invalid characters seen.
//!
unsigned int Utf16Seq::convert16(const utf16_t* s, size_t numU16s, utf32_t defaultChar)
{
    reset();
    if (numU16s == 0)
    {
        return 0;
    }

    // Assume no errors (most likely scenario).
    unsigned int minCap = static_cast<unsigned int>(numU16s);
    if (countChars(s, numU16s, numChars_))
    {
        if ((minCap <= capacity()) || growTo(minCap))
        {
            setSize16(minCap);
            memcpy(seq_, s, byteSize_);
        }
        return 0;
    }

    // Prepare worst-case buffer.
    minCap <<= 1;
    unsigned int oldCap = growBuf(minCap);
    unsigned int invalidCharCount = 0;
    size_t charCount = numU16s;
    const utf16_t* p16 = s;
    const utf16_t* p16End = p16 + charCount;

    // Iterate through each UTF16 character.
    // Plenty of room in existing buffer for encoding.
    Utf16 default16(defaultChar);
    for (utf16_t* p = seq_; p16 < p16End;)
    {
        if ((*p16 >= Utf16::HiHalfMin) && (*p16 <= Utf16::LoHalfMax))
        {
            if ((p16 + 1 < p16End) && (*p16 <= Utf16::HiHalfMax) &&
                (p16[1] >= Utf16::LoHalfMin) && (p16[1] <= Utf16::LoHalfMax))
            {
                *p++ = p16[0];
                *p++ = p16[1];
                --charCount;
            }
            else
            {
                p += default16.encode(p);
                ++invalidCharCount;
            }
            p16 += 2;
        }
        else
        {
            *p++ = *p16++;
        }
    }

    // Summarize result.
    // Return number of invalid characters seen.
    setLength16(numU16s, charCount);
    shrinkBuf(oldCap);
    return invalidCharCount;
}


//!
//! Convert from given UTF16 data (numU16s non-native-endian shorts starting at
//! s). Save result in this instance. Given data may contain some invalid byte
//! sequence. For each invalid character seen, use defaultChar as its replacement.
//! Return the number of invalid characters seen.
//!
unsigned int Utf16Seq::convert61(const utf16_t* s, size_t numU16s, utf32_t defaultChar)
{
    reset();
    if (numU16s == 0)
    {
        return 0;
    }

    // Prepare worst-case buffer.
    unsigned int minCap = static_cast<unsigned int>(numU16s) << 1;
    unsigned int oldCap = growBuf(minCap);
    unsigned int invalidCharCount = 0;
    size_t charCount = numU16s;
    const utf16_t* p61 = s;
    const utf16_t* p61End = p61 + charCount;

    // Iterate through each UTF16 character.
    // Plenty of room in existing buffer for encoding.
    Utf16 default16(defaultChar);
    for (utf16_t* p = seq_; p61 < p61End;)
    {
        unsigned short u16A = bswap16(*p61);
        if ((u16A >= Utf16::HiHalfMin) && (u16A <= Utf16::LoHalfMax))
        {
            if ((p61 + 1 < p61End) && (u16A <= Utf16::HiHalfMax))
            {
                unsigned short u16B = bswap16(p61[1]);
                if ((u16B >= Utf16::LoHalfMin) && (u16B <= Utf16::LoHalfMax))
                {
                    *p++ = u16A;
                    *p++ = u16B;
                    --charCount;
                    p61 += 2;
                    continue;
                }
            }
            p += default16.encode(p);
            ++invalidCharCount;
            p61 += 2;
        }
        else
        {
            *p++ = u16A;
            ++p61;
        }
    }

    // Summarize result.
    // Return number of invalid characters seen.
    setLength16(numU16s, charCount);
    shrinkBuf(oldCap);
    return invalidCharCount;
}


//!
//! Convert from given UTF8 data (numU8s bytes starting at s). Save
//! result in this instance. Given data may contain some invalid byte
//! sequence. For each invalid character seen, use defautlChar as its
//! replacement. Return the number of invalid characters seen.
//!
unsigned int Utf16Seq::convert8(const utf8_t* s, size_t numU8s, utf32_t defaultChar)
{
    reset();
    if (numU8s == 0)
    {
        return 0;
    }

    // Prepare worst-case buffer.
    unsigned int minCap = static_cast<unsigned int>(numU8s);
    unsigned int oldCap = growBuf(minCap);
    unsigned int invalidCharCount = 0;
    size_t charCount = 0;
    size_t remaining = numU8s;
    const utf8_t* p8 = s;

    // Iterate through each UTF8 character.
    // Plenty of room in existing buffer for encoding.
    utf16_t* p16 = seq_;
    Utf16 default16(defaultChar);
    Utf8 c8;
    for (Utf16 c16; remaining > 0; p16 += c16.encode(p16))
    {
        size_t bytesConsumed = c8.decode(p8, remaining);
        if (bytesConsumed > 0)
        {
            c16.resetWithValidChar(c8.asU32());
        }
        else
        {
            ++invalidCharCount;
            c16 = default16;
            bytesConsumed = 1;
        }
        p8 += bytesConsumed;
        remaining -= bytesConsumed;
        ++charCount;
    }

    // Summarize result.
    // Return number of invalid characters seen.
    setLength16(p16 - seq_, charCount);
    shrinkBuf(oldCap);
    return invalidCharCount;
}


//!
//! Shrink given array into this sequence. Given array holds numChars
//! characters, some of which might be unsuitable for this sequence.
//! For each invalid character seen, use defaultChar as its replacement.
//! Return the number of invalid characters seen.
//!
unsigned int Utf16Seq::shrink(const utf32_t* s, size_t numChars, utf32_t defaultChar)
{
    reset();
    if (numChars == 0)
    {
        return 0;
    }

    // Prepare worst-case buffer.
    unsigned int minCap = static_cast<unsigned int>(numChars) << 1;
    unsigned int oldCap = growBuf(minCap);
    unsigned int invalidCharCount = 0;
    const utf32_t* p32 = s;
    const utf32_t* p32End = p32 + numChars;

    // Iterate through each UTF32 character.
    // Plenty of room in existing buffer for encoding.
    Utf16 default16(defaultChar);
    utf16_t* p16 = seq_;
    for (Utf16 c; p32 < p32End; ++p32, p16 += c.encode(p16))
    {
        if (Utf16::isValid(*p32))
        {
            c.resetWithValidChar(*p32);
        }
        else
        {
            ++invalidCharCount;
            c = default16;
        }
    }

    // Summarize result.
    // Return number of invalid characters seen.
    setLength16(p16 - seq_, numChars);
    shrinkBuf(oldCap);
    return invalidCharCount;
}


//!
//! Detach raw UTF16 sequence from instance. The return sequence is allocated
//! from the heap and is to be freed by the caller using the delete[] operator
//! when done. Instance is now disabled and must not be used further.
//!
utf16_t* Utf16Seq::detachRaw()
{
    utf16_t* raw = seq_;
    seq_ = 0;
    reset();
    setCapacity(0);

    return raw;
}


//!
//! Expand into an array of 32-bit values. Return the resulting array.
//! The resulting array is allocated using the heap and is to be freed
//! by the user using the delete[] operator. The resulting array has
//! numChars() 32-bit values.
//!
utf32_t* Utf16Seq::expand() const
{
    utf32_t* s = new utf32_t[numChars_];
    return expand(s);
}


//!
//! Expand into given array. Return the given array. The given array s
//! is allocated by the user and must be big enough to hold numChars()
//! 32-bit values.
//!
utf32_t* Utf16Seq::expand(utf32_t* s) const
{

    // No decoding required if each character uses just one short.
    const utf16_t* p = seq_;
    const utf16_t* pEnd = p + numU16s_;
    if (numChars_ == numU16s_)
    {
        for (; p < pEnd; *s++ = *p++);
    }

    // Some decoding required if some character uses more than one short.
    else
    {
        for (; p < pEnd; ++s)
        {
            p += Utf16::decodeValidSeq(p, *s);
        }
    }

    // Return the given array.
    return s - numChars_;
}


//!
//! Iterate the sequence from high to low (i.e., right to left).
//! Invoke callback at each character.
//!
void Utf16Seq::applyHiToLo(cb1_t cb, void* arg) const
{

    // No decoding required if each character uses just one short.
    if (numChars_ == numU16s_)
    {
        for (size_t i = numChars_; i-- > 0;)
        {
            cb(arg, i, seq_[i]);
        }
    }

    // Some decoding required if some character uses more than one short.
    else
    {
        const utf16_t* p = seq_ + numU16s_;
        for (size_t i = numChars_; i-- > 0;)
        {
            p -= ((p[-1] >= Utf16::LoHalfMin) && (p[-1] <= Utf16::LoHalfMax))? 2: 1;
            utf32_t c = Utf16::convertValidSeq(p);
            cb(arg, i, c);
        }
    }
}


//!
//! Iterate the sequence from low to high (i.e., left to right).
//! Invoke callback at each character.
//!
void Utf16Seq::applyLoToHi(cb1_t cb, void* arg) const
{

    // No decoding required if each character uses just one short.
    if (numChars_ == numU16s_)
    {
        for (size_t i = 0; i < numChars_; ++i)
        {
            cb(arg, i, seq_[i]);
        }
    }

    // Some decoding required if some character uses more than one short.
    else
    {
        const utf16_t* p = seq_;
        for (size_t i = 0; i < numChars_; ++i)
        {
            utf32_t c;
            p += Utf16::decodeValidSeq(p, c);
            cb(arg, i, c);
        }
    }
}


//!
//! Reset instance with UTF16 data (numU16s shorts starting at s).
//! Given data holds numChars characters suitable for this sequence.
//!
void Utf16Seq::reset(const utf16_t* s, size_t numU16s, size_t numChars)
{
    reset();
    unsigned int minCap = static_cast<unsigned int>(numU16s);
    if ((numChars > 0) && ((minCap <= capacity()) || growTo(minCap)))
    {
        setLength16(numU16s, numChars);
        memcpy(seq_, s, byteSize_);
    }
}


//!
//! Reset instance with given UTF8 data (numU8s bytes starting at s).
//! Given data holds numChars characters suitable for this sequence.
//!
void Utf16Seq::reset(const utf8_t* s, size_t numU8s, size_t numChars)
{
    reset();
    if (numChars == 0)
    {
        return;
    }

    // Simple 7-bit ASCII case.
    unsigned int minCap = static_cast<unsigned int>(numChars);
    if (numU8s == numChars)
    {
        growBuf(minCap);
        const utf8_t* p8 = s;
        const utf8_t* p8End = p8 + numU8s;
        for (utf16_t* p16 = seq_; p8 < p8End; *p16++ = *p8++);
        setLength16(numChars, numChars);
        return;
    }

    // Prepare worst-case buffer.
    minCap <<= 1;
    unsigned int oldCap = growBuf(minCap);
    const utf8_t* p8 = s;
    const utf8_t* p8End = p8 + numU8s;

    // Iterate through each UTF8 character.
    // Plenty of room in existing buffer for encoding.
    Utf8 c8;
    utf16_t* p16 = seq_;
    for (Utf16 c16; p8 < p8End; p16 += c16.encode(p16))
    {
        p8 += c8.decode(p8);
        c16.resetWithValidChar(c8.asU32());
    }

    // Summarize result.
    setLength16(p16 - seq_, numChars);
    shrinkBuf(oldCap);
}


//!
//! Shrink given array into this sequence. Given array holds numChars
//! characters suitable for this sequence. Behavior is unpredictable
//! if it contains unsuitable characters. Use the other shrink() method
//! if the given array might have unsuitable characters.
//!
void Utf16Seq::shrink(const utf32_t* s, size_t numChars)
{
    reset();
    if (numChars > 0)
    {

        // Prepare worst-case buffer.
        unsigned int minCap = static_cast<unsigned int>(numChars) << 1;
        unsigned int oldCap = growBuf(minCap);
        const utf32_t* p32 = s;
        const utf32_t* p32End = p32 + numChars;

        // Iterate through each UTF32 character.
        // Plenty of room in existing buffer for encoding.
        utf16_t* p16 = seq_;
        for (Utf16 c; p32 < p32End; p16 += c.encode(p16))
        {
            c.resetWithValidChar(*p32++);
        }

        // Summarize result.
        setLength16(p16 - seq_, numChars);
        shrinkBuf(oldCap);
    }
}


void Utf16Seq::shrinkBuf(unsigned int oldCap)
{
    for (unsigned int cap = oldCap; cap > 0; cap <<= 1)
    {
        if (cap >= numU16s_)
        {
            resize(cap);
            break;
        }
    }
}


//!
//! Construct an unattached Utf16Seq iterator.
//!
Utf16Seq::Itor::Itor()
{
    copyMade_ = false;
    curIndex_ = INVALID_INDEX;
    p1_ = 0;
    p2_ = 0;
    seq_ = 0;
}


//!
//! Construct a Utf16Seq iterator. Attach iterator to given sequence. A
//! deep copy of the given sequence is made if makeCopy is true. Normally,
//! a deep copy should be made only if the given sequence can change during
//! iterating.
//!
Utf16Seq::Itor::Itor(const Utf16Seq& seq, bool makeCopy)
{
    copyMade_ = false;
    p1_ = 0;
    p2_ = 0;
    seq_ = 0;

    // attach() is also responsible for resetting the iterator and
    // that's why curIndex_ does not have to be initialized here.
    attach(seq, makeCopy);
}


Utf16Seq::Itor::~Itor()
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
bool Utf16Seq::Itor::next(utf32_t& c)
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
            p2_ = p1_ + Utf16::decodeValidSeq(p1_, c) - 1;
        }
    }

    // Retrieve right character.
    else if (++curIndex_ < seq_->numChars_)
    {
        ok = true;
        p1_ = p2_ + 1;
        p2_ = p1_ + Utf16::decodeValidSeq(p1_, c) - 1;
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
bool Utf16Seq::Itor::peekLeft(utf32_t& c) const
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
            const utf16_t* p = seq_->seq_ + seq_->numU16s_;
            p -= ((p[-1] >= Utf16::LoHalfMin) && (p[-1] <= Utf16::LoHalfMax))? 2: 1;
            c = Utf16::convertValidSeq(p);
        }
    }

    // Peek at left character.
    else if ((curIndex_ - 1) < seq_->numChars_)
    {
        ok = true;
        const utf16_t* p = p1_;
        p -= ((p[-1] >= Utf16::LoHalfMin) && (p[-1] <= Utf16::LoHalfMax))? 2: 1;
        c = Utf16::convertValidSeq(p);
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
bool Utf16Seq::Itor::peekRight(utf32_t& c) const
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
            c = Utf16::convertValidSeq(seq_->seq_);
        }
    }

    // Peek at right character.
    else if ((curIndex_ + 1) < seq_->numChars_)
    {
        ok = true;
        c = Utf16::convertValidSeq(p2_ + 1);
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
bool Utf16Seq::Itor::prev(utf32_t& c)
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
            p2_ = seq_->seq_ + seq_->numU16s_ - 1;
            p1_ = ((*p2_ >= Utf16::LoHalfMin) && (*p2_ <= Utf16::LoHalfMax))? (p2_ - 1): (p2_);
            c = Utf16::convertValidSeq(p1_);
        }
    }

    // Retrieve left character.
    else if (--curIndex_ < seq_->numChars_)
    {
        ok = true;
        p2_ = p1_ - 1;
        p1_ = ((*p2_ >= Utf16::LoHalfMin) && (*p2_ <= Utf16::LoHalfMax))? (p2_ - 1): (p2_);
        c = Utf16::convertValidSeq(p1_);
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
void Utf16Seq::Itor::attach(const Utf16Seq& seq, bool makeCopy)
{

    // Destroy old copy.
    if (copyMade_)
    {
        delete seq_;
    }

    // Make private copy of given sequence if necessary.
    seq_ = makeCopy? new Utf16Seq(seq): &seq;
    copyMade_ = makeCopy;

    // Reset iterator.
    reset();
}


//!
//! Detach from any attached sequence. Ignore if iterator is unattached.
//!
void Utf16Seq::Itor::detach()
{
    if (copyMade_)
    {
        delete seq_;
        copyMade_ = false;
    }

    seq_ = 0;
}

END_NAMESPACE1
