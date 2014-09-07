#include "syskit/Atomic32.hpp"

#include "syskit-ut-pch.h"
#include "Atomic32Suite.hpp"

using namespace syskit;


Atomic32Suite::Atomic32Suite()
{
}


Atomic32Suite::~Atomic32Suite()
{
}


void Atomic32Suite::testDecrement00()
{
    Atomic32 a(0xdcba9876U);
    a.decrementBy(0x10101010U);
    bool ok = (a == 0xccaa8866U);
    CPPUNIT_ASSERT(ok);

    unsigned int old = 0x11223344U;
    a.decrementBy(0x01010101U, old);
    ok = ((a == 0xcba98765U) && (old == 0xccaa8866U));
    CPPUNIT_ASSERT(ok);

    a.decrement();
    ok = (a == 0xcba98764U);
    CPPUNIT_ASSERT(ok);

    a.decrement(old);
    ok = ((a == 0xcba98763U) && (old == 0xcba98764U));
    CPPUNIT_ASSERT(ok);
}


void Atomic32Suite::testIncrement00()
{
    Atomic32 a(0x87654321U);
    a.incrementBy(0x22222222U);
    bool ok = (a == 0xa9876543U);
    CPPUNIT_ASSERT(ok);

    unsigned int old = 0x11223344U;
    a.incrementBy(0x33333333U, old);
    ok = ((a == 0xdcba9876U) && (old == 0xa9876543U));
    CPPUNIT_ASSERT(ok);

    a.increment();
    CPPUNIT_ASSERT(a == 0xdcba9877U);
    a.increment(old);
    CPPUNIT_ASSERT((a == 0xdcba9878U) && (old == 0xdcba9877U));
}


void Atomic32Suite::testOp00()
{
    Atomic32 a(0x12345678U);
    bool ok = (a.asWord() == 0x12345678U);
    CPPUNIT_ASSERT(ok);

    unsigned int rc = ++a;
    ok = ((rc == 0x12345679U) && (a == 0x12345679U));
    CPPUNIT_ASSERT(ok);

    rc = a++;
    ok = ((rc == 0x12345679U) && (a == 0x1234567aU));
    CPPUNIT_ASSERT(ok);

    rc = --a;
    ok = ((rc == 0x12345679U) && (a == 0x12345679U));
    CPPUNIT_ASSERT(ok);

    rc = a--;
    ok = ((rc == 0x12345679U) && (a == 0x12345678U));
    CPPUNIT_ASSERT(ok);

    a += 0x11111111U;
    ok = (a == 0x23456789U);
    CPPUNIT_ASSERT(ok);

    a -= 0x11111111U;
    ok = (a == 0x12345678U);
    CPPUNIT_ASSERT(ok);

    a = 0x87654321U;
    ok = (a == 0x87654321U);
    CPPUNIT_ASSERT(ok);
}


void Atomic32Suite::testSet00()
{
    Atomic32 a;
    bool ok = (a.asWord() == 0);
    CPPUNIT_ASSERT(ok);

    a.set(0x56789abcU);
    ok = (a == 0x56789abcU);
    CPPUNIT_ASSERT(ok);

    unsigned int old = 0x12233344U;
    a.set(0xabcd1234U, old);
    ok = ((a == 0xabcd1234U) && (old == 0x56789abcU));
    CPPUNIT_ASSERT(ok);

    a.setIfEqual(0x11122333U, 0xabcd1234U);
    ok = (a == 0x11122333U);
    CPPUNIT_ASSERT(ok);

    a.setIfEqual(0x22233444U, 0xabcd1234U);
    ok = (a == 0x11122333U);
    CPPUNIT_ASSERT(ok);

    a.setIfEqual(0x22233444U, 0x11122333U, old);
    ok = ((a == 0x22233444U) && (old == 0x11122333U));
    CPPUNIT_ASSERT(ok);

    a.setIfEqual(0x11122333U, 0x11122333U, old);
    ok = ((a == 0x22233444U) && (old == 0x22233444U));
    CPPUNIT_ASSERT(ok);
}
