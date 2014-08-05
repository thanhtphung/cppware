/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_UTF8_SEQ_HPP
#define SYSKIT_UTF8_SEQ_HPP

#include <string.h>
#include "syskit/UtfSeq.hpp"
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(syskit)

class Utf8;


//! sequence of utf8 characters
class Utf8Seq: public UtfSeq
    //!
    //! A class representing a sequence of UTF8 characters. UTF8 characters
    //! are most efficiently accessed as an array of 32-bit values. However,
    //! the quadrupled memory usage usually cannot be justified if the majority
    //! of characters are ASCII. This class can help reduce memory usage and
    //! can also help ease interoperability. Use convertX() to convert
    //! from other UTF forms. This class uses the UTF8 definition as described
    //! in the Utf8 class. Example:
    //!\code
    //! Utf8Seq seq;
    //! utf16_t raw[3] = {0xaaaaU, 0xbbbbU, 0xccccU};
    //! if (seq.convert16(raw, 3, Utf8Seq::DEFAULT_CHAR))
    //! {
    //!   unsigned int numChars = seq.numChars();
    //!   utf32_t* s = seq.expand();
    //!   :
    //!   // access UTF8 characters as an array of 32-bit values
    //!   :
    //!   delete[] s;
    //! }
    //!\endcode
    //!
{

public:
    enum
    {
        DefaultCap = 1024
    };

    // Constructors.
    Utf8Seq(Utf8Seq* seq);
    Utf8Seq(const Utf8Seq& seq);
    Utf8Seq(const Utf8Seq& seq, size_t startAt, size_t charCount, unsigned int capacity = DefaultCap);
    Utf8Seq(const char* s, size_t length, unsigned int capacity = DefaultCap);
    Utf8Seq(unsigned int capacity = DefaultCap);
    Utf8Seq(utf8_t* s, size_t numU8s, size_t numChars);

    // Operators.
    bool operator !=(const Utf8Seq& seq) const;
    bool operator <(const Utf8Seq& seq) const;
    bool operator <=(const Utf8Seq& seq) const;
    bool operator >(const Utf8Seq& seq) const;
    bool operator >=(const Utf8Seq& seq) const;
    bool operator ==(const Utf8Seq& seq) const;
    const Utf8Seq& operator +=(const Utf8& c);
    const Utf8Seq& operator +=(const Utf8Seq& seq);
    const Utf8Seq& operator +=(char c);
    const Utf8Seq& operator =(const Utf8Seq& seq);

    // Raw access.
    bool isAscii() const;
    const char* ascii() const;
    const utf8_t* raw() const;
    const utf8_t* raw(unsigned int& byteSize) const;
    utf8_t* detachRaw();
    utf8_t* detachRaw(unsigned int& byteSize);
    void attachRaw(utf8_t* s, size_t numU8s, size_t numChars);

    bool truncate(size_t numChars);
    unsigned int getByteSize(size_t startAt, size_t charCount) const;
    void getByteSizes(size_t startAt, size_t charCount, unsigned int& size0, unsigned int& size1) const;
    void append(const Utf8Seq& seq, size_t startAt, size_t charCount);
    void append(const char* s, size_t length);
    void append(const utf8_t* s, size_t numU8s, size_t numChars);
    void append(size_t count, char c);
    void setU8(size_t index, unsigned char u8);

    unsigned int convert16(const utf16_t* s, size_t numU16s, utf32_t defaultChar = '?');
    unsigned int convert61(const utf16_t* s, size_t numU16s, utf32_t defaultChar = '?');
    unsigned int convert8(const utf8_t* s, size_t numU8s, utf32_t defaultChar = '?');
    void reset();
    void reset(const Utf8Seq& seq, size_t startAt, size_t charCount);
    void reset(const char* s, size_t length);
    void reset(const utf16_t* s, size_t numU16s, size_t numChars);
    void reset(const utf8_t* s, size_t numU8s, size_t numChars);

    // Utilities.
    static bool countChars(const utf8_t* s, size_t numU8s, unsigned int& numChars);
    static bool isValid(const utf8_t* s, size_t numU8s, const utf8_t** badSeq = 0);
    static int compareP(const void* item0, const void* item1);
    static int comparePR(const void* item0, const void* item1);

    // Override Growable.
    virtual ~Utf8Seq();
    virtual bool resize(unsigned int newCap);

    // Override UtfSeq.
    virtual utf32_t operator [](size_t index) const;

    virtual UtfSeq* clone() const;
    virtual unsigned int shrink(const utf32_t* s, size_t numChars, utf32_t defaultChar);
    virtual utf32_t* expand() const;
    virtual utf32_t* expand(utf32_t* s) const;
    virtual void shrink(const utf32_t* s, size_t numChars);

    virtual bool applyHiToLo(cb0_t cb, void* arg = 0) const;
    virtual bool applyLoToHi(cb0_t cb, void* arg = 0) const;
    virtual void applyHiToLo(cb1_t cb, void* arg = 0) const;
    virtual void applyLoToHi(cb1_t cb, void* arg = 0) const;


    //! sequence iterator
    class Itor
        //!
        //! A class representing a Utf8Seq iterator. It provides a scheme to
        //! traverse the characters in a Utf8Seq instance without having to
        //! expand the sequence. To iterate left to right, use the next() and/or
        //! applyLoToHi() methods. To iterate right to left, use the prev() and/or
        //! applyHiToLo() methods. Peeking is supported via the peekLeft() and
        //! peekRight() methods. Example:
        //!\code
        //! Utf8Seq::Itor it(seq, false /*makeCopy*/);
        //! utf32_t c;
        //! while (it.next(c))
        //! {
        //!   //do something with each character in the sequence
        //! }
        //!\endcode
        //!
    {

    public:

        // Constructors and destructor.
        Itor();
        Itor(const Utf8Seq& seq, bool makeCopy = false);
        ~Itor();

        // Iterator support.
        bool next(utf32_t& c);
        bool peekLeft(utf32_t& c) const;
        bool peekRight(utf32_t& c) const;
        bool prev(utf32_t& c);
        unsigned int curOffset() const;
        void reset();

        // Utilities.
        const Utf8Seq* seq() const;
        void attach(const Utf8Seq& seq, bool makeCopy = false);
        void detach();

    private:
        const Utf8Seq* seq_;
        const utf8_t* p1_;
        const utf8_t* p2_;
        unsigned int curIndex_;
        bool copyMade_;

        Itor(const Itor&); //prohibit usage
        const Itor& operator =(const Itor&); //prohibit usage

    };

protected:
    const utf8_t* seek(size_t index) const;
    void addNull();
    void addNullIfNone();
    void resetWithNull();
    void rmNull();

private:
    utf8_t* seq_;

    void appendAscii8(const unsigned char*, size_t);
    void reset16(const utf16_t*, size_t);
    void reset16(const utf16_t*, size_t, size_t);
    void shrinkBuf(unsigned int);

};

