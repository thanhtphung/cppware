/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_NEWS_ANCHOR_HPP
#define APPKIT_NEWS_ANCHOR_HPP

#include <cstdio>
#include "syskit/Singleton.hpp"
#include "syskit/SpinSection.hpp"
#include "syskit/macros.h"

DECLARE_CLASS1(syskit, ItemQ)
DECLARE_CLASS1(syskit, Thread)

BEGIN_NAMESPACE1(appkit)

class Cmd;
class CmdMap;
class NewsCmd;
class NewsSource;
class NewsStory;
class NewsSubject;
class String;


//! news anchor
class NewsAnchor: public syskit::Singleton
    //!
    //! A class representing a news anchor. This serves as a registry for all NewsSubject
    //! instances. It also serves as a messenger delivering news to a registered audience.
    //! With this design, news stories occur when NewsSubject instances are udpated via
    //! Observable::update(). Observers attached to a NewsSubject instance are considered
    //! part of the registered audience. The news anchor delivers news stories to the audience
    //! via Observer::onUpdate(). News stories can occur concurrently, but they are delivered
    //! sequentially using one dedicated news anchor thread.
    //!
{

public:
    enum
    {
        LogHeadlines = 0x04,
        FlushLogEntries = 0x80
    };

    typedef bool(*cb0_t)(void* arg, NewsSubject* source);
    typedef void(*cb1_t)(void* arg, NewsSubject* source);

    static const char CMD_NEWS_NAME[];
    static const char SOURCE_NEWS_NAME[];

    syskit::ItemQ& toBeAiredQ();
    NewsSubject* findSourceByName(const char* name) const;
    NewsSubject* sourceNews();
    bool addSource(NewsSubject* source);
    bool headlineLoggingIsEnabled() const;
    bool observe(NewsSubject* news);
    bool rmSource(NewsSubject* source);
    char logPathHi() const;
    char logPathLo() const;
    const syskit::ItemQ& toBeAiredQ() const;
    const NewsSubject* cmdNews() const;
    const NewsSubject* sourceNews() const;
    size_t threadId() const;
    unsigned int maxSourceNameLength() const;
    void setLogMode(bool logHeadlines = true, bool flushLogEntries = false);
    void waitTilFlushed() const;

    bool apply(cb0_t cb, void* arg = 0) const;
    void apply(cb1_t cb, void* arg = 0) const;

    static NewsAnchor* instance();
    static String formHeadline(const NewsSubject& source, const NewsStory& story);

protected:
    NewsAnchor(const char* id, unsigned int initialRefCount);
    virtual ~NewsAnchor();

private:
    typedef void(*cmdMapUpdateCb_t)(void* arg, const String& cmdName, bool available);
    typedef void(*crashCb_t)(void* arg);

    cmdMapUpdateCb_t oldCmdMapUpdateCb_;
    void* oldCmdMapUpdateArg_;

    crashCb_t oldCrashCb_;
    void* oldCrashArg_;

    std::FILE* volatile curLog_;
    syskit::ItemQ* toBeAiredQ_;
    NewsCmd* cmd_;
    NewsSource* newsSource_;
    NewsSubject* cmdNews_;
    NewsSubject* sourceNews_;
    syskit::SpinSection mutable ss_;
    syskit::Thread* anchor_;
    bool defaultHeadlinesAreReady_;
    size_t anchorThreadId_;
    unsigned int volatile logMask_;

    static const char ID1[];
    static const char LOG_PATH_HI;
    static const char LOG_PATH_LO;

    NewsAnchor(const NewsAnchor&); //prohibit usage
    const NewsAnchor& operator =(const NewsAnchor&); //prohibit usage

    void createDefaultHeadlines();
    void logHeadline(const NewsSubject&, const NewsStory&, std::FILE*);
    void loop();

    static syskit::Singleton* create(const char*, unsigned int, void*);
    static int findSourceByName(const void*, const void*);
    static void announceCmdAvailability(void*, const char*, Cmd*, unsigned char);
    static void onCmdMapUpdate(void*, const String&, bool);
    static void onCrash(void* arg);
    static void* anchorEntry(void*);

};

//! Return true if headline logging is current enabled.
inline bool NewsAnchor::headlineLoggingIsEnabled() const
{
    bool enabled = ((logMask_ & LogHeadlines) != 0);
    return enabled;
}

//! Return the LogPath setting used for news logging support.
inline char NewsAnchor::logPathHi() const
{
    return LOG_PATH_HI;
}

//! Return the LogPath setting used for news logging support.
inline char NewsAnchor::logPathLo() const
{
    return LOG_PATH_LO;
}

//! The news anchor runs in a dedicated thread.
//! Return the identifier of the running thread.
inline size_t NewsAnchor::threadId() const
{
    return anchorThreadId_;
}

END_NAMESPACE1

#endif
