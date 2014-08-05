#include "appkit/App.hpp"
#include "appkit/BufPoolCmd.hpp"
#include "appkit/Cmd.hpp"
#include "appkit/CmdCmd.hpp"
#include "appkit/CmdLine.hpp"
#include "appkit/CmdMap.hpp"
#include "appkit/Messenger.hpp"
#include "appkit/MiscDbugCmd.hpp"
#include "appkit/String.hpp"
#include "appkit/SysIo.hpp"
#include "appkit/U32.hpp"
#include "netkit/Paddr.hpp"

#include "appkit-ut-pch.h"
#include "CmdSuite.hpp"

using namespace appkit;
using namespace netkit;
using namespace syskit;

const char NEW_LINE = '\n';

BEGIN_NAMESPACE


class NoOpCmd: public Cmd
{
public:
    NoOpCmd(const char* nameSet);
    virtual ~NoOpCmd();
    virtual bool onRun(const CmdLine& req);
private:
    NoOpCmd(const NoOpCmd&); //prohibit usage
    const NoOpCmd& operator =(const NoOpCmd&); //prohibit usage
};

NoOpCmd::NoOpCmd(const char* nameSet):
Cmd(nameSet)
{
}

NoOpCmd::~NoOpCmd()
{
}

bool NoOpCmd::onRun(const CmdLine& /*req*/)
{
    bool cmdIsValid = false;
    return cmdIsValid;
}


class SelfDestructiveCmd: public Cmd
{
public:
    SelfDestructiveCmd();
    virtual ~SelfDestructiveCmd();
    virtual bool onRun(const CmdLine& req);
private:
    SelfDestructiveCmd(const SelfDestructiveCmd&); //prohibit usage
    const SelfDestructiveCmd& operator =(const SelfDestructiveCmd&); //prohibit usage
};

SelfDestructiveCmd::SelfDestructiveCmd():
Cmd("cmd-deleteOnRun")
{
}

SelfDestructiveCmd::~SelfDestructiveCmd()
{
}

bool SelfDestructiveCmd::onRun(const CmdLine& req)
{
    respond(req, "Done.");
    delete this;
    bool cmdIsValid = true;
    return cmdIsValid;
}

END_NAMESPACE


CmdSuite::CmdSuite()
{
}


CmdSuite::~CmdSuite()
{
}


bool CmdSuite::cb0a(void* arg, const char* name, Cmd* cmd, unsigned char cmdIndex)
{
    size_t* maxNameLength = static_cast<size_t*>(arg);
    size_t nameLength = strlen(name);
    if (*maxNameLength < nameLength)
    {
        *maxNameLength = nameLength;
    }

    bool ok = (cmdIndex < cmd->numNames()) && (strstr(cmd->nameSet(), name) != 0);
    bool keepGoing = ok;
    return keepGoing;
}


void CmdSuite::cb0b(void* arg, const char* name, Cmd* cmd, unsigned char cmdIndex)
{
    if ((cmdIndex >= cmd->numNames()) || (strstr(cmd->nameSet(), name) == 0))
    {
        bool* ok = static_cast<bool*>(arg);
        *ok = false;
    }
}


void CmdSuite::onCmdMapUpdate(void* arg, const String& cmdName, bool available)
{
    String* cmdMapUpdates = static_cast<String*>(arg);
    *cmdMapUpdates += available? "++": "--";
    *cmdMapUpdates += cmdName;
}


