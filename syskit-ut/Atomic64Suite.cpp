#include "syskit/Atomic64.hpp"

#include "syskit-ut-pch.h"
#include "Atomic64Suite.hpp"

using namespace syskit;


Atomic64Suite::Atomic64Suite()
{
}


Atomic64Suite::~Atomic64Suite()
{
}


void Atomic64Suite::testDecrement00()
{
    Atomic64 a(0x55555555dcba9876ULL);
    a.decrementBy(0x10101010ULL);
    bool ok = (a == 0x55555555ccaa8866ULL);
    CPPUNIT_ASSERT(ok);

    unsigned long long old = 0x11223344ULL;
    a.decrementBy(0x01010101ULL, old);
    ok = ((a == 0x55555555cba98765ULL) && (old == 0x55555555ccaa8866ULL));
    CPPUNIT_ASSERT(ok);

    a.decrement();
    ok = (a == 0x55555555cba98764ULL);
    CPPUNIT_ASSERT(ok);

    a.decrement(old);
    ok = ((a == 0x55555555cba98763ULL) && (old == 0x55555555cba98764ULL));
    CPPUNIT_ASSERT(ok);
}


void Atomic64Suite::testIncrement00()
{
    Atomic64 a(0x5555555587654321ULL);
    a.incrementBy(0x22222222ULL);
    bool ok = (a == 0x55555555a9876543ULL);
    CPPUNIT_ASSERT(ok);

    unsigned long long old = 0x11223344ULL;
    a.incrementBy(0x33333333ULL, old);
    ok = ((a == 0x55555555dcba9876ULL) && (old == 0x55555555a9876543ULL));
    CPPUNIT_ASSERT(ok);

    a.increment();
    CPPUNIT_ASSERT(a == 0x55555555dcba9877ULL);
    a.increment(old);
    CPPUNIT_ASSERT((a == 0x55555555dcba9878ULL) && (old == 0x55555555dcba9877ULL));
}


void Atomic64Suite::testOp00()
{
    Atomic64 a(0x5555555512345678ULL);
    bool ok = (a.asWord() == 0x5555555512345678ULL);
    CPPUNIT_ASSERT(ok);

    unsigned long long rc = ++a;
    ok = ((rc == 0x5555555512345679ULL) && (a == 0x5555555512345679ULL));
    CPPUNIT_ASSERT(ok);

    rc = a++;
    ok = ((rc == 0x5555555512345679ULL) && (a == 0x555555551234567aULL));
    CPPUNIT_ASSERT(ok);

    rc = --a;
    ok = ((rc == 0x5555555512345679ULL) && (a == 0x5555555512345679ULL));
    CPPUNIT_ASSERT(ok);

    rc = a--;
    ok = ((rc == 0x5555555512345679ULL) && (a == 0x5555555512345678ULL));
    CPPUNIT_ASSERT(ok);

    a += 0x11111111ULL;
    ok = (a == 0x5555555523456789ULL);
    CPPUNIT_ASSERT(ok);

    a -= 0x11111111ULL;
    ok = (a == 0x5555555512345678ULL);
    CPPUNIT_ASSERT(ok);

    a = 0x5555555587654321ULL;
    ok = (a == 0x5555555587654321ULL);
    CPPUNIT_ASSERT(ok);
}


void Atomic64Suite::testSet00()
{
    Atomic64 a;
    bool ok = (a.asWord() == 0);
    CPPUNIT_ASSERT(ok);

    a.set(0x5555555556789abcULL);
    ok = (a == 0x5555555556789abcULL);
    CPPUNIT_ASSERT(ok);

    unsigned long long old = 0x12233344ULL;
    a.set(0x55555555abcd1234ULL, old);
    ok = ((a == 0x55555555abcd1234ULL) && (old == 0x5555555556789abcULL));
    CPPUNIT_ASSERT(ok);

    a.setIfEqual(0x5555555511122333ULL, 0x55555555abcd1234ULL);
    ok = (a == 0x5555555511122333ULL);
    CPPUNIT_ASSERT(ok);

    a.setIfEqual(0x5555555522233444ULL, 0x55555555abcd1234ULL);
    ok = (a == 0x5555555511122333ULL);
    CPPUNIT_ASSERT(ok);

    a.setIfEqual(0x5555555522233444ULL, 0x5555555511122333ULL, old);
    ok = ((a == 0x5555555522233444ULL) && (old == 0x5555555511122333ULL));
    CPPUNIT_ASSERT(ok);

    a.setIfEqual(0x5555555511122333ULL, 0x5555555511122333ULL, old);
    ok = ((a == 0x5555555522233444ULL) && (old == 0x5555555522233444ULL));
    CPPUNIT_ASSERT(ok);
}
