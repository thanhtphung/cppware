#ifndef STD_SUITE_HPP
#define STD_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(appkit, Directory)
DECLARE_CLASS1(appkit, String)


class StdSuite: public CppUnit::TestFixture
{

public:
    StdSuite();

    virtual ~StdSuite();

private:
    CPPUNIT_TEST_SUITE(StdSuite);
    CPPUNIT_TEST(testIfstream00);
    CPPUNIT_TEST_SUITE_END();

    void testIfstream00();

    static bool doCtor(void*, const appkit::Directory&, const appkit::String&);

};

#endif
