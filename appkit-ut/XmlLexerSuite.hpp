#ifndef XML_LEXER_SUITE_HPP
#define XML_LEXER_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class XmlLexerSuite: public CppUnit::TestFixture
{

public:
    XmlLexerSuite();

    virtual ~XmlLexerSuite();

private:
    CPPUNIT_TEST_SUITE(XmlLexerSuite);
    CPPUNIT_TEST(testEscape00);
    CPPUNIT_TEST(testScan00);
    CPPUNIT_TEST(testScan01);
    CPPUNIT_TEST(testScan02);
    CPPUNIT_TEST(testScan03);
    CPPUNIT_TEST(testUnescape00);
    CPPUNIT_TEST(testUnescape01);
    CPPUNIT_TEST(testUnescape02);
    CPPUNIT_TEST(testValidateElementName00);
    CPPUNIT_TEST_SUITE_END();

    XmlLexerSuite(const XmlLexerSuite&); //prohibit usage
    const XmlLexerSuite& operator =(const XmlLexerSuite&); //prohibit usage

    void testEscape00();
    void testScan00();
    void testScan01();
    void testScan02();
    void testScan03();
    void testUnescape00();
    void testUnescape01();
    void testUnescape02();
    void testValidateElementName00();

};

#endif
