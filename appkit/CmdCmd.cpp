/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/MappedTxtFile.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/Bool.hpp"
#include "appkit/CmdMap.hpp"
#include "appkit/DelimitedTxt.hpp"
#include "appkit/LogPath.hpp"
#include "appkit/CmdCmd.hpp"
#include "appkit/SysIo.hpp"
#include "appkit/U32.hpp"
#include "appkit/crt.hpp"

using namespace syskit;

const char EQUALS_SIGN = '=';
const char NEW_LINE = '\n';
const char VERTICAL_LINE = '|';

// Supported command set.
const char CMD_SET[] =
" cmd-show"
" cmd-showlog"
" help"
;

// Usage texts. One per command. Must match supported command set.
const char USAGE_1[] =
"Usage:\n"
"  cmd-showlog [--head=xxx]\n"
"              [--tail=xxx]\n\n"
"Examples:\n"
"  cmd-showlog\n"
"  cmd-showlog --head=99\n"
"  cmd-showlog --tail=99\n"
;

const char USAGE_2[] =
"Usage:\n"
"  help [--v]\n\n"
"Examples:\n"
"  help\n"
"  help --v\n"
;

const char* const USAGE[] =
{
    "",      //cmd-show
    USAGE_1, //cmd-showlog
    USAGE_2  //help
};

// Extended names. One per command. Must match supported command set.
const char* const X_NAME[] =
{
    "show command stats", //cmd-show
    "show command log",	//cmd-showlog
    "get command hints"   //help
};

// Extended usage texts. One per command. Must match supported command set.
const char X_USAGE_2[] =
"\n"
"Options:\n"
"--v\n"
"  Be verbvose.\n"
;

const char* const X_USAGE[] =
{
    "",       //cmd-show
    "",       //cmd-showlog
    X_USAGE_2 //help
};

BEGIN_NAMESPACE1(appkit)


// Command doers. One per command. Must match supported command set.
CmdCmd::doer_t CmdCmd::doer_[] =
{
    &CmdCmd::doShow,    //cmd-show
    &CmdCmd::doShowLog, //cmd-showlog
    &CmdCmd::doHelp     //help
};


CmdCmd::CmdCmd(SysIo* sysIo):
Cmd(CMD_SET)
{
    sysIo_ = sysIo;
}


CmdCmd::~CmdCmd()
{
}


//
// help [--v]
//
bool CmdCmd::doHelp(const CmdLine& req)
{

    // Header.
    String rsp("Available commands:\n");

    // Body.
    const CmdMap* map = Cmd::map();
    String optK("v");
    bool beVerbose = Bool(req.opt(optK), false /*defaultV*/);
    map->apply(beVerbose? showName9: showName, &rsp);

    // Terminating response.
    respond(req, rsp);

    bool cmdIsValid = true;
    return cmdIsValid;
}


//
// cmd-show
//
bool CmdCmd::doShow(const CmdLine& req)
{
    const CmdMap* map = Cmd::map();
    String rsp;
    cmdShowArg_t arg;
    arg.rsp = &rsp;
    arg.cmdWidth = map->maxNameLength() + 1;
    unsigned char cmdIndex = Cmd::cmdIndex(req);
    arg.runs = numRuns(cmdIndex, arg.bytes, arg.pkts, arg.fails);
    arg.sprints = numSprints(cmdIndex);

    // Header.
    sprintf(rsp, "  %-*s%8s%10s%12s%10s%6s\n" "  %-*s%8s%10s%12s%10s%6s\n",
        arg.cmdWidth, "cmd", "runs", "sprints", "rspBytes", "rspPkts", "fails",
        arg.cmdWidth, "---", "----", "-------", "--------", "-------", "-----");

    // Body.
    map->apply(showStat, &arg);

    // Terminating response.
    respond(req, rsp);

    bool cmdIsValid = true;
    return cmdIsValid;
}


