#ifndef S32_SUITE_HPP
#define S32_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>


class S32Suite: public CppUnit::TestFixture
{

public:
    S32Suite();

    virtual ~S32Suite();

private:
    CPPUNIT_TEST_SUITE(S32Suite);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testEof00);
    CPPUNIT_TEST(testIsXxx00);
    CPPUNIT_TEST(testToXxx00);
    CPPUNIT_TEST_SUITE_END();

    S32Suite(const S32Suite&); //prohibit usage
    const S32Suite& operator =(const S32Suite&); //prohibit usage

    void testCtor00();
    void testEof00();
    void testIsXxx00();
    void testToXxx00();

};

#endif
