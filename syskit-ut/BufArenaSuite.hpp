#ifndef BUF_ARENA_SUITE_HPP
#define BUF_ARENA_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(syskit, BufArena)


class BufArenaSuite: public CppUnit::TestFixture
{

public:
    BufArenaSuite();

    virtual ~BufArenaSuite();

private:
    CPPUNIT_TEST_SUITE(BufArenaSuite);
    CPPUNIT_TEST(testBufPool00);
    CPPUNIT_TEST(testBufPool01);
    CPPUNIT_TEST(testBufPool02);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST_SUITE_END();

    BufArenaSuite(const BufArenaSuite&); //prohibit usage
    const BufArenaSuite& operator =(const BufArenaSuite&); //prohibit usage

    void testBufPool00();
    void testBufPool01();
    void testBufPool02();
    void testCtor00();
    void testCtor01();

    static void validate(syskit::BufArena&);

};

#endif
