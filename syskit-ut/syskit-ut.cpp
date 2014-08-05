//
// Perform syskit unit tests.
//
#include "appkit/UnitTestBed.hpp"
#include "syskit/sys.hpp"

#include "syskit-ut-pch.h"
#include "Atomic32Suite.hpp"
#include "Atomic64Suite.hpp"
#include "BitVec32Suite.hpp"
#include "BitVec64Suite.hpp"
#include "BitVecSuite.hpp"
#include "BomSuite.hpp"
#include "BstSuite.hpp"
#include "BufArenaSuite.hpp"
#include "CriSectionSuite.hpp"
#include "D64HeapSuite.hpp"
#include "D64VecSuite.hpp"
#include "F32HeapSuite.hpp"
#include "F32VecSuite.hpp"
#include "FifoSuite.hpp"
#include "GrowableSuite.hpp"
#include "HashTableSuite.hpp"
#include "HeapSuite.hpp"
#include "HeapXSuite.hpp"
#include "ItemQSuite.hpp"
#include "LifoSuite.hpp"
#include "MappedFileSuite.hpp"
#include "MappedTxtFileSuite.hpp"
#include "MiscSuite.hpp"
#include "ProcessSuite.hpp"
#include "PrimeSuite.hpp"
#include "RefCountedSuite.hpp"
#include "SemaphoreSuite.hpp"
#include "ShmSuite.hpp"
#include "SpinSectionSuite.hpp"
#include "ThreadSuite.hpp"
#include "TreeSuite.hpp"
#include "TrieSuite.hpp"
#include "U16HeapSuite.hpp"
#include "U16VecSuite.hpp"
#include "U32HeapSuite.hpp"
#include "U32VecSuite.hpp"
#include "U64HeapSuite.hpp"
#include "U64VecSuite.hpp"
#include "Utf16Suite.hpp"
#include "Utf16SeqSuite.hpp"
#include "Utf8SeqSuite.hpp"
#include "Utf8Suite.hpp"
#include "VecSuite.hpp"
#include "ZippedSuite.hpp"

using namespace appkit;
using namespace syskit;

CPPUNIT_TEST_SUITE_REGISTRATION(Atomic32Suite);
CPPUNIT_TEST_SUITE_REGISTRATION(Atomic64Suite);
CPPUNIT_TEST_SUITE_REGISTRATION(BitVec32Suite);
CPPUNIT_TEST_SUITE_REGISTRATION(BitVec64Suite);
CPPUNIT_TEST_SUITE_REGISTRATION(BitVecSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(BomSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(BstSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(BufArenaSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(CriSectionSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(D64HeapSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(D64VecSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(F32HeapSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(F32VecSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(FifoSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(GrowableSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(HashTableSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(HeapSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(HeapXSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(ItemQSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(LifoSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(MappedFileSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(MappedTxtFileSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(MiscSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(PrimeSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(ProcessSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(RefCountedSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(SemaphoreSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(ShmSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(SpinSectionSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(ThreadSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(TreeSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(TrieSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(U16HeapSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(U16VecSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(U32HeapSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(U32VecSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(U64HeapSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(U64VecSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(Utf16SeqSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(Utf16Suite);
CPPUNIT_TEST_SUITE_REGISTRATION(Utf8SeqSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(Utf8Suite);
CPPUNIT_TEST_SUITE_REGISTRATION(VecSuite);
CPPUNIT_TEST_SUITE_REGISTRATION(ZippedSuite);

int wmain()
{
    UnitTestBed unitTestBed;
    bool ok = unitTestBed.runAll();
    return ok? 0: 1;
}
