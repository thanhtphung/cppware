/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <time.h>
#include "syskit/ItemQ.hpp"
#include "syskit/Thread.hpp"
#include "syskit/Vec.hpp"
#include "syskit/sys.hpp"

#include "appkit-pch.h"
#include "appkit/Bool.hpp"
#include "appkit/CmdMap.hpp"
#include "appkit/LogPath.hpp"
#include "appkit/NewsAnchor.hpp"
#include "appkit/NewsCmd.hpp"
#include "appkit/NewsStory.hpp"
#include "appkit/NewsSubject.hpp"
#include "appkit/WinApp.hpp"
#include "appkit/U64.hpp"
#include "appkit/crt.hpp"

using namespace syskit;

const char ENABLE_DEFAULT_NEWS[] = "__AppkitEnableDefaultNews";
const char NEW_LINE[] = "\n";
const unsigned int CMD_NEWS_Q_CAP = 32; //news headlines
const unsigned int LOG_FILE_CAP = 65535; //news headlines
const unsigned int SOURCE_NEWS_Q_CAP = 16; //news headlines
const unsigned int THREAD_STACK_SIZE = 0; //bytes, 0=use-default
const unsigned int TO_BE_AIRED_Q_CAP = 256; //news headlines

BEGIN_NAMESPACE1(appkit)


// Collection of news sources.
// Each NewsSubject instance can reside in one and only one of these collections.
class NewsSource: private Vec
{
public:
    using Vec::DefaultCap;
    using Vec::INVALID_INDEX;
    using Vec::operator delete;
    using Vec::operator new;
    using Vec::findIndex;
    using Vec::numItems;
    NewsSource(unsigned int capacity = DefaultCap, int growBy = -1);
    NewsSubject* peek(size_t index) const;
    bool add(NewsSubject* source);
    bool rm(NewsSubject* source);
    unsigned int maxNameLength() const;
    virtual ~NewsSource();
private:
    unsigned int maxNameLength_;
    NewsSource(const NewsSource&); //prohibit usage
    const NewsSource& operator =(const NewsSource&); //prohibit usage
    void updateMaxNameLength();
};

// Peek into the collection and locate a source. Each source in the collection
// can be identified by an index less than numItems(). Don't do any error checking.
// Behavior is unpredictable if given index is invalid.
inline NewsSubject* NewsSource::peek(size_t index) const
{
    NewsSubject* source = static_cast<NewsSubject*>(Vec::peek(index));
    return source;
}

inline unsigned int NewsSource::maxNameLength() const
{
    return maxNameLength_;
}

NewsSource::NewsSource(unsigned int capacity, int growBy):
Vec(capacity, growBy)
{
    maxNameLength_ = 0;
}

NewsSource::~NewsSource()
{
}

// Add given source to the collection. Mark its location in the collection so that
// it can be efficiently removed.
bool NewsSource::add(NewsSubject* source)
{
    unsigned int numItems = Vec::numItems();
    bool ok = Vec::add(source);
    if (ok)
    {
        size_t nameLength = strlen(source->name());
        if (nameLength > maxNameLength_)
        {
            maxNameLength_ = static_cast<unsigned int>(nameLength);
        }
        source->setIndexInSource(numItems);
    }

    return ok;
}

// Remove given source from the collection.
bool NewsSource::rm(NewsSubject* source)
{
    unsigned int index = source->indexInSource();
    bool maintainOrder = false;
    bool ok = Vec::rmFromIndex(index, maintainOrder);
    if (ok)
    {
        if (strlen(source->name()) == maxNameLength_)
        {
            updateMaxNameLength();
        }
        source->setIndexInSource(INVALID_INDEX);
        if (index < Vec::numItems())
        {
            source = peek(index);
            source->setIndexInSource(index);
        }
    }

    return ok;
}

// A source with a long name was removed,
// so find the length of the longest source name.
void NewsSource::updateMaxNameLength()
{
    size_t maxNameLength = 0;
    for (size_t i = Vec::numItems(); i > 0;)
    {
        const NewsSubject* source = peek(--i);
        size_t nameLength = strlen(source->name());
        if (nameLength == maxNameLength_)
        {
            maxNameLength = maxNameLength_;
            break; //no change since another name is as long
        }
        if (nameLength > maxNameLength)
        {
            maxNameLength = nameLength;
        }
    }

    maxNameLength_ = static_cast<unsigned int>(maxNameLength);
}


