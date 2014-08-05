/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/BufPool.hpp"
#include "syskit/Vec.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/NewsAnchor.hpp"
#include "appkit/NewsStory.hpp"
#include "appkit/NewsSubject.hpp"
#include "appkit/StringVec.hpp"

using namespace appkit;
using namespace syskit;

BEGIN_NAMESPACE

class HeadlineNews: public NewsSubject
{
public:
    HeadlineNews(const char* name, unsigned int qCap = DefaultQCap, int qGrowth = DefaultQGrowth);
protected:
    virtual ~HeadlineNews();
private:
    HeadlineNews(const HeadlineNews&); //prohibit usage
    const HeadlineNews& operator =(const HeadlineNews&); //prohibit usage
};

HeadlineNews::HeadlineNews(const char* name, unsigned int qCap, int qGrowth):
RefCounted(1U /*initialRefCount*/),
NewsSubject(name, 1U /*initialRefCount*/, qCap, qGrowth)
{
}

HeadlineNews::~HeadlineNews()
{
}

END_NAMESPACE

BEGIN_NAMESPACE1(appkit)


//!
//! Construct observable subject. With this observable subject, updates are performed in the
//! caller's (i.e., newsmaker's) context, and notifications are performed in a dedicated news
//! anchor's context. News stories can be queued for the news anchor consumption. The queue
//! has an initial capacity of qCap stories. The queue does not grow if qGrowth is zero,
//! exponentially grows by doubling if qGrowth is negative, and grows by qGrowth stories
//! otherwise.
//!
NewsSubject::NewsSubject(const char* name, unsigned int initialRefCount, unsigned int qCap, int qGrowth):
RefCounted(initialRefCount),
Observable(name, initialRefCount),
ItemQ::Item(),
storyQ_(qCap, qGrowth)
{
    indexInSource_ = Vec::INVALID_INDEX;
    newsAnchor_ = NewsAnchor::instance();
    newsAnchor_->addRef();
    newsAnchor_->addSource(this);
    story_ = 0;
}


NewsSubject::~NewsSubject()
{
    NewsStory::release(story_);
    newsAnchor_->rmSource(this);
    newsAnchor_->rmRef();
}


//!
//! This is how a news story is created. The story is then broadcasted to its audience
//! by a dedicated news anchor. With respect to ownership, the news anchor claims the
//! ownership and will destroy the story after its broadcast using the delete operator.
//!
NewsStory* NewsSubject::createStory(unsigned long long filetime, const String& summary)
{
    NewsStory* story = new NewsStory(filetime, summary);
    return story;
}


//!
//! Attach given observer to a named observable subject. Return attached observable.
//! Return zero if not found.
//!
NewsSubject* NewsSubject::attachByName(Observer* observer, const char* name)
{
    const NewsAnchor* anchor = NewsAnchor::instance();
    NewsSubject* attachedSource = 0;
    doAttach1Arg_t arg(&attachedSource, observer, name);
    anchor->apply(doAttach1, &arg);
    return attachedSource;
}


//!
//! Create observable one-liner news. Each news story contains only the basic free-format
//! summary string and nothing more. The newsmaker performs updates using update(), and its
//! audience retrieves the news stories via summary() or magnifiedSummary(). The returned
//! observable must be freed using rmRef() when done. News stories can be queued for the
//! news anchor consumption. The queue has an initial capacity of qCap stories. The queue
//! does not grow if qGrowth is zero, exponentially grows by doubling if qGrowth is negative,
//! and grows by qGrowth stories otherwise.
//!
NewsSubject* NewsSubject::createHeadline(const char* name, unsigned int qCap, int qGrowth)
{
    NewsSubject* subject = new HeadlineNews(name, qCap, qGrowth);
    return subject;
}


//!
//! Detach given observer from a named observable subject. Return detached observable.
//! Return zero if not found.
//!
NewsSubject* NewsSubject::detachByName(Observer* observer, const char* name)
{
    const NewsAnchor* anchor = NewsAnchor::instance();
    NewsSubject* detachedSource = 0;
    doDetach1Arg_t arg(&detachedSource, observer, name);
    anchor->apply(doDetach1, &arg);
    return detachedSource;
}


