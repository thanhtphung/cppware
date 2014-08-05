#ifndef LIFO_SUITE_HPP
#define LIFO_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(syskit, Lifo)


class LifoSuite: public CppUnit::TestFixture
{

public:
    LifoSuite();

    virtual ~LifoSuite();

private:
    syskit::Lifo* q0_;
    syskit::Lifo* q1_;

    CPPUNIT_TEST_SUITE(LifoSuite);
    CPPUNIT_TEST(testPeek00);
    CPPUNIT_TEST(testPush00);
    CPPUNIT_TEST(testPush01);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST_SUITE_END();

    void testPeek00();
    void testPush00();
    void testPush01();
    void testOp00();

};

#endif
