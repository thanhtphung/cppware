#include "appkit/Directory.hpp"
#include "appkit/String.hpp"
#include "appkit/std.hpp"

#include "appkit-ut-pch.h"
#include "StdSuite.hpp"

using namespace appkit;


StdSuite::StdSuite()
{
}


StdSuite::~StdSuite()
{
}


//
// Try constructing an input file stream for each file seen.
// Keep going until a non-ASCII path is seen.
//
bool StdSuite::doCtor(void* /*arg*/, const Directory& parent, const String& childName)
{
    bool keepGoing;
    if (Directory::nameIsDir(childName))
    {
        String path(parent.path());
        path += childName;
        ifstream ifs(path);
        wifstream wifs(path);
        bool ok = ifs.good() && wifs.good();
        CPPUNIT_ASSERT(ok);
        keepGoing = path.isAscii();
    }
    else
    {
        keepGoing = false;
    }

    return keepGoing;
}


void StdSuite::testIfstream00()
{
    Directory dir("../etc/");
    dir.applyChildFirst(doCtor, 0);
}
