/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_CMD_CMD_HPP
#define APPKIT_CMD_CMD_HPP

#include <cstdio>
#include "appkit/Cmd.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)

class String;
class SysIo;


//! dbug commands to troubleshoot Cmd usage
class CmdCmd: public Cmd
{

public:
    CmdCmd(SysIo* sysIo);

    virtual ~CmdCmd();
    virtual bool onRun(const CmdLine& req);
    virtual const char* usage(unsigned char cmdIndex) const;
    virtual const char* xName(unsigned char cmdIndex) const;
    virtual const char* xUsage(unsigned char cmdIndex) const;

private:
    typedef bool (CmdCmd::*doer_t)(const CmdLine& req);

    typedef struct cmdShowLogArg_s
    {
        cmdShowLogArg_s(const char* rsp, int cmdCount);
        const char* rsp;
        int cmdCount;
    } cmdShowLogArg_t;

    typedef struct
    {
        String* rsp;
        unsigned int cmdWidth;
        unsigned int fails;
        unsigned int pkts;
        unsigned int runs;
        unsigned int sprints;
        unsigned long long bytes;
    } cmdShowArg_t;

    SysIo* sysIo_;

    static doer_t doer_[];

    CmdCmd(const CmdCmd&); //prohibit usage
    const CmdCmd& operator =(const CmdCmd&); //prohibit usage

    bool doHelp(const CmdLine&);
    bool doShow(const CmdLine&);
    bool doShowLog(const CmdLine&);
    void headLog(const CmdLine&, std::FILE*, int, int);
    void headLog(const CmdLine&, const String&, const String&, char, char, int&);
    void tailLog(const CmdLine&, int);
    void tailLog(const CmdLine&, std::FILE*, int, int);
    void tailLog(const CmdLine&, const String&, const String&, char, char, int);

    static bool showName(void*, const char*, Cmd*, unsigned char);
    static bool showName9(void*, const char*, Cmd*, unsigned char);
    static bool showStat(void*, const char*, Cmd*, unsigned char);
    static bool touchupCmdLog(void*, const char*, size_t);

};

inline CmdCmd::cmdShowLogArg_s::cmdShowLogArg_s(const char* rsp, int cmdCount)
{
    this->cmdCount = cmdCount;
    this->rsp = rsp;
}

END_NAMESPACE1

#endif