//!
//! Attach given observer to named observable subjects. Return names of the successfully
//! attached observables.
//!
StringVec NewsSubject::attachByNames(Observer* observer, const StringVec* names)
{
    StringVec attached;
    if (names && (names->numItems() > 0))
    {
        const NewsAnchor* anchor = NewsAnchor::instance();
        StringVec sortedNames(*names);
        sortedNames.sort();
        doAttachNArg_t arg(&attached, observer, &sortedNames);
        anchor->apply(doAttachN, &arg);
    }

    return &attached; //move guts from attached to returned vector
}


//!
//! Detach given observer from named observable subjects. Return names of the successfully
//! detached observables.
//!
StringVec NewsSubject::detachByNames(Observer* observer, const StringVec* names)
{
    StringVec detached;
    if (names && (names->numItems() > 0))
    {
        const NewsAnchor* anchor = NewsAnchor::instance();
        StringVec sortedNames(*names);
        sortedNames.sort();
        doDetachNArg_t arg(&detached, observer, &sortedNames);
        anchor->apply(doDetachN, &arg);
    }

    return &detached; //move guts from detached to returned vector
}


bool NewsSubject::doAttach1(void* arg, NewsSubject* source)
{
    doAttach1Arg_t* p = static_cast<doAttach1Arg_t*>(arg);
    bool keepGoing = (strcmp(p->name, source->name()) != 0);
    if (!keepGoing)
    {
        source->attach(p->observer); //failure indicates already attached
        *p->attachedSource = source;
    }

    return keepGoing;
}


bool NewsSubject::doAttachN(void* arg, NewsSubject* source)
{
    bool keepGoing = true;
    doAttachNArg_t* p = static_cast<doAttachNArg_t*>(arg);
    String name(source->name());
    if (p->sortedNames->search(name))
    {
        source->attach(p->observer); //failure indicates already attached
        p->attached->add(name);
        if (p->attached->numItems() == p->sortedNames->numItems())
        {
            keepGoing = false;
        }
    }

    return keepGoing;
}


bool NewsSubject::doDetach1(void* arg, NewsSubject* source)
{
    doDetach1Arg_t* p = static_cast<doDetach1Arg_t*>(arg);
    bool keepGoing = (strcmp(p->name, source->name()) != 0);
    if (!keepGoing)
    {
        const Observer* detachedObserver = source->detach(p->observer);
        *p->detachedSource = detachedObserver? source: 0;
    }

    return keepGoing;
}


bool NewsSubject::doDetachN(void* arg, NewsSubject* source)
{
    bool keepGoing = true;
    doDetachNArg_t* p = static_cast<doDetachNArg_t*>(arg);
    String name(source->name());
    if (p->sortedNames->search(name))
    {
        const Observer* detachedObserver = source->detach(p->observer);
        if (detachedObserver)
        {
            p->detached->add(name);
            if (p->detached->numItems() == p->sortedNames->numItems())
            {
                keepGoing = false;
            }
        }
    }

    return keepGoing;
}


//
// Create and enqueue story for news anchor consumption.
// Return true if successful.
//
bool NewsSubject::putStory(unsigned long long filetime, const String& summary)
{
    NewsStory* story = createStory(filetime, summary);
    unsigned int timeoutInMsecs = 0;
    bool ok = storyQ_.put(story, timeoutInMsecs);
    return ok;
}


//
// Dequeue story.
// Story is to be broadcasted to its audience by the news anchor.
//
const NewsStory* NewsSubject::getStory()
{
    ItemQ::Item* item = 0;
    storyQ_.get(item);
    story_ = dynamic_cast<const NewsStory*>(item);
    return story_;
}


//!
//! Mark subject as updated and return current time as latest update time. Also associate
//! this update with given summary. Observers will be notified ASAP via notify() and
//! Observer::onUpdate(). Notifications are performed in a separate news anchor thread.
//! To clarify, everytime a news subject is updated, a story is created via createStory().
//! This story is then broadcasted to its audience by a dedicated news anchor. As a result,
//! multiple stories within one news subject can be in the queue waiting to be broadcasted.
//!
unsigned long long NewsSubject::update(const String& summary)
{
    unsigned long long updateTime = Observable::update(summary);
    putStory(updateTime, summary);
    newsAnchor_->observe(this);
    return updateTime;
}


void NewsSubject::release() const
{
    rmRef();
}


//
// Nullify the most recently broadcasted story.
//
void NewsSubject::nullifyStory()
{
    NewsStory::release(story_);
    story_ = 0;
}

END_NAMESPACE1
