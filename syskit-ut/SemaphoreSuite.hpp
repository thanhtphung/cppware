#ifndef SEMAPHORE_SUITE_HPP
#define SEMAPHORE_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class SemaphoreSuite: public CppUnit::TestFixture
{

public:
    SemaphoreSuite();

    virtual ~SemaphoreSuite();

private:
    CPPUNIT_TEST_SUITE(SemaphoreSuite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testDecrement00);
    CPPUNIT_TEST(testDecrement01);
    CPPUNIT_TEST(testDetach00);
    CPPUNIT_TEST(testLock00);
    CPPUNIT_TEST(testLock01);
    CPPUNIT_TEST(testReset00);
    CPPUNIT_TEST(testWait00);
    CPPUNIT_TEST(testWait01);
    CPPUNIT_TEST_SUITE_END();

    void testCtor00();
    void testCtor01();
    void testDecrement00();
    void testDecrement01();
    void testDetach00();
    void testLock00();
    void testLock01();
    void testReset00();
    void testWait00();
    void testWait01();

    static void* entry00(void*);
    static void* entry01(void*);

};

#endif