//! Return true if this sequence is less than given sequence.
//! Ignore locale.
inline bool Utf8Seq::operator <(const Utf8Seq& seq) const
{
    void* item0 = const_cast<Utf8Seq*>(this);
    void* item1 = const_cast<Utf8Seq*>(&seq);
    bool isLt = (compareP(item0, item1) < 0);
    return isLt;
}

//! Return true if this sequence is less than or equal to given sequence.
//! Ignore locale.
inline bool Utf8Seq::operator <=(const Utf8Seq& seq) const
{
    void* item0 = const_cast<Utf8Seq*>(this);
    void* item1 = const_cast<Utf8Seq*>(&seq);
    bool isLe = (compareP(item0, item1) <= 0);
    return isLe;
}

//! Return true if this sequence is greater than given sequence.
//! Ignore locale.
inline bool Utf8Seq::operator >(const Utf8Seq& seq) const
{
    void* item0 = const_cast<Utf8Seq*>(this);
    void* item1 = const_cast<Utf8Seq*>(&seq);
    bool isGt = (compareP(item0, item1) > 0);
    return isGt;
}

//! Return true if this sequence is greater than or equal to given sequence.
//! Ignore locale.
inline bool Utf8Seq::operator >=(const Utf8Seq& seq) const
{
    void* item0 = const_cast<Utf8Seq*>(this);
    void* item1 = const_cast<Utf8Seq*>(&seq);
    bool isGe = (compareP(item0, item1) >= 0);
    return isGe;
}

