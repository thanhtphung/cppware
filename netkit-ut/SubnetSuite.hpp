#ifndef SUBNET_SUITE_HPP
#define SUBNET_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class SubnetSuite: public CppUnit::TestFixture
{

public:
    SubnetSuite();

    virtual ~SubnetSuite();

private:
    CPPUNIT_TEST_SUITE(SubnetSuite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST_SUITE_END();

    SubnetSuite(const SubnetSuite&); //prohibit usage
    const SubnetSuite& operator =(const SubnetSuite&); //prohibit usage

    void testCtor00();

};

#endif
