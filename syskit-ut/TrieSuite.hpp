#ifndef TRIE_SUITE_HPP
#define TRIE_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class TrieSuite: public CppUnit::TestFixture
{

public:
    TrieSuite();

    virtual ~TrieSuite();

private:
    CPPUNIT_TEST_SUITE(TrieSuite);
    CPPUNIT_TEST(testAdd00);
    CPPUNIT_TEST(testAdd01);
    CPPUNIT_TEST(testAdd02);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testFind00);
    CPPUNIT_TEST(testKey00);
    CPPUNIT_TEST(testKey01);
    CPPUNIT_TEST(testKey02);
    CPPUNIT_TEST(testKey03);
    CPPUNIT_TEST(testKey04);
    CPPUNIT_TEST(testKey05);
    CPPUNIT_TEST(testKey06);
    CPPUNIT_TEST(testKey07);
    CPPUNIT_TEST(testRm00);
    CPPUNIT_TEST(testRm01);
    CPPUNIT_TEST(testRm02);
    CPPUNIT_TEST(testRm03);
    CPPUNIT_TEST(testSize00);
    CPPUNIT_TEST_SUITE_END();

    TrieSuite(const TrieSuite&); //prohibit usage
    const TrieSuite& operator =(const TrieSuite&); //prohibit usage

    void testAdd00();
    void testAdd01();
    void testAdd02();
    void testCtor00();
    void testFind00();
    void testKey00();
    void testKey01();
    void testKey02();
    void testKey03();
    void testKey04();
    void testKey05();
    void testKey06();
    void testKey07();
    void testRm00();
    void testRm01();
    void testRm02();
    void testRm03();
    void testSize00();

    static bool validateOrder(void*, const unsigned char*, void*);
    static void deleteV(void*, const unsigned char*, void*);
    static void rmKv(void*, const unsigned char*, void*);

};

#endif
