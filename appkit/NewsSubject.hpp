/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_NEWS_SUBJECT_HPP
#define APPKIT_NEWS_SUBJECT_HPP

#include "appkit/Observable.hpp"
#include "syskit/ItemQ.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)

class NewsAnchor;
class NewsStory;
class StringVec;


//! observable news
class NewsSubject: public Observable, public syskit::ItemQ::Item
    //!
    //! A class representing an observable subject in the observer design pattern. An observer can
    //! attach to and detach from an observable subject. When an observable subject is updated, its
    //! observers are notified. This implementation requires that both observable subjects and
    //! observers are reference counted. To help troubleshooting, both can be named with free-format
    //! texts, and some counters (update count, notify count, etc.) are provided. With the default
    //! Observable class, updates and notifications are performed in the caller's contexts. With
    //! this NewsSubject class, updates are performed in the caller's (i.e., newsmaker's) context,
    //! and notifications are performed in a dedicated news anchor's context.
    //!
{

public:
    enum
    {
        DefaultQCap = 4,
        DefaultQGrowth = -1
    };

    const syskit::ItemQ& storyQ() const;
    const NewsStory* story() const;

    // Override Observable.
    virtual unsigned long long update(const String& summary = String());

    static NewsSubject* attachByName(Observer* observer, const char* name);
    static NewsSubject* createHeadline(const char* name, unsigned int qCap = DefaultQCap, int qGrowth = DefaultQGrowth);
    static NewsSubject* detachByName(Observer* observer, const char* name);
    static StringVec attachByNames(Observer* observer, const StringVec* names);
    static StringVec detachByNames(Observer* observer, const StringVec* names);

protected:
    NewsSubject(const char* name, unsigned int initialRefCount = 1U, unsigned int qCap = DefaultQCap, int qGrowth = DefaultQGrowth);

    virtual ~NewsSubject();
    virtual NewsStory* createStory(unsigned long long filetime, const String& summary);
    virtual void release() const;

private:
    typedef struct doAttach1Arg_s
    {
        doAttach1Arg_s(NewsSubject** attachedSource, Observer* observer, const char* name);
        NewsSubject** attachedSource;
        Observer* observer;
        const char* name;
    } doAttach1Arg_t;

    typedef struct doAttachNArg_s
    {
        doAttachNArg_s(StringVec* attached, Observer* observer, const StringVec* sortedNames);
        StringVec* attached;
        Observer* observer;
        const StringVec* sortedNames;
    } doAttachNArg_t;

    typedef struct doDetach1Arg_s
    {
        doDetach1Arg_s(NewsSubject** detachedSource, Observer* observer, const char* name);
        NewsSubject** detachedSource;
        Observer* observer;
        const char* name;
    } doDetach1Arg_t;

    typedef struct doDetachNArg_s
    {
        doDetachNArg_s(StringVec* detached, Observer* observer, const StringVec* sortedNames);
        StringVec* detached;
        Observer* observer;
        const StringVec* sortedNames;
    } doDetachNArg_t;

    syskit::ItemQ storyQ_;
    NewsAnchor* newsAnchor_;
    const NewsStory mutable* story_;
    unsigned int indexInSource_;

    NewsSubject(const NewsSubject&); //prohibit usage
    const NewsSubject& operator =(const NewsSubject&); //prohibit usage

    bool putStory(unsigned long long, const String&);
    const NewsStory* getStory();
    unsigned int indexInSource() const;
    void nullifyStory();
    void setIndexInSource(unsigned int);

    static bool doAttach1(void*, NewsSubject*);
    static bool doAttachN(void*, NewsSubject*);
    static bool doDetach1(void*, NewsSubject*);
    static bool doDetachN(void*, NewsSubject*);

    friend class NewsAnchor;
    friend class NewsSource;

};

inline const syskit::ItemQ& NewsSubject::storyQ() const
{
    return storyQ_;
}

//! Return the updated subject as a news story from an observer's perspective.
//! Functional only at Observer::onUpdate() time.
inline const NewsStory* NewsSubject::story() const
{
    return story_;
}

inline unsigned int NewsSubject::indexInSource() const
{
    return indexInSource_;
}

inline void NewsSubject::setIndexInSource(unsigned int indexInSource)
{
    indexInSource_ = indexInSource;
}

inline NewsSubject::doAttach1Arg_s::doAttach1Arg_s(NewsSubject** attachedSource, Observer* observer, const char* name)
{
    this->attachedSource = attachedSource;
    this->observer = observer;
    this->name = name;
}

inline NewsSubject::doAttachNArg_s::doAttachNArg_s(StringVec* attached, Observer* observer, const StringVec* sortedNames)
{
    this->attached = attached;
    this->observer = observer;
    this->sortedNames = sortedNames;
}

inline NewsSubject::doDetach1Arg_s::doDetach1Arg_s(NewsSubject** detachedSource, Observer* observer, const char* name)
{
    this->detachedSource = detachedSource;
    this->observer = observer;
    this->name = name;
}

inline NewsSubject::doDetachNArg_s::doDetachNArg_s(StringVec* detached, Observer* observer, const StringVec* sortedNames)
{
    this->detached = detached;
    this->observer = observer;
    this->sortedNames = sortedNames;
}

END_NAMESPACE1

#endif
