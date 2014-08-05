#ifndef HASH_TABLE_SUITE_HPP
#define HASH_TABLE_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class HashTableSuite: public CppUnit::TestFixture
{

public:
    HashTableSuite();

    virtual ~HashTableSuite();

private:
    CPPUNIT_TEST_SUITE(HashTableSuite);
    CPPUNIT_TEST(testAdd00);
    CPPUNIT_TEST(testAdd01);
    CPPUNIT_TEST(testAdd02);
    CPPUNIT_TEST(testAdd03);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testSize00);
    CPPUNIT_TEST_SUITE_END();

    HashTableSuite(const HashTableSuite&); //prohibit usage
    const HashTableSuite& operator =(const HashTableSuite&); //prohibit usage

    void testAdd00();
    void testAdd01();
    void testAdd02();
    void testAdd03();
    void testCtor00();
    void testSize00();

    static bool cb0a(void*, void*);
    static bool cb0b(void*, void*);
    static void deleteItem(void*, void*);

};

#endif
