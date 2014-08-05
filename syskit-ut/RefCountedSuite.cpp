#include "syskit/RefCounted.hpp"

#include "syskit-ut-pch.h"
#include "RefCountedSuite.hpp"

using namespace syskit;

BEGIN_NAMESPACE

class One: public RefCounted
{
public:
    One(bool* destroyed = 0);
protected:
    virtual ~One();
private:
    bool* destroyed_;
};

One::One(bool* destroyed):
RefCounted(0)
{

    destroyed_ = destroyed;
    if (destroyed_ != 0)
    {
        *destroyed_ = false;
    }
}

One::~One()
{

    if (destroyed_ != 0)
    {
        *destroyed_ = true;
    }
}

END_NAMESPACE


RefCountedSuite::RefCountedSuite()
{
}


RefCountedSuite::~RefCountedSuite()
{
}


void RefCountedSuite::testCount00()
{
    bool destroyed = false;
    One* one = new One(&destroyed);
    {
        RefCounted::Count count(*one);
        bool ok = (!destroyed);
        CPPUNIT_ASSERT(ok);
    }
    bool ok = destroyed;
    CPPUNIT_ASSERT(ok);

    one = new One(&destroyed);
    {
        bool skipAddRef = false;
        RefCounted::Count count(*one, skipAddRef);
        bool ok = (!destroyed);
        CPPUNIT_ASSERT(ok);
    }
    ok = destroyed;
    CPPUNIT_ASSERT(ok);
}


void RefCountedSuite::testCount01()
{
    bool destroyed = false;
    One* one = new One(&destroyed);
    one->addRef();
    {
        bool skipAddRef = true;
        RefCounted::Count count(*one, skipAddRef);
        bool ok = (!destroyed);
        CPPUNIT_ASSERT(ok);
    }

    bool ok = destroyed;
    CPPUNIT_ASSERT(ok);
}


void RefCountedSuite::testCtor00()
{
    One* one0 = new One;
    one0->addRef();
    bool ok = one0->rmRef();
    CPPUNIT_ASSERT(ok);

    One* one1 = new One;
    one1->addRef();
    one1->addRef();
    ok = ((!one1->rmRef()) && (one1->refCount() == 1UL) && (one1->clone() == 0));
    CPPUNIT_ASSERT(ok);
    ok = one1->rmRef();
    CPPUNIT_ASSERT(ok);

    One* one2 = new One;
    one2->incrementRefBy(7);
    one2->decrementRefBy(3);
    ok = ((!one2->rmRef(2)) && (one2->refCount() == 2UL));
    CPPUNIT_ASSERT(ok);
    ok = one2->rmRef(2);
    CPPUNIT_ASSERT(ok);
}


void RefCountedSuite::testSize00()
{
    bool ok = (sizeof(RefCounted) == sizeof(void*) * 2);
    CPPUNIT_ASSERT(ok);
}
