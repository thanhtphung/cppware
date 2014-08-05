#include "appkit/DelimitedTxt.hpp"
#include "appkit/Directory.hpp"
#include "appkit/TempDir.hpp"
#include "appkit/crt.hpp"
#include "syskit/MappedTxtFile.hpp"
#include "syskit/RoZipped.hpp"
#include "syskit/Zipped.hpp"

#include "syskit-ut-pch.h"
#include "ZippedSuite.hpp"

using namespace appkit;
using namespace syskit;


ZippedSuite::ZippedSuite()
{
}


ZippedSuite::~ZippedSuite()
{
}


//
// Non-ASCII path.
//
bool ZippedSuite::cb0a(void* /*arg*/, const Directory& parent, const String& childName)
{
    bool keepGoing = true;
    if (!parent.path().isAscii())
    {
        DelimitedTxt txt(childName, false, '.');
        const char* ext;
        size_t length;
        if (txt.prev(ext, length) && (length == 3) && (memcmp(ext, "kmz", 3) == 0))
        {
            String path(parent.path());
            path += childName;
            RoZipped z(path.widen());
            if (z.isOk() && (z.path() == path) && (z.numItems() > 0))
            {
                keepGoing = false;
            }
        }
    }

    return keepGoing;
}


//
// Keep going with the extraction if things seem logical.
//
bool ZippedSuite::onProgress0(void* /*arg*/,
    unsigned int itemsExtracted,
    unsigned int itemsBeingExtracted,
    unsigned long long bytesExtracted,
    unsigned long long bytesBeingExtracted)
{
    bool keepGoing = (itemsExtracted <= itemsBeingExtracted) && (bytesExtracted <= bytesBeingExtracted);
    return keepGoing;
}


//
// Cancel the extraction immediately.
//
bool ZippedSuite::onProgress1(void* /*arg*/,
    unsigned int /*itemsExtracted*/,
    unsigned int /*itemsBeingExtracted*/,
    unsigned long long /*bytesExtracted*/,
    unsigned long long /*bytesBeingExtracted*/)
{
    bool keepGoing = false;
    return keepGoing;
}


//
// Cancel the extraction when almost done.
//
bool ZippedSuite::onProgress2(void* /*arg*/,
    unsigned int itemsExtracted,
    unsigned int itemsBeingExtracted,
    unsigned long long bytesExtracted,
    unsigned long long bytesBeingExtracted)
{
    bool keepGoing = (itemsExtracted < itemsBeingExtracted - 1) && (bytesExtracted <= bytesBeingExtracted);
    return keepGoing;
}


//
// Cancel the extraction when almost done.
//
bool ZippedSuite::onProgress3(void* arg,
    unsigned int itemsExtracted,
    unsigned int itemsBeingExtracted,
    unsigned long long /*bytesExtracted*/,
    unsigned long long /*bytesBeingExtracted*/)
{
    bool cancelNow = (itemsExtracted == itemsBeingExtracted - 1);
    if (cancelNow)
    {
        RoZipped* z = static_cast<RoZipped*>(arg);
        z->cancelExtraction();
    }

    bool keepGoing = true;
    return keepGoing;
}


void ZippedSuite::testCancel00()
{
    RoZipped z(L"../../../etc/KMLSample.kmz");

    z.monitorExtraction(onProgress1, &z);
    {
        bool deleteAllOnDestruction = true;
        TempDir tempDir(deleteAllOnDestruction);
        const wchar_t* item = z.item(0);
        bool ok = (!z.extract(tempDir.path().widen(), item)) && z.extractionCanceled();
        CPPUNIT_ASSERT(ok);
    }

    z.monitorExtraction(onProgress0, &z);
    bool deleteAllOnDestruction = true;
    TempDir tempDir(deleteAllOnDestruction);
    const wchar_t* item = z.item(0);
    bool ok = z.extract(tempDir.path().widen(), item);
    CPPUNIT_ASSERT(ok);
}


