/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_UNIT_TEST_BED_HPP
#define APPKIT_UNIT_TEST_BED_HPP

#include <ostream>
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)

class CmdLine;
class String;


//! unit test bed
class UnitTestBed
    //!
    //! A class representing a unit test bed. This test bed supports executions
    //! of unit tests using the cppunit framework. Construct an instance to run
    //! registered tests. Test cases must be registered using the CPPUNIT_TEST()
    //! macro, and test suites must be registered using the CPPUNIT_TEST_SUITE(),
    //! CPPUNIT_TEST_SUITE_END(), and CPPUNIT_TEST_SUITE_REGISTRATION() macros.
    //! Test case validation must be done using the CPPUNIT_ASSERT() macro. By
    //! default, all registered tests are performed via runAll(). To run specific
    //! test suites and/or test cases, command-line arguments can be used. Example:
    //!\code
    //! int main()
    //! {
    //!   UnitTestBed unitTestBed;
    //!   bool ok = unitTestBed.runAll();
    //!   return ok? 0: 1;
    //! }
    //!\endcode
    //!
{

public:
    UnitTestBed();
    UnitTestBed(const CmdLine& cmdLine, std::ostream& display);
    ~UnitTestBed();

    bool isQuiet() const;
    bool isVerbose() const;
    bool runAll(const char* registryName = 0) const;

private:
    typedef bool (UnitTestBed::*run_t)(const char* registryName) const;

    bool beQuiet_;
    bool beVerbose_;
    const CmdLine& cmdLine_;
    const String* oldDir_;
    run_t run_;
    std::ostream& display_;

    UnitTestBed(const UnitTestBed&); //prohibit usage
    const UnitTestBed& operator =(const UnitTestBed&); //prohibit usage

    bool run(const char*) const;
    bool runQuietly(const char*) const;
    void construct();

};

//! Return true if the test bed runs in quiet mode.
//! The quiet mode is activated using the --q command-line option.
inline bool UnitTestBed::isQuiet() const
{
    return beQuiet_;
}

//! Return true if the test bed runs in verbose mode.
//! The verbose mode is activated using the --v command-line option.
inline bool UnitTestBed::isVerbose() const
{
    return beVerbose_;
}

END_NAMESPACE1

#endif
