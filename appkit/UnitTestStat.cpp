/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <cppunit/Test.h>
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/CmdLine.hpp"
#include "appkit/UnitTestStat.hpp"

using namespace CppUnit;

BEGIN_NAMESPACE1(appkit)


UnitTestStat::UnitTestStat(bool beQuiet, bool beVerbose, std::ostream& display):
TestResult(),
display_(display)
{
    beQuiet_ = beQuiet;
    beVerbose_ = beVerbose;
    numFailed_ = 0;
    numOkayed_ = 0;
    numStarted_ = 0;
    testFailed_ = false;
    where_ = 0;

    if (beQuiet_)
    {
        reportEnd_ = &UnitTestStat::reportQuietEnd;
        reportStart_ = &UnitTestStat::reportQuietStart;
    }
    else if (beVerbose_)
    {
        reportEnd_ = &UnitTestStat::reportVerboseEnd;
        reportStart_ = &UnitTestStat::reportVerboseStart;
    }
    else
    {
        reportEnd_ = &UnitTestStat::reportEnd;
        reportStart_ = &UnitTestStat::reportStart;
    }
}


UnitTestStat::~UnitTestStat()
{
    delete where_;
}


void UnitTestStat::addError(CppUnit::Test* /*test*/, CppUnit::Exception* e)
{
    testFailed_ = true;
    delete where_;
    where_ = new SourceLine(e->sourceLine());
}


void UnitTestStat::addFailure(CppUnit::Test* /*test*/, CppUnit::Exception* e)
{
    testFailed_ = true;
    delete where_;
    where_ = new SourceLine(e->sourceLine());
}


void UnitTestStat::endTest(CppUnit::Test* test)
{
    testFailed_? ++numFailed_: ++numOkayed_;
    (this->*reportEnd_)(*test);
}


void UnitTestStat::startTest(CppUnit::Test* test)
{
    ++numStarted_;
    testFailed_ = false;
    (this->*reportStart_)(*test);
}


void UnitTestStat::reportEnd(const CppUnit::Test& test) const
{
    if (testFailed_)
    {
        display_ << test.getName();
        if (where_->fileName().empty() && where_->lineNumber() < 0)
        {
            display_ << ": failed";
        }
        else
        {
            display_ << ": failed (file=\"";
            display_ << where_->fileName();
            display_ << "\" line=";
            display_ << where_->lineNumber();
            display_ << ')';
        }
        display_ << std::endl;
    }
}


void UnitTestStat::reportQuietEnd(const CppUnit::Test& /*test*/) const
{
}


void UnitTestStat::reportQuietStart(const CppUnit::Test& /*test*/) const
{
}


void UnitTestStat::reportStart(const CppUnit::Test& /*test*/) const
{
}


void UnitTestStat::reportVerboseEnd(const CppUnit::Test& /*test*/) const
{
    if (testFailed_)
    {
        if (where_->fileName().empty() && where_->lineNumber() < 0)
        {
            display_ << ": failed";
        }
        else
        {
            display_ << ": failed (file=\"";
            display_ << where_->fileName();
            display_ << "\" line=";
            display_ << where_->lineNumber();
            display_ << ')';
        }
        display_ << std::endl;
    }
    else
    {
        display_ << ": ok";
        display_ << std::endl;
    }
}


void UnitTestStat::reportVerboseStart(const CppUnit::Test& test) const
{
    display_ << test.getName();
    display_.flush();
}

END_NAMESPACE1
