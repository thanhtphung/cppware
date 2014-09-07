#include "appkit/Directory.hpp"
#include "appkit/TempDir.hpp"
#include "appkit/TempFile.hpp"
#include "syskit/AtomicWord.hpp"
#include "syskit/CallStack.hpp"
#include "syskit/Date.hpp"
#include "syskit/DevNull.hpp"
#include "syskit/Foundation.hpp"
#include "syskit/Singleton.hpp"
#include "syskit/Thread.hpp"
#include "syskit/TickTime.hpp"
#include "syskit/Utc.hpp"

#include "syskit-ut-pch.h"
#include "MiscSuite.hpp"

using namespace appkit;
using namespace syskit;

typedef struct
{
    const char* date;
    const char* s;
    unsigned int n;
} sample_t;

const sample_t DATE_SAMPLE[] =
{
    {"Jan 01 0000", "0000-01-01", 101U},
    {"Dec 31 9999", "9999-12-31", 99991231U},
    {"Aug 07 2009", "2009-08-07", 20090807U},
    {"Feb 13 2004", "2004-02-13", 20040213U},
    {"Mar 14 2005", "2005-03-14", 20050314U},
    {"Apr 15 2006", "2006-04-15", 20060415U},
    {"May 25 2007", "2007-05-25", 20070525U},
    {"Jun 26 2008", "2008-06-26", 20080626U},
    {"Jul 27 2009", "2009-07-27", 20090727U},
    {"Sep 28 2010", "2010-09-28", 20100928U},
    {"Oct 29 2011", "2011-10-29", 20111029U},
    {"Nov 30 2012", "2012-11-30", 20121130U}
};
const size_t NUM_DATE_SAMPLES = sizeof(DATE_SAMPLE) / sizeof(DATE_SAMPLE[0]);

BEGIN_NAMESPACE

int compare(const void* item0, const void* item1)
{
    return (item0 < item1)? (-1): ((item0>item1)? 1: 0);
}

class X1: public Singleton
{
public:
    static Singleton* create(const char* id, unsigned int initialRefCount, void* arg);
protected:
    X1(const char* id, unsigned int initialRefCount = 1U);
    virtual ~X1();
private:
    X1(const X1&); //prohibit usage
    const X1& operator =(const X1&); //prohibit usage
};

X1::X1(const char* id, unsigned int initialRefCount):
RefCounted(initialRefCount),
Singleton(id, initialRefCount)
{
}

X1::~X1()
{
}

Singleton* X1::create(const char* id, unsigned int initialRefCount, void* /*arg*/)
{
    Singleton* x1 = new X1(id, initialRefCount);
    return x1;
}

END_NAMESPACE


MiscSuite::MiscSuite()
{
}


MiscSuite::~MiscSuite()
{
}


void MiscSuite::testAtomicWord00()
{
    AtomicWord atomicWord;
    bool ok = (sizeof(atomicWord) == sizeof(size_t));
    CPPUNIT_ASSERT(ok);

    ok = (sizeof(int) == 4);
    CPPUNIT_ASSERT(ok);
}


void MiscSuite::testCallStack00()
{
    unsigned long exceptionCode = 0;
    String basename("stack-dump.txt");
    TempDir temp;
    TempFile dump(temp, basename);

    {
        String::W w(dump.path().widen());
        CallStack cs(0, w, exceptionCode);
        const CallStack::Frame* curFrame = cs.curFrame();
        bool ok = (curFrame != 0) && (curFrame->next() != 0) && (curFrame->eip() != 0);
        CPPUNIT_ASSERT(ok);
        const wchar_t* desc = cs.describe();
        ok = (*desc != 0);
        CPPUNIT_ASSERT(ok);
        delete[] desc;
        cs.dump();
    }

    Directory dir(temp.path());
    Directory::Attr attr;
    dir.getChildAttr(attr, basename);
    bool ok = (attr.size() > 999);
    CPPUNIT_ASSERT(ok);
}


