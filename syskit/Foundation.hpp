/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_FOUNDATION_HPP
#define SYSKIT_FOUNDATION_HPP

#include "syskit/RefCounted.hpp"
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(syskit)

class BufPool;
class CriSection;
class Vec;
struct foundation_s;


//! foundation for syskit classes
class Foundation: public RefCounted
    //!
    //! A singleton representing the foundation for some of the syskit classes. The
    //! foundation provides a per-process pool of small-sized buffers accessible via
    //! BufPool. Some support for per-process singletons is also provided.
    //!
{

public:
    bool isOk() const;

    static const Foundation& instance();

protected:
    virtual ~Foundation();

private:
    BufPool* bufPool_;
    CriSection* singletonVecCs_;
    Vec* singletonVec_;
    foundation_s* foundation_;

    Foundation(const Foundation&); //prohibit usage
    const Foundation& operator =(const Foundation&); //prohibit usage

    Foundation();
    BufPool* bufPool() const;
    Vec* singletonVec() const;
    Vec* singletonVec(CriSection*&) const;
    void construct();
    void destruct();
    winTopFilter_t winTopFilter() const;

    friend class BufPool;
    friend class CallStack;
    friend class Singleton;

};

inline BufPool* Foundation::bufPool() const
{
    return bufPool_;
}

inline Vec* Foundation::singletonVec() const
{
    return singletonVec_;
}

inline Vec* Foundation::singletonVec(CriSection*& cs) const
{
    cs = singletonVecCs_;
    return singletonVec_;
}

//! Return true if foundation was successfully constructed.
inline bool Foundation::isOk() const
{
    bool ok = (foundation_ != 0);
    return ok;
}

END_NAMESPACE1

#endif
