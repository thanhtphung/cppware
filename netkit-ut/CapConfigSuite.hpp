#ifndef CAP_CONFIG_SUITE_HPP
#define CAP_CONFIG_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class CapConfigSuite: public CppUnit::TestFixture
{

public:
    CapConfigSuite();

    virtual ~CapConfigSuite();

private:
    CPPUNIT_TEST_SUITE(CapConfigSuite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testName00);
    CPPUNIT_TEST_SUITE_END();

    CapConfigSuite(const CapConfigSuite&); //prohibit usage
    const CapConfigSuite& operator =(const CapConfigSuite&); //prohibit usage

    void testCtor00();
    void testCtor01();
    void testName00();

};

#endif
