#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "appkit/Directory.hpp"
#include "syskit/MappedFile.hpp"
#include "syskit/MappedTxtFile.hpp"

#include "syskit-ut-pch.h"
#include "MappedFileSuite.hpp"

using namespace appkit;
using namespace syskit;


MappedFileSuite::MappedFileSuite()
{
}


MappedFileSuite::~MappedFileSuite()
{
}


//
// Non-ASCII path.
//
bool MappedFileSuite::cb0a(void* arg, const Directory& parent, const String& childName)
{
    bool keepGoing = true;
    if ((!Directory::nameIsDir(childName)) && (!parent.path().isAscii()))
    {
        String path(parent.path());
        path += childName;
        MappedFile file(path.widen(), true /*readOnly*/, 0U /*mapSize*/);
        if (file.isOk() && (file.path() == path))
        {
            keepGoing = false;
        }
    }

    return keepGoing;
}


void MappedFileSuite::cb1a(void* arg, const Directory& parent, const String& childName)
{
    if (Directory::nameIsDir(childName))
    {
        Directory dir(parent.path() + childName);
        dir.apply(cb1a, arg);
        return;
    }

    String path(parent.path());
    path += childName;
    bool readOnly = true;
    MappedFile file0(path.widen(), readOnly, 1U /*mapSize*/);
    if ((!file0.isOk()) || (file0.mapSize() <= 1) || (file0.addrOf(0) == 0))
    {
        bool* failed = static_cast<bool*>(arg);
        *failed = true;
    }

    // First and last byte should be readable.
    // File contents should be identical regardless of map size.
    if (file0.size() > 0)
    {
        size_t fullSize = static_cast<size_t>(file0.size());
        size_t partialSize = (file0.mapSize() <= file0.size())? static_cast<size_t>(file0.mapSize()): fullSize;
        unsigned char sum = 0;
        sum += *file0.addrOf(0);
        sum += *file0.addrOf(fullSize - 1);
        unsigned char* full = new unsigned char[fullSize];
        unsigned char* partial = new unsigned char[partialSize];
        unsigned long long offset = 0;
        file0.getBytes(full, offset, fullSize);
        file0.getBytes(partial, offset, partialSize);
        bool skipBom = false;
        MappedTxtFile file1(path.widen(), readOnly, skipBom);
        if (memcmp(file1.image(), full, fullSize) || memcmp(full, partial, partialSize))
        {
            bool* failed = static_cast<bool*>(arg);
            *failed = true;
        }
        delete[] partial;
        delete[] full;
    }
}


//
// Not a file.
//
void MappedFileSuite::testCtor00()
{
    String path("/");
    MappedFile file(path.widen(), 0U /*mapSize*/);
    bool ok = (!file.isOk()) && (file.path() == path);
    CPPUNIT_ASSERT(ok);

    ok = (file.handle() == INVALID_HANDLE_VALUE) && (file.mapSize() == 0) && (file.numMaps() == 0);
    CPPUNIT_ASSERT(ok);

    ok = (file.size() == 0);
    CPPUNIT_ASSERT(ok);
}


void MappedFileSuite::testCtor01()
{
    String path("../../../etc/122333.txt");
    MappedFile file(path.widen(), true /*readOnly*/, 0U /*mapSize*/);
    bool ok = file.isOk() && (file.path() == path);
    CPPUNIT_ASSERT(ok);

    unsigned long long size = file.size();
    const unsigned char* image = file.map(0);
    ok = (size == 6) && (memcmp(image, "122333", static_cast<size_t>(size)) == 0);
    CPPUNIT_ASSERT(ok);

    Directory dir("../../../etc/");
    ok = (!dir.applyChildFirst(cb0a, 0));
    CPPUNIT_ASSERT(ok);

    bool failed = false;
    dir.apply(cb1a, &failed);
    ok = (!failed);
    CPPUNIT_ASSERT(ok);
}


//
// Empty file.
//
void MappedFileSuite::testCtor02()
{
    String path("../../../etc/empty.txt");
    MappedFile file(path.widen(), true /*readOnly*/, 1U /*mapSize*/);
    bool ok = file.isOk() && (file.path() == path) && (file.mapSize() > 1U);
    CPPUNIT_ASSERT(ok);

    // Read-only file cannot be resized.
    ok = (!file.resize(123));
    CPPUNIT_ASSERT(ok);

    const unsigned char* image = file.map();
    unsigned long long size = file.size();
    ok = (image != 0) && (size == 0);
    CPPUNIT_ASSERT(ok);
}
