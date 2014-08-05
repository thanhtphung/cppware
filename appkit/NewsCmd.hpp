/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_NEWS_CMD_HPP
#define APPKIT_NEWS_CMD_HPP

#include "appkit/Cmd.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)

class NewsSubject;
class Observer;


//! dbug commands to troubleshoot the observables
class NewsCmd: public Cmd
{

public:
    NewsCmd();

    // Override Cmd.
    virtual ~NewsCmd();
    virtual bool onRun(const CmdLine& req);
    virtual const char* usage(unsigned char cmdIndex) const;
    virtual const char* xName(unsigned char cmdIndex) const;

protected:

    // Override Cmd.
    virtual bool allowReq(const CmdLine& req, const netkit::Paddr& dbugger);

private:
    typedef bool (NewsCmd::*doer_t)(const CmdLine& req);

    static doer_t doer_[];

    NewsCmd(const NewsCmd&); //prohibit usage
    const NewsCmd& operator =(const NewsCmd&); //prohibit usage

    bool doMake(const CmdLine&);
    bool doShow(const CmdLine&);
    bool doShowAudience(const CmdLine&);
    bool doShowQ(const CmdLine&);
    bool doWatch(const CmdLine&);

    static void formObserverLine(void*, void*);
    static void formSubjectLine(void*, NewsSubject*);
    static void formSubjectLine9(void*, NewsSubject*);
    static void formSubjectSummary(void*, NewsSubject*);
    static void getAudience(void*, NewsSubject*);
    static void getAudience(void*, Observer*);
    static void getSubjects(void*, NewsSubject*);

};

END_NAMESPACE1

#endif
