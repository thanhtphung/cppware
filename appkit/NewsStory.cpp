/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/CmdLine.hpp"
#include "appkit/NewsStory.hpp"

using namespace syskit;

BEGIN_NAMESPACE1(appkit)


NewsStory::NewsStory(const NewsStory& story):
ItemQ::Item(story),
summary_(story.summary_)
{
    magnifiedSummary_ = (story.magnifiedSummary_ == 0)? 0: new CmdLine(*story.magnifiedSummary_);
    time_ = story.time_;
}


NewsStory::NewsStory(unsigned long long filetime, const String& summary):
ItemQ::Item(),
summary_(summary)
{
    magnifiedSummary_ = 0;
    time_ = filetime;
}


NewsStory::~NewsStory()
{
    delete magnifiedSummary_;
}


const NewsStory& NewsStory::operator =(const NewsStory& story)
{
    if (this != &story)
    {
        ItemQ::Item::operator =(story);
        delete magnifiedSummary_;
        magnifiedSummary_ = (story.magnifiedSummary_ == 0)? 0: new CmdLine(*story.magnifiedSummary_);
        time_ = story.time_;
    }

    return *this;
}


//!
//! Return summary as a CmdLine instance. Each news story summary is a free-format
//! string. Some news story summary uses a CmdLine syntax. When the CmdLine syntax
//! is utilized, use of this method helps minimize the summary parsing when there
//! are multiple observers.
//!
const CmdLine& NewsStory::magnifiedSummary() const
{
    if (magnifiedSummary_ == 0)
    {
        magnifiedSummary_ = new CmdLine(summary_);
    }

    return *magnifiedSummary_;
}

END_NAMESPACE1
