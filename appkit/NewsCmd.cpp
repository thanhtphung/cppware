/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/ItemQ.hpp"
#include "syskit/Semaphore.hpp"
#include "syskit/Tree.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/Bool.hpp"
#include "appkit/NewsAnchor.hpp"
#include "appkit/NewsCmd.hpp"
#include "appkit/NewsSubject.hpp"
#include "appkit/Observer.hpp"
#include "appkit/StringVec.hpp"
#include "appkit/U32.hpp"
#include "appkit/crt.hpp"

using namespace appkit;
using namespace netkit;
using namespace syskit;

inline unsigned int maxOf(unsigned int a, unsigned int b)
{
    return (a > b)? a: b;
}

// Supported command set.
const char CMD_SET[] =
" news-make"
" news-show"
" news-showaudience"
" news-showq"
" news-watch"
;

// Usage texts. One per command. Must match supported command set.
const char USAGE_0[] =
"Usage:\n"
"  news-make --subject=xxx summary\n\n"
"Examples:\n"
"  news-make --subject=appkit::CmdNews cmd --add news-make\n"
"  news-make --subject=appkit::SourceNews source --rm appkit::CmdNews\n"
;

const char USAGE_1[] =
"Usage:\n"
"  news-show [--headlines]\n"
"  news-show [--v]\n\n"
"Examples:\n"
"  news-show\n"
"  news-show --headlines\n"
"  news-show --v\n"
;

const char USAGE_2[] =
"Usage:\n"
"  news-showaudience [--all]\n\n"
"Examples:\n"
"  news-showaudience\n"
"  news-showaudience --all\n"
;

const char USAGE_4[] =
"Usage:\n"
"  news-watch [--maxheadlines=xxx] [--allbutthese] [subject...]\n"
"             [--maxsecs=xxx]\n\n"
"Examples:\n"
"  news-watch\n"
"  news-watch --maxsecs=30 devicekit::Orientation\n"
;

const char* const USAGE[] =
{
    USAGE_0, //news-make
    USAGE_1, //news-show
    USAGE_2, //news-showaudience
    "",      //news-showq
    USAGE_4  //news-watch
};

// Extended names. One per command. Must match supported command set.
const char* const X_NAME[] =
{
    "fabricate news stories", //news-make
    "show news subjects",     //news-show
    "show news observers",    //news-showaudience
    "show news queue",        //news-showq
    "watch ongoing news"      //news-showq
};

const double SECS_PER_FILETIME = 1e-7; //1 filetime == 100 nsecs
const unsigned int MSECS_PER_SEC = 1000;
const unsigned int NEWS_WATCH_HEADLINES = 300;
const unsigned int NEWS_WATCH_SECS = 9;

BEGIN_NAMESPACE


// collection of known news observers
class KnownAudience: private Tree
{
public:
    using Tree::compare_t;
    using Tree::apply;
    using Tree::numItems;
    KnownAudience(compare_t compare);
    const KnownAudience& operator =(KnownAudience* that);
    String stringify(const char* delim, size_t maxItems) const;
    bool add(Observer* observer);
    bool rm(Observer* observer);
    unsigned int getMaxNameLength() const;
    virtual ~KnownAudience();
    static int compareName(const void*, const void*);
private:
    KnownAudience(const KnownAudience&); //prohibit usage
    const KnownAudience& operator =(const KnownAudience&); //prohibit usage
    static void checkNameLength(void*, void*);
    static void move(void*, void*);
    static void rmRef(void*, void*);
    static void stringify(void*, void*);
};

KnownAudience::KnownAudience(compare_t compare):
Tree(compare)
{
}

KnownAudience::~KnownAudience()
{
    void* arg = 0;
    apply(rmRef, arg);
}

//! Transfer resource ownership. Nullify source.
const KnownAudience& KnownAudience::operator =(KnownAudience* that)
{
    that->apply(move, this);
    that->reset();
    return *this;
}

//! Return string containing audience member names.
String KnownAudience::stringify(const char* delim, size_t maxItems) const
{
    StringVec names;
    apply(stringify, &names);
    return names.stringify(delim, maxItems);
}

