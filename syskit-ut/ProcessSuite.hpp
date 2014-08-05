#ifndef PROCESS_SUITE_HPP
#define PROCESS_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(syskit, Module)
DECLARE_CLASS1(syskit, Process)


class ProcessSuite: public CppUnit::TestFixture
{

public:
    ProcessSuite();

    virtual ~ProcessSuite();

private:
    CPPUNIT_TEST_SUITE(ProcessSuite);
    CPPUNIT_TEST(testAffinityMask00);
    CPPUNIT_TEST(testApply00);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testMyId00);
    CPPUNIT_TEST(testStartTime00);
    CPPUNIT_TEST_SUITE_END();

    void testAffinityMask00();
    void testApply00();
    void testCtor00();
    void testMyId00();
    void testStartTime00();

    static bool cb0a(void*, const syskit::Module&);
    static bool cb0b(void*, const syskit::Module&);
    static bool cb1a(void*, const syskit::Process&);
    static bool cb1b(void*, const syskit::Process&);

};

#endif
