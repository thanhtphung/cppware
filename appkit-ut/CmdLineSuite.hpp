#ifndef CMD_LINE_SUITE_HPP
#define CMD_LINE_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class CmdLineSuite: public CppUnit::TestFixture
{

public:
    CmdLineSuite();
    virtual ~CmdLineSuite();

private:
    CPPUNIT_TEST_SUITE(CmdLineSuite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testCtor03);
    CPPUNIT_TEST(testCtor04);
    CPPUNIT_TEST(testCtor05);
    CPPUNIT_TEST(testCtor06);
    CPPUNIT_TEST(testCtor07);
    CPPUNIT_TEST(testNew00);
    CPPUNIT_TEST(testSize00);
    CPPUNIT_TEST_SUITE_END();

    CmdLineSuite(const CmdLineSuite&); //prohibit usage
    const CmdLineSuite& operator =(const CmdLineSuite&); //prohibit usage

    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testCtor03();
    void testCtor04();
    void testCtor05();
    void testCtor06();
    void testCtor07();
    void testNew00();
    void testSize00();

};

#endif
