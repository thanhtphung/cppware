/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_REF_COUNTED_HPP
#define SYSKIT_REF_COUNTED_HPP

#include "syskit/Atomic32.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! reference-counted object
class RefCounted
    //!
    //! A base class representing a reference-counted object. A reference-counted object
    //! is destroyed when its reference count reaches zero via a rmRef() method. Derived
    //! classes can override the destroy() method if some application-specific code is
    //! required right before destruction or if the object cannot be destroyed via the
    //! delete operator. Derived classes should implement the clone() method if duplication
    //! of the reference-counted objects is allowed. Example:
    //!\code
    //! singleton->addRef(); //mark singleton in-use
    //! :
    //! singleton->rmRef(); //destruct singleton on last use
    //!\endcode
    //!
{

public:

    //! reference count
    class Count
        //!
        //! Helper class to increment/decrement a reference count against
        //! a RefCount instance using construction/destruction. Example:
        //!\code
        //! RefCounted::Count count(*singleton); //at count construction, mark singleton in-use
        //!                                      //at count destruction, destruct singleton on last use
        //!\endcode
        //!
    {
    public:
        Count(const RefCounted& refCounted);
        Count(const RefCounted& refCounted, bool skipAddRef);
        ~Count();
    private:
        const RefCounted& refCounted_;
        Count(const Count&); //prohibit usage
        const Count& operator =(const Count&); //prohibit usage
    };

    bool rmRef() const;
    bool rmRef(unsigned int delta) const;
    unsigned int refCount() const;
    void addRef() const;
    void decrementRefBy(unsigned int delta) const;
    void incrementRefBy(unsigned int delta) const;

    virtual RefCounted* clone() const;

    static bool rmRef(RefCounted* refCounted);

protected:
    RefCounted(unsigned int initialRefCount = 0U);

    virtual ~RefCounted();
    virtual void destroy() const;

private:
    Atomic32 mutable count_;

    RefCounted(const RefCounted&); //prohibit usage
    const RefCounted& operator =(const RefCounted&); //prohibit usage

};

//! Decrement reference count. Destroy given instance if its reference count reaches
//! zero after decrementing. Return true if instance is destroyed because of this
//! decrement operation. Decrementing a null reference count is a no-op.
inline bool RefCounted::rmRef(RefCounted* refCounted)
{
    bool destroyed = (refCounted == 0)? false: refCounted->rmRef();
    return destroyed;
}

//! Return the current reference count.
inline unsigned int RefCounted::refCount() const
{
    return count_.asWord();
}

//! Increment reference count. A reference-counted instance is destroyed
//! when its reference count reaches zero via a rmRef() method.
inline void RefCounted::addRef() const
{
    ++count_;
}

//! Decrement reference count by given delta. A reference-counted instance
//! is destroyed when its reference count reaches zero via a rmRef() method.
inline void RefCounted::decrementRefBy(unsigned int delta) const
{
    count_ -= delta;
}

//! Increment reference count by given delta. A reference-counted instance
//! is destroyed when its reference count reaches zero via a rmRef() method.
inline void RefCounted::incrementRefBy(unsigned int delta) const
{
    count_ += delta;
}

END_NAMESPACE1

#include "syskit/win/link-with-syskit.h"
#endif
