/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_NEWS_STORY_HPP
#define APPKIT_NEWS_STORY_HPP

#include "appkit/String.hpp"
#include "syskit/ItemQ.hpp"
#include "syskit/macros.h"

DECLARE_CLASS1(syskit, Utc)

BEGIN_NAMESPACE1(appkit)

class CmdLine;


#if _WIN32
#pragma pack(push,4)
#endif

//! observable news story
class NewsStory: public syskit::ItemQ::Item
    //!
    //! A class representing part of an observable subject in the observer design pattern. An observer
    //! can attach to and detach from an observable subject. When an observable subject is updated, its
    //! observers are notified. This implementation requires that both observable subjects and observers
    //! are reference counted. To help troubleshooting, both can be named with free-format texts, and
    //! some counters (update count, notify count, etc.) are provided. This is part of the NewsSubject
    //! class design. When a news subject is updated, a NewsStory story is created. The story is then
    //! broadcasted to its audience by a dedicated news anchor. As a result, multiple stories within one
    //! news subject can be in the queue waiting to be broadcasted.
    //!
{

public:
    NewsStory(const NewsStory& story);
    NewsStory(unsigned long long filetime, const String& summary);

    const NewsStory& operator =(const NewsStory& story);
    static void operator delete(void* p, size_t size);
    static void operator delete(void* p, void* buf);
    static void* operator new(size_t size);
    static void* operator new(size_t size, void* buf);

    const CmdLine& magnifiedSummary() const;
    const String& summary() const;
    syskit::Utc time() const;
    unsigned long long timeAsFiletime() const;

protected:
    virtual ~NewsStory();

private:
    String summary_;
    const CmdLine mutable* magnifiedSummary_;
    unsigned long long time_;

};

#if _WIN32
#pragma pack(pop)
#endif

END_NAMESPACE1

#include "syskit/BufPool.hpp"
#include "syskit/Utc.hpp"

BEGIN_NAMESPACE1(appkit)

inline void NewsStory::operator delete(void* p, size_t size)
{
    syskit::BufPool::freeBuf(p, size);
}

inline void NewsStory::operator delete(void* /*p*/, void* /*buf*/)
{
}

inline void* NewsStory::operator new(size_t size)
{
    void* buf = syskit::BufPool::allocateBuf(size);
    return buf;
}

inline void* NewsStory::operator new(size_t /*size*/, void* buf)
{
    return buf;
}

inline const String& NewsStory::summary() const
{
    return summary_;
}

inline syskit::Utc NewsStory::time() const
{
    return time_;
}

inline unsigned long long NewsStory::timeAsFiletime() const
{
    return time_;
}

END_NAMESPACE1

#endif