void MiscSuite::testDate00()
{
    bool ok = true;
    for (unsigned int i = 0; i < NUM_DATE_SAMPLES; ++i)
    {
        const sample_t& r = DATE_SAMPLE[i];
        Date date(r.date);
        char buf[Date::MaxDateStringLength + 1];
        if ((date != r.n) || (String(r.s) != date.asString(buf)))
        {
            ok = false;
            break;
        }
    }

    Date date0("Jan 01 0000");
    Date date1("Dec 31 9999");
    Date date2("Aug  7 2009");
    char s0[Date::MaxDateStringLength + 1];
    char s1[Date::MaxDateStringLength + 1];
    char s2[Date::MaxDateStringLength + 1];
    ok = (date0 == 101U) && (String("0000-01-01") == date0.asString(s0)) &&
        (date1 == 99991231U) && (String("9999-12-31") == date1.asString(s1)) &&
        (date2 == 20090807U) && (String("2009-08-07") == date2.asString(s2));
    CPPUNIT_ASSERT(ok);

    char t[Date::MaxTimeStringLength + 1];
    unsigned int secs = 0;
    ok = (String("00000:00:00:00") == Date::formatTime(t, secs));
    CPPUNIT_ASSERT(ok);

    secs = 0xffffffffU;
    ok = (String("49710:06:28:15") == Date::formatTime(t, secs));
    CPPUNIT_ASSERT(ok);
}


void MiscSuite::testDevNull00()
{
    DevNull nullDevice0;
    bool ok = (nullDevice0.fd() >= 0);
    CPPUNIT_ASSERT(ok);

    DevNull nullDevice1(nullDevice0.fd());
    ok = (nullDevice1.fd() >= 0);
    CPPUNIT_ASSERT(ok);
}


void MiscSuite::testFoundation00()
{
    const Foundation& r = Foundation::instance();
    bool ok = r.isOk();
    CPPUNIT_ASSERT(ok);
}


void MiscSuite::testSingleton00()
{
    const char* id = "not-a-singleton";
    const Singleton* singleton = Singleton::getSingleton(id);
    bool ok = (singleton == 0);
    CPPUNIT_ASSERT(ok);

    id = "anonymous::X1";
    singleton = Singleton::getSingleton(id);
    ok = (singleton == 0);
    CPPUNIT_ASSERT(ok);

    Singleton* x1 = Singleton::getSingleton(id, X1::create, 1U /*initialRefCount*/, 0 /*createArg*/);
    ok = (x1 != 0) && (x1->refCount() == 1) && (x1 == Singleton::getSingleton(id, X1::create, 1U /*initialRefCount*/, 0 /*createArg*/));
    CPPUNIT_ASSERT(ok);
    x1->rmRef();
    x1 = Singleton::getSingleton(id);
    ok = (x1 == 0);
    CPPUNIT_ASSERT(ok);
}


void
MiscSuite::testTickTime00()
{
    bool ok = true;
    double t0 = TickTime().asSecs();
    for (unsigned int i = 9999; i > 0; --i)
    {
        double t1 = TickTime().asSecs();
        if (t0 < t1)
        {
            t0 = t1;
            continue;
        }
        ok = false;
        break;
    }
    CPPUNIT_ASSERT(ok);
}


void MiscSuite::testTickTime01()
{
    bool ok = true;
    double t0 = TickTime().asMsecs();
    for (unsigned int i = 9999; i > 0; --i)
    {
        double t1 = TickTime().asMsecs();
        if (t0 < t1)
        {
            t0 = t1;
            continue;
        }
        ok = false;
        break;
    }
    CPPUNIT_ASSERT(ok);
}


void MiscSuite::testUtc00()
{
    bool ok = true;
    Utc t0;
    for (unsigned int i = 0; i < 32; ++i)
    {
        Thread::takeANap(1);
        Utc t1;
        if ((t0 < t1) && (t1 > t0))
        {
            t0 = t1;
            continue;
        }
        ok = false;
        break;
    }
    CPPUNIT_ASSERT(ok);

    Utc t1(t0);
    ok = (t1 == t0);
    CPPUNIT_ASSERT(ok);

    unsigned long long now64 = Utc::nowAsFiletime();
    t0 = Utc(now64);
    ok = (t0.toFiletime() == now64);
    CPPUNIT_ASSERT(ok);

    t0 = Utc(0U, 0U);
    t1 = Utc(Utc::ZERO);
    ok = (t1 == t0);
    CPPUNIT_ASSERT(ok);
}