const char NewsAnchor::CMD_NEWS_NAME[] = "appkit::CmdNews";
const char NewsAnchor::ID1[] = "appkit::NewsAnchor"; //singleton ID
const char NewsAnchor::LOG_PATH_HI = 'z';
const char NewsAnchor::LOG_PATH_LO = 'a';
const char NewsAnchor::SOURCE_NEWS_NAME[] = "appkit::SourceNews";


NewsAnchor::NewsAnchor(const char* id, unsigned int initialRefCount):
RefCounted(initialRefCount),
Singleton(id, initialRefCount),
ss_()
{
    cmdNews_ = 0;
    curLog_ = stdout;
    defaultHeadlinesAreReady_ = false;
    logMask_ = LogHeadlines;
    WinApp* app = WinApp::instance();
    if (app)
    {
        app->setLogMask(app->logMask() | logMask_);
    }
    sourceNews_ = 0;

    int growBy = -1;
    toBeAiredQ_ = new ItemQ(TO_BE_AIRED_Q_CAP, growBy);
    newsSource_ = new NewsSource(NewsSource::DefaultCap, growBy);

    // Construct an empty string before starting the news anchor thread. This is a workaround
    // for some intermittent crash and needs to be investigated further. It appears the crash
    // occurs when the news anchor thread uses String for logging. Constructing a string here
    // ensures String is ready for use before the news anchor thread runs. TODO: investigate.
    String s;
    const wchar_t* dumpPath = Thread::dumpPath();
    Thread::monitorCrash(onCrash, this, dumpPath, &oldCrashCb_, &oldCrashArg_);
    anchor_ = new Thread(anchorEntry, this, THREAD_STACK_SIZE);
    anchorThreadId_ = anchor_->id();

    oldCmdMapUpdateCb_ = 0;
    oldCmdMapUpdateArg_ = 0;
    cmd_ = new NewsCmd;
}


NewsAnchor::~NewsAnchor()
{

    // Delete the news command handler before restoring the command map monitoring.
    // This allows announcing of the news command unavailability. No more command
    // availability/unavailability announcements after this point.
    CmdMap* cmdMap = cmd_->map();
    cmdMap->addRef();
    delete cmd_;
    cmdMap->monitor(oldCmdMapUpdateCb_, oldCmdMapUpdateArg_);
    cmdMap->rmRef();

    // cmdNews_ and sourceNews_ are built-in news subjects owned by the news anchor.
    // They need to be constructed/destructed without impacting the anchor reference
    // count so that the news anchor can be destructed via reference counting.
    if (cmdNews_)
    {
        for (; cmdNews_->refCount() > 1; Thread::yield());
        cmdNews_->rmRef();
        this->addRef(); //special case for cmdNews_ destruction (-1 --> 0)
        for (; sourceNews_->refCount() > 1; Thread::yield());
        NewsSubject* sourceNews = sourceNews_;
        sourceNews_ = 0; //disable announcing news source unavailability
        sourceNews->rmRef();
        this->addRef(); //special case for sourceNews_ destruction (-1 --> 0)
    }

    anchor_->killAndWait();
    delete anchor_;
    const wchar_t* dumpPath = Thread::dumpPath();
    Thread::monitorCrash(oldCrashCb_, oldCrashArg_, dumpPath);

    delete newsSource_;
    delete toBeAiredQ_;
}


//!
//! Return the queue of news items to be broadcasted.
//!
ItemQ& NewsAnchor::toBeAiredQ()
{
    return *toBeAiredQ_;
}


//!
//! Return per-process singleton. Construct on first use. Destruct at last use.
//!
NewsAnchor* NewsAnchor::instance()
{
    static NewsAnchor* s_anchor = dynamic_cast<NewsAnchor*>(getSingleton(ID1, create, 0U /*initialRefCount*/, 0));
    for (; s_anchor == 0; Thread::yield());
    return s_anchor;
}


