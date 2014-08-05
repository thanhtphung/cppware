#include "appkit/CmdLine.hpp"
#include "appkit/String.hpp"
#include "appkit/U32.hpp"

#include "appkit-ut-pch.h"
#include "CmdLineSuite.hpp"

using namespace appkit;
using namespace syskit;


CmdLineSuite::CmdLineSuite()
{
}


CmdLineSuite::~CmdLineSuite()
{
}


void CmdLineSuite::testCtor00()
{
    CmdLine cmdLine;
    bool ok = cmdLine.asString().empty();
    CPPUNIT_ASSERT(ok);

    ok = ((cmdLine.numArgs() == 0) && (cmdLine.numOpts() == 0) && (cmdLine.tag() == 0));
    CPPUNIT_ASSERT(ok);
}


void CmdLineSuite::testCtor01()
{
    const CmdLine& cmdLine = CmdLine::instance();
    bool ok = ((cmdLine.arg(0) != 0) && (cmdLine.numArgs() > 0));
    CPPUNIT_ASSERT(ok);
}


//
// Verify command name parsing.
//
void CmdLineSuite::testCtor02()
{
    CmdLine cmdLine("normalArg0");
    bool ok = ((cmdLine.numArgs() == 1) && (cmdLine.numOpts() == 0));
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.arg(0) == "normalArg0");
    CPPUNIT_ASSERT(ok);

    String cmd("'quotedArg0'");
    cmdLine = cmd;
    ok = ((cmdLine.numArgs() == 1) && (cmdLine.numOpts() == 0));
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.arg(0) == "quotedArg0");
    CPPUNIT_ASSERT(ok);

    cmdLine = "'arg0 w/ white space'\t";
    ok = ((cmdLine.numArgs() == 1) && (cmdLine.numOpts() == 0));
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.arg(0) == "arg0 w/ white space");
    CPPUNIT_ASSERT(ok);

    cmdLine.reset("truncatedArg0xxx", 13);
    ok = ((cmdLine.numArgs() == 1) && (cmdLine.numOpts() == 0));
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.arg(0) == "truncatedArg0");
    CPPUNIT_ASSERT(ok);
}


//
// Verify option parsing.
//
void CmdLineSuite::testCtor03()
{
    CmdLine cmdLine("arg0 -optName0=optVal0 -optName1=( optVal1\t) --optName2 --optName3");
    bool ok = ((cmdLine.numArgs() == 1) && (cmdLine.numOpts() == 4));
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.arg(0) == "arg0");
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.opt("optName0") == "optVal0");
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.opt("optName1") == "( optVal1\t)");
    CPPUNIT_ASSERT(ok);
    ok = cmdLine.opt("optName2")->empty();
    CPPUNIT_ASSERT(ok);
    ok = cmdLine.opt("optName3")->empty();
    CPPUNIT_ASSERT(ok);

    ok = (cmdLine.opt("optname4") == 0);
    CPPUNIT_ASSERT(ok);
    String defaultV("optval4");
    ok = (cmdLine.opt("optname4", &defaultV) == &defaultV);
    CPPUNIT_ASSERT(ok);

    String n("t");
    n += L'\u00ea';
    n += 'n';
    String v("T");
    v.append(reinterpret_cast<const utf8_t*>("\xe1\xba\xa5"), 3, 1);
    v += 'n';
    String s("arg0 -optName0='opt Val 0' --n1=\"\\0\\x1\\Z\\n\" -");
    s += n;
    s += '=';
    s += v;
    s += ' ';
    s += v;
    cmdLine = s;
    ok = ((cmdLine.numArgs() == 2) && (cmdLine.numOpts() == 3));
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.arg(0) == "arg0");
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.arg(1) == v);
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.opt("optName0") == "opt Val 0");
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.opt("n1") == "\0\1?\n");
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.opt(n) == v);
    CPPUNIT_ASSERT(ok);
}


//
// Verify argument parsing.
//
void CmdLineSuite::testCtor04()
{
    CmdLine cmdLine("arg0 {arg 1} [arg 2] <arg 3> arg4");
    bool ok = ((cmdLine.numArgs() == 5) && (cmdLine.numOpts() == 0));
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.arg(0) == "arg0");
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.arg(1) == "{arg 1}");
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.arg(2) == "[arg 2]");
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.arg(3) == "<arg 3>");
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.arg(4) == "arg4");
    CPPUNIT_ASSERT(ok);

    ok = (cmdLine.arg(5) == 0);
    CPPUNIT_ASSERT(ok);
    String defaultV("arg5");
    ok = (cmdLine.arg(5, &defaultV) == &defaultV);
    CPPUNIT_ASSERT(ok);

    cmdLine = "arg0 \"-arg1 w/ quotes\" 'arg2=\\0\\x1\\Z\\n' |arg 3|";
    ok = ((cmdLine.numArgs() == 4) && (cmdLine.numOpts() == 0));
    cmdLine = "arg0 \"-arg1 w/ quotes\" \"arg2=\\0\\x1\\Z\\n\" |arg 3| 'c:\\a b c\\123.xyz'";
    ok = ((cmdLine.numArgs() == 5) && (cmdLine.numOpts() == 0));
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.arg(0) == "arg0");
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.arg(1) == "-arg1 w/ quotes");
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.arg(2) == "arg2=\0\1?\n");
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.arg(3) == "|arg 3|");
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.arg(4) == "c:\\a b c\\123.xyz");
    CPPUNIT_ASSERT(ok);

    cmdLine = "arg0 '' \"\" \"'\" '\"'";
    ok = ((cmdLine.numArgs() == 5) && (cmdLine.numOpts() == 0));
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.arg(0) == "arg0");
    CPPUNIT_ASSERT(ok);
    ok = cmdLine.arg(1)->empty();
    CPPUNIT_ASSERT(ok);
    ok = cmdLine.arg(2)->empty();
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.arg(3) == "'");
    CPPUNIT_ASSERT(ok);
    ok = (*cmdLine.arg(4) == "\"");
    CPPUNIT_ASSERT(ok);
}


