#include "appkit/Directory.hpp"
#include "appkit/TempDir.hpp"
#include "appkit/TempFile.hpp"
#include "syskit/Bom.hpp"
#include "syskit/MappedTxtFile.hpp"

#include "syskit-ut-pch.h"
#include "MappedTxtFileSuite.hpp"

using namespace appkit;
using namespace syskit;

typedef struct
{
    unsigned long /*Bom::type_e*/ type;
    const char* file;
} sample_t;

const sample_t SAMPLE[] =
{
    {Bom::None, "bomed-0.txt"},
    {Bom::Utf8, "bomed-8.txt"},
    {Bom::Utf16, "bomed-16.txt"},
    {Bom::Utf61, "bomed-61.txt"},
    {Bom::Utf32, "bomed-32.txt"},
    {Bom::Utf23, "bomed-23.txt"},
};

const unsigned long NUM_SAMPLES = sizeof(SAMPLE) / sizeof(SAMPLE[0]);


MappedTxtFileSuite::MappedTxtFileSuite()
{
}


MappedTxtFileSuite::~MappedTxtFileSuite()
{
}


//
// Non-ASCII path.
//
bool MappedTxtFileSuite::cb0a(void* arg, const Directory& parent, const String& childName)
{
    bool keepGoing = true;
    if (!parent.path().isAscii())
    {
        String path(parent.path());
        path += childName;
        MappedTxtFile file(path.widen());
        if (file.isOk() && (file.path() == path))
        {
            keepGoing = false;
        }
    }

    return keepGoing;
}


//
// Not a file.
//
void MappedTxtFileSuite::testCtor00()
{
    MappedTxtFile file0(L"/");
    bool ok = (!file0.isOk()) && (file0.imageSize() == 0);
    CPPUNIT_ASSERT(ok);

    unsigned long long size = 123;
    bool failIfExists = false;
    Bom bom(Bom::None);
    MappedTxtFile file1(L"/", size, failIfExists, bom);
    ok = (!file1.isOk()) && (file1.imageSize() == 0);
    CPPUNIT_ASSERT(ok);
}


void MappedTxtFileSuite::testCtor01()
{
    String path("../../../etc/122333.txt");
    MappedTxtFile file(path.widen());
    bool ok = (file.isOk() && (file.path() == path));
    CPPUNIT_ASSERT(ok);
    ok = (file.addrOf(0) == file.image());
    CPPUNIT_ASSERT(ok);

    unsigned long long imageSize = 0x12345678ULL;
    const unsigned char* image = file.image(imageSize);
    ok = ((imageSize == 6) && (memcmp(image, "122333", static_cast<unsigned long>(imageSize)) == 0));
    CPPUNIT_ASSERT(ok);

    Directory dir("../../../etc/");
    ok = (!dir.applyChildFirst(cb0a, 0));
    CPPUNIT_ASSERT(ok);
}


//
// Empty file.
//
void MappedTxtFileSuite::testCtor02()
{
    bool readOnly = true;
    bool skipBom = true;
    MappedTxtFile file(L"../../../etc/empty.txt", readOnly, skipBom);
    bool ok = (file.isOk() && (file.path() == String("../../../etc/empty.txt")));
    CPPUNIT_ASSERT(ok);

    unsigned long long imageSize = 0x12345678ULL;
    const unsigned char* image = file.image(imageSize);
    ok = ((imageSize == 0) && (image != 0));
    CPPUNIT_ASSERT(ok);
}


void MappedTxtFileSuite::testCtor03()
{
    bool readOnly = true;
    bool skipBom = true;
    MappedTxtFile in(L"../../../etc/KMLSample.kml", readOnly, skipBom);

    // Copy from in to out.
    String basename("KMLSample.kml");
    TempDir tempDir;
    TempFile tempFile(tempDir, basename);
    unsigned long long size = in.imageSize();
    bool failIfExists = true;
    Bom bom(Bom::None);
    MappedTxtFile* out = new MappedTxtFile(tempFile.path().widen(), size, failIfExists, bom);
    memcpy(out->image(), in.image(), static_cast<size_t>(size));
    bool ok = (in == *out);
    delete out;
    CPPUNIT_ASSERT(ok);

    // File already exists.
    out = new MappedTxtFile(tempFile.path().widen(), size, failIfExists, bom);
    ok = (!out->isOk());
    delete out;
    CPPUNIT_ASSERT(ok);
}


//
// BOMs.
//
void MappedTxtFileSuite::testCtor04()
{
    String path0("../../../etc/");
    String path(path0);
    path += SAMPLE[2].file;
    MappedTxtFile file0(path.widen(), true /*readOnly*/, false /*skipBom*/);
    String s0;
    String s1;
    s0.resetX(file0.image(), static_cast<size_t>(file0.imageSize()));

    bool ok = true;
    for (unsigned long i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        path = path0;
        path += r.file;
        MappedTxtFile file1(path.widen(), true /*readOnly*/, false /*skipBom*/);
        s1.resetX(file1.image(), static_cast<size_t>(file1.imageSize()));
        if ((s1 != s0) || (MappedTxtFile(path.widen(), true /*readOnly*/, true /*skipBom*/).bom().type() != r.type))
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
}


void MappedTxtFileSuite::testLoadFrom00()
{
    String basename("KMLSample.kml");
    TempDir tempDir;
    TempFile tempFile(tempDir, basename);

    // Populate a memory-mapped file using loadFrom().
    unsigned long long size = 0;
    bool failIfExists = true;
    Bom bom(Bom::None);
    MappedTxtFile* out = new MappedTxtFile(tempFile.path().widen(), size, failIfExists, bom);
    bool ok = out->loadFrom(L"../../../etc/KMLSample.kml");
    CPPUNIT_ASSERT(ok);

    // Resizing is a no-op if size does not change.
    ok = out->resize(out->size());
    CPPUNIT_ASSERT(ok);

    bool readOnly = true;
    bool skipBom = true;
    MappedTxtFile in(L"../../../etc/KMLSample.kml", readOnly, skipBom);
    ok = (in == *out);
    delete out;
    CPPUNIT_ASSERT(ok);

    readOnly = false;
    skipBom = false;
    ok = in.remap(tempFile.path().widen(), readOnly, skipBom);
    CPPUNIT_ASSERT(ok);
    ok = in.resize(0);
    CPPUNIT_ASSERT(ok);
}


void MappedTxtFileSuite::testLoadFrom01()
{
    bool readOnly = true;
    bool skipBom = false;
    MappedTxtFile file(L"../../../etc/122333.txt", readOnly, skipBom);
    bool ok = (!file.loadFrom(L"/"));
    CPPUNIT_ASSERT(ok);

    ok = (!file.loadFrom(L"../../../etc/KMLSample.kml"));
    CPPUNIT_ASSERT(ok);
}


void MappedTxtFileSuite::testSaveIn00()
{
    String basename("KMLSample.kml");
    TempDir tempDir;
    TempFile tempFile(tempDir, basename);

    bool readOnly = true;
    bool skipBom = true;
    MappedTxtFile in(L"../../../etc/KMLSample.kml", readOnly, skipBom);
    bool ok = in.saveIn(tempFile.path().widen());
    CPPUNIT_ASSERT(ok);

    ok = (!in.saveIn(L"/"));
    CPPUNIT_ASSERT(ok);
}