//!
//! Locate a news source by name. If multiple sources have duplicate names, any of the
//! sources can be returned. Return zero if not found.
//!
NewsSubject* NewsAnchor::findSourceByName(const char* name) const
{
    SpinSection::Lock lock(ss_);
    unsigned int i = newsSource_->findIndex(name, findSourceByName);
    return (i == NewsSource::INVALID_INDEX)? 0: newsSource_->peek(i);
}


//!
//! The news anchor does announce when sources become available/unavailable. This
//! is done using an observable named SOURCE_NEWS_NAME with the summary syntax of
//! "source --(add|rm) <observableName>". Return this observable.
//!
NewsSubject* NewsAnchor::sourceNews()
{
    for (; !defaultHeadlinesAreReady_; Thread::yield());
    return sourceNews_;
}


//
// This method should be invoked just once per process via Singleton::getSingleton().
//
Singleton* NewsAnchor::create(const char* id, unsigned int initialRefCount, void* /*arg*/)
{
    NewsAnchor* anchor = new NewsAnchor(id, initialRefCount);
    return anchor;
}


String NewsAnchor::formHeadline(const NewsSubject& source, const NewsStory& story)
{

    // Form timestamp (example: "|10/25/11 15:46:11|123456| ").
    struct tm now;
    time_t t = time(0);
    localtime_s(&now, &t);
    char stamp[32 + U64::MaxDigits + 2 + 1];
    size_t size = strftime(stamp, 32, "|%c|", &now);
    size += U64::toDigits(story.timeAsFiletime(), stamp + size);
    stamp[size++] = '|';
    stamp[size++] = ' ';
    stamp[size++] = 0;

    String logEntry(stamp, size);
    logEntry += source.name();
    logEntry += " #";
    logEntry += story.summary();

    // A fabricated news story has a double-null terminated summary.
    // Make it look normal as a log entry.
    if (*(logEntry.raw() + logEntry.byteSize() - 2) == 0)
    {
        logEntry.truncate(logEntry.length() - 1);
    }

    logEntry += NEW_LINE;
    return logEntry;
}


//!
//! Add given observable to the registry. Return true if successful.
//!
bool NewsAnchor::addSource(NewsSubject* source)
{
    bool ok;
    {
        SpinSection::Lock lock(ss_);
        ok = newsSource_->add(source);
    }

    // Announce news source availability. Every source can be announced here except for the
    // sourceNews observable itself. That observable is still being constructed at this point.
    if (ok && (sourceNews_ != 0) && *source->name())
    {
        String summary("source --add ");
        summary += source->name();
        sourceNews_->update(summary);
    }

    return ok;
}


