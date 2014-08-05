#ifndef UDP_CLIENT_SUITE_HPP
#define UDP_CLIENT_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class UdpClientSuite: public CppUnit::TestFixture
{

public:
    UdpClientSuite();

    virtual ~UdpClientSuite();

private:
    CPPUNIT_TEST_SUITE(UdpClientSuite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testCtor03);
    CPPUNIT_TEST(testNull00);
    CPPUNIT_TEST(testSnd00);
    CPPUNIT_TEST_SUITE_END();

    UdpClientSuite(const UdpClientSuite&); //prohibit usage
    const UdpClientSuite& operator =(const UdpClientSuite&); //prohibit usage

    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testCtor03();
    void testNull00();
    void testSnd00();

};

#endif