void ZippedSuite::testCancel01()
{
    RoZipped z(L"../../../etc/Shapes_samples.kmz");

    z.monitorExtraction(onProgress2, &z);
    {
        bool deleteAllOnDestruction = true;
        TempDir tempDir(deleteAllOnDestruction);
        bool flattenDirHierarchy = false;
        bool ok = (!z.extractAll(tempDir.path().widen(), flattenDirHierarchy)) && z.extractionCanceled();
        CPPUNIT_ASSERT(ok);
    }

    z.monitorExtraction(onProgress3, &z);
    bool deleteAllOnDestruction = true;
    TempDir tempDir(deleteAllOnDestruction);
    bool flattenDirHierarchy = false;
    bool ok = (!z.extractAll(tempDir.path().widen(), flattenDirHierarchy)) && z.extractionCanceled();
    CPPUNIT_ASSERT(ok);
}


//
// Empty file.
// Not a file.
//
void ZippedSuite::testCtor00()
{
    RoZipped z0(L"../../../etc/empty.txt");
    bool ok = ((!z0.isOk()) && (z0.path() == String(L"../../../etc/empty.txt")) && (z0.numItems() == 0));
    CPPUNIT_ASSERT(ok);

    RoZipped z1(L"../../../etc/");
    ok = ((!z1.isOk()) && (z1.path() == String(L"../../../etc/")) && (z1.numItems() == 0));
    CPPUNIT_ASSERT(ok);
}


//
// Non-empty non-zip file.
//
void ZippedSuite::testCtor01()
{
    RoZipped z0(L"../../../etc/122333.txt");
    bool ok = ((!z0.isOk()) && (z0.path() == String(L"../../../etc/122333.txt")) && (z0.numItems() == 0));
    CPPUNIT_ASSERT(ok);

    RoZipped z1(L"../../../etc/KMLSample.gz");
    ok = ((!z1.isOk()) && (z1.path() == String(L"../../../etc/KMLSample.gz")) && (z1.numItems() == 0));
    CPPUNIT_ASSERT(ok);
}


//
// One item zipped.
// With comments.
//
void ZippedSuite::testCtor02()
{
    RoZipped z(L"../../../etc/KMLSample.kmz");
    bool ok = (z.isOk() && (z.path() == String(L"../../../etc/KMLSample.kmz")) && (z.numItems() == 1));
    CPPUNIT_ASSERT(ok);
    ok = (String(z.comment()) == "KMZSample");
    CPPUNIT_ASSERT(ok);

    const wchar_t* item = z.item(0);
    ok = ((item != 0) && (item == String("KMLSample.kml")));
    CPPUNIT_ASSERT(ok);
    ok = (String(z.comment(0)) == "KMLSample");
    CPPUNIT_ASSERT(ok);

    bool deleteAllOnDestruction = true;
    TempDir tempDir(deleteAllOnDestruction);
    ok = z.extract(tempDir.path().widen(), item);
    CPPUNIT_ASSERT(ok);
    ok = MappedTxtFile(L"../../../etc/KMLSamples.kml") == MappedTxtFile((tempDir.path() + "KMLSample.kml").widen());
    CPPUNIT_ASSERT(ok);
}


