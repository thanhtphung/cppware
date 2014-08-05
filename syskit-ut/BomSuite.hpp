#ifndef BOM_SUITE_HPP
#define BOM_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class BomSuite: public CppUnit::TestFixture
{

public:
    BomSuite();

    virtual ~BomSuite();

private:
    CPPUNIT_TEST_SUITE(BomSuite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testDecode00);
    CPPUNIT_TEST_SUITE_END();

    void testCtor00();
    void testDecode00();

};

#endif
