#ifndef BAD_SUITE_HPP
#define BAD_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class BadSuite: public CppUnit::TestFixture
{

public:
    BadSuite();

    virtual ~BadSuite();

private:
    CPPUNIT_TEST_SUITE(BadSuite);
    CPPUNIT_TEST(testException00);
    CPPUNIT_TEST(testFailure00);
    CPPUNIT_TEST(testFailure01);
    CPPUNIT_TEST_SUITE_END();

    BadSuite(const BadSuite&); //prohibit usage
    const BadSuite& operator =(const BadSuite&); //prohibit usage

    void testException00();
    void testFailure00();
    void testFailure01();

};

#endif