//
// Multiple-item zipped.
// No comments.
//
void ZippedSuite::testCtor03()
{
    String path("../../../etc/Shapes_samples.kmz");
    RoZipped z0(path.widen());
    bool ok = (z0.isOk() && (z0.path() == path) && (z0.numItems() == 3));
    CPPUNIT_ASSERT(ok);
    ok = (*z0.comment() == 0);
    CPPUNIT_ASSERT(ok);
    unsigned long long size[3] = {z0.size(0), z0.size(1), z0.size(2)};

    bool deleteAllOnDestruction = true;
    TempDir tempDir0(deleteAllOnDestruction);
    bool flattenDirHierarchy = true;
    ok = z0.extractAll(tempDir0.path().widen(), flattenDirHierarchy);
    CPPUNIT_ASSERT(ok);
    String basename(z0.item(0));
    ok = MappedTxtFile(("../../../etc/" + basename).widen()) == MappedTxtFile((tempDir0.path() + basename).widen());
    CPPUNIT_ASSERT(ok);
    basename = z0.item(1);
    ok = MappedTxtFile(("../../../etc/" + basename).widen()) == MappedTxtFile((tempDir0.path() + basename).widen());
    CPPUNIT_ASSERT(ok);
    basename = z0.item(2);
    ok = MappedTxtFile(("../../../etc/" + basename).widen()) == MappedTxtFile((tempDir0.path() + basename).widen());
    CPPUNIT_ASSERT(ok);

    TempDir tempDir1(deleteAllOnDestruction);
    path = tempDir1.path() + "Shapes_samples0.kmz";
    {
        Zipped z1(path.widen());
        MappedFile sibling(tempDir0.path().widen());
        ok = z1.isOk() && z1.addDirectory(sibling) && z1.save("");
        CPPUNIT_ASSERT(ok);
        RoZipped z2(path.widen());
        ok = z2.isOk() && (z2.numItems() == 3) && (z2.size(0) == size[0]) && (z2.size(1) == size[1]) && (z2.size(2) == size[2]);
        CPPUNIT_ASSERT(ok);
    }

    path = tempDir1.path() + "Shapes_samples1.kmz";
    {
        Zipped z1(path.widen(), 1 /*mapSize*/);
        String::W inDir(tempDir0.path().widen());
        ok = z1.isOk() && z1.addDirectory(inDir) && z1.save("");
        CPPUNIT_ASSERT(ok);
        RoZipped z2(path.widen(), 1 /*mapSize*/);
        ok = z2.isOk() && (z2.numItems() == 3) && (z2.size(0) == size[0]) && (z2.size(1) == size[1]) && (z2.size(2) == size[2]);
        CPPUNIT_ASSERT(ok);
    }

#if 0
    path = tempDir1.path() + "StreetTest.qzp";
    {
        Zipped z1(path.widen(), 1 /*mapSize*/);
        ok = z1.isOk() && z1.addDirectory(tempDir0.path().widen()) && z1.save("");
        CPPUNIT_ASSERT(ok);
        RoZipped z2(path.widen(), 1 /*mapSize*/);
        ok = z2.isOk() && (z2.numItems() == 3) && (z2.size(0) == size[0]) && (z2.size(1) == size[1]) && (z2.size(2) == size[2]);
        CPPUNIT_ASSERT(ok);
    }
#endif
}


//
// Non-ASCII path.
//
void ZippedSuite::testCtor04()
{
    Directory dir("../../../etc/");
    bool ok = (!dir.applyChildFirst(cb0a, 0));
    CPPUNIT_ASSERT(ok);
}


void ZippedSuite::testCtor05()
{
    String path("../../../etc/StreetTest.qzp");
    RoZipped z0(path.widen(), 1 /*mapSize*/);
    bool ok = (z0.isOk() && (z0.path() == path) && (z0.numItems() == 26));
    CPPUNIT_ASSERT(ok);

    unsigned long long size[26] =
    {
        98, 4875, 84, 172, 933, 495, 100, 697, 43473, 56, 1048632, 16178052, 1048632,
        11375, 11375, 94148, 11375, 11375, 1048632, 76, 2692, 49, 1835, 10203, 10203, 10203
    };

    for (int i = 0; i < 26; ++i)
    {
        if (z0.size(i) != size[i])
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    bool deleteAllOnDestruction = true;
    TempDir tempDir0(deleteAllOnDestruction);
    bool flattenDirHierarchy = false;
    ok = z0.extractAll(tempDir0.path().widen(), flattenDirHierarchy);
    CPPUNIT_ASSERT(ok);

    Directory dir(tempDir0.path());
    StringVec childVec;
    dir.list(childVec);
    for (unsigned int i = 0, numChildren = childVec.numItems(); i < numChildren; ++i)
    {
    }
#if 0
    ok = (*z0.comment() == 0);
    CPPUNIT_ASSERT(ok);
    unsigned long long size[3] = {z0.size(0), z0.size(1), z0.size(2)};

    bool deleteAllOnDestruction = true;
    TempDir tempDir0(deleteAllOnDestruction);
    bool flattenDirHierarchy = true;
    ok = z0.extractAll(tempDir0.path().widen(), flattenDirHierarchy);
    CPPUNIT_ASSERT(ok);
    String basename(z0.item(0));
    ok = MappedTxtFile(("../../../etc/" + basename).widen()) == MappedTxtFile((tempDir0.path() + basename).widen());
    CPPUNIT_ASSERT(ok);
    basename = z0.item(1);
    ok = MappedTxtFile(("../../../etc/" + basename).widen()) == MappedTxtFile((tempDir0.path() + basename).widen());
    CPPUNIT_ASSERT(ok);
    basename = z0.item(2);
#endif
}
