#include "appkit/CmdLine.hpp"
#include "appkit/NewsAnchor.hpp"
#include "appkit/NewsObserver.hpp"
#include "appkit/NewsStory.hpp"
#include "appkit/NewsSubject.hpp"
#include "appkit/Observable.hpp"
#include "appkit/Observer.hpp"
#include "appkit/U32.hpp"
#include "syskit/ItemQ.hpp"
#include "syskit/Singleton.hpp"
#include "syskit/Thread.hpp"
#include "syskit/Utc.hpp"

#include "appkit-ut-pch.h"
#include "ObserverSuite.hpp"

using namespace appkit;
using namespace syskit;

const char NEWS_ID1[] = "anonymous::News1";
const char OBSERVABLE_SUBJECT0_ID1[] = "anonymous::ObservableSubject0";
const char OBSERVABLE_SUBJECT1_ID1[] = "anonymous::ObservableSubject1";

BEGIN_NAMESPACE


//
// Sample observable subject singleton.
//
class ObservableSubject: public Observable, public Singleton
{
public:
    static ObservableSubject* instance0();
    static ObservableSubject* instance1();
protected:
    ObservableSubject(const char* singletonId, const char* subjectName, unsigned int initialRefCount);
    virtual ~ObservableSubject();
private:
    ObservableSubject(const ObservableSubject&); //prohibit usage
    const ObservableSubject& operator =(const ObservableSubject&); //prohibit usage
    static Singleton* create(const char* id, unsigned int initialRefCount, void* arg);
};

ObservableSubject::ObservableSubject(const char* singletonId, const char* subjectName, unsigned int initialRefCount):
RefCounted(initialRefCount),
Observable(subjectName, initialRefCount),
Singleton(singletonId, initialRefCount)
{
}

ObservableSubject::~ObservableSubject()
{
}

ObservableSubject* ObservableSubject::instance0()
{
    static ObservableSubject* s_subject = dynamic_cast<ObservableSubject*>(getSingleton(OBSERVABLE_SUBJECT0_ID1,
        create,
        0U /*initialRefCount*/,
        0 /*createArg*/));
    static ObservableSubject::Count s_lock(*s_subject);
    return s_subject;
}

ObservableSubject* ObservableSubject::instance1()
{
    static ObservableSubject* s_subject = dynamic_cast<ObservableSubject*>(getSingleton(OBSERVABLE_SUBJECT1_ID1,
        create,
        0U /*initialRefCount*/,
        0 /*createArg*/));
    static ObservableSubject::Count s_lock(*s_subject);
    return s_subject;
}

Singleton* ObservableSubject::create(const char* id, unsigned int initialRefCount, void* /*arg*/)
{
    const char* subjectName = id;
    Singleton* subject1 = new ObservableSubject(id, subjectName, initialRefCount);
    return subject1;
}


//
// Sample observable news subject singleton.
//
class News1: public NewsSubject, public Singleton
{
public:
    News1(const char* singletonId, const char* subjectName, unsigned int initialRefCount);
    virtual ~News1();
    static News1* instance();
private:
    News1(const News1&); //prohibit usage
    const News1& operator =(const News1&); //prohibit usage
    static Singleton* create(const char* id, unsigned int initialRefCount, void* arg);
};

News1::News1(const char* singletonId, const char* subjectName, unsigned int initialRefCount):
RefCounted(initialRefCount),
NewsSubject(subjectName, initialRefCount),
Singleton(singletonId, initialRefCount)
{
}

News1::~News1()
{
}

News1* News1::instance()
{
    static News1* s_news = dynamic_cast<News1*>(getSingleton(NEWS_ID1, create, 0U /*initialRefCount*/, 0 /*createArg*/));
    static News1::Count s_lock(*s_news);
    return s_news;
}

Singleton* News1::create(const char* id, unsigned int initialRefCount, void* /*arg*/)
{
    const char* subjectName = id;
    Singleton* subject1 = new News1(id, subjectName, initialRefCount);
    return subject1;
}


//
// Sample observable news subject.
//
class News: public NewsSubject
{
public:
    News(const char* name);
protected:
    virtual ~News();
private:
    News(const News&); //prohibit usage
    const News& operator =(const News&); //prohibit usage
};

News::News(const char* name):
RefCounted(1U /*initialRefCount*/),
NewsSubject(name, 1U /*initialRefCount*/)
{
}

News::~News()
{
}


//
// Sample attached observer.
//
class SubjectObserver: public Observer
{
public:
    SubjectObserver(const char* name, Observable& subject);
    const String& updatesObserved() const;
protected:
    virtual ~SubjectObserver();
    virtual void onUpdate(unsigned long long time, const appkit::String& summary, Observable* subject);
private:
    Observable* subject_;
    String updatesObserved_;
    SubjectObserver(const SubjectObserver&); //prohibit usage
    const SubjectObserver& operator =(const SubjectObserver&); //prohibit usage
};

inline const String& SubjectObserver::updatesObserved() const
{
    return updatesObserved_;
}

