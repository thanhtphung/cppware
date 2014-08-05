#ifndef CMD_SUITE_HPP
#define CMD_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(appkit, Cmd)
DECLARE_CLASS1(appkit, String)


class CmdSuite: public CppUnit::TestFixture
{

public:
    CmdSuite();

    virtual ~CmdSuite();

private:
    CPPUNIT_TEST_SUITE(CmdSuite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testMessenger00);
    CPPUNIT_TEST(testSprint00);
    CPPUNIT_TEST_SUITE_END();

    CmdSuite(const CmdSuite&); //prohibit usage
    const CmdSuite& operator =(const CmdSuite&); //prohibit usage

    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testMessenger00();
    void testSprint00();

    static bool cb0a(void*, const char*, appkit::Cmd*, unsigned char);
    static void cb0b(void*, const char*, appkit::Cmd*, unsigned char);
    static void onCmdMapUpdate(void*, const appkit::String&, bool);

};

#endif
