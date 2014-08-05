#ifndef MAPPED_TXT_FILE_SUITE_HPP
#define MAPPED_TXT_FILE_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(appkit, Directory)
DECLARE_CLASS1(appkit, String)


class MappedTxtFileSuite: public CppUnit::TestFixture
{

public:
    MappedTxtFileSuite();

    virtual ~MappedTxtFileSuite();

private:
    CPPUNIT_TEST_SUITE(MappedTxtFileSuite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testCtor03);
    CPPUNIT_TEST(testCtor04);
    CPPUNIT_TEST(testLoadFrom00);
    CPPUNIT_TEST(testLoadFrom01);
    CPPUNIT_TEST(testSaveIn00);
    CPPUNIT_TEST_SUITE_END();

    MappedTxtFileSuite(const MappedTxtFileSuite&); //prohibit usage
    const MappedTxtFileSuite& operator =(const MappedTxtFileSuite&); //prohibit usage

    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testCtor03();
    void testCtor04();
    void testLoadFrom00();
    void testLoadFrom01();
    void testSaveIn00();

    static bool cb0a(void*, const appkit::Directory&, const appkit::String&);

};

#endif
