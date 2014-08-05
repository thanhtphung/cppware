#include "appkit/Path.hpp"
#include "appkit/Str.hpp"
#include "appkit/String.hpp"
#include "appkit/StringVec.hpp"
#include "appkit/UwordSet.hpp"
#include "syskit/Module.hpp"
#include "syskit/Process.hpp"
#include "syskit/Utc.hpp"

#include "syskit-ut-pch.h"
#include "ProcessSuite.hpp"

using namespace appkit;
using namespace syskit;

static bool exeIsPurified(const StringVec& modules)
{

    bool isPurified = false;
    for (unsigned long i = modules.numItems(); i > 0;)
    {
        const String& module = modules.peek(--i);
        if (module.endsWith(".exe", true /*ignoreCase*/))
        {
            isPurified = (module.find('$') != String::INVALID_INDEX);
            break;
        }
    }

    return isPurified;
}


ProcessSuite::ProcessSuite()
{
}


ProcessSuite::~ProcessSuite()
{
}


bool ProcessSuite::cb0a(void* arg, const Module& module)
{
    bool skipNormalization = true;
    Path path(module.path(), skipNormalization);
    StringVec* modules = static_cast<StringVec*>(arg);
    modules->add(path.basename());

    bool keepGoing = true;
    return keepGoing;
}


bool ProcessSuite::cb0b(void*, const Module& module)
{
    bool skipNormalization = true;
    Path path(module.path(), skipNormalization);
    bool keepGoing = (Str::compareKI(path.basename().ascii(), "version.dll") != 0);
    return keepGoing;
}


bool ProcessSuite::cb1a(void* arg, const Process& process)
{
    UwordSet* pids = static_cast<UwordSet*>(arg);
    bool keepGoing = pids->add(process.id());
    return keepGoing;
}


bool ProcessSuite::cb1b(void* /*arg*/, const Process& process)
{
    bool keepGoing = (process.id() != Process::myId());
    return keepGoing;
}


void ProcessSuite::testAffinityMask00()
{
    size_t affinityMask = Process::affinityMask();
    bool ok = (affinityMask != 0);
    CPPUNIT_ASSERT(ok);

    ok = Process::setAffinityMask(affinityMask);
    CPPUNIT_ASSERT(ok);
}


void ProcessSuite::testApply00()
{
    UwordSet pids;
    unsigned long maxPids = 4096;
    bool ok = Process::apply(maxPids, cb1a, &pids) && pids.contains(Process::myId());
    CPPUNIT_ASSERT(ok);

    ok = (!Process::apply(maxPids, cb1b, 0));
    CPPUNIT_ASSERT(ok);

    pids.reset();
    maxPids = 3;
    ok = Process::apply(maxPids, cb1a, &pids) && (pids.numKeys() == 3);
    CPPUNIT_ASSERT(ok);
}


void ProcessSuite::testCtor00()
{
    Process process(Process::myId());
    const wchar_t* user = process.user();
    bool ok = (user != 0) && (*user != 0);
    CPPUNIT_ASSERT(ok);
    delete[] user;

    StringVec modules;
    ok = process.apply(cb0a, &modules);
    CPPUNIT_ASSERT(ok);

    // Run the test, but ignore the result if being purified.
    modules.sort(String::comparePI);
    String k0("kernel32.dll");
    String k1("version.dll");
    size_t foundIndex;
    bool purified = exeIsPurified(modules);
    ok = (modules.search(String::comparePI, k0, foundIndex) && modules.search(String::comparePI, k1, foundIndex)) || purified;
    CPPUNIT_ASSERT(ok);

    // Run the test, but ignore the result if being purified.
    ok = (!process.apply(cb0b, 0)) || purified;
    CPPUNIT_ASSERT(ok);
}


void ProcessSuite::testMyId00()
{
    bool ok = (Process::myId() != 0);
    CPPUNIT_ASSERT(ok);
}


void ProcessSuite::testStartTime00()
{
    Utc now;
    Utc startTime(Process::startTime());
    bool ok = (startTime < now);
    CPPUNIT_ASSERT(ok);
}
