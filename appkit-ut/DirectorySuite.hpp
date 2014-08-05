#ifndef DIRECTORY_SUITE_HPP
#define DIRECTORY_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(appkit, Directory)
DECLARE_CLASS1(appkit, String)


class DirectorySuite: public CppUnit::TestFixture
{

public:
    DirectorySuite();

    virtual ~DirectorySuite();

private:
    CPPUNIT_TEST_SUITE(DirectorySuite);
    CPPUNIT_TEST(testApply00);
    CPPUNIT_TEST(testApply01);
    CPPUNIT_TEST(testApply02);
    CPPUNIT_TEST(testApply03);
    CPPUNIT_TEST(testApply04);
    CPPUNIT_TEST(testAttr00);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCurrent00);
    CPPUNIT_TEST(testFind00);
    CPPUNIT_TEST(testGetTemp00);
    CPPUNIT_TEST(testHasChild00);
    CPPUNIT_TEST(testList00);
    CPPUNIT_TEST(testList01);
    CPPUNIT_TEST(testNormalizeSlashes00);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST_SUITE_END();

    DirectorySuite(const DirectorySuite&); //prohibit usage
    const DirectorySuite& operator =(const DirectorySuite&); //prohibit usage

    void testApply00();
    void testApply01();
    void testApply02();
    void testApply03();
    void testApply04();
    void testAttr00();
    void testCtor00();
    void testCtor01();
    void testCurrent00();
    void testFind00();
    void testGetTemp00();
    void testHasChild00();
    void testList00();
    void testList01();
    void testNormalizeSlashes00();
    void testOp00();

    static bool cb0a(void*, const appkit::Directory&, const appkit::String&);
    static bool cb0b(void*, const appkit::Directory&, const appkit::String&);
    static void cb1a(void*, const appkit::Directory&, const appkit::String&);

};

#endif