//! Add given observer to collection.
//! Return true if successful (also increment observer reference count).
bool KnownAudience::add(Observer* observer)
{
    observer->addRef();
    bool ok = Tree::add(observer);
    if (!ok)
    {
        observer->rmRef();
    }

    return ok;
}

//! Remove given observer from collection.
//! Return true if successful (also decrement observer reference count).
bool KnownAudience::rm(Observer* observer)
{
    bool ok = Tree::rm(observer);
    if (ok)
    {
        observer->rmRef();
    }

    return ok;
}

// Compare opaque items as Observer* such that result is sorted by observer names.
int KnownAudience::compareName(const void* item0, const void* item1)
{
    const char* k0 = static_cast<const Observer*>(item0)->name();
    const char* k1 = static_cast<const Observer*>(item1)->name();
    int rc = strcmp(k0, k1);
    if (rc == 0)
    {
        rc = (item0 < item1)? (-1): ((item0>item1)? 1: 0);
    }

    return rc;
}

//! Determine and return maximum observer name length.
//! Return a minimum length of 8.
unsigned int KnownAudience::getMaxNameLength() const
{
    unsigned int maxNameLength = 8;
    void* arg = &maxNameLength;
    apply(checkNameLength, arg);
    return maxNameLength;
}

// This callback is invoked for each item in a KnownAudience instance.
// Determine maximum name length.
void KnownAudience::checkNameLength(void* arg, void* item)
{
    const Observer* observer = static_cast<const Observer*>(item);
    size_t nameLength = strlen(observer->name());
    unsigned int* maxNameLength = static_cast<unsigned int*>(arg);
    if (nameLength > *maxNameLength)
    {
        *maxNameLength = static_cast<unsigned int>(nameLength);
    }
}

// This callback is invoked for each item from a source KnownAudience instance.
// Move item into a destination KnownAudience instance specified by arg.
void KnownAudience::move(void* arg, void* item)
{
    KnownAudience* dst = static_cast<KnownAudience*>(arg);
    dst->Tree::add(item);
}

// This callback is invoked for each item in a KnownAudience instance.
// Decrement reference count of each.
void KnownAudience::rmRef(void* /*arg*/, void* item)
{
    const Observer* observer = static_cast<const Observer*>(item);
    observer->rmRef();
}

// This callback is invoked for each item in a KnownAudience instance.
// Collect observer names.
void KnownAudience::stringify(void* arg, void* item)
{
    StringVec* names = static_cast<StringVec*>(arg);
    const Observer* observer = static_cast<const Observer*>(item);
    names->add(observer->name());
}


class NewsWatch: public Observer
{
public:
    NewsWatch(Semaphore* sem, const CmdLine* activeReq, unsigned int maxHeadlines);
    virtual ~NewsWatch();
protected:
    virtual void onUpdate(unsigned long long filetime, const String& summary, Observable* subject);
private:
    const CmdLine* activeReq_;
    unsigned int headlines_;
    unsigned int maxHeadlines_;
    Semaphore* sem_;
    NewsWatch(const NewsWatch&); //prohibit usage
    const NewsWatch& operator =(const NewsWatch&); //prohibit usage
    void cancel();
};

NewsWatch::NewsWatch(Semaphore* sem, const CmdLine* activeReq, unsigned int maxHeadlines):
RefCounted(1U /*initialRefCount*/),
Observer("appkit::NewsWatch", 1U /*initialRefCount*/)
{
    activeReq_ = activeReq;
    headlines_ = 0;
    maxHeadlines_ = maxHeadlines;
    sem_ = sem;
}

NewsWatch::~NewsWatch()
{
}

void NewsWatch::cancel()
{
    sem_->increment();
}

void NewsWatch::onUpdate(unsigned long long /*filetime*/, const String& /*summary*/, Observable* subject)
{
    const NewsSubject* source = dynamic_cast<const NewsSubject*>(subject);
    const NewsStory* story = source->story();
    String headline(NewsAnchor::formHeadline(*source, *story));
    bool ok = Cmd::formRsp(*activeReq_, "%s", headline.ascii());
    if ((++headlines_ >= maxHeadlines_) || (!ok))
    {
        cancel();
    }
}

END_NAMESPACE

BEGIN_NAMESPACE1(appkit)


