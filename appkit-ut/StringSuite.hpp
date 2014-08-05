#ifndef STRING_SUITE_HPP
#define STRING_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class StringSuite: public CppUnit::TestFixture
{

public:
    StringSuite();

    virtual ~StringSuite();

private:
    CPPUNIT_TEST_SUITE(StringSuite);
    CPPUNIT_TEST(testAppend00);
    CPPUNIT_TEST(testAppend01);
    CPPUNIT_TEST(testAppend02);
    CPPUNIT_TEST(testAttachRaw00);
    CPPUNIT_TEST(testCompare00);
    CPPUNIT_TEST(testCopy00);
    CPPUNIT_TEST(testCopy01);
    //CPPUNIT_TEST(testCopy02);
    CPPUNIT_TEST(testCow00);
    CPPUNIT_TEST(testCow01);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testCtor03);
    CPPUNIT_TEST(testCtor04);
    CPPUNIT_TEST(testCtor05);
    CPPUNIT_TEST(testCtor06);
    CPPUNIT_TEST(testCtor07);
    CPPUNIT_TEST(testCtor08);
    CPPUNIT_TEST(testFind00);
    CPPUNIT_TEST(testFind01);
    CPPUNIT_TEST(testFind02);
    CPPUNIT_TEST(testFormAscii8a);
    CPPUNIT_TEST(testFormBox00);
    CPPUNIT_TEST(testFormUtfX00);
    CPPUNIT_TEST(testFormUtfX01);
    CPPUNIT_TEST(testFormUtfX02);
    CPPUNIT_TEST(testFormUtfX03);
    CPPUNIT_TEST(testFormUtfX04);
    CPPUNIT_TEST(testHash00);
    CPPUNIT_TEST(testNew00);
    CPPUNIT_TEST(testOp00);
    CPPUNIT_TEST(testOp01);
    CPPUNIT_TEST(testOp02);
    CPPUNIT_TEST(testOp03);
    //CPPUNIT_TEST(testPerf00);
    //CPPUNIT_TEST(testPerf01);
    //CPPUNIT_TEST(testPerf02);
    CPPUNIT_TEST(testReset00);
    CPPUNIT_TEST(testResize00);
    CPPUNIT_TEST(testRfind00);
    CPPUNIT_TEST(testRfind01);
    CPPUNIT_TEST(testSize00);
    CPPUNIT_TEST(testSubstr);
    CPPUNIT_TEST(testTrimSpace00);
    CPPUNIT_TEST(testVec00);
    CPPUNIT_TEST(testWiden00);
    CPPUNIT_TEST(testWith00);
    CPPUNIT_TEST_SUITE_END();

    StringSuite(const StringSuite&); //prohibit usage
    const StringSuite& operator =(const StringSuite&); //prohibit usage

    void testAppend00();
    void testAppend01();
    void testAppend02();
    void testAttachRaw00();
    void testCompare00();
    void testCopy00();
    void testCopy01();
    //void testCopy02();
    void testCow00();
    void testCow01();
    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testCtor03();
    void testCtor04();
    void testCtor05();
    void testCtor06();
    void testCtor07();
    void testCtor08();
    void testFind00();
    void testFind01();
    void testFind02();
    void testFormAscii8a();
    void testFormBox00();
    void testFormUtfX00();
    void testFormUtfX01();
    void testFormUtfX02();
    void testFormUtfX03();
    void testFormUtfX04();
    void testHash00();
    void testNew00();
    void testOp00();
    void testOp01();
    void testOp02();
    void testOp03();
    //void testPerf00();
    //void testPerf01();
    //void testPerf02();
    void testReset00();
    void testResize00();
    void testRfind00();
    void testRfind01();
    void testSize00();
    void testSubstr();
    void testTrimSpace00();
    void testVec00();
    void testWiden00();
    void testWith00();

    static void* entry00(void*);

};

#endif
