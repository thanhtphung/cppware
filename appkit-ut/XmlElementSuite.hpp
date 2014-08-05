#ifndef XML_ELEMENT_SUITE_HPP
#define XML_ELEMENT_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(appkit, XmlElement)


class XmlElementSuite: public CppUnit::TestFixture
{

public:
    XmlElementSuite();

    virtual ~XmlElementSuite();

private:
    CPPUNIT_TEST_SUITE(XmlElementSuite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testDetach00);
    CPPUNIT_TEST(testFind00);
    CPPUNIT_TEST(testFind01);
    CPPUNIT_TEST(testFind02);
    CPPUNIT_TEST(testFind03);
    CPPUNIT_TEST(testFullName00);
    CPPUNIT_TEST(testNew00);
    CPPUNIT_TEST(testSetBody00);
    CPPUNIT_TEST(testSize00);
    CPPUNIT_TEST(testStringPair00);
    CPPUNIT_TEST_SUITE_END();

    XmlElementSuite(const XmlElementSuite&); //prohibit usage
    const XmlElementSuite& operator =(const XmlElementSuite&); //prohibit usage

    void testCtor00();
    void testCtor01();
    void testDetach00();
    void testFind00();
    void testFind01();
    void testFind02();
    void testFind03();
    void testFullName00();
    void testNew00();
    void testSetBody00();
    void testSize00();
    void testStringPair00();

    static bool cb0(void*, const appkit::XmlElement&);
    static bool cb1(void*, const appkit::XmlElement&);

};

#endif