//!
//! Iterate the registry. Invoke callback at each known news source. The callback
//! should return true to continue iterating and should return false to abort
//! iterating. Return false if the callback aborted the iterating. Return true
//! otherwise. Behavior is unpredictable if callback attempts updating the registry
//! during the iterating.
//!
bool NewsAnchor::apply(cb0_t cb, void* arg) const
{
    bool ok = true;
    {
        SpinSection::Lock lock(ss_);
        for (size_t i = 0, numSources = newsSource_->numItems(); i < numSources; ++i)
        {
            NewsSubject* source = newsSource_->peek(i);
            if (!cb(arg, source))
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
//! Observe that given subject has been updated. Broadcast this news to interested
//! audience ASAP. Return true if successful. That is, if messaging to the dedicated
//! news anchor thread was successful.
//!
bool NewsAnchor::observe(NewsSubject* news)
{
    news->addRef();
    ItemQ::Item* item = news;
    unsigned int timeoutInMsecs = 0;
    bool ok = toBeAiredQ_->put(item, timeoutInMsecs);
    return ok;
}


//!
//! Remove given observable from the registry.
//! Return true if successful.
//!
bool NewsAnchor::rmSource(NewsSubject* source)
{
    bool ok;
    {
        SpinSection::Lock lock(ss_);
        ok = newsSource_->rm(source);
    }

    // Announce news source unavailability. It's quite unlikely the sourceNews observable
    // itself does not exist at this point, but it seems worthwhile being defensive here.
    // Also, sourceNews_ is zero when it removes itself from the registry.
    if (ok && (sourceNews_ != 0))
    {
        String summary("source --rm ");
        summary += source->name();
        sourceNews_->update(summary);
    }

    return ok;
}


//!
//! Return the queue of news items to be broadcasted.
//!
const ItemQ& NewsAnchor::toBeAiredQ() const
{
    return *toBeAiredQ_;
}


//!
//! The news anchor does announce when sources become available/unavailable. This
//! is done using an observable named SOURCE_NEWS_NAME with the summary syntax of
//! "source --(add|rm) <observableName>". Return this observable.
//!
const NewsSubject* NewsAnchor::sourceNews() const
{
    for (; !defaultHeadlinesAreReady_; Thread::yield());
    return sourceNews_;
}


//!
//! The news anchor does also announce when Cmd commands become available/unavailable.
//! This is done using an observable named CMD_NEWS_NAME with the summary syntax of
//! "cmd --(add|rm) <cmdName>". Return this observable.
//!
const NewsSubject* NewsAnchor::cmdNews() const
{
    for (; !defaultHeadlinesAreReady_; Thread::yield());
    return cmdNews_;
}


//
// Compare opaque items looking for matching observable news. Treat first item as
// a news subject name. Treat second item as a pointer to a NewsSubject instance.
// Return non-zero if the items differ.
//
int NewsAnchor::findSourceByName(const void* item0, const void* item1)
{
    const char* k0 = static_cast<const char*>(item0);
    const char* k1 = static_cast<const NewsSubject*>(item1)->name();
    return strcmp(k0, k1);
}


//!
//! Return the length of the longest news source name in the registry.
//!
unsigned int NewsAnchor::maxSourceNameLength() const
{
    return newsSource_->maxNameLength();
}


//
// Callback used to announce availability of commands existed before the command
// map monitoring.
//
void NewsAnchor::announceCmdAvailability(void* arg, const char* name, Cmd* /*cmd*/, unsigned char /*cmdIndex*/)
{
    NewsAnchor* anchor = static_cast<NewsAnchor*>(arg);

    String summary("cmd --add ");
    summary += name;
    anchor->cmdNews_->update(summary);
}


//!
//! Iterate the registry. Invoke callback at each known news source. Behavior is
//! unpredictable if callback attempts updating the registry during the iterating.
//!
void NewsAnchor::apply(cb1_t cb, void* arg) const
{
    SpinSection::Lock lock(ss_);
    size_t numSources = newsSource_->numItems();
    for (size_t i = 0; i < numSources; ++i)
    {
        NewsSubject* source = newsSource_->peek(i);
        cb(arg, source);
    }
}


void NewsAnchor::createDefaultHeadlines()
{

    // Allow disabling default news (Cmd/NewsSubject availability) via an environment variable.
    // If __AppkitEnableDefaultNews does exist, use it to determine if default news should be
    // disabled. By default, Cmd and NewsSubject availablility is announced by the news anchor.
    // Note that cmdNews_ and sourceNews_ are built-in news subjects owned by the news anchor.
    // They need to be constructed/destructed without impacting the anchor reference count so
    // that the news anchor can be destructed via reference counting.
    String k(ENABLE_DEFAULT_NEWS);
    String v;
    bool enableDefaultNews = (!getenv(k, v)) || Bool(v);
    if (enableDefaultNews)
    {

        // Monitor for command map updates so command availability/unavailability can be
        // announced. Some commands have already been created before this monitoring, so
        // try reporting them also. The reporting of those commands is a little questionable
        // as some/all of them can still exist after the news anchor destruction.
        int qGrowth = -1;
        cmdNews_ = NewsSubject::createHeadline(CMD_NEWS_NAME, CMD_NEWS_Q_CAP, qGrowth);
        this->rmRef(); //special case for cmdNews_ construction
        CmdMap* cmdMap = cmd_->map();
        cmdMap->apply(announceCmdAvailability, this);
        cmdMap->monitor(onCmdMapUpdate, this, &oldCmdMapUpdateCb_, &oldCmdMapUpdateArg_);

        // Source news availablity cannot be announced in addSource() as the observable does
        // not exist there yet. Announce its availability here after it's been constructed.
        sourceNews_ = NewsSubject::createHeadline(SOURCE_NEWS_NAME, SOURCE_NEWS_Q_CAP, qGrowth);
        this->rmRef(); //special case for sourceNews_ construction
        String summary("source --add ");
        summary += sourceNews_->name();
        sourceNews_->update(summary);
    }
}


//
// Log given story that was just broadcasted.
//
void NewsAnchor::logHeadline(const NewsSubject& news, const NewsStory& story, std::FILE* log)
{
    if ((logMask_ & LogHeadlines) == 0)
    {
        return;
    }

    String logEntry(formHeadline(news, story));
    size_t size = logEntry.byteSize() - 1;
    size_t count = 1;
    std::fwrite(logEntry.ascii(), size, count, log);
    bool flushLogEntries = ((logMask_ & FlushLogEntries) != 0);
    if (flushLogEntries)
    {
        std::fflush(log);
    }
}


//
// Main loop for the news anchor thread.
//
void NewsAnchor::loop()
{
    createDefaultHeadlines();
    defaultHeadlinesAreReady_ = true;

    const char* name = "-news-";
    LogPath logPath(name, LOG_PATH_LO, LOG_PATH_HI);
    curLog_ = logPath.curLog();

    ItemQ::Item* item;
    unsigned int storyCount = 0;
    unsigned int timeoutInMsecs = ItemQ::ETERNITY;
    while ((!Thread::isTerminating()) && toBeAiredQ_->get(item, timeoutInMsecs))
    {
        NewsSubject* news = dynamic_cast<NewsSubject*>(item);

        try
        {
            const NewsStory* story = news->getStory();
            news->notify(story->timeAsFiletime(), story->summary());
            logHeadline(*news, *story, curLog_);
            news->nullifyStory();
        }
        catch (...)
        {
        }

        news->rmRef();

        if ((++storyCount % LOG_FILE_CAP) == 0)
        {
            curLog_ = logPath.reopen();
        }
    }
}


//
// Callback used to announce command availability/unavailability. This is invoked
// when a command is added to or removed from the command map.
//
void NewsAnchor::onCmdMapUpdate(void* arg, const String& cmdName, bool available)
{
    NewsAnchor* anchor = static_cast<NewsAnchor*>(arg);

    String summary("cmd ");
    summary += available? "--add ": "--rm ";
    summary += cmdName;
    anchor->cmdNews_->update(summary);

    anchor->oldCmdMapUpdateCb_(anchor->oldCmdMapUpdateArg_, cmdName, available);
}


//
// Flush buffered I/O at crash.
//
void NewsAnchor::onCrash(void* arg)
{
    const NewsAnchor* anchor = static_cast<const NewsAnchor*>(arg);
    std::fflush(anchor->curLog_);
    anchor->oldCrashCb_(anchor->oldCrashArg_);
}


//!
//! Enable/Disable logging. By default, headline logging is enabled. Log entries are
//! buffered and flushed as needed. To flush entries as soon as they are logged, set
//! flushLogEntries to true.
//!
void NewsAnchor::setLogMode(bool logHeadlines, bool flushLogEntries)
{
    unsigned int flushMask = flushLogEntries? FlushLogEntries: 0;
    unsigned int logMask = logHeadlines? (LogHeadlines | flushMask): (flushMask);
    if (logMask != logMask_)
    {
        logMask_ = logMask;
        std::fflush(curLog_);
    }
}


//!
//! Wait until all outstanding news stories have been broadcasted. No-op if invoked by
//! the dedicated news anchor thread.
//!
void NewsAnchor::waitTilFlushed() const
{
    if (Thread::myId() != anchorThreadId_)
    {
        while (toBeAiredQ_->numItems() > 0)
        {
            Thread::yield();
        }
    }
}


//
// Entry point for the news anchor thread.
//
void* NewsAnchor::anchorEntry(void* arg)
{
    NewsAnchor* anchor = static_cast<NewsAnchor*>(arg);
    anchor->loop();
    return 0;
}

END_NAMESPACE1