// Command doers. One per command. Must match supported command set.
NewsCmd::doer_t NewsCmd::doer_[] =
{
    &NewsCmd::doMake,         //news-make
    &NewsCmd::doShow,         //news-show
    &NewsCmd::doShowAudience, //news-showaudience
    &NewsCmd::doShowQ,        //news-showq
    &NewsCmd::doWatch         //news-watch
};


NewsCmd::NewsCmd():
Cmd(CMD_SET)
{
}


NewsCmd::~NewsCmd()
{
}


//!
//! This method is invoked when a command is being authenticated. The method returns
//! true/false to allow/disallow the command execution.
//!
bool NewsCmd::allowReq(const CmdLine& req, const Paddr& /*dbugger*/)
{

    // Always log this command.
    if (cmdIndex(req) == 0) //news-make
    {
        overrideLogMode(req, true /*alwaysLogReq*/, false /*alwaysLogRsp*/);
    }

    bool allow = true;
    return allow;
}


//
// news-make --subject=xxx summary
//
bool NewsCmd::doMake(const CmdLine& req)
{
    String defaultV;
    const char* optK = "subject";
    const String* subject = req.opt(optK, &defaultV);
    const NewsAnchor& newsAnchor = *NewsAnchor::instance();
    NewsSubject* source = newsAnchor.findSourceByName(subject->ascii());
    bool cmdIsValid = (source != 0) && (req.numArgs() > 1);
    if (cmdIsValid)
    {
        size_t startAt = 1;
        size_t itemCount = req.numArgs() - 1;
        const char* delim = " ";
        String summary(StringVec(req.argVec(), startAt, itemCount).stringify(delim));
        size_t count = 2;
        char c = 0;
        summary.append(count, c); //make this a double-null terminated string to differentiate against real updates
        source->update(summary);
        String rsp("Done.");
        respond(req, rsp);
    }

    return cmdIsValid;
}


//
// news-show [--headlines]
// news-show [--v]
//
bool NewsCmd::doShow(const CmdLine& req)
{
    String rsp;
    StringVec tbl;
    const NewsAnchor& newsAnchor = *NewsAnchor::instance();
    bool showHeadlines = Bool(req.opt("headlines"), false /*defaultV*/);
    bool beVerbose = Bool(req.opt("v"), false /*defaultV*/);
    unsigned int nameWidth = maxOf(newsAnchor.maxSourceNameLength(), 10);
    unsigned long long nowAsFiletime = Utc::nowAsFiletime();
    void* arg[3] = {&tbl, &nameWidth, &nowAsFiletime};
    NewsAnchor::cb1_t cb = beVerbose? (formSubjectLine9): (showHeadlines? formSubjectSummary: formSubjectLine);
    newsAnchor.apply(cb, arg);
    if (tbl.numItems() == 0)
    {
        rsp = "None.";
    }

    // Show stats per news source.
    // Order by name.
    else
    {
        String buf;
        tbl.sort();
        rsp = (sprintf(buf, "  %-*s%11s%11s%11s%7s\n", nameWidth, "observable", "observers", "updates", "notifies", "wmark"), buf);
        rsp += (sprintf(buf, "  %-*s%11s%11s%11s%7s\n", nameWidth, "----------", "---------", "-------", "--------", "-----"), buf);
        rsp += tbl.stringify();
    }

    respond(req, rsp);
    bool cmdIsValid = true;
    return cmdIsValid;
}


//
// news-showaudience [--all]
//
bool NewsCmd::doShowAudience(const CmdLine& req)
{
    String tbl;
    const NewsAnchor& newsAnchor = *NewsAnchor::instance();
    Tree::compare_t compare = 0;
    KnownAudience observers(compare);
    newsAnchor.apply(getAudience, &observers);
    if (observers.numItems() == 0)
    {
        tbl = "None.";
    }

    // Show stats per observer.
    // Order by name.
    else
    {
        String buf;
        bool showAll = Bool(req.opt("all"), false /*defaultV*/);
        unsigned int nameWidth = observers.getMaxNameLength();
        tbl += (sprintf(buf, "  %-*s%11s\n", nameWidth, "observer", "updates"), buf);
        tbl += (sprintf(buf, "  %-*s%11s\n", nameWidth, "--------", "-------"), buf);
        KnownAudience audience(KnownAudience::compareName);
        audience = &observers;
        void* arg[3] = {&tbl, &nameWidth, &showAll};
        audience.apply(formObserverLine, arg);
    }

    respond(req, tbl);
    bool cmdIsValid = true;
    return cmdIsValid;
}


