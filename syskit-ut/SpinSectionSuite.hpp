#ifndef SPIN_SECTION_SUITE_HPP
#define SPIN_SECTION_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/SpinSection.hpp"


class SpinSectionSuite: public CppUnit::TestFixture
{

public:
    SpinSectionSuite();

    virtual ~SpinSectionSuite();

private:
    static syskit::SpinSection ss_;
    static unsigned long long u64_;

    CPPUNIT_TEST_SUITE(SpinSectionSuite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST_SUITE_END();

    void testCtor00();
    void testCtor01();

    static void* entry00(void*);

};

#endif
