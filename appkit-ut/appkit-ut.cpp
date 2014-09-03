//
// Perform appkit unit tests.
//
#include "appkit/App.hpp"
#include "appkit/UnitTestBed.hpp"
#include "netkit/Winsock.hpp"
#include "syskit/Singleton.hpp"
#include "syskit/sys.hpp"

#include "appkit-ut-pch.h"
#include "CmdLineSuite.hpp"
#include "CmdSuite.hpp"
#include "CrtSuite.hpp"
#include "CsvRowSuite.hpp"
#include "D64Suite.hpp"
#include "DelimitedTxtSuite.hpp"
#include "DicFileSuite.hpp"
#include "DirectorySuite.hpp"
#include "F32Suite.hpp"
#include "MiscSuite.hpp"
#include "ObserverSuite.hpp"
#include "PathSuite.hpp"
#include "QuotedStringSuite.hpp"
#include "S32Suite.hpp"
#include "StdSuite.hpp"
#include "StrSuite.hpp"
#include "StringDicSuite.hpp"
#include "StringSuite.hpp"
#include "StringVecSuite.hpp"
#include "TokenizerSuite.hpp"
#include "U16SetSuite.hpp"
#include "U16Suite.hpp"
#include "U32SetSuite.hpp"
#include "U32Suite.hpp"
#include "U64SetSuite.hpp"
#include "U64Suite.hpp"
#include "U8Suite.hpp"
#include "UnitTestBedSuite.hpp"
#include "XmlDocSuite.hpp"
#include "XmlElementSuite.hpp"
#include "XmlLexerSuite.hpp"

using namespace appkit;
using namespace syskit;

CPPUNIT_TEST_SUITE_REGISTRATION(CmdLineSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(CmdSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(CrtSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(CsvRowSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(D64Suite);
CPPUNIT_TEST_SUITE_REGISTRATION(DelimitedTxtSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(DicFileSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(DirectorySuite);
CPPUNIT_TEST_SUITE_REGISTRATION(F32Suite);
CPPUNIT_TEST_SUITE_REGISTRATION(MiscSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(ObserverSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(PathSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(QuotedStringSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(S32Suite);
CPPUNIT_TEST_SUITE_REGISTRATION(StdSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(StrSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(StringDicSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(StringSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(StringVecSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(TokenizerSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(U16SetSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(U16Suite);
CPPUNIT_TEST_SUITE_REGISTRATION(U32SetSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(U32Suite);
CPPUNIT_TEST_SUITE_REGISTRATION(U64SetSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(U64Suite);
CPPUNIT_TEST_SUITE_REGISTRATION(U8Suite);
CPPUNIT_TEST_SUITE_REGISTRATION(UnitTestBedSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(XmlDocSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(XmlElementSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(XmlLexerSuite);

using namespace appkit;
using namespace netkit;
using namespace syskit;

int wmain()
{
    Singleton::create_t create = 0;
    unsigned int initialRefCount = 0U;
    void* createArg = 0;
    App* app = App::instance(create, initialRefCount, createArg);
    RefCounted::Count lock(*app);

    Winsock winsock;
    UnitTestBed unitTestBed;
    bool ok = unitTestBed.runAll();
    return ok? 0: 1;
}