//
// news-showq
//
bool NewsCmd::doShowQ(const CmdLine& req)
{
    const NewsAnchor& newsAnchor = *NewsAnchor::instance();
    const ItemQ& toBeAiredQ = newsAnchor.toBeAiredQ();
    ItemQ::Stat stat(toBeAiredQ);

    formRsp(req, "%8s: newsmakers --> newsanchor\n", "newsq");
    formRsp(req, "%8s:\n", "------");
    formRsp(req, "%8s: %u\n", "cap", stat.capacity());
    formRsp(req, "%8s: %u\n", "cap0", stat.initialCap());
    formRsp(req, "%8s: %d\n", "grow", stat.growthFactor());
    formRsp(req, "%8s: %u\n", "length", stat.length());
    formRsp(req, "%8s: %u\n", "wmark", stat.usagePeak());
    formRsp(req, "%8s: %llu\n", "gets", stat.numGets());
    formRsp(req, "%8s: %llu\n", "puts", stat.numPuts());
    formRsp(req, "%8s: %u\n", "fails", stat.numFails());

    respond(req, "", 1);
    bool cmdIsValid = true;
    return cmdIsValid;
}


//
// news-watch [--maxheadlines=xxx] [--allbutthese] [subject...]
//            [--maxsecs=xxx     ]
//
bool NewsCmd::doWatch(const CmdLine& req)
{

    // Identify news subjects of interest.
    const NewsAnchor& newsAnchor = *NewsAnchor::instance();
    StringVec* subjectNames = new StringVec;
    if (req.numArgs() > 1)
    {
        const StringVec& argVec = req.argVec();
        size_t startAt = 1;
        size_t itemCount = argVec.numItems() - 1;
        if (req.opt("allbutthese")) //watch all but these news subjects
        {
            newsAnchor.apply(getSubjects, subjectNames);
            *subjectNames = *subjectNames - StringVec(argVec, startAt, itemCount);
        }
        else //watch only these news subjects
        {
            *subjectNames = StringVec(argVec, startAt, itemCount);
        }
    }
    else //watch all
    {
        newsAnchor.apply(getSubjects, subjectNames);
    }

    unsigned int maxHeadlines = U32(req.opt("maxheadlines"), NEWS_WATCH_HEADLINES);
    unsigned int maxSecs = U32(req.opt("maxsecs"), NEWS_WATCH_SECS);

    Semaphore sem(0U);
    NewsWatch* newsWatch = new NewsWatch(&sem, &req, maxHeadlines);
    NewsSubject::attachByNames(newsWatch, subjectNames);
    unsigned int waitTimeInMsecs = maxSecs * MSECS_PER_SEC;
    sem.decrement(waitTimeInMsecs);
    NewsSubject::detachByNames(newsWatch, subjectNames);
    delete subjectNames;
    newsWatch->rmRef();

    String rsp("Done.");
    respond(req, rsp);
    bool cmdIsValid = true;
    return cmdIsValid;
}


//!
//! Run command using dedicated agent threads provided by the Cmd framework. Return
//! true if command is valid. A false return results in some command usage hints as
//! response to the request.
//!
bool NewsCmd::onRun(const CmdLine& req)
{
    doer_t doer = doer_[cmdIndex(req)];
    bool cmdIsValid = (this->*doer)(req);
    return cmdIsValid;
}


const char* NewsCmd::usage(unsigned char cmdIndex) const
{
    return USAGE[cmdIndex];
}


const char* NewsCmd::xName(unsigned char cmdIndex) const
{
    return X_NAME[cmdIndex];
}


