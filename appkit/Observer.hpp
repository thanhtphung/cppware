/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_OBSERVER_HPP
#define APPKIT_OBSERVER_HPP

#include "syskit/Atomic32.hpp"
#include "syskit/RefCounted.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)

class Observable;
class String;


//! observer
class Observer: public virtual syskit::RefCounted
    //!
    //! A class representing an observer in the observer design pattern. An observer can attach
    //! to and detach from an observable subject. When an observable subject is updated, its
    //! observers are notified. This implementation requires that both observable subjects and
    //! observers are reference counted. To help troubleshooting, both can be named with free-format
    //! texts, and some counters (update count, notify count, etc.) are provided.
    //!
{

public:
    const char* name() const;
    unsigned int updateCount() const;

    virtual bool operator !=(const Observer& observer) const;
    virtual bool operator ==(const Observer& observer) const;

protected:
    Observer(const char* name, unsigned int initialRefCount = 1U);
    void incrementUpdateCount();

    virtual ~Observer();
    virtual void onUpdate(unsigned long long filetime, const String& summary, Observable* subject) = 0;

private:
    syskit::Atomic32 updateCount_;
    const char* name_;

    Observer(const Observer&); //prohibit usage
    const Observer& operator =(const Observer&); //prohibit usage

    friend class Observable;

};

//! Return observer name. This is a free-format string.
inline const char* Observer::name() const
{
    return name_;
}

//! Return the cumulative count of updates that have been observed by this observer.
inline unsigned int Observer::updateCount() const
{
    return updateCount_;
}

//! Increment the cumulative count of updates that have been observed by this observer.
inline void Observer::incrementUpdateCount()
{
    ++updateCount_;
}

END_NAMESPACE1

#endif
