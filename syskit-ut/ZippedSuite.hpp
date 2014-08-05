#ifndef ZIPPED_SUITE_HPP
#define ZIPPED_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(appkit, Directory)
DECLARE_CLASS1(appkit, String)


class ZippedSuite: public CppUnit::TestFixture
{

public:
    ZippedSuite();

    virtual ~ZippedSuite();

private:
    CPPUNIT_TEST_SUITE(ZippedSuite);
    CPPUNIT_TEST(testCancel00);
    CPPUNIT_TEST(testCancel01);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testCtor03);
    CPPUNIT_TEST(testCtor04);
    CPPUNIT_TEST(testCtor05);
    CPPUNIT_TEST_SUITE_END();

    ZippedSuite(const ZippedSuite&); //prohibit usage
    const ZippedSuite& operator =(const ZippedSuite&); //prohibit usage

    void testCancel00();
    void testCancel01();
    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testCtor03();
    void testCtor04();
    void testCtor05();

    static bool cb0a(void*, const appkit::Directory&, const appkit::String&);
    static bool onProgress0(void*, unsigned int, unsigned int, unsigned long long, unsigned long long);
    static bool onProgress1(void*, unsigned int, unsigned int, unsigned long long, unsigned long long);
    static bool onProgress2(void*, unsigned int, unsigned int, unsigned long long, unsigned long long);
    static bool onProgress3(void*, unsigned int, unsigned int, unsigned long long, unsigned long long);

};

#endif