SubjectObserver::SubjectObserver(const char* name, Observable& subject):
RefCounted(1U /*initialRefCount*/),
Observer(name, 1U /*initialRefCount*/),
updatesObserved_()
{
    subject_ = &subject;
    subject_->attach(this);
}

SubjectObserver::~SubjectObserver()
{
    subject_->detach(this);
}

void SubjectObserver::onUpdate(unsigned long long /*time*/, const String& summary, Observable* subject)
{
    updatesObserved_ += summary;
    NewsSubject* news = dynamic_cast<NewsSubject*>(subject);
    if (news != 0)
    {
        return;
    }

    subject->detach(this);
    unsigned long long updateTime = subject->update(summary);
    subject->notify(updateTime, summary);
    subject->attach(this);
}

END_NAMESPACE


ObserverSuite::ObserverSuite()
{
    const NewsAnchor* anchor = NewsAnchor::instance();
    anchor->addRef();
}


ObserverSuite::~ObserverSuite()
{
    const NewsAnchor* anchor = NewsAnchor::instance();
    anchor->rmRef();
}


bool ObserverSuite::cb0a(void* arg, Observer* observer)
{
    StringVec* observerNames = static_cast<StringVec*>(arg);
    String name(observer->name());
    bool keepGoing = (observerNames->rm(name));
    return keepGoing;
}


bool ObserverSuite::cb0b(void* /*arg*/, Observer* /*observer*/)
{
    bool keepGoing = false;
    return keepGoing;
}


void ObserverSuite::cb1a(void* arg, Observer* observer)
{
    StringVec* observerNames = static_cast<StringVec*>(arg);
    String name(observer->name());
    observerNames->rm(name);
}


void ObserverSuite::testApply00()
{
    NewsSubject* news = new News("anonymous::News");
    Observable::cb1_t cb1 = 0;
    Observable::cb0_t cb0 = 0;
    void* arg = 0;
    news->apply(cb1, arg);
    bool ok = news->apply(cb0, arg);
    CPPUNIT_ASSERT(ok);

    SubjectObserver* observer0 = new SubjectObserver("observer0", *news);
    StringVec observerNames;
    observerNames.add("observer0");
    cb1 = cb1a;
    arg = &observerNames;
    ok = (news->apply(cb1, arg), (observerNames.numItems() == 0));
    CPPUNIT_ASSERT(ok);
    cb0 = cb0a;
    observerNames.add("observer0");
    ok = news->apply(cb0, arg) && (observerNames.numItems() == 0);
    CPPUNIT_ASSERT(ok);

    SubjectObserver* observer1 = new SubjectObserver("observer1", *news);
    SubjectObserver* observer2 = new SubjectObserver("observer2", *news);
    observerNames.add("observer0");
    observerNames.add("observer1");
    observerNames.add("observer2");
    ok = (news->apply(cb1, arg), (observerNames.numItems() == 0));
    CPPUNIT_ASSERT(ok);
    cb0 = cb0b;
    ok = (!news->apply(cb0, arg));
    CPPUNIT_ASSERT(ok);
    cb0 = cb0a;
    observerNames.add("observer0");
    observerNames.add("observer1");
    observerNames.add("observer2");
    ok = news->apply(cb0, arg) && (observerNames.numItems() == 0);
    CPPUNIT_ASSERT(ok);

    observer2->rmRef();
    observer1->rmRef();
    observer0->rmRef();
    news->rmRef();
}


void ObserverSuite::testAttachByName00()
{
    NewsObserver* observer = 0;
    NewsSubject* attached = NewsSubject::attachByName(observer, "not-a-news-source");
    bool ok = (attached == 0);
    CPPUNIT_ASSERT(ok);

    observer = new NewsObserver("anonymous::News0", "observer");
    ok = (observer->subject() == "anonymous::News0");
    CPPUNIT_ASSERT(ok);
    ok = observer->isIdle() && (!observer->isActive()) && observer->isDetached() && (!observer->isAttached());
    CPPUNIT_ASSERT(ok);
    observer->activate();
    ok = (!observer->isIdle()) && observer->isActive() && observer->isDetached() && (!observer->isAttached());
    CPPUNIT_ASSERT(ok);
    observer->activate(); //no-op
    ok = (!observer->isIdle()) && observer->isActive() && observer->isDetached() && (!observer->isAttached());
    CPPUNIT_ASSERT(ok);
    observer->deactivate();
    ok = observer->isIdle() && (!observer->isActive()) && observer->isDetached() && (!observer->isAttached());
    CPPUNIT_ASSERT(ok);
    observer->deactivate(); //no-op
    ok = observer->isIdle() && (!observer->isActive()) && observer->isDetached() && (!observer->isAttached());
    CPPUNIT_ASSERT(ok);

    NewsSubject* news = new News("anonymous::News0");
    observer->activate();
    ok = (!observer->isIdle()) && observer->isActive() && (!observer->isDetached()) && observer->isAttached();
    CPPUNIT_ASSERT(ok);
    observer->deactivate();
    ok = observer->isIdle() && (!observer->isActive()) && observer->isDetached() && (!observer->isAttached());
    CPPUNIT_ASSERT(ok);
    news->rmRef();
    observer->rmRef();
}


