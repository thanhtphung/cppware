#ifndef CRI_SECTION_SUITE_HPP
#define CRI_SECTION_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/CriSection.hpp"


class CriSectionSuite: public CppUnit::TestFixture
{

public:
    CriSectionSuite();

    virtual ~CriSectionSuite();

private:
    static syskit::CriSection cs_;
    static unsigned long long u64_;

    CPPUNIT_TEST_SUITE(CriSectionSuite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST_SUITE_END();

    void testCtor00();
    void testCtor01();

    static void* entry00(void*);

};

#endif
