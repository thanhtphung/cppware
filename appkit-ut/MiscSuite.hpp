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
    CPPUNIT_TEST(testStrArray00);
    CPPUNIT_TEST(testToeprint00);
    CPPUNIT_TEST_SUITE_END();

    MiscSuite(const MiscSuite&); //prohibit usage
    const MiscSuite& operator =(const MiscSuite&); //prohibit usage

    void testStrArray00();
    void testToeprint00();

};

#endif
