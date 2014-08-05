#ifndef DIC_FILE_SUITE_HPP
#define DIC_FILE_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class DicFileSuite: public CppUnit::TestFixture
{

public:
    DicFileSuite();

    virtual ~DicFileSuite();

private:
    CPPUNIT_TEST_SUITE(DicFileSuite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testSave00);
    CPPUNIT_TEST_SUITE_END();

    DicFileSuite(const DicFileSuite&); //prohibit usage
    const DicFileSuite& operator =(const DicFileSuite&); //prohibit usage

    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testSave00();

    static void* entry00(void*);

};

#endif