void CmdSuite::testCtor00()
{
    Singleton::create_t create = 0;
    unsigned long initialRefCount = 0UL;
    void* createArg = 0;
    App* app = App::instance(create, initialRefCount, createArg);
    RefCounted::Count lock(*app);

    const CmdLine& cmdLine = CmdLine::instance();
    SysIo sysIo(cmdLine);
    unsigned int addr = INADDR_ANY;
    bool ok = sysIo.isOk() &&
        (sysIo.acl().numAddrs() == 0) &&
        (sysIo.addr(0).port() != 0) &&
        (sysIo.addr(1).port() != 0) &&
        (sysIo.ioAddr() == Paddr(addr, 0U));
    CPPUNIT_ASSERT(ok);

    CmdMap* map = CmdMap::instance();
    String cmdMapUpdates;
    CmdMap::updateCb_t oldCmdMapUpdateCb;
    void* oldCmdMapUpdateArg;
    map->monitor(onCmdMapUpdate, &cmdMapUpdates, &oldCmdMapUpdateCb, &oldCmdMapUpdateArg);
    ok = (oldCmdMapUpdateCb != 0);
    CPPUNIT_ASSERT(ok);

    Cmd* cmd0 = new BufPoolCmd;
    Cmd* cmd1a = new CmdCmd(&sysIo);
    Cmd* cmd1b = new MiscDbugCmd(&sysIo);
    ok = (cmd0->numErrs() == 0) && (cmd0->numNames() != 0) &&
        (cmd1a->numErrs() == 0) && (cmd1a->numNames() != 0);
    (cmd1b->numErrs() == 0) && (cmd1b->numNames() != 0);
    CPPUNIT_ASSERT(ok);

    size_t maxNameLength = 0;
    ok = map->apply(cb0a, &maxNameLength) && (maxNameLength == map->maxNameLength());
    CPPUNIT_ASSERT(ok);
    map->apply(cb0b, &ok);
    CPPUNIT_ASSERT(ok);

    CmdLine req("cmd-showlog");
    String rsp;
    ok = cmd1a->sprint(req, rsp);
    CPPUNIT_ASSERT(ok);
    req = "version-show";
    ok = cmd1b->sprint(req, rsp) && (!rsp.empty()) && (cmd1b->numSprints(2U /*cmdIndex*/) == 1);
    CPPUNIT_ASSERT(ok);
    req = "bufpool-show";
    ok = cmd0->sprint(req, rsp) && (!rsp.empty());
    CPPUNIT_ASSERT(ok);
    req = "not-a-known-command --abc=123";
    ok = (!cmd0->sprint(req, rsp)) && rsp.empty();
    CPPUNIT_ASSERT(ok);
    req = "help --v"; //spoofing not allowed
    ok = (!cmd0->sprint(req, rsp)) && rsp.empty();
    CPPUNIT_ASSERT(ok);

    Cmd* cmd2 = new BufPoolCmd;
    Cmd* cmd3a = new CmdCmd(&sysIo);
    Cmd* cmd3b = new MiscDbugCmd(&sysIo);
    ok = (cmd2->numErrs() != 0) && (cmd2->numErrs() == cmd2->numNames()) &&
        (cmd3a->numErrs() != 0) && (cmd3a->numErrs() == cmd3a->numNames()) &&
        (cmd3b->numErrs() != 0) && (cmd3b->numErrs() == cmd3b->numNames());
    CPPUNIT_ASSERT(ok);

    delete cmd3b;
    delete cmd3a;
    delete cmd2;
    delete cmd1b;
    delete cmd1a;
    delete cmd0;

    ok = (cmdMapUpdates == "++bufpool-resetstat"
        "++bufpool-show"
        "++bufpool-shrink"
        "++cmd-show"
        "++cmd-showlog"
        "++help"
        "++acl-config"
        "++env"
        "++version-show"
        "--acl-config"
        "--env"
        "--version-show"
        "--cmd-show"
        "--cmd-showlog"
        "--help"
        "--bufpool-resetstat"
        "--bufpool-show"
        "--bufpool-shrink");
    CPPUNIT_ASSERT(ok);
    map->monitor(oldCmdMapUpdateCb, oldCmdMapUpdateArg);
}


void CmdSuite::testCtor01()
{
    Cmd* cmd = new NoOpCmd("abc xyz 123 xyz abc");
    bool ok = (cmd->numErrs() == 2) && (cmd->numNames() == 5);
    CPPUNIT_ASSERT(ok);

    delete cmd;
    String tooMany;
    for (unsigned long i = 260; i > 0;)
    {
        tooMany += U32(--i).toString();
        tooMany += ' ';
    }
    cmd = new NoOpCmd(tooMany.ascii());
    ok = (cmd->numErrs() == 4) && (cmd->numNames() == 260);
    CPPUNIT_ASSERT(ok);

    delete cmd;
}


void CmdSuite::testCtor02()
{
    Cmd* cmd = new NoOpCmd("no-op");
    bool ok = (cmd->numErrs() == 0) && (cmd->numNames() == 1);
    CPPUNIT_ASSERT(ok);

    unsigned int pkts = 999;
    unsigned int fails = 999;
    unsigned long long bytes = 999;
    unsigned long numRuns = cmd->numRuns(0U, bytes, pkts, fails);
    ok = (numRuns == 0) && (bytes == 0) && (pkts == 0) && (fails == 0);
    CPPUNIT_ASSERT(ok);

    ok = (*cmd->usage(0U) == 0) && (*cmd->xName(0U) == 0) && (*cmd->xUsage(0U) == 0);
    CPPUNIT_ASSERT(ok);

    delete cmd;
}


void CmdSuite::testMessenger00()
{
    Messenger* nullMessenger = Messenger::createNull();
    bool ok = nullMessenger->isOk();
    CPPUNIT_ASSERT(ok);

    String cmd;
    String rsp;
    String event;
    Messenger::onRsp_t onRsp = 0;
    void* arg = 0;
    ok = nullMessenger->deliver(cmd) &&
        nullMessenger->deliver(cmd, rsp) &&
        nullMessenger->deliver(cmd, onRsp, arg) &&
        nullMessenger->inform(event);
    CPPUNIT_ASSERT(ok);

    delete nullMessenger;
}


void CmdSuite::testSprint00()
{
    Cmd* cmd = new SelfDestructiveCmd;
    bool ok = (cmd->numErrs() == 0) && (cmd->numNames() == 1);
    CPPUNIT_ASSERT(ok);

    CmdLine req("cmd-deleteOnRun");
    String rsp;
    ok = cmd->sprint(req, rsp) && (rsp == "Done.");
    CPPUNIT_ASSERT(ok);

    // Self-destrutive commands can be constructed again.
    cmd = new SelfDestructiveCmd;
    ok = (cmd->numErrs() == 0) && (cmd->numNames() == 1);
    CPPUNIT_ASSERT(ok);
    delete cmd;
}