void ObserverSuite::testCtor00()
{
    ObservableSubject* subject = ObservableSubject::instance0();

    bool ok = (String(subject->id()) == OBSERVABLE_SUBJECT0_ID1) &&
        (String(subject->name()) == OBSERVABLE_SUBJECT0_ID1) &&
        (subject->updateSummary().empty()) &&
        (subject->updateTime() == Utc::ZERO) &&
        (subject->notifyCount() == 0U) &&
        (subject->numAttachedObservers() == 0U) &&
        (subject->updateCount() == 0U);
    CPPUNIT_ASSERT(ok);

    String updateSummary("0");
    unsigned long long updateTime = subject->update(updateSummary);
    Utc updateUtc(updateTime);
    ok = (updateUtc == subject->updateTime()) && (subject->updateSummary() == updateSummary) && (subject->updateCount() == 1U);
    CPPUNIT_ASSERT(ok);

    unsigned int observerCount = subject->notify(updateTime, updateSummary);
    ok = (observerCount == 0U);
    CPPUNIT_ASSERT(ok);

    SubjectObserver* observer = new SubjectObserver("observer", *subject);
    ok = (!subject->attach(observer)) && (subject->numAttachedObservers() == 1U);
    CPPUNIT_ASSERT(ok);
    Thread::takeANap(10);
    updateSummary = "1";
    observerCount = subject->notify(subject->update(updateSummary), updateSummary);
    ok = (updateUtc < subject->updateTime()) &&
        (subject->updateSummary() == updateSummary) &&
        (observerCount == 1U) &&
        (observer->updatesObserved() == updateSummary);
    CPPUNIT_ASSERT(ok);

    subject = ObservableSubject::instance1();
    ok = subject->attach(observer) && (!subject->attach(observer)) && (subject->numAttachedObservers() == 1U);
    CPPUNIT_ASSERT(ok);
    ok = subject->detach(observer) && (!subject->detach(observer)) && (subject->numAttachedObservers() == 0U);
    CPPUNIT_ASSERT(ok);

    observer->rmRef();
}


void ObserverSuite::testCtor01()
{
    News1* news1 = News1::instance();
    SubjectObserver* observer = new SubjectObserver("observer", *news1);
    bool ok = (observer->updateCount() == 0U);
    CPPUNIT_ASSERT(ok);

    // Flood the news room.
    // Let the anchor finish broadcasting all items.
    String updateSummary;
    String updates;
    U32 u32;
    for (unsigned int i = 0; i < 100; ++i)
    {
        u32 = i;
        updateSummary = u32;
        news1->update(updateSummary);
        updates += updateSummary;
    }
    NewsAnchor* anchor = NewsAnchor::instance();
    for (; anchor->toBeAiredQ().numItems() > 0; Thread::takeANap(10));
    ok = (observer->updateCount() == 100) && (observer->updatesObserved().endsWith(updates));
    CPPUNIT_ASSERT(ok);

    // Flood the news room.
    // The anchor should fall behind.
    for (unsigned int i = 0; i < 9999; ++i)
    {
        u32 = i;
        updateSummary = u32;
        news1->update(updateSummary);
        updates += updateSummary;
    }
    ok = (anchor->toBeAiredQ().numItems() > 0) &&
        (observer->updateCount() < 100 + 9999) &&
        (!observer->updatesObserved().endsWith(updates));
    CPPUNIT_ASSERT(ok);

    // Let the anchor finish broadcasting all items.
    for (; anchor->toBeAiredQ().numItems() > 0; Thread::takeANap(10));
    ok = observer->updatesObserved().endsWith(updates);
    CPPUNIT_ASSERT(ok);
    ItemQ& toBeAiredQ = anchor->toBeAiredQ();
    toBeAiredQ.resetStat();
    ok = (ItemQ::Stat(toBeAiredQ).numGets() == 0);
    CPPUNIT_ASSERT(ok);
    observer->rmRef();
}


void ObserverSuite::testFindSourceByName00()
{
    NewsSubject* news = new News("anonymous::News");
    const NewsAnchor* anchor = NewsAnchor::instance();
    bool ok = (news->refCount() == 1) &&
        (anchor->findSourceByName("anonymous::News") == news) &&
        (anchor->findSourceByName("not-a-news-source") == 0);
    CPPUNIT_ASSERT(ok);
    news->rmRef();

    News1* news1 = News1::instance();
    const char* name = news1->name();
    ok = (anchor->findSourceByName(name) == news1);
    CPPUNIT_ASSERT(ok);

    news = NewsSubject::createHeadline("yaSubject");
    news->update("1");
    news->update("2");
    news->update("3");
    Thread::takeANap(10); //give news anchor time to observe the updates even though there are no observers
    const NewsSubject* yaSubject = anchor->findSourceByName("yaSubject");
    ok = (yaSubject->refCount() == 1) && (yaSubject == news);
    CPPUNIT_ASSERT(ok);
    news->rmRef();
}


void ObserverSuite::testSize00()
{
    bool ok = (sizeof(NewsStory) == (sizeof(void*) * 3) + 8);
    CPPUNIT_ASSERT(ok);
}
