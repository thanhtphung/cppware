/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_UTF_SEQ_HPP
#define SYSKIT_UTF_SEQ_HPP

#include "syskit/Growable.hpp"
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(syskit)


//! sequence of UTFx characters
class UtfSeq: public Growable
    //!
    //! An abstract base class for a sequence of UTFx characters. UTFx
    //! characters are most efficiently accessed as an array of 32-bit
    //! characters when memory usage is not an issue. Use expand() and
    //! shrink() to convert between fixed-length 32-bit characters and
    //! variable-length UTFx characters.
    //!
{

public:
    typedef bool(*cb0_t)(void* arg, size_t index, utf32_t c);
    typedef void(*cb1_t)(void* arg, size_t index, utf32_t c);

    unsigned int byteSize() const;
    unsigned int numChars() const;

    // Override Growable.
    virtual ~UtfSeq();
    virtual bool setGrowth(int growBy);

    virtual utf32_t operator [](size_t index) const = 0;

    virtual UtfSeq* clone() const = 0;
    virtual unsigned int shrink(const utf32_t* s, size_t numChars, utf32_t defaultChar) = 0;
    virtual utf32_t* expand() const = 0;
    virtual utf32_t* expand(utf32_t* s) const = 0;
    virtual void shrink(const utf32_t* s, size_t numChars) = 0;

    virtual bool applyHiToLo(cb0_t cb, void* arg = 0) const = 0;
    virtual bool applyLoToHi(cb0_t cb, void* arg = 0) const = 0;
    virtual void applyHiToLo(cb1_t cb, void* arg = 0) const = 0;
    virtual void applyLoToHi(cb1_t cb, void* arg = 0) const = 0;

protected:
    unsigned int byteSize_;
    unsigned int numChars_;

    UtfSeq(const UtfSeq& seq);
    UtfSeq(unsigned int capacity, size_t byteSize, size_t numChars);
    const UtfSeq& operator =(const UtfSeq& seq);
    bool growTo(unsigned int minCap);
    unsigned int growBuf(unsigned int minCap);
    void setByteSize(size_t byteSize);
    void setLength(size_t byteSize, size_t numChars);

};

inline const UtfSeq& UtfSeq::operator =(const UtfSeq& seq)
{
    byteSize_ = seq.byteSize_;
    numChars_ = seq.numChars_;
    return *this;
}

//! Return the sequence size in bytes.
inline unsigned int UtfSeq::byteSize() const
{
    return byteSize_;
}

inline unsigned int UtfSeq::growBuf(unsigned int minCap)
{
    unsigned int oldCap = capacity();
    if (oldCap < minCap) resize(minCap);
    return oldCap;
}

//! Return the number of UTFx characters held in the sequence.
inline unsigned int UtfSeq::numChars() const
{
    return numChars_;
}

inline void UtfSeq::setByteSize(size_t byteSize)
{
    byteSize_ = static_cast<unsigned int>(byteSize);
}

inline void UtfSeq::setLength(size_t byteSize, size_t numChars)
{
    byteSize_ = static_cast<unsigned int>(byteSize);
    numChars_ = static_cast<unsigned int>(numChars);
}

END_NAMESPACE1

#endif