//
// Make sure unreasonably long commands can be handled.
//
void CmdLineSuite::testCtor05()
{

    // 1000 options.
    String longCmd("0");
    const char* optMarker[2] = {"-", "--"};
    for (unsigned long i = 0; i <= 999; ++i)
    {
        char opt[U32::MaxDigits];
        size_t optLength = U32::toDigits(i, opt);
        longCmd += ' ';
        longCmd += optMarker[i % 2];
        longCmd.append(opt, optLength);
        longCmd += '=';
        longCmd.append(opt, optLength); //-i=i or --i=i
    }

    // 1000 arguments.
    for (unsigned long i = 1; i <= 999; ++i)
    {
        char arg[U32::MaxDigits];
        size_t argLength = U32::toDigits(i, arg);
        longCmd.append(1, ' ');
        longCmd.append(arg, argLength);
    }

    CmdLine cmdLine0(longCmd);
    CmdLine cmdLine1(cmdLine0);
    cmdLine0 = cmdLine1;
    bool ok = ((cmdLine0.numArgs() == 1000) && (cmdLine0.numOpts() == 1000));
    CPPUNIT_ASSERT(ok);

    for (unsigned long i = 0; i <= 999; ++i)
    {
        char arg[U32::MaxDigits + 1];
        size_t argLength = U32::toDigits(i, arg);
        arg[argLength] = 0;
        if (*cmdLine0.arg(i) != arg)
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
    for (unsigned long i = 0; i <= 999; ++i)
    {
        char opt[U32::MaxDigits + 1];
        size_t optLength = U32::toDigits(i, opt);
        opt[optLength] = 0;
        if (*cmdLine0.opt(opt) != opt)
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
}


//
// Make sure a standalone single/double hyphen is not considered an option.
//
void CmdLineSuite::testCtor06()
{
    String cmd("arg0 -");
    CmdLine cmd0;
    cmd0 = cmd;
    bool ok = ((cmd0.numArgs() == 2) && (cmd0.numOpts() == 0));
    CPPUNIT_ASSERT(ok);
    ok = (*cmd0.arg(1, 0) == "-");
    CPPUNIT_ASSERT(ok);

    CmdLine cmd1("arg0 ----", 7);
    ok = ((cmd1.numArgs() == 2) && (cmd1.numOpts() == 0));
    CPPUNIT_ASSERT(ok);
    ok = (*cmd1.arg(1, 0) == "--");
    CPPUNIT_ASSERT(ok);
}


void CmdLineSuite::testCtor07()
{
    CmdLine cmdLine0;
    cmdLine0.setTag(this);
    bool ok = (cmdLine0.tag() == this);
    CPPUNIT_ASSERT(ok);

    CmdLine cmdLine1;
    cmdLine0 = cmdLine0;
    cmdLine1 = cmdLine0;
    ok = (cmdLine1.asString().empty() && (cmdLine1.tag() == this));
    CPPUNIT_ASSERT(ok);

    CmdLine cmdLine2(cmdLine0);
    ok = (cmdLine2.asString().empty() && (cmdLine2.tag() == this));
    CPPUNIT_ASSERT(ok);
    ok = (cmdLine2 == cmdLine0);
    CPPUNIT_ASSERT(ok);
}


void CmdLineSuite::testNew00()
{
    CmdLine* cmdLine = new CmdLine;
    bool ok = (cmdLine != 0);
    CPPUNIT_ASSERT(ok);
    delete cmdLine;

    unsigned char buf[sizeof(*cmdLine)];
    cmdLine = new(buf)CmdLine;
    ok = (reinterpret_cast<unsigned char*>(cmdLine) == buf);
    CPPUNIT_ASSERT(ok);
    cmdLine->CmdLine::~CmdLine();
}


void CmdLineSuite::testSize00()
{
    size_t size = sizeof(CmdLine);
    bool ok = (size == sizeof(String) + sizeof(StringDic) + sizeof(StringVec) + sizeof(void*)); //Win32:52 x64:88
    CPPUNIT_ASSERT(ok);
}
