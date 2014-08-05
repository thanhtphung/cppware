#ifndef THREAD_SUITE_HPP
#define THREAD_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class ThreadSuite: public CppUnit::TestFixture
{

public:
    ThreadSuite();

    virtual ~ThreadSuite();

private:
    CPPUNIT_TEST_SUITE(ThreadSuite);
    CPPUNIT_TEST(testAffinity00);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testDetach00);
    CPPUNIT_TEST(testKill00);
    CPPUNIT_TEST(testKill01);
    CPPUNIT_TEST(testMonitorCrash00);
    CPPUNIT_TEST(testMyId00);
    CPPUNIT_TEST_SUITE_END();

    void testAffinity00();
    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testDetach00();
    void testKill00();
    void testKill01();
    void testMonitorCrash00();
    void testMyId00();

    static void crashCb0(void*);
    static void* entrance00(void*);
    static void* entrance01(void*);
    static void* entrance02(void*);
    static void* entrance03(void*);
    static void* entrance04(void*);
    static void* entrance05(void*);

};

#endif
