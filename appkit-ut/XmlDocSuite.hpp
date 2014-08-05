#ifndef XML_DOC_SUITE_HPP
#define XML_DOC_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(appkit, Directory)
DECLARE_CLASS1(appkit, String)


class XmlDocSuite: public CppUnit::TestFixture
{

public:
    XmlDocSuite();

    virtual ~XmlDocSuite();

private:
    CPPUNIT_TEST_SUITE(XmlDocSuite);
    CPPUNIT_TEST(testCdata00);
    CPPUNIT_TEST(testComment00);
    CPPUNIT_TEST(testContent00);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testCtor02);
    CPPUNIT_TEST(testEmptyElement00);
    CPPUNIT_TEST(testFind00);
    CPPUNIT_TEST(testLoadFrom00);
    CPPUNIT_TEST(testLoadFrom01);
    CPPUNIT_TEST(testLoadFrom02);
    CPPUNIT_TEST(testLoadFrom03);
    CPPUNIT_TEST(testProlog00);
    CPPUNIT_TEST(testProlog01);
    CPPUNIT_TEST(testUnknownElement00);
    CPPUNIT_TEST_SUITE_END();

    XmlDocSuite(const XmlDocSuite&); //prohibit usage
    const XmlDocSuite& operator =(const XmlDocSuite&); //prohibit usage

    void testCdata00();
    void testComment00();
    void testContent00();
    void testCtor00();
    void testCtor01();
    void testCtor02();
    void testEmptyElement00();
    void testFind00();
    void testLoadFrom00();
    void testLoadFrom01();
    void testLoadFrom02();
    void testLoadFrom03();
    void testProlog00();
    void testProlog01();
    void testUnknownElement00();

    static bool loadXmlFile(void*, const appkit::Directory&, const appkit::String&);

};

#endif
