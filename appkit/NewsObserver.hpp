/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_NEWS_OBSERVER_HPP
#define APPKIT_NEWS_OBSERVER_HPP

#include "appkit/Observer.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)

class NewsSubject;


//! news observer
class NewsObserver: public Observer
    //!
    //! A class representing an observer in the observer design pattern. An observer can attach
    //! to and detach from an observable subject. When an observable subject is updated, its
    //! observers are notified. This implementation requires that both observable subjects and
    //! observers are reference counted. To help troubleshooting, both can be named with free-format
    //! texts, and some counters (update count, notify count, etc.) are provided. With the default
    //! Observer class, there's no built-in support for attaching to an observable at availability.
    //! With this NewsObserver class, an observer can attach to a named observable as soon as it
    //! is available. Also, the observer can follow the named observable as it comes and goes.
    //!
{

public:
    NewsObserver(const char* subject, const char* observerName, unsigned int initialRefCount = 1U);
    bool isActive() const;
    bool isAttached() const;
    bool isDetached() const;
    bool isIdle() const;
    const String& subject() const;
    void activate();
    void deactivate();

    virtual void onAttach(NewsSubject* attached);
    virtual void onDetach(NewsSubject* detached, const String& newsSubject);

protected:
    virtual ~NewsObserver();
    virtual void onUpdate(unsigned long long filetime, const String& summary, Observable* subject);

private:
    class SourceWatch: public Observer
    {
    public:
        SourceWatch(NewsObserver* newsObserver, const char* name);
        void activate();
        void deactivate();
    protected:
        virtual ~SourceWatch();
        virtual void onUpdate(unsigned long long filetime, const String& summary, Observable* subject);
    private:
        NewsObserver* newsObserver_;
        const NewsSubject* sourceNews_;
        SourceWatch(const SourceWatch&); //prohibit usage
        const SourceWatch& operator =(const SourceWatch&); //prohibit usage
    };

    syskit::Atomic32 active_;
    syskit::Atomic32 attached_;
    SourceWatch* sourceWatch_;
    String* subject_;

    NewsObserver(const NewsObserver&); //prohibit usage
    const NewsObserver& operator =(const NewsObserver&); //prohibit usage

    void onSourceAdd(const String&);
    void onSourceRemove(const String&);

};

//! Return true if observer is active. A NewsObserver instance is initially idle at construction,
//! becomes active at activate(), and can be idle again at deactivate() or at destruction. Multiple
//! occurrences of activate()/deactivate() are allowed. Observer must be active to see updates to
//! the news subject of interest.
inline bool NewsObserver::isActive() const
{
    bool active = (active_.asWord() != 0);
    return active;
}

//! Return true if observer is attached to its news subject of interest. Only active observers can
//! be attached. Being attached implies its news subject of interest is available.
inline bool NewsObserver::isAttached() const
{
    bool attached = (attached_.asWord() != 0);
    return attached;
}

//! Return true if observer is detached from its news subject of interest. Idle observers are always
//! detached. Being detached implies its news subject of interest is unavailable.
inline bool NewsObserver::isDetached() const
{
    bool detached = (attached_.asWord() == 0);
    return detached;
}

//! Return true if observer is idle. A NewsObserver instance is initially idle at construction,
//! becomes active at activate(), and can be idle again at deactivate() or at destruction. Multiple
//! occurrences of activate()/deactivate() are allowed. Observer must be active to see updates to
//! the news subject of interest.
inline bool NewsObserver::isIdle() const
{
    bool idle = (active_.asWord() == 0);
    return idle;
}

END_NAMESPACE1

#endif
