#include <errno.h>
#include "appkit/Directory.hpp"
#include "appkit/String.hpp"
#include "appkit/TempDir.hpp"
#include "appkit/TempFile.hpp"
#include "appkit/Uword.hpp"
#include "appkit/crt.hpp"
#include "syskit/MappedTxtFile.hpp"
#include "syskit/Process.hpp"

#include "appkit-ut-pch.h"
#include "CrtSuite.hpp"

using namespace appkit;
using namespace syskit;


CrtSuite::CrtSuite()
{
}


CrtSuite::~CrtSuite()
{
}


bool CrtSuite::doOpen(void* /*arg*/, const Directory& parent, const String& childName)
{
    bool keepGoing;
    const String MODE("rb");
    if (Directory::nameIsDir(childName))
    {
        String path(parent.path());
        path += childName;
        FILE* file = fopen(path, MODE);
        bool ok = (file != 0);
        CPPUNIT_ASSERT(ok);
        fclose(file);
        keepGoing = path.isAscii();
    }
    else
    {
        keepGoing = false;
    }

    return keepGoing;
}


//
// int access(const String& path, int mode);
//
void CrtSuite::testAccess00()
{
    String srcPath("../../../etc/non-existent.txt");
    bool ok = ((access(srcPath, 00) != 0) && (errno == ENOENT));
    CPPUNIT_ASSERT(ok);

    // Unsupported mode.
    srcPath = "../../../etc/empty.txt";
    ok = ((access(srcPath, 01) != 0) && (errno == EINVAL));
    CPPUNIT_ASSERT(ok);

    // Existent?
    ok = ((access(srcPath, 00) == 0) && (errno == 0));
    CPPUNIT_ASSERT(ok);

    // Writable?
    TempDir temp;
    TempFile dst(temp, "empty.txt");
    copy(srcPath, dst.path());
    chmod(dst.path(), 02);
    ok = ((access(dst.path(), 02) == 0) && (errno == 0));
    CPPUNIT_ASSERT(ok);

    // Read-only?
    chmod(dst.path(), 00);
    ok = ((access(dst.path(), 02) != 0) && (errno == EACCES));
    CPPUNIT_ASSERT(ok);
    ok = ((access(dst.path(), 00) == 0) && (errno == 0));
    CPPUNIT_ASSERT(ok);
}


//
// int chdir(const String& path);
//
void CrtSuite::testChdir00()
{
    Directory current = Directory::current();

    String path("./not-a-dir");
    bool ok = ((chdir(path) != 0) && (errno == ENOENT));
    CPPUNIT_ASSERT(ok);

    path = "";
    ok = ((chdir(path) != 0) && (errno == EINVAL));
    CPPUNIT_ASSERT(ok);

    path = "../../../etc/";
    ok = ((chdir(path) == 0) && (errno == 0));
    CPPUNIT_ASSERT(ok);

    Directory::setCurrent(current.path());
}


//
// int chmod(const String& path, int mode);
//
void CrtSuite::testChmod00()
{
    String path("../../../etc/non-existent.txt");
    bool ok = ((chmod(path, 00) != 0) && (errno == ENOENT));
    CPPUNIT_ASSERT(ok);

    // Unsupported mode.
    path = "../../../etc/empty.txt";
    ok = ((chmod(path, 01) != 0) && (errno == EINVAL));
    CPPUNIT_ASSERT(ok);
}


//
// int copy(const String& srcPath, const String& dstPath, bool failIfExists);
//
void CrtSuite::testCopy00()
{

    // Invalid source.
    String srcPath("../../../etc/src-non-existent.txt");
    String tempDir(Directory::getTemp());
    String dstPath(tempDir);
    bool ok = ((copy(srcPath, dstPath) != 0) && (errno == ENOENT));
    CPPUNIT_ASSERT(ok);

    // Invalid destination.
    srcPath = "../../../etc/122333.txt";
    dstPath += "...";
    ok = ((copy(srcPath, dstPath) != 0) && (errno != 0));
    CPPUNIT_ASSERT(ok);

    srcPath = "../../../etc/122333.txt";
    dstPath = tempDir;
    dstPath += "122333.txt";
    ok = (copy(srcPath, dstPath, true /*failIfExists*/) == 0);
    CPPUNIT_ASSERT(ok);
    ok = ((copy(srcPath, dstPath, true /*failIfExists*/) != 0) && (errno == EEXIST));
    CPPUNIT_ASSERT(ok);
    ok = (MappedTxtFile(srcPath.widen()) == MappedTxtFile(dstPath.widen()));
    CPPUNIT_ASSERT(ok);
    chmod(dstPath, 02);
    ok = (unlink(dstPath) == 0);
    CPPUNIT_ASSERT(ok);
}


