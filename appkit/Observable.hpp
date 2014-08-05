/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_OBSERVABLE_HPP
#define APPKIT_OBSERVABLE_HPP

#include "appkit/String.hpp"
#include "syskit/Atomic32.hpp"
#include "syskit/RefCounted.hpp"
#include "syskit/SpinSection.hpp"
#include "syskit/macros.h"

DECLARE_CLASS1(syskit, Utc)

BEGIN_NAMESPACE1(appkit)

class NewsAudience;
class Observer;


//! observable subject
class Observable: public virtual syskit::RefCounted
    //!
    //! A class representing an observable subject in the observer design pattern. An observer can
    //! attach to and detach from an observable subject. When an observable subject is updated, its
    //! observers are notified. This implementation requires that both observable subjects and
    //! observers are reference counted. To help troubleshooting, both can be named with free-format
    //! texts, and some counters (update count, notify count, etc.) are provided.
    //!
{

public:
    typedef bool(*cb0_t)(void* arg, Observer* observer);
    typedef void(*cb1_t)(void* arg, Observer* observer);

    String updateSummary() const;
    syskit::Utc updateTime() const;
    const char* name() const;
    unsigned int notifyCount() const;
    unsigned int numAttachedObservers() const;
    unsigned int updateCount() const;
    unsigned long long updateTimeAsFiletime() const;

    bool apply(cb0_t cb, void* arg = 0) const;
    void apply(cb1_t cb, void* arg = 0) const;

    virtual Observer* detach(Observer* observer) const;
    virtual bool attach(Observer* observer) const;
    virtual unsigned int notify(unsigned long long updateTime, const String& updateSummary);
    virtual unsigned long long update(const String& summary);

protected:
    Observable(const char* name, unsigned int initialRefCount = 0U);
    syskit::SpinSection& ss() const;

    virtual ~Observable();

private:
    syskit::Atomic32 notifyCount_;
    syskit::Atomic32 updateCount_;
    NewsAudience* observers_;
    syskit::SpinSection mutable ss_;
    String updateSummary_;
    const char* name_;
    unsigned long long updateTime_;

    Observable(const Observable&); //prohibit usage
    const Observable& operator =(const Observable&); //prohibit usage

};

END_NAMESPACE1

#include "syskit/Utc.hpp"

BEGIN_NAMESPACE1(appkit)

//! A critical section is used to synchronize access to attached observers. A derived
//! class can use the same critical section to synchronize access to other application
//! data. This method returns that critical section.
inline syskit::SpinSection& Observable::ss() const
{
    return ss_;
}

//! Return summary of the last subject update. Return empty string if subject not updated yet.
inline String Observable::updateSummary() const
{
    return updateSummary_;
}

//! Return time of the last subject update. Return Utc::ZERO if subject not updated yet.
inline syskit::Utc Observable::updateTime() const
{
    return updateTime_;
}

//! Return observable subject name. This is a free-format string.
inline const char* Observable::name() const
{
    return name_;
}

//! Return the cumulative count of notifications that have occurred for this subject.
inline unsigned int Observable::notifyCount() const
{
    return notifyCount_;
}

//! Return the cumulative count of updates that have occurred for this subject.
inline unsigned int Observable::updateCount() const
{
    return updateCount_;
}

//! Return time of the last subject update. Return Utc::ZERO if subject not updated yet.
inline unsigned long long Observable::updateTimeAsFiletime() const
{
    return updateTime_;
}

END_NAMESPACE1

#endif
