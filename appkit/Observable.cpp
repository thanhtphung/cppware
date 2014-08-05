/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <string.h>
#include "syskit/RefVec.hpp"
#include "syskit/Utc.hpp"
#include "syskit/sys.hpp"

#include "appkit-pch.h"
#include "appkit/Observable.hpp"
#include "appkit/Observer.hpp"

using namespace syskit;

BEGIN_NAMESPACE1(appkit)


// collection of news observers
class NewsAudience: private RefVec
{
public:
    using RefVec::operator delete;
    using RefVec::operator new;
    using RefVec::addIfNotFound;
    using RefVec::numItems;
    using RefVec::rm;
    NewsAudience(const NewsAudience& audience);
    NewsAudience(unsigned int capacity = DefaultCap, int growBy = -1);
    Observer* peek(size_t index) const;
    virtual ~NewsAudience();
    static int diffObservers(const void* item0, const void* item1);
private:
    const NewsAudience& operator =(const NewsAudience&); //prohibit usage
};

// Peek into the audience and locate an observer. Each observer in the audience
// can be identified by an index less than numItems(). Don't do any error checking.
// Behavior is unpredictable if given index is invalid.
inline Observer* NewsAudience::peek(size_t index) const
{
    Observer* observer = dynamic_cast<Observer*>(RefVec::peek(index));
    return observer;
}

NewsAudience::NewsAudience(const NewsAudience& audience):
RefVec(audience)
{
}

NewsAudience::NewsAudience(unsigned int capacity, int growBy):
RefVec(capacity, growBy)
{
}

NewsAudience::~NewsAudience()
{
}

// Compare two Observer instances. Return non-zero if they differ.
int NewsAudience::diffObservers(const void* item0, const void* item1)
{
    const Observer* observer0 = dynamic_cast<const Observer*>(static_cast<const RefCounted*>(item0));
    const Observer* observer1 = dynamic_cast<const Observer*>(static_cast<const RefCounted*>(item1));
    return (*observer0 != *observer1);
}


//!
//! Construct observable subject.
//!
Observable::Observable(const char* name, unsigned int initialRefCount):
RefCounted(initialRefCount),
notifyCount_(0U),
updateCount_(0U),
ss_(),
updateSummary_()
{
    name_ = syskit::strdup(name);
    observers_ = new NewsAudience;
    updateTime_ = Utc::ZERO;
}


Observable::~Observable()
{
    delete observers_;
    delete[] name_;
}


//!
//! Detach given observer from the observable subject. Return detached observer.
//! Return zero if given observer was not attached to this observable subject.
//!
Observer* Observable::detach(Observer* observer) const
{
    SpinSection::Lock lock(ss_);
    RefCounted* removed = 0;
    bool maintainOrder = false;
    observers_->rm(observer, NewsAudience::diffObservers, removed, maintainOrder);
    return dynamic_cast<Observer*>(removed);
}


//!
//! Invoke callback at each attached observer. The callback should return true
//! to continue iterating and should return false to abort iterating. Return
//! false if the callback aborted the iterating. Return true otherwise. Behavior
//! is unpredictable if callback attempts attaching/detaching observers to/from
//! this subject during the iterating.
//!
bool Observable::apply(cb0_t cb, void* arg) const
{
    bool ok = true;
    {
        SpinSection::Lock lock(ss_);
        size_t observerCount = observers_->numItems();
        for (size_t i = 0; i < observerCount; ++i)
        {
            Observer* observer = observers_->peek(i);
            if (!cb(arg, observer))
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
//! Attach given observer to the observable subject. Attached observers are notified of
//! updates to the observable subject via the Observer::onUpdate() method. An observable
//! subject is considered updated when Observable::update() is invoked. Return true if
//! successful. Return false otherwise (i.e., if observer already attached to the subject.)
//! The "Observer::operator !=()" method is used to determine if two Observer instances are
//! identical.
//!
bool Observable::attach(Observer* observer) const
{
    SpinSection::Lock lock(ss_);
    bool ok = observers_->addIfNotFound(observer, NewsAudience::diffObservers);
    return ok;
}


//!
//! Notify attached observers of update. Observers which attach to or detach from
//! this observable subject during this invocation are not notified. That is, during
//! this invocation, a newly attached observer is not notified, and a newly detached
//! observer is still notified. Return the number of observers notified. For synchronous
//! observable subjects, updateTime and updateSummary should be identical at update()
//! and notify(). For asynchronous observable subjects, multiple updates can occur
//! before a notification, and updateTime and updateSummary parameters in notify()
//! might not be the same as Observable::updateTime() and Observable::updateSummary()
//! due to delays.
//!
unsigned int Observable::notify(unsigned long long updateTime, const String& updateSummary)
{
    ++notifyCount_;
    NewsAudience* observers;
    unsigned char buf[sizeof(*observers)];
    {
        SpinSection::Lock lock(ss_);
        observers = new(buf)NewsAudience(*observers_);
    }

    unsigned int observerCount = observers->numItems();
    for (unsigned int i = 0; i < observerCount; ++i)
    {
        Observer* observer = observers->peek(i);
        if (observer->refCount() > 1)
        {
            observer->onUpdate(updateTime, updateSummary, this);
            observer->incrementUpdateCount();
        }
        else //avoid notifying end-of-life observers
        {
            --observerCount;
        }
    }

    observers->NewsAudience::~NewsAudience();
    return observerCount;
}


//!
//! Return the current number of attached observers.
//!
unsigned int Observable::numAttachedObservers() const
{
    unsigned int observerCount = observers_->numItems();
    return observerCount;
}


//!
//! Mark subject as updated and return current time as latest update time. Also associate
//! this update with given summary. Caller is expected to notify observers ASAP via notify().
//!
unsigned long long Observable::update(const String& summary)
{
    ++updateCount_;
    updateSummary_ = summary.ascii(); //TODO: investigate root cause of crash if not making deep copy
    updateTime_ = Utc::nowAsFiletime();
    return updateTime_;
}


//!
//! Invoke callback at each attached observer. Behavior is unpredictable if callback
//! attempts attaching/detaching observers to/from this subject during the iterating.
//!
void Observable::apply(cb1_t cb, void* arg) const
{
    SpinSection::Lock lock(ss_);
    size_t observerCount = observers_->numItems();
    for (size_t i = 0; i < observerCount; ++i)
    {
        Observer* observer = observers_->peek(i);
        cb(arg, observer);
    }
}

END_NAMESPACE1