//
// cmd-showlog [--head=xxx]
//             [--tail=xxx]
//
bool CmdCmd::doShowLog(const CmdLine& req)
{

    // Log file is not seekable.
    // Command likely being run via sprint().
    std::FILE* log = Cmd::log(req);
    int posInCurLog = std::ftell(log);
    if (posInCurLog < 0)
    {
        String rsp("None.");
        respond(req, rsp);
    }

    // Show command log head if the head option is specified. Show command log tail otherwise.
    // Disable response logging for this command to minimize confusion in the log. If zero is
    // given as a head or tail option, interpret it as a one.
    else
    {
        bool logReq = sysIo_->reqLoggingIsEnabled();
        bool logRsp = sysIo_->rspLoggingIsEnabled();
        bool flushLogEntries = sysIo_->logEntriesAreFlushed();
        sysIo_->setLogMode(logReq, false /*logRsp*/, flushLogEntries);

        String optK("head");
        const String* optV = req.opt(optK);
        if (optV == 0)
        {
            optK = "tail";
            unsigned int defaultV = 10;
            unsigned int cmdCount = U32(req.opt(optK), defaultV);
            tailLog(req, log, posInCurLog, cmdCount? cmdCount: 1);
        }
        else
        {
            unsigned int cmdCount = U32(*optV);
            headLog(req, log, posInCurLog, cmdCount? cmdCount: 1);
        }

        sysIo_->setLogMode(logReq, logRsp, flushLogEntries);
    }

    bool cmdIsValid = true;
    return cmdIsValid;
}


//!
//! Run command using dedicated agent threads provided by the Cmd framework. Return
//! true if command is valid. A false return results in some command usage hints as
//! response to the request.
//!
bool CmdCmd::onRun(const CmdLine& req)
{
    doer_t doer = doer_[cmdIndex(req)];
    bool cmdIsValid = (this->*doer)(req);
    return cmdIsValid;
}


//
// Show available command.
//
bool CmdCmd::showName(void* arg, const char* name, Cmd* /*cmd*/, unsigned char /*cmdIndex*/)
{
    String& rsp = *reinterpret_cast<String*>(arg);
    rsp += "  ";
    rsp += name;
    rsp += NEW_LINE;
    bool keepGoing = true;
    return keepGoing;
}


//
// Show available command in verbose mode.
//
bool CmdCmd::showName9(void* arg, const char* name, Cmd* cmd, unsigned char cmdIndex)
{
    String& rsp = *reinterpret_cast<String*>(arg);
    rsp += "  ";
    rsp += name;
    const char* xName = cmd->xName(cmdIndex);
    if (*xName != 0)
    {
        const CmdMap* map = cmd->map();
        size_t width = map->maxNameLength() + 1;
        String padding(width - strlen(name), ' ');
        rsp += padding;
        rsp += '#';
        rsp += xName;
    }

    rsp += NEW_LINE;
    bool keepGoing = true;
    return keepGoing;
}


//
// Show command execution stats.
//
bool CmdCmd::showStat(void* arg, const char* name, Cmd* cmd, unsigned char cmdIndex)
{
    const cmdShowArg_t* p = static_cast<const cmdShowArg_t*>(arg);
    if (strcmp(name, "cmd-show") == 0)
    {
        String line;
        sprintf(line, "  %-*s%8u%10u%12llu%10u%6u\n", p->cmdWidth, name, p->runs, p->sprints, p->bytes, p->pkts, p->fails);
        *p->rsp += line;
    }
    else
    {
        unsigned int fails;
        unsigned int rspPkts;
        unsigned long long rspBytes;
        unsigned int runs = cmd->numRuns(cmdIndex, rspBytes, rspPkts, fails);
        unsigned int sprints = cmd->numSprints(cmdIndex);
        if (runs || sprints || fails)
        {
            String line;
            sprintf(line, "  %-*s%8u%10u%12llu%10u%6u\n", p->cmdWidth, name, runs, sprints, rspBytes, rspPkts, fails);
            *p->rsp += line;
        }
    }

    bool keepGoing = true;
    return keepGoing;
}


