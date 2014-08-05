#ifndef MISC_SUITE_HPP
#define MISC_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class MiscSuite: public CppUnit::TestFixture
{

public:
    MiscSuite();

    virtual ~MiscSuite();

private:
    CPPUNIT_TEST_SUITE(MiscSuite);
    CPPUNIT_TEST(testAtomicWord00);
    CPPUNIT_TEST(testCallStack00);
    CPPUNIT_TEST(testDate00);
    CPPUNIT_TEST(testDevNull00);
    CPPUNIT_TEST(testFoundation00);
    CPPUNIT_TEST(testSingleton00);
    CPPUNIT_TEST(testTickTime00);
    CPPUNIT_TEST(testTickTime01);
    CPPUNIT_TEST(testUtc00);
    CPPUNIT_TEST_SUITE_END();

    void testAtomicWord00();
    void testCallStack00();
    void testDate00();
    void testDevNull00();
    void testFoundation00();
    void testSingleton00();
    void testTickTime00();
    void testTickTime01();
    void testUtc00();

};

#endif
