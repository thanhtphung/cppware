#ifndef CRT_SUITE_HPP
#define CRT_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(appkit, Directory)
DECLARE_CLASS1(appkit, String)


class CrtSuite: public CppUnit::TestFixture
{

public:
    CrtSuite();

    virtual ~CrtSuite();

private:
    CPPUNIT_TEST_SUITE(CrtSuite);
    CPPUNIT_TEST(testAccess00);
    CPPUNIT_TEST(testChdir00);
    CPPUNIT_TEST(testChmod00);
    CPPUNIT_TEST(testCopy00);
    CPPUNIT_TEST(testEnv00);
    CPPUNIT_TEST(testFopen00);
    CPPUNIT_TEST(testLink00);
    CPPUNIT_TEST(testRename00);
    CPPUNIT_TEST(testSprintf00);
    CPPUNIT_TEST_SUITE_END();

    void testAccess00();
    void testChdir00();
    void testChmod00();
    void testCopy00();
    void testEnv00();
    void testFopen00();
    void testLink00();
    void testRename00();
    void testSprintf00();

    static bool doOpen(void*, const appkit::Directory&, const appkit::String&);

};

#endif