//! Return true if this sequence equals given sequence.
inline bool Utf8Seq::operator ==(const Utf8Seq& seq) const
{
    return (byteSize_ == seq.byteSize_) &&
        (numChars_ == seq.numChars_) &&
        (memcmp(seq_, seq.seq_, byteSize_) == 0);
}

//! Return true if this sequence does not equal given sequence.
inline bool Utf8Seq::operator !=(const Utf8Seq& seq) const
{
    return !(operator ==(seq));
}

//! Return true if this sequence contains only 7-bit ASCII characters.
inline bool Utf8Seq::isAscii() const
{
    return (numChars_ == byteSize_);
}

//! Return the raw UTF8 sequence as if it were a null-terminated ASCII string.
inline const char* Utf8Seq::ascii() const
{
    return reinterpret_cast<const char*>(seq_);
}

//! Return the raw UTF8 sequence. Use byteSize() to obtain its
//! length in bytes. Use numChars() to obtain its character count.
inline const utf8_t* Utf8Seq::raw() const
{
    return seq_;
}

//! Return the raw UTF8 sequence. Also return its length in bytes
//! in byteSize. Use numChars() to obtain its character count.
inline const utf8_t* Utf8Seq::raw(unsigned int& byteSize) const
{
    byteSize = byteSize_;
    return seq_;
}

//! Return the byte size for charCount characters starting at startAt.
inline unsigned int Utf8Seq::getByteSize(size_t startAt, size_t charCount) const
{
    unsigned int size0;
    unsigned int size1;
    getByteSizes(startAt, charCount, size0, size1);
    return size1;
}

//! Add terminating null.
inline void Utf8Seq::addNull()
{
    const char NULL_CHAR = 0;
    this->operator +=(NULL_CHAR);
}

//! Make sequence null-terminated if it's not already so.
inline void Utf8Seq::addNullIfNone()
{
    if ((byteSize_ == 0) || (seq_[byteSize_ - 1] != 0))
    {
        const char NULL_CHAR = 0;
        this->operator +=(NULL_CHAR);
    }
}

//! Detach raw UTF8 sequence from instance. The return sequence is allocated
//! from the heap and is to be freed by the caller using the delete[] operator
//! when done. Instance is now disabled and must not be used further.
inline utf8_t* Utf8Seq::detachRaw(unsigned int& byteSize)
{
    byteSize = byteSize_;
    return detachRaw();
}

//! Reset instance by making it an empty sequence.
inline void Utf8Seq::reset()
{
    numChars_ = 0;
    byteSize_ = 0;
}

//! Reset instance with UTF16 data (numU16s shorts starting at s).
//! Given data holds numChars characters suitable for this sequence.
//! Use the convert16() method if input validation is required. If
//! either size is unknown, zero is allowed, but at least one size
//! must be non-zero.
inline void Utf8Seq::reset(const utf16_t* s, size_t numU16s, size_t numChars)
{
    (numU16s == numChars)? reset16(s, numChars): reset16(s, numU16s, numChars);
}

//! Assuming sufficient capacity, reset instance with a terminating null.
inline void Utf8Seq::resetWithNull()
{
    seq_[0] = 0;
    numChars_ = 1;
    byteSize_ = 1;
}

//! Remove terminating null assuming sequence is null-terminated.
inline void Utf8Seq::rmNull()
{
    --numChars_;
    --byteSize_;
}

//! Treat sequence as raw bytes and update the given byte. Don't do any error checking.
inline void Utf8Seq::setU8(size_t index, unsigned char u8)
{
    seq_[index] = u8;
}

//! Return the attached sequence. Return zero if unattached.
inline const Utf8Seq* Utf8Seq::Itor::seq() const
{
    return seq_;
}

//! Return the current offset. This is how far in bytes from the start of the sequence
//! the current character is. Return INVALID_INDEX if the current character is undefined
//! (iterating has not started or if the sequence is empty).
inline unsigned int Utf8Seq::Itor::curOffset() const
{
    unsigned int offset = (p1_ != 0)? static_cast<unsigned int>(p1_ - seq_->seq_): (INVALID_INDEX);
    return offset;
}

//! Reset the iterator to its initial state. That is, next() will return the first
//! character, and prev() will return the last character.
inline void Utf8Seq::Itor::reset()
{
    curIndex_ = INVALID_INDEX;
}

END_NAMESPACE1

#endif
