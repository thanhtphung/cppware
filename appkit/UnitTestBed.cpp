/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <iostream>
#include <stdexcept>
#include <cppunit/TestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/Bool.hpp"
#include "appkit/CmdLine.hpp"
#include "appkit/Directory.hpp"
#include "appkit/Path.hpp"
#include "appkit/UnitTestBed.hpp"
#include "appkit/UnitTestStat.hpp"

#if _WIN32
#if _DEBUG
#pragma comment(lib,"cppunitd")
#else
#pragma comment(lib,"cppunit")
#endif
#endif

const char DEFAULT_REGISTRY[] = "All Tests";

// Usage text.
const char USAGE[] =
"Usage:\n"
"  testdriver [--?            ] [testname...]\n"
"             [--??           ]\n"
"             [--q            ]\n"
"             [--startinexedir]\n"
"             [--v            ]\n\n"
"Examples:\n"
"  appkit-ut --v\n"
"  appkit-ut TokenizerSuite CmdLineSuite::testCtor00\n"
;

// Extended usage text.
const char X_USAGE[] =
"\n"
"Arguments:\n"
"testname\n"
"  To-be-run test suite(s) and/or test case(es).\n\n"
"Options:\n"
"--?\n"
"--??\n"
"  Show (extended) usage.\n"
"--q\n"
"--v\n"
"  Be quiet/verbose.\n"
"--startinexedir (true)\n"
"  Change current directory to the executable location at start?\n"
;

BEGIN_NAMESPACE1(appkit)


//!
//! Construct a default unit test bed configurable via the command line.
//!
UnitTestBed::UnitTestBed():
cmdLine_(CmdLine::instance()),
display_(std::cout)
{
    String optK("startinexedir");
    Bool startInExeDir(cmdLine_.opt(optK), true /*defaultV*/);
    if (startInExeDir)
    {
        const String* arg0 = cmdLine_.arg(0);
        Path path(*arg0, false /*skipNormalization*/);
        String exeDir(path.fullName(true /*beautify*/));
        path.reset(exeDir, true /*skipNormalization*/);
        exeDir = path.dirname();
        String currentDir(Directory::getCurrent());
        oldDir_ = ((exeDir != currentDir) && Directory::setCurrent(exeDir))? new String(currentDir): 0;
    }
    else
    {
        oldDir_ = 0;
    }

    construct();
}


//!
//! Construct a unit test bed. Using given command line for configuration.
//! Use given display instead of standard output.
//!
UnitTestBed::UnitTestBed(const CmdLine& cmdLine, std::ostream& display):
cmdLine_(cmdLine),
display_(display)
{
    oldDir_ = 0;
    construct();
}


UnitTestBed::~UnitTestBed()
{

    // Restore current directory if it was changed at construction.
    if (oldDir_ != 0)
    {
        Directory::setCurrent(*oldDir_);
        delete oldDir_;
    }
}


bool UnitTestBed::run(const char* registryName) const
{
    std::string key((registryName == 0)? DEFAULT_REGISTRY: registryName);
    CppUnit::TestFactoryRegistry& registry = CppUnit::TestFactoryRegistry::getRegistry(key);
    CppUnit::Test* testSuite = registry.makeTest();
    CppUnit::TestRunner runner;
    runner.addTest(testSuite);

    UnitTestStat result(beQuiet_, beVerbose_, display_);
    bool ok = true;
    size_t numArgs = cmdLine_.numArgs();
    if (numArgs > 1)
    {
        std::string testName;
        for (unsigned int i = 1; i < numArgs; ++i)
        {
            const String* arg = cmdLine_.arg(i);
            testName.assign(arg->ascii(), arg->byteSize() - 1);
            display_ << "running=";
            display_ << *arg;
            display_ << "...";
            display_ << std::endl;
            try
            {
                runner.run(result, testName);
            }
            catch (std::invalid_argument& /*e*/)
            {
                display_ << *arg;
                display_ << ": skipped (unknown test name)";
                display_ << std::endl;
                ok = false;
            }
        }
    }

    else
    {
        display_ << "running=all...";
        display_ << std::endl;
        runner.run(result);
    }

    display_ << "run=";
    display_ << result.numStarted();
    display_ << " ok=";
    display_ << result.numOkayed();
    display_ << " failed=";
    display_ << result.numFailed();
    display_ << std::endl;
    return (ok && (result.numFailed() == 0));
}


//!
//! Run all registered tests. Return true if there are no test failures. Return
//! false otherwise. By default, all registered tests are performed. To run certain
//! test suites and/or test cases, specify the desired test names using command-line
//! arguments. For example, "appkit-ut --v CmdLineSuite TokenizerSuite::testOp00"
//! runs only the CmdLineSuite test suite and the TokenizerSuite::testOp00 test
//! case.
//!
bool UnitTestBed::runAll(const char* registryName) const
{

    // Show extended usage if asked.
    String optK("??");
    Bool giveHelp(cmdLine_.opt(optK), false /*defaultV*/);
    if (giveHelp)
    {
        display_ << USAGE;
        display_ << X_USAGE;
        return true;
    }

    // Show usage if asked.
    optK = "?";
    giveHelp = Bool(cmdLine_.opt(optK), false /*defaultV*/);
    if (giveHelp)
    {
        display_ << USAGE;
        return true;
    }

    // Run tests.
    return (this->*run_)(registryName);
}


bool UnitTestBed::runQuietly(const char* registryName) const
{
    std::string key((registryName == 0)? DEFAULT_REGISTRY: registryName);
    CppUnit::TestFactoryRegistry& registry = CppUnit::TestFactoryRegistry::getRegistry(key);
    CppUnit::Test* testSuite = registry.makeTest();
    CppUnit::TestRunner runner;
    runner.addTest(testSuite);

    UnitTestStat result(beQuiet_, beVerbose_, display_);
    bool ok = true;
    size_t numArgs = cmdLine_.numArgs();
    if (numArgs > 1)
    {
        std::string testName;
        for (unsigned int i = 1; i < numArgs; ++i)
        {
            const String* arg = cmdLine_.arg(i);
            testName.assign(arg->ascii(), arg->byteSize() - 1);
            try
            {
                runner.run(result, testName);
            }
            catch (std::invalid_argument& /*e*/)
            {
                ok = false;
            }
        }
    }

    else
    {
        runner.run(result);
    }

    return (ok && (result.numFailed() == 0));
}


void UnitTestBed::construct()
{
    String optK("q");
    Bool beQuiet(cmdLine_.opt(optK), false /*defaultV*/);
    optK = "v";
    Bool beVerbose(cmdLine_.opt(optK), false /*defaultV*/);
    if (beQuiet && beVerbose)
    {
        beQuiet_ = false;
        beVerbose_ = false;
    }
    else
    {
        beQuiet_ = beQuiet;
        beVerbose_ = beVerbose;
    }

    run_ = beQuiet_? &UnitTestBed::runQuietly: &UnitTestBed::run;
}

END_NAMESPACE1
