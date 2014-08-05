/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_SET_HPP
#define APPKIT_SET_HPP

#include <ostream>
#include "syskit/Growable.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)

class String;


#if _WIN32
#pragma pack(push,4)
#endif

//! generic set
class Set: public syskit::Growable
    //!
    //! An abstract base class representing a generic set. Set instances
    //! can be compared using the compare() method. A set instance can be
    //! serialized into an ASCII stream using toString() or an ostream.
    //!
{

public:
    enum compareResult_e
    {

        //! the sets are disjoint
        Disjoint,

        //! both sets are empty
        Empty,

        //! this set is empty, the other is not
        EmptySubset,

        //! this set is not empty, the other is
        EmptySuperset,

        //! the sets are identical
        Equal,

        //! the sets overlap
        Overlap,

        //! this set is a subset of the other, and they are not equal
        Subset,

        //! this set is a superset of the other, and they are not equal
        Superset,

        //! unknown result due to incompatible sets
        Unknown
    };

    static void operator delete(void* p, size_t size);
    static void operator delete(void* p, void* buf);
    static void* operator new(size_t size);
    static void* operator new(size_t size, void* buf);
    const char* type() const;

    // Override Growable.
    virtual ~Set();
    virtual bool setGrowth(int growBy);

    virtual Set* clone() const = 0;
    virtual String toString(const char* delim = ",") const = 0;
    virtual compareResult_e compare(const Set& set) const = 0;

protected:
    Set(const Set& set);
    Set(const char* type, unsigned int capacity);

    const Set& operator =(const Set&);

    static void validateDelim(char& delim);

private:
    const char* type_;

};

#if _WIN32
#pragma pack(pop)
#endif

END_NAMESPACE1

#include "appkit/String.hpp"
#include "syskit/BufPool.hpp"

BEGIN_NAMESPACE1(appkit)

inline void Set::operator delete(void* p, size_t size)
{
    syskit::BufPool::freeBuf(p, size);
}

inline void Set::operator delete(void* /*p*/, void* /*buf*/)
{
}

inline void* Set::operator new(size_t size)
{
    void* buf = syskit::BufPool::allocateBuf(size);
    return buf;
}

inline void* Set::operator new(size_t /*size*/, void* buf)
{
    return buf;
}

//! Return the set type. This is a free-format short ASCII string.
//! It is intended to be used to categorize different types of set
//! for troubleshooting. See derived class for details on this value.
inline const char* Set::type() const
{
    return type_;
}

//! Validate given range delimiter. A valid range delimiter must be
//! non-null and must not be a hyphen. If given delimiter is invalid,
//! it will transform into the default comma delimiter.
inline void Set::validateDelim(char& delim)
{
    if ((delim == 0) || (delim == '-'))
    {
        delim = ',';
    }
}

//! Append ASCII form of given set into given output stream. Return
//! given output stream. See derived class for details on ASCII form
//! of a Set instance.
inline std::ostream& operator <<(std::ostream& os, const Set& set)
{
    String s(set.toString(","));
    return os.write(s.ascii(), static_cast<std::streamsize>(s.byteSize()));
}

END_NAMESPACE1

#endif
