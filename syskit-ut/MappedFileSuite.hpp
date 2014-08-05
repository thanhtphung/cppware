#ifndef MAPPED_FILE_SUITE_HPP
#define MAPPED_FILE_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(appkit, Directory)
DECLARE_CLASS1(appkit, String)


class MappedFileSuite: public CppUnit::TestFixture
{

public:
    MappedFileSuite();

    virtual ~MappedFileSuite();

private:
    CPPUNIT_TEST_SUITE(MappedFileSuite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST_SUITE_END();

    MappedFileSuite(const MappedFileSuite&); //prohibit usage
    const MappedFileSuite& operator =(const MappedFileSuite&); //prohibit usage

    void testCtor00();
    void testCtor01();
    void testCtor02();

    static bool cb0a(void*, const appkit::Directory&, const appkit::String&);
    static void cb1a(void*, const appkit::Directory&, const appkit::String&);

};

#endif