//
// Form observer line in news-showaudience output.
// This callback is invoked for each known news observer.
//
void NewsCmd::formObserverLine(void* arg, void* item)
{
    void** p = static_cast<void**>(arg);
    const bool* showAll = static_cast<const bool*>(p[2]);

    // Skip proxies if not showing all.
    const Observer* observer = static_cast<const Observer*>(item);
    if ((!*showAll) && String(observer->name()).endsWith(".proxy"))
    {
        return;
    }

    String* tbl = static_cast<String*>(p[0]);
    const unsigned int* nameWidth = static_cast<const unsigned int*>(p[1]);

    String buf;
    *tbl += (sprintf(buf, "  %-*s%11u\n", *nameWidth, observer->name(), observer->updateCount()), buf);
}


//
// Form subject line in news-show output.
// This callback is invoked for each known news source.
//
void NewsCmd::formSubjectLine(void* arg, NewsSubject* source)
{

    // Don't care about idle sources.
    ItemQ::Stat stat(source->storyQ());
    if ((source->numAttachedObservers() == 0) && (stat.usagePeak() == 0))
    {
        return;
    }

    void** p = static_cast<void**>(arg);
    StringVec* tbl = static_cast<StringVec*>(p[0]);
    const unsigned int* nameWidth = static_cast<const unsigned int*>(p[1]);

    String buf;
    sprintf(buf, "  %-*s%11u%11u%11u%7u\n",
        *nameWidth, source->name(),
        source->numAttachedObservers(),
        source->updateCount(),
        source->notifyCount(),
        stat.usagePeak());
    tbl->add(buf);
}


//
// Form verbose subject line in news-show output.
// This callback is invoked for each known news source.
//
void NewsCmd::formSubjectLine9(void* arg, NewsSubject* source)
{
    void** p = static_cast<void**>(arg);
    StringVec* tbl = static_cast<StringVec*>(p[0]);
    const unsigned int* nameWidth = static_cast<const unsigned int*>(p[1]);

    Tree::compare_t compare = 0;
    KnownAudience observers(compare);
    source->apply(getAudience, &observers);
    String audience = observers.stringify(", " /*delim*/, 0U - 1 /*maxItems*/);

    String buf;
    ItemQ::Stat stat(source->storyQ());
    sprintf(buf, "  %-*s%11u%11u%11u%7u  #%s\n",
        *nameWidth, source->name(),
        source->numAttachedObservers(),
        source->updateCount(),
        source->notifyCount(),
        stat.usagePeak(),
        audience.ascii());
    tbl->add(buf);
}


void NewsCmd::formSubjectSummary(void* arg, NewsSubject* source)
{

    // Don't care about sources which have not yet been updated.
    if (source->updateCount() == 0)
    {
        return;
    }

    void** p = static_cast<void**>(arg);
    StringVec* tbl = static_cast<StringVec*>(p[0]);
    const unsigned int* nameWidth = static_cast<const unsigned int*>(p[1]);
    unsigned long long* nowAsFiletime = static_cast<unsigned long long*>(p[2]);

    unsigned long long updateTimeAsFiletime = source->updateTimeAsFiletime();
    unsigned long long filetimeAgo = *nowAsFiletime - updateTimeAsFiletime;
    double secsAgo = filetimeAgo * SECS_PER_FILETIME;

    ItemQ::Stat stat(source->storyQ());
    String buf;
    sprintf(buf, "  %-*s%11u%11u%11u%7u  #%s |%.15g|\n",
        *nameWidth, source->name(),
        source->numAttachedObservers(),
        source->updateCount(),
        source->notifyCount(),
        stat.usagePeak(),
        source->updateSummary().ascii(),
        secsAgo);
    tbl->add(buf);
}


//
// Identify all unique observers attached to known news sources.
// This callback is invoked for each known news source.
//
void NewsCmd::getAudience(void* arg, NewsSubject* source)
{
    source->apply(getAudience, arg);
}


//
// Identify all unique observers attached to known news sources.
// This callback is invoked for each observer attached to each known news source.
//
void NewsCmd::getAudience(void* arg, Observer* observer)
{
    KnownAudience* observers = static_cast<KnownAudience*>(arg);
    observers->add(observer);
}


//
// Collect names of known news sources.
// This callback is invoked for each known news source.
//
void NewsCmd::getSubjects(void* arg, NewsSubject* source)
{
    StringVec* subjectNames = static_cast<StringVec*>(arg);
    subjectNames->add(source->name());
}

END_NAMESPACE1
