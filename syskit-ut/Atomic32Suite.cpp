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
    Atomic32 a(0xdcba9876UL);
    a.decrementBy(0x10101010UL);
    bool ok = (a == 0xccaa8866UL);
    CPPUNIT_ASSERT(ok);

    unsigned int old = 0x11223344UL;
    a.decrementBy(0x01010101UL, old);
    ok = ((a == 0xcba98765UL) && (old == 0xccaa8866UL));
    CPPUNIT_ASSERT(ok);

    a.decrement();
    ok = (a == 0xcba98764UL);
    CPPUNIT_ASSERT(ok);

    a.decrement(old);
    ok = ((a == 0xcba98763UL) && (old == 0xcba98764UL));
    CPPUNIT_ASSERT(ok);
}


void Atomic32Suite::testIncrement00()
{
    Atomic32 a(0x87654321UL);
    a.incrementBy(0x22222222UL);
    bool ok = (a == 0xa9876543UL);
    CPPUNIT_ASSERT(ok);

    unsigned int old = 0x11223344UL;
    a.incrementBy(0x33333333UL, old);
    ok = ((a == 0xdcba9876UL) && (old == 0xa9876543UL));
    CPPUNIT_ASSERT(ok);

    a.increment();
    CPPUNIT_ASSERT(a == 0xdcba9877UL);
    a.increment(old);
    CPPUNIT_ASSERT((a == 0xdcba9878UL) && (old == 0xdcba9877UL));
}


void Atomic32Suite::testOp00()
{
    Atomic32 a(0x12345678UL);
    bool ok = (a.asWord() == 0x12345678UL);
    CPPUNIT_ASSERT(ok);

    unsigned int rc = ++a;
    ok = ((rc == 0x12345679UL) && (a == 0x12345679UL));
    CPPUNIT_ASSERT(ok);

    rc = a++;
    ok = ((rc == 0x12345679UL) && (a == 0x1234567aUL));
    CPPUNIT_ASSERT(ok);

    rc = --a;
    ok = ((rc == 0x12345679UL) && (a == 0x12345679UL));
    CPPUNIT_ASSERT(ok);

    rc = a--;
    ok = ((rc == 0x12345679UL) && (a == 0x12345678UL));
    CPPUNIT_ASSERT(ok);

    a += 0x11111111UL;
    ok = (a == 0x23456789UL);
    CPPUNIT_ASSERT(ok);

    a -= 0x11111111UL;
    ok = (a == 0x12345678UL);
    CPPUNIT_ASSERT(ok);

    a = 0x87654321UL;
    ok = (a == 0x87654321UL);
    CPPUNIT_ASSERT(ok);
}


void Atomic32Suite::testSet00()
{
    Atomic32 a;
    bool ok = (a.asWord() == 0);
    CPPUNIT_ASSERT(ok);

    a.set(0x56789abcUL);
    ok = (a == 0x56789abcUL);
    CPPUNIT_ASSERT(ok);

    unsigned int old = 0x12233344UL;
    a.set(0xabcd1234UL, old);
    ok = ((a == 0xabcd1234UL) && (old == 0x56789abcUL));
    CPPUNIT_ASSERT(ok);

    a.setIfEqual(0x11122333UL, 0xabcd1234UL);
    ok = (a == 0x11122333UL);
    CPPUNIT_ASSERT(ok);

    a.setIfEqual(0x22233444UL, 0xabcd1234UL);
    ok = (a == 0x11122333UL);
    CPPUNIT_ASSERT(ok);

    a.setIfEqual(0x22233444UL, 0x11122333UL, old);
    ok = ((a == 0x22233444UL) && (old == 0x11122333UL));
    CPPUNIT_ASSERT(ok);

    a.setIfEqual(0x11122333UL, 0x11122333UL, old);
    ok = ((a == 0x22233444UL) && (old == 0x22233444UL));
    CPPUNIT_ASSERT(ok);
}
