/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_UNIT_TEST_STAT_HPP
#define APPKIT_UNIT_TEST_STAT_HPP

#include <ostream>
#include <cppunit/Exception.h>
#include <cppunit/TestResult.h>
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)


class UnitTestStat: public CppUnit::TestResult
{

public:
    UnitTestStat(bool beQuiet, bool beVerbose, std::ostream& display);

    unsigned int numFailed() const;
    unsigned int numOkayed() const;
    unsigned int numStarted() const;

    virtual ~UnitTestStat();
    virtual void addError(CppUnit::Test* test, CppUnit::Exception* e);
    virtual void addFailure(CppUnit::Test* test, CppUnit::Exception* e);
    virtual void endTest(CppUnit::Test* test);
    virtual void startTest(CppUnit::Test* test);

private:
    typedef void (UnitTestStat::*report_t)(const CppUnit::Test&) const;

    CppUnit::SourceLine* where_;
    bool beQuiet_;
    bool beVerbose_;
    bool testFailed_;
    report_t reportEnd_;
    report_t reportStart_;
    std::ostream& display_;
    unsigned int numFailed_;
    unsigned int numOkayed_;
    unsigned int numStarted_;

    void reportEnd(const CppUnit::Test&) const;
    void reportQuietEnd(const CppUnit::Test&) const;
    void reportQuietStart(const CppUnit::Test&) const;
    void reportStart(const CppUnit::Test&) const;
    void reportVerboseEnd(const CppUnit::Test&) const;
    void reportVerboseStart(const CppUnit::Test&) const;

};

inline unsigned int UnitTestStat::numOkayed() const
{
    return numOkayed_;
}

inline unsigned int UnitTestStat::numFailed() const
{
    return numFailed_;
}

inline unsigned int UnitTestStat::numStarted() const
{
    return numStarted_;
}

END_NAMESPACE1

#endif