bool CmdCmd::touchupCmdLog(void* arg, const char* line, size_t length)
{

    // These lines are constant at the callback interface, but they reside in the
    // buffer referenced to by DelimitedTxt, and they can be touched up as needed.
    // To minimize confusion in the cmd-showlog output, replace vertical lines with
    // equals signs. Abort the iterating after about cmdCount commands have been
    // seen. Assume a command echo in the log files starts with a vertical line and
    // is at least 30 characters long.
    bool keepGoing = true;
    if ((length > 0) && (line[0] == VERTICAL_LINE))
    {

        char* p = const_cast<char*>(line);
        p[0] = EQUALS_SIGN;
        for (size_t i = 1; i < length; ++i)
        {
            if (p[i] == VERTICAL_LINE)
            {
                p[i] = EQUALS_SIGN;
            }
        }

        if (length > 30)
        {
            cmdShowLogArg_t* p = static_cast<cmdShowLogArg_t*>(arg);
            if (--p->cmdCount <= 0)
            {
                p->rsp = line;
                keepGoing = false;
            }
        }
    }

    return keepGoing;
}


const char* CmdCmd::usage(unsigned char cmdIndex) const
{
    return USAGE[cmdIndex];
}


const char* CmdCmd::xName(unsigned char cmdIndex) const
{
    return X_NAME[cmdIndex];
}


const char* CmdCmd::xUsage(unsigned char cmdIndex) const
{
    return X_USAGE[cmdIndex];
}


//
// Show the head end of the command log including the current open log file.
// Show at most cmdCount commands.
//
void CmdCmd::headLog(const CmdLine& req, std::FILE* log, int posInCurLog, int cmdCount)
{
    const char* name = (reqPort(req) == sysIo_->addr(0).port())? "-sysio-a": "-sysio-b";
    char lo = sysIo_->logPathLo();
    char hi = sysIo_->logPathHi();
    String newestPath(LogPath::newestPath(name, lo, hi));
    String oldestPath(LogPath::oldestPath(name, lo, hi));
    headLog(req, newestPath, oldestPath, lo, hi, cmdCount);

    // Need to show log entries from current log file now as the
    // older log files do not have all of the requested enries.
    if (cmdCount > 0)
    {

        // Read the whole current log into a null-terminated buffer. It's likely only
        // a small head-end portion is needed, but the simplified logic is more important
        // for this not-frequently-used nice-to-have troubleshooting capability.
        std::fseek(log, 0, SEEK_SET);
        int bufSize = posInCurLog;
        char* buf = new char[bufSize];
        std::fread(buf, bufSize, 1, log);
        std::fseek(log, posInCurLog, SEEK_SET);

        // To minimize confusion in the cmd-showlog output, replace vertical lines with equals signs.
        bool makeCopy = false;
        DelimitedTxt txt(buf, bufSize, makeCopy);
        cmdShowLogArg_t arg(buf + bufSize, cmdCount + 1);
        txt.applyLoToHi(touchupCmdLog, &arg);

        size_t rspLength = (arg.rsp - buf);
        respond(req, buf, rspLength);
        delete[] buf;
    }

    const char* rsp = "";
    respond(req, rsp);
}


//
// Show the head end of the command log excluding the current open log file.
// Show at most cmdCount commands as partial response(s). Recursively if necessary.
//
void CmdCmd::headLog(const CmdLine& req, const String& newestPath, const String& curPath, char lo, char hi, int& cmdCount)
{
    bool readOnly = true;
    bool skipBom = true;
    MappedTxtFile logFile(curPath.widen(), readOnly, skipBom);
    if (!logFile.isOk())
    {
        return;
    }

    // To minimize confusion in the cmd-showlog output, replace vertical lines with equals signs.
    size_t bufSize = static_cast<size_t>(logFile.imageSize());
    char* buf = new char[bufSize];
    memcpy(buf, logFile.image(), bufSize);
    bool makeCopy = false;
    DelimitedTxt txt(buf, bufSize, makeCopy);
    cmdShowLogArg_t arg(buf + bufSize, cmdCount + 1);
    txt.applyLoToHi(touchupCmdLog, &arg);

    int cmdsShown = cmdCount - arg.cmdCount + 1;
    cmdCount -= cmdsShown;
    size_t rspLength = (arg.rsp - buf);
    respond(req, buf, rspLength);
    delete[] buf;

    // Recursively if necessary.
    if (cmdCount > 0)
    {
        String newerPath(LogPath::newerPath(curPath, lo, hi));
        if (newerPath != newestPath)
        {
            headLog(req, newestPath, newerPath, lo, hi, cmdCount);
        }
    }
}


