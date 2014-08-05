#ifndef ITEM_Q_SUITE_HPP
#define ITEM_Q_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class ItemQSuite: public CppUnit::TestFixture
{

public:
    ItemQSuite();

    virtual ~ItemQSuite();

private:
    CPPUNIT_TEST_SUITE(ItemQSuite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testDtor00);
    CPPUNIT_TEST(testExpedite00);
    CPPUNIT_TEST(testExpedite01);
    CPPUNIT_TEST(testGet00);
    CPPUNIT_TEST(testGet01);
    CPPUNIT_TEST(testGet02);
    CPPUNIT_TEST(testGrow00);
    CPPUNIT_TEST(testGrow01);
    CPPUNIT_TEST(testGrow02);
    CPPUNIT_TEST(testPut00);
    CPPUNIT_TEST(testPut01);
    CPPUNIT_TEST_SUITE_END();

    ItemQSuite(const ItemQSuite&); //prohibit usage
    const ItemQSuite& operator =(const ItemQSuite&); //prohibit usage

    void testCtor00();
    void testCtor01();
    void testDtor00();
    void testExpedite00();
    void testExpedite01();
    void testGet00();
    void testGet01();
    void testGet02();
    void testGrow00();
    void testGrow01();
    void testGrow02();
    void testPut00();
    void testPut01();

};

#endif
