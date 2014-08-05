#ifndef OBSERVER_SUITE_HPP
#define OBSERVER_SUITE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "syskit/macros.h"

DECLARE_CLASS1(appkit, Observer)


class ObserverSuite: public CppUnit::TestFixture
{

public:
    ObserverSuite();

    virtual ~ObserverSuite();

private:
    CPPUNIT_TEST_SUITE(ObserverSuite);
    CPPUNIT_TEST(testApply00);
    CPPUNIT_TEST(testAttachByName00);
    CPPUNIT_TEST(testCtor00);
    CPPUNIT_TEST(testCtor01);
    CPPUNIT_TEST(testFindSourceByName00);
    CPPUNIT_TEST(testSize00);
    CPPUNIT_TEST_SUITE_END();

    ObserverSuite(const ObserverSuite&); //prohibit usage
    const ObserverSuite& operator =(const ObserverSuite&); //prohibit usage

    void testApply00();
    void testAttachByName00();
    void testCtor00();
    void testCtor01();
    void testFindSourceByName00();
    void testSize00();

    static bool cb0a(void*, appkit::Observer*);
    static bool cb0b(void*, appkit::Observer*);
    static void cb1a(void*, appkit::Observer*);

};

#endif
