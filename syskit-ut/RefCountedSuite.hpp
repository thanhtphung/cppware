#ifndef REF_COUNTED_SUITE_HPP
#define REF_COUNTED_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class RefCountedSuite: public CppUnit::TestFixture
{

public:
    RefCountedSuite();

    virtual ~RefCountedSuite();

private:
    CPPUNIT_TEST_SUITE(RefCountedSuite);
    CPPUNIT_TEST(testCount00);
    CPPUNIT_TEST(testCount01);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testSize00);
    CPPUNIT_TEST_SUITE_END();

    RefCountedSuite(const RefCountedSuite&); //prohibit usage
    const RefCountedSuite& operator =(const RefCountedSuite&); //prohibit usage

    void testCount00();
    void testCount01();
    void testCtor00();
    void testSize00();

};

#endif
