/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_UTF16_SEQ_HPP
#define SYSKIT_UTF16_SEQ_HPP

#include <string.h>
#include "syskit/UtfSeq.hpp"
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(syskit)


//! sequence of utf16 characters
class Utf16Seq: public UtfSeq
    //!
    //! A class representing a sequence of UTF16 characters. UTF16 characters
    //! are most efficiently accessed as an array of 32-bit values. However,
    //! the doubled memory usage is questionable if the majority of characters
    //! require just 16 bits each. This class can help reduce memory usage and
    //! can also help ease interoperability. Use convertX() to convert from
    //! other UTF forms. This class uses the UTF16 definition as described in
    //! the Utf16 class. Example:
    //!\code
    //! Utf16Seq seq;
    //! utf8_t raw[3] = {0xaaU, 0xbbU, 0xccU};
    //! if (s.convert8(raw, 3))
    //! {
    //!   unsigned int numChars = seq.numChars();
    //!   utf32_t* s = seq.expand();
    //!   :
    //!   // access UTF16 characters as an array of 32-bit values
    //!   :
    //!   delete[] s;
    //! }
    //!\endcode
    //!
{

public:
    enum
    {
        DefaultCap = 512
    };

    // Constructors.
    Utf16Seq(Utf16Seq* seq);
    Utf16Seq(const Utf16Seq& seq);
    Utf16Seq(unsigned int capacity = DefaultCap);
    Utf16Seq(utf16_t* s, size_t numU16s, size_t numChars);

    // Operators.
    bool operator !=(const Utf16Seq& seq) const;
    bool operator ==(const Utf16Seq& seq) const;
    const Utf16Seq& operator +=(const Utf16Seq& seq);
    const Utf16Seq& operator =(const Utf16Seq& seq);

    // Raw access.
    const utf16_t* raw() const;
    const utf16_t* raw(unsigned int& numU16s) const;
    utf16_t* detachRaw();
    utf16_t* detachRaw(unsigned int& numU16s);

    unsigned int convert16(const utf16_t* s, size_t numU16s, utf32_t defaultChar = '?');
    unsigned int convert61(const utf16_t* s, size_t numU16s, utf32_t defaultChar = '?');
    unsigned int convert8(const utf8_t* s, size_t numU8s, utf32_t defaultChar = '?');
    unsigned int numU16s() const;
    void reset();
    void reset(const utf16_t* s, size_t numU16s, size_t numChars);
    void reset(const utf8_t* s, size_t numU8s, size_t numChars);

    // Utilities.
    static bool countChars(const utf16_t* s, size_t numU16s, unsigned int& numChars);
    static bool isValid(const utf16_t* s, size_t numU16s, const utf16_t** badSeq = 0);

    // Override Growable.
    virtual ~Utf16Seq();
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
        //! A class representing a Utf16Seq iterator. It provides a scheme to
        //! traverse the characters in a Utf16Seq instance without having to
        //! expand the sequence. To iterate left to right, use the next() and/or
        //! applyLoToHi() methods. To iterate right to left, use the prev() and/or
        //! applyHiToLo() methods. Peeking is supported via the peekLeft() and
        //! peekRight() methods. Example:
        //!\code
        //! Utf16Seq::Itor it(seq, false /*makeCopy*/);
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
        Itor(const Utf16Seq& seq, bool makeCopy = false);
        ~Itor();

        // Iterator support.
        bool next(utf32_t& c);
        bool peekLeft(utf32_t& c) const;
        bool peekRight(utf32_t& c) const;
        bool prev(utf32_t& c);
        unsigned int curOffset() const;
        void reset();

        // Utilities.
        const Utf16Seq* seq() const;
        void attach(const Utf16Seq& seq, bool makeCopy = false);
        void detach();

    private:
        const Utf16Seq* seq_;
        const utf16_t* p1_;
        const utf16_t* p2_;
        unsigned int curIndex_;
        bool copyMade_;

        Itor(const Itor&); //prohibit usage
        const Itor& operator =(const Itor&); //prohibit usage

    };

private:
    utf16_t* seq_;
    unsigned int numU16s_;

    void setLength16(size_t, size_t);
    void setSize16(unsigned int);
    void shrinkBuf(unsigned int);

};

//! Return true if this sequence equals given sequence.
inline bool Utf16Seq::operator ==(const Utf16Seq& seq) const
{
    return (numChars_ == seq.numChars_) &&
        (numU16s_ == seq.numU16s_) &&
        (memcmp(seq_, seq.seq_, byteSize_) == 0);
}

//! Return true if this sequence does not equal given sequence.
inline bool Utf16Seq::operator !=(const Utf16Seq& seq) const
{
    return !(operator ==(seq));
}

//! Return the raw UTF16 sequence. Use numU16s() to obtain its length in
//! shorts and byteSize() to obtain its length in bytes. Use numChars()
//! to obtain its character count.
inline const utf16_t* Utf16Seq::raw() const
{
    return seq_;
}

//! Return the raw UTF16 sequence. Also return its length in shorts in
//! numU16s. Use byteSize() to obtain its length in bytes. Use numChars()
//! to obtain its character count.
inline const utf16_t* Utf16Seq::raw(unsigned int& numU16s) const
{
    numU16s = numU16s_;
    return seq_;
}

//! Return the UTF16 sequence length in shorts.
inline unsigned int Utf16Seq::numU16s() const
{
    return numU16s_;
}

//! Detach raw UTF16 sequence from instance. The return sequence is allocated
//! from the heap and is to be freed by the caller using the delete[] operator
//! when done. Instance is now disabled and must not be used further.
inline utf16_t* Utf16Seq::detachRaw(unsigned int& numU16s)
{
    numU16s = numU16s_;
    return detachRaw();
}

//! Reset instance by making it an empty sequence.
inline void Utf16Seq::reset()
{
    byteSize_ = 0;
    numChars_ = 0;
    numU16s_ = 0;
}

inline void Utf16Seq::setLength16(size_t numU16s, size_t numChars)
{
    numU16s_ = static_cast<unsigned int>(numU16s);
    byteSize_ = numU16s_ << 1;
    numChars_ = static_cast<unsigned int>(numChars);
}

inline void Utf16Seq::setSize16(unsigned int numU16s)
{
    numU16s_ = numU16s;
    byteSize_ = numU16s_ << 1;
}

//! Return the attached sequence. Return zero if unattached.
inline const Utf16Seq* Utf16Seq::Itor::seq() const
{
    return seq_;
}

//! Return the current offset. This is how far in shorts from the start of the sequence
//! the current character is. Return INVALID_INDEX if the current character is undefined
//! (iterating has not started or if the sequence is empty).
inline unsigned int Utf16Seq::Itor::curOffset() const
{
    unsigned int offset = (p1_ != 0)? static_cast<unsigned int>(p1_ - seq_->seq_): (INVALID_INDEX);
    return offset;
}

//! Reset the iterator to its initial state. That is, next()
//! will return the first character, and prev() will return the
//! last character.
inline void Utf16Seq::Itor::reset()
{
    curIndex_ = INVALID_INDEX;
}

END_NAMESPACE1

#endif