void CrtSuite::testEnv00()
{
    const wchar_t SOME_K0[] = L"--aNewK";
    const wchar_t SOME_K1[] = L"__yaNewK";
    const wchar_t SOME_V0[] = L"%__yaNewK%someV%__yaNewK%%__notAKey%--ttp-1.0";
#define SOME_V1  L"\xa9--ttp-2.0"
    const wchar_t EXPANDED_V0[] = SOME_V1 L"someV" SOME_V1 L"%__notAKey%--ttp-1.0";

    String k(SOME_K0);
    String v;
    bool ok = (!getenv(k, v)) && (!putenv(k, 0));
    CPPUNIT_ASSERT(ok);

    v = SOME_V0;
    ok = putenv(k, &v);
    CPPUNIT_ASSERT(ok);
    k = SOME_K1;
    v = SOME_V1;
    ok = putenv(k, &v);
    CPPUNIT_ASSERT(ok);

    k = SOME_K0;
    v.reset();
    ok = getenv(k, v, false /*expandEnv*/) && (v == SOME_V0);
    CPPUNIT_ASSERT(ok);
    ok = getenv(k, v, true /*expandEnv*/) && (v == EXPANDED_V0);
    CPPUNIT_ASSERT(ok);
    k = SOME_K1;
    v.reset();
    ok = getenv(k, v) && (v == SOME_V1);
    CPPUNIT_ASSERT(ok);

    k = SOME_K0;
    v = SOME_V0;
    ok = putenv(k, 0) && (!getenv(k, v)) && v.empty();
    CPPUNIT_ASSERT(ok);
    k = SOME_K1;
    v = SOME_V1;
    ok = putenv(k, 0) && (!getenv(k, v)) && v.empty();
    CPPUNIT_ASSERT(ok);
}


//
// FILE* fopen(const String& path, const String& mode);
//
void CrtSuite::testFopen00()
{
    Directory dir("../../../etc/");
    dir.applyChildFirst(doOpen, 0);
}


//
// int link(const String& oldPath, const String& newPath);
//
void CrtSuite::testLink00()
{

    // Invalid source.
    String oldPath("../../../etc/src-non-existent.txt");
    String tempDir(Directory::getTemp());
    String newPath(tempDir);
    bool ok = ((link(oldPath, newPath) != 0) && (errno == ENOENT));
    CPPUNIT_ASSERT(ok);

    // Invalid destination.
    oldPath = "../../../etc/122333.txt";
    newPath += ".......";
    ok = ((link(oldPath, newPath) != 0) && (errno != 0));
    CPPUNIT_ASSERT(ok);

    oldPath = "../../../etc/122333.txt";
    newPath = tempDir;
    newPath += "122333.txt";
    ok = (link(oldPath, newPath) == 0) || (errno == EXDEV);
    CPPUNIT_ASSERT(ok);
    ok = (link(oldPath, newPath) != 0) && ((errno == EEXIST) || (errno == EXDEV));
    CPPUNIT_ASSERT(ok);
    ok = (errno == EXDEV) || ((MappedTxtFile(oldPath.widen()) == MappedTxtFile(newPath.widen())));
    CPPUNIT_ASSERT(ok);
    chmod(newPath, 02);
    ok = (unlink(newPath) == 0) || (errno == ENOENT);
    CPPUNIT_ASSERT(ok);
}


//
// int rename(const String& oldPath, const String& newPath);
//
void CrtSuite::testRename00()
{
    String dirName("../tmp/");
    mkdir(dirName);
    bool ok = ((mkdir(dirName) != 0) && (errno == EEXIST));
    CPPUNIT_ASSERT(ok);

    char name[Uword::MaxDigits + 1 + 1];
    size_t myId = Process::myId();
    size_t i = Uword::toDigits(myId, name);
    name[i] = 'a';
    name[i + 1] = 0;
    String srcName(dirName);
    srcName += name;
    const String MODE("wb");
    FILE* file = fopen(srcName, MODE);
    fclose(file);

    name[i] = 'b';
    String dstName(dirName);
    dstName += name;
    ok = ((rename(srcName, dstName) == 0) && (unlink(dstName) == 0));
    CPPUNIT_ASSERT(ok);
    ok = ((rename(srcName, dstName) != 0) && (errno == ENOENT));
    CPPUNIT_ASSERT(ok);

    rmdir(dirName);
    ok = ((rmdir(dirName) != 0) && (errno == ENOENT));
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - int sprintf(String& s, const char* format, ...);
// - int swprintf(String& s, const wchar_t* format, ...);
//
void CrtSuite::testSprintf00()
{
    String s;
    int length = sprintf(s, "%u:%08x:%s", 1234, 0x56788765, "4321");
    bool ok = ((length == 18) && (s == "1234:56788765:4321"));
    CPPUNIT_ASSERT(ok);

    length = swprintf(s, L"%u:%08x:%s", 8765, 0x43211234, L"5678");
    ok = ((length == 18) && (s == L"8765:43211234:5678"));
    CPPUNIT_ASSERT(ok);
}