//
// Show the tail end of the command log including the current open log file.
// Show at most cmdCount commands.
//
void CmdCmd::tailLog(const CmdLine& req, std::FILE* log, int posInCurLog, int cmdCount)
{

    // Current log file is empty.
    if (posInCurLog == 0)
    {
        tailLog(req, cmdCount);
        const char* rsp = "";
        respond(req, rsp);
        return;
    }

    // Read the whole current log into a null-terminated buffer. It's likely only
    // a small tail-end portion is needed, but the simplified logic is more important
    // for this not-frequently-used nice-to-have troubleshooting capability.
    std::fseek(log, 0, SEEK_SET);
    int bufSize = posInCurLog;
    char* buf = new char[bufSize + 1];
    std::fread(buf, bufSize, 1, log);
    buf[bufSize] = 0;
    std::fseek(log, posInCurLog, SEEK_SET);

    // To minimize confusion in the cmd-showlog output, replace vertical lines with equals signs.
    bool makeCopy = false;
    DelimitedTxt txt(buf, bufSize, makeCopy);
    cmdShowLogArg_t arg(buf, cmdCount);
    txt.applyHiToLo(touchupCmdLog, &arg);

    // Need to show log entries from older log files first as the
    // current log file does not have all of the requested enries.
    if (arg.cmdCount > 0)
    {
        tailLog(req, arg.cmdCount);
    }

    size_t rspLength = bufSize - (arg.rsp - buf) + 1;
    respond(req, arg.rsp, rspLength);
    delete[] buf;
}


//
// Show the tail end of the command log excluding the current open log file.
// Show at most cmdCount commands as partial response(s). Recursively if necessary.
//
void CmdCmd::tailLog(const CmdLine& req, const String& newestPath, const String& curPath, char lo, char hi, int cmdCount)
{
    bool readOnly = true;
    bool skipBom = true;
    MappedTxtFile logFile(curPath.widen(), readOnly, skipBom);
    if (!logFile.isOk())
    {
        return;
    }

    // To minimize confusion in the cmd-showlog output, replace vertical lines with equals signs.
    size_t bufSize = static_cast<size_t>(logFile.imageSize());
    char* buf = new char[bufSize];
    memcpy(buf, logFile.image(), bufSize);
    bool makeCopy = false;
    DelimitedTxt txt(buf, bufSize, makeCopy);
    cmdShowLogArg_t arg(buf, cmdCount);
    txt.applyHiToLo(touchupCmdLog, &arg);

    // Recursively if necessary.
    if (arg.cmdCount > 0)
    {
        String olderPath(LogPath::olderPath(curPath, lo, hi));
        if (olderPath != newestPath)
        {
            tailLog(req, newestPath, olderPath, lo, hi, arg.cmdCount);
        }
    }

    size_t rspLength = bufSize - (arg.rsp - buf);
    respond(req, arg.rsp, rspLength);
    delete[] buf;
}


//
// Show the tail end of the command log excluding the current open log file.
// Show at most cmdCount commands as partial response(s).
//
void CmdCmd::tailLog(const CmdLine& req, int cmdCount)
{
    const char* name = (reqPort(req) == sysIo_->addr(0).port())? "-sysio-a": "-sysio-b";
    char lo = sysIo_->logPathLo();
    char hi = sysIo_->logPathHi();
    String newestPath(LogPath::newestPath(name, lo, hi));
    String curPath(LogPath::olderPath(newestPath, lo, hi));
    tailLog(req, newestPath, curPath, lo, hi, cmdCount);
}

END_NAMESPACE1
