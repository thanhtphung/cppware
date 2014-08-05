#ifndef IP_ADDR_SET_SUITE_HPP
#define IP_ADDR_SET_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(netkit, IpAddrSet)


class IpAddrSetSuite: public CppUnit::TestFixture
{

public:
    IpAddrSetSuite();

    virtual ~IpAddrSetSuite();

private:
    CPPUNIT_TEST_SUITE(IpAddrSetSuite);
    CPPUNIT_TEST(testAdd00);
    CPPUNIT_TEST(testAdd01);
    CPPUNIT_TEST(testAdd02);
    CPPUNIT_TEST(testAdd03);
    CPPUNIT_TEST(testAdd05);
    CPPUNIT_TEST(testChk00);
    CPPUNIT_TEST(testCmp00);
    CPPUNIT_TEST(testCmp01);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testCtor03);
    CPPUNIT_TEST(testCtor04);
    CPPUNIT_TEST(testItor00);
    CPPUNIT_TEST(testItor01);
    CPPUNIT_TEST(testItor02);
    CPPUNIT_TEST(testItor03);
    CPPUNIT_TEST(testNew00);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST(testOp01);
    CPPUNIT_TEST(testOp02);
    CPPUNIT_TEST(testOp03);
    CPPUNIT_TEST(testOp04);
    CPPUNIT_TEST(testOp05);
    CPPUNIT_TEST(testRm00);
    CPPUNIT_TEST(testRm01);
    CPPUNIT_TEST(testSize00);
    CPPUNIT_TEST_SUITE_END();

    IpAddrSetSuite(const IpAddrSetSuite&); //prohibit usage
    const IpAddrSetSuite& operator =(const IpAddrSetSuite&); //prohibit usage

    void testAdd00();
    void testAdd01();
    void testAdd02();
    void testAdd03();
    void testAdd05();
    void testChk00();
    void testCmp00();
    void testCmp01();
    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testCtor03();
    void testCtor04();
    void testItor00();
    void testItor01();
    void testItor02();
    void testItor03();
    void testNew00();
    void testOp00();
    void testOp01();
    void testOp02();
    void testOp03();
    void testOp04();
    void testOp05();
    void testRm00();
    void testRm01();
    void testSize00();
    void validateSet(const netkit::IpAddrSet&);

};

#endif
