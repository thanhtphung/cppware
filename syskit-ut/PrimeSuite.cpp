#include "syskit/Prime.hpp"

#include "syskit-ut-pch.h"
#include "PrimeSuite.hpp"

using namespace syskit;


PrimeSuite::PrimeSuite()
{
}


PrimeSuite::~PrimeSuite()
{
}


void PrimeSuite::testCtor00()
{
    Prime n0(1U);
    bool ok = (n0.isOk() && (n0 == 2U) && Prime::isOne(n0));
    CPPUNIT_ASSERT(ok);

    n0 = Prime(300U);
    ok = (n0.isOk() && (n0 == 307U) && Prime::isOne(n0));
    CPPUNIT_ASSERT(ok);

    n0 = Prime(268435455U);
    ok = (n0.isOk() && (n0 == 268435459U) && Prime::isOne(n0));
    CPPUNIT_ASSERT(ok);

    Prime n1(n0);
    ok = (n1 == n0);
    CPPUNIT_ASSERT(ok);

    n1 = Prime(4294967293U);
    ok = ((!n1.isOk()) && (n1 == 0U) && (!Prime::isOne(n1)));
    CPPUNIT_ASSERT(ok);
}


void PrimeSuite::testFindHi00()
{
    unsigned int n0 = Prime::findHi(300U);
    bool ok = ((n0 == 293U) && Prime::isOne(n0));
    CPPUNIT_ASSERT(ok);

    n0 = Prime::findHi(268435455U);
    ok = ((n0 == 268435399U) && Prime::isOne(n0));
    CPPUNIT_ASSERT(ok);

    n0 = Prime::findHi(1U);
    ok = (n0 == 0U);
    CPPUNIT_ASSERT(ok);
}
