#include "appkit-ut-pch.h"
#include "BadSuite.hpp"


BadSuite::BadSuite()
{
}


BadSuite::~BadSuite()
{
}


void BadSuite::testException00()
{
    std::exception e;
    throw e;
}


void BadSuite::testFailure00()
{
    bool ok = false;
    CPPUNIT_ASSERT(ok);
}


void BadSuite::testFailure01()
{
    bool ok = false;
    CPPUNIT_ASSERT(ok);
}
