/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/CmdLine.hpp"
#include "appkit/NewsAnchor.hpp"
#include "appkit/NewsObserver.hpp"
#include "appkit/NewsStory.hpp"
#include "appkit/NewsSubject.hpp"
#include "appkit/String.hpp"

using namespace appkit;
using namespace syskit;

BEGIN_NAMESPACE1(appkit)


//!
//! Construct observer interested in given news subject.
//!
NewsObserver::NewsObserver(const char* subject, const char* observerName, unsigned int initialRefCount):
RefCounted(initialRefCount),
Observer(observerName, initialRefCount),
active_(0U),
attached_(0U)
{
    String proxyName(observerName);
    proxyName += ".proxy";
    sourceWatch_ = new SourceWatch(this, proxyName.ascii());
    subject_ = new String(subject);
}


NewsObserver::~NewsObserver()
{
    sourceWatch_->deactivate();
    if (attached_.asWord())
    {
        NewsSubject::detachByName(this, subject_->ascii());
    }

    delete subject_;
    sourceWatch_->rmRef();
}


//!
//! Return the news subject of interest. This is the subject specified at construction.
//!
const String& NewsObserver::subject() const
{
    return *subject_;
}


//!
//! Activate observer. That is, attach and follow the news subject of interest. The subject was
//! specified at construction. Invoke onAttach() and onDetach() when the subject attach/detach
//! occurs. With this invocation, the observer is now considered active. This method is a no-op
//! for active observers. That is, a subsequent activate() is meaningless without some deactivate()
//! in between.
//!
void NewsObserver::activate()
{
    unsigned int oldV;
    active_.set(1U, oldV);
    if (oldV) //already active
    {
        return;
    }

    // Try attaching now in case the subject is already available.
    sourceWatch_->activate();
    NewsSubject* attached = NewsSubject::attachByName(this, subject_->ascii());
    if (attached)
    {
        attached_.set(1U, oldV);
        if (oldV == 0U)
        {
            onAttach(attached);
        }
    }
}


//!
//! Deactivate observer. That is, detach from the news subject of interest. The subject was
//! specified at construction. Also invoke onDetach() if the subject detaching occurs now.
//! With this invocation, the observer is now considered idle. This method is a no-op for
//! idle observers. That is, a subsequent deactivate() is meaningless without some activate()
//! in between.
//!
void NewsObserver::deactivate()
{
    unsigned int oldV;
    active_.set(0U, oldV);
    if (oldV == 0U) //already idle
    {
        return;
    }

    // Detach now if attached.
    sourceWatch_->deactivate();
    if (attached_.asWord())
    {
        NewsSubject* detached = NewsSubject::detachByName(this, subject_->ascii());
        if (detached)
        {
            attached_.set(0U, oldV);
            if (oldV)
            {
                onDetach(detached, *subject_);
            }
        }
    }
}


//!
//! This method is invoked when an attach occurs. The default implementation is a no-op.
//! An attach can occur at activate() time if the news subject of interest is available.
//! It can also occur when the news subject of interest becomes available afterwards.
//!
void NewsObserver::onAttach(NewsSubject* /*attached*/)
{
}


//!
//! This method is invoked when a detach occurs. The default implementation is a no-op.
//! The detached argument can be zero if the news subject of interest no longer exists.
//! A detach can occur at deactivate() time if the news subject of interest is attached to
//! the observer. It can also occur when the news subject of interest becomes unavailable
//! afterwards.
//!
void NewsObserver::onDetach(NewsSubject* /*detached*/, const String& /*newsSubject*/)
{
}


//
// This method is invoked at observable updates in the dedicated news anchor thread context.
// The given followed news subject is now available. As a follower, attach to it.
//
void NewsObserver::onSourceAdd(const String& newsSubject)
{
    NewsSubject* attached = NewsSubject::attachByName(this, newsSubject.ascii());
    if (attached)
    {
        unsigned int oldV;
        attached_.set(1U, oldV);
        if (oldV == 0U)
        {
            onAttach(attached);
        }
    }
}


//
// This method is invoked at observable updates in the dedicated news anchor thread context.
// The given followed news subject is no longer available. As a follower, wait for its next
// availability.
//
void NewsObserver::onSourceRemove(const String& newsSubject)
{
    unsigned int oldV;
    attached_.set(0U, oldV);
    if (oldV)
    {
        NewsSubject* detached = 0;
        onDetach(detached, newsSubject);
    }
}


//!
//! This method is invoked at observable updates in the dedicated news anchor thread context.
//! The default implementation is a no-op.
//!
void NewsObserver::onUpdate(unsigned long long /*filetime*/, const String& /*summary*/, Observable* /*subject*/)
{
}


NewsObserver::SourceWatch::SourceWatch(NewsObserver* newsObserver, const char* name):
RefCounted(1U /*initialRefCount*/),
Observer(name, 1U /*initialRefCount*/)
{
    newsObserver_ = newsObserver;
    sourceNews_ = 0;
}


NewsObserver::SourceWatch::~SourceWatch()
{
    deactivate();
}


void NewsObserver::SourceWatch::activate()
{
    const NewsAnchor* anchor = NewsAnchor::instance();
    sourceNews_ = anchor->sourceNews();
    sourceNews_->attach(this);
}


void NewsObserver::SourceWatch::deactivate()
{
    const NewsSubject* sourceNews = sourceNews_;
    sourceNews_ = 0;
    if (sourceNews)
    {
        sourceNews->detach(this);
    }
}


//
// This method is invoked at observable updates in the dedicated news anchor thread context.
// SourceWatch basically serves as a proxy for NewsObserver. These updates are forwarded to
// NewsObserver as either onSourceAdd() or onSourceRemove().
//
void NewsObserver::SourceWatch::onUpdate(unsigned long long /*filetime*/, const String& /*summary*/, Observable* /*subject*/)
{
    if (sourceNews_ == 0)
    {
        return;
    }

    const NewsStory* story = sourceNews_->story();
    const CmdLine& magnifiedSummary = story->magnifiedSummary();
    const String* newsSubject = magnifiedSummary.arg(1);
    if (newsSubject && (newsObserver_->subject() == *newsSubject))
    {
        String optK("add");
        const String* optV = magnifiedSummary.opt(optK);
        optV? newsObserver_->onSourceAdd(*newsSubject): newsObserver_->onSourceRemove(*newsSubject);
    }
}

END_NAMESPACE1
