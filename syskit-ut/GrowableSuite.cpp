#include "syskit/Growable.hpp"

#include "syskit-ut-pch.h"
#include "GrowableSuite.hpp"

using namespace syskit;

BEGIN_NAMESPACE

class Container: public Growable
{
public:
    using Growable::grow;
    using Growable::increaseCap;
    using Growable::nextCap;
    Container(const Container& container);
    Container(unsigned int capacity, int growBy);
    virtual ~Container();
    virtual bool resize(unsigned int newCap);
};

Container::Container(const Container& container):
Growable(container)
{
}

Container::Container(unsigned int capacity, int growBy):
Growable(capacity, growBy)
{
}

Container::~Container()
{
}

bool Container::resize(unsigned int newCap)
{
    return canGrow()? (setCapacity(newCap), true): (Growable::resize(newCap));
}

END_NAMESPACE


GrowableSuite::GrowableSuite()
{
}


GrowableSuite::~GrowableSuite()
{
}


void GrowableSuite::testCtor00()
{
    Container fixed(123, 0);
    bool ok = ((fixed.capacity() == 123) && (fixed.initialCap() == 123));
    CPPUNIT_ASSERT(ok);

    int growBy = 456;
    ok = ((!fixed.canGrow()) && (!fixed.canGrow(growBy)) && (growBy == 0) && (fixed.growthFactor() == 0));
    CPPUNIT_ASSERT(ok);

    ok = ((!fixed.grow()) && (!fixed.resize(999)));
    CPPUNIT_ASSERT(ok);
}


void GrowableSuite::testCtor01()
{
    Container flex(22, -1);
    bool ok = ((flex.capacity() == 22) && (flex.initialCap() == 22));
    CPPUNIT_ASSERT(ok);

    int growBy = 22;
    ok = (flex.canGrow() && flex.canGrow(growBy) && (growBy == -1) && (flex.growthFactor() == -1));
    CPPUNIT_ASSERT(ok);

    ok = (flex.grow() && (flex.capacity() == 44) && (flex.initialCap() == 22));
    CPPUNIT_ASSERT(ok);

    flex.setGrowth(11);
    ok = (flex.grow() && (flex.capacity() == 55) && (flex.initialCap() == 22));
    CPPUNIT_ASSERT(ok);

    ok = (flex.resize(999) && (flex.capacity() == 999) && (flex.initialCap() == 22));
    CPPUNIT_ASSERT(ok);

    flex.setGrowth(-1);
    while (flex.grow());
    ok = (flex.capacity() > 999);
    CPPUNIT_ASSERT(ok);
}


void GrowableSuite::testCtor02()
{
    Container flex0(33, 11);
    Container flex1(flex0);
    bool ok = ((flex1.capacity() == 33) && (flex1.initialCap() == 33));
    CPPUNIT_ASSERT(ok);

    int growBy = 22;
    ok = (flex1.canGrow() && flex1.canGrow(growBy) && (growBy == 11) && (flex1.growthFactor() == 11));
    CPPUNIT_ASSERT(ok);

    flex0.grow();
    flex1 = flex0;
    ok = ((flex1.capacity() == 44) && (flex1.initialCap() == 33));
    CPPUNIT_ASSERT(ok);

    flex1 = flex1;
    ok = ((flex1.capacity() == 44) && (flex1.initialCap() == 33));
    CPPUNIT_ASSERT(ok);
}


void GrowableSuite::testCtor03()
{
    Container flex0(0, -1);
    bool ok = ((flex0.capacity() == 1) && (flex0.initialCap() == 1));
    CPPUNIT_ASSERT(ok);

    Container flex1(0, 123);
    ok = ((flex1.capacity() == 0) && (flex1.initialCap() == 0));
    CPPUNIT_ASSERT(ok);
    ok = (!flex1.setGrowth(-1));
    CPPUNIT_ASSERT(ok);
}


void GrowableSuite::testNextCap00()
{
    Container flex0(111, 222);
    bool ok = ((flex0.nextCap(99) == 111) && (flex0.nextCap(345) == 555));
    CPPUNIT_ASSERT(ok);
    ok = (flex0.nextCap(0xffffffffUL) == 0);
    CPPUNIT_ASSERT(ok);

    Container flex1(0x87654321UL, 0x7fffffffL);
    ok = (flex1.nextCap() == 0);
    CPPUNIT_ASSERT(ok);

    Container flex2(7, -1);
    ok = (flex2.nextCap(99) == 112);
    CPPUNIT_ASSERT(ok);
    ok = (flex2.nextCap(0xfffffffeUL) == 0);
    CPPUNIT_ASSERT(ok);
    ok = ((flex2.increaseCap(3) == 10) && (flex2.capacity() == 10));
    CPPUNIT_ASSERT(ok);
}
