#include "appkit/CmdLine.hpp"
#include "appkit/UnitTestBed.hpp"

#include "appkit-ut-pch.h"
#include "BadSuite.hpp"
#include "S32Suite.hpp"
#include "UnitTestBedSuite.hpp"

using namespace appkit;

const char* const
FAILED_SUITE = "Failed Tests";

const char* const
OKAYED_SUITE = "Okayed Tests";

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(BadSuite, FAILED_SUITE);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(S32Suite, OKAYED_SUITE);


UnitTestBedSuite::UnitTestBedSuite():
display_()
{
}


UnitTestBedSuite::~UnitTestBedSuite()
{
}


//
// No command-line arguments.
//
void UnitTestBedSuite::testCtor00()
{
    CmdLine cmdLine("toolpp-ut");
    UnitTestBed bed(cmdLine, display_);
    bool ok = ((!bed.isQuiet()) && (!bed.isVerbose()));
    CPPUNIT_ASSERT(ok);
}


//
// Quiet and verbose modes.
//
void UnitTestBedSuite::testCtor01()
{
    CmdLine cmdLine("toolpp-ut -q -v");
    UnitTestBed bed0(cmdLine, display_);
    bool ok = ((!bed0.isQuiet()) && (!bed0.isVerbose()) && bed0.runAll(OKAYED_SUITE));
    CPPUNIT_ASSERT(ok);

    cmdLine = "toolpp-ut -q";
    UnitTestBed bed1(cmdLine, display_);
    ok = (bed1.isQuiet() && (!bed1.isVerbose()) && bed1.runAll(OKAYED_SUITE));
    CPPUNIT_ASSERT(ok);

    cmdLine = "toolpp-ut -v";
    UnitTestBed bed2(cmdLine, display_);
    ok = ((!bed2.isQuiet()) && bed2.isVerbose() && bed2.runAll(OKAYED_SUITE));
    CPPUNIT_ASSERT(ok);
}


//
// Specified test case/suite.
//
void UnitTestBedSuite::testCtor02()
{
    CmdLine cmdLine("toolpp-ut S32Suite UnitTestBedSuite::testCtor00");
    UnitTestBed bed0(cmdLine, display_);
    bool ok = bed0.runAll();
    CPPUNIT_ASSERT(ok);

    cmdLine = "toolpp-ut -q S32Suite UnitTestBedSuite::testCtor00";
    UnitTestBed bed1(cmdLine, display_);
    ok = bed1.runAll();
    CPPUNIT_ASSERT(ok);

    cmdLine = "toolpp-ut -v S32Suite UnitTestBedSuite::testCtor00";
    UnitTestBed bed2(cmdLine, display_);
    ok = bed2.runAll();
    CPPUNIT_ASSERT(ok);
}


//
// Unknown test name.
//
void UnitTestBedSuite::testCtor03()
{
    CmdLine cmdLine("toolpp-ut 123 S32Suite");
    UnitTestBed bed0(cmdLine, display_);
    bool ok = (!bed0.runAll());
    CPPUNIT_ASSERT(ok);

    cmdLine = "toolpp-ut -q 123 S32Suite";
    UnitTestBed bed1(cmdLine, display_);
    ok = (!bed1.runAll());
    CPPUNIT_ASSERT(ok);

    cmdLine = "toolpp-ut -q S32Suite 123";
    UnitTestBed bed2(cmdLine, display_);
    ok = (!bed2.runAll());
    CPPUNIT_ASSERT(ok);
}


//
// Failed tests.
//
void UnitTestBedSuite::testCtor04()
{
    CmdLine cmdLine("toolpp-ut");
    UnitTestBed bed0(cmdLine, display_);
    bool ok = (!bed0.runAll(FAILED_SUITE));
    CPPUNIT_ASSERT(ok);

    cmdLine = "toolpp-ut -q";
    UnitTestBed bed1(cmdLine, display_);
    ok = (!bed1.runAll(FAILED_SUITE));
    CPPUNIT_ASSERT(ok);

    cmdLine = "toolpp-ut -v";
    UnitTestBed bed2(cmdLine, display_);
    ok = (!bed2.runAll(FAILED_SUITE));
    CPPUNIT_ASSERT(ok);
}


//
// Show usage.
//
void UnitTestBedSuite::testCtor05()
{
    CmdLine cmdLine("toolpp-ut --?");
    UnitTestBed bed0(cmdLine, display_);
    bool ok = bed0.runAll();
    CPPUNIT_ASSERT(ok);

    cmdLine = "toolpp-ut --??";
    UnitTestBed bed1(cmdLine, display_);
    ok = bed1.runAll();
    CPPUNIT_ASSERT(ok);
}
