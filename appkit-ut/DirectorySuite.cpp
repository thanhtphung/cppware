#include "appkit/DelimitedTxt.hpp"
#include "appkit/Directory.hpp"
#include "appkit/Path.hpp"
#include "appkit/Str.hpp"

#include "appkit-ut-pch.h"
#include "DirectorySuite.hpp"

using namespace appkit;
using namespace syskit;


DirectorySuite::DirectorySuite()
{
}


DirectorySuite::~DirectorySuite()
{
}


bool DirectorySuite::cb0a(void* arg, const Directory& /*parent*/, const String& childName)
{
    String& children = *static_cast<String*>(arg);
    children += childName;
    children += Directory::DELIM;
    return true;
}


bool DirectorySuite::cb0b(void* arg, const Directory& /*parent*/, const String& /*childName*/)
{
    size_t& i = *static_cast<size_t*>(arg);
    return (i-- > 0);
}


void DirectorySuite::cb1a(void* arg, const Directory& /*parent*/, const String& childName)
{
    String& children = *static_cast<String*>(arg);
    children += childName;
    children += Directory::DELIM;
}


void DirectorySuite::testApply00()
{
    String children0;
    String children1;
    Directory dir("../../../etc/");
    dir.list(children0);
    bool ok = (dir.apply(cb0a, &children1) && (children0 == children1));
    CPPUNIT_ASSERT(ok);

    ok = (dir.applyChildFirst(cb0a, &children0) && dir.applyParentFirst(cb0a, &children1));
    CPPUNIT_ASSERT(ok);
    ok = ((children0.length() == children1.length()) && (children0.byteSize() == children1.byteSize()));
    CPPUNIT_ASSERT(ok);

    DelimitedTxt txt0(children0, false, Directory::DELIM);
    DelimitedTxt txt1(children1, false, Directory::DELIM);
    ok = (txt0.countLines() == txt1.countLines());
    CPPUNIT_ASSERT(ok);
}


void DirectorySuite::testApply01()
{
    String children0;
    String children1;
    Directory dir("../../../etc/");
    dir.list(children0);
    dir.apply(cb1a, &children1);
    bool ok = (children0 == children1);
    CPPUNIT_ASSERT(ok);

    dir.applyChildFirst(cb1a, &children0);
    dir.applyParentFirst(cb1a, &children1);
    ok = ((children0.length() == children1.length()) && (children0.byteSize() == children1.byteSize()));
    CPPUNIT_ASSERT(ok);

    DelimitedTxt txt0(children0, false, Directory::DELIM);
    DelimitedTxt txt1(children1, false, Directory::DELIM);
    ok = (txt0.countLines() == txt1.countLines());
    CPPUNIT_ASSERT(ok);
}


void DirectorySuite::testApply02()
{
    Directory dir(".../");
    Directory::cb0_t cb0 = 0;
    bool ok = (dir.apply(cb0, 0) && dir.applyChildFirst(cb0, 0) && dir.applyParentFirst(cb0, 0));
    CPPUNIT_ASSERT(ok);

    Directory::cb1_t cb1 = 0;
    dir.apply(cb1, 0);
    dir.applyChildFirst(cb1, 0);
    dir.applyParentFirst(cb1, 0);
    CPPUNIT_ASSERT(true);
}


void DirectorySuite::testApply03()
{
    Directory dir("../../../etc/");
    size_t i = 99999UL;
    bool ok = dir.apply(cb0b, &i);
    CPPUNIT_ASSERT(ok);
    size_t numEntries = 99999UL - i - 1;

    for (size_t i = numEntries; i > 0; --i)
    {
        size_t j = i;
        if (dir.apply(cb0b, &j))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void DirectorySuite::testApply04()
{
    Directory dir("../../../etc/");
    size_t i = 99999UL;
    bool ok = dir.applyChildFirst(cb0b, &i);
    CPPUNIT_ASSERT(ok);
    size_t numEntries = 99999UL - i - 1;

    for (size_t i = numEntries; i > 0; --i)
    {
        size_t j = i;
        size_t k = i;
        if (dir.applyChildFirst(cb0b, &j) || dir.applyParentFirst(cb0b, &k))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void DirectorySuite::testAttr00()
{
    Utc zero(0UL, 0UL);
    Directory::Attr attr0;
    bool ok = (!attr0.isDir()) &&
        (attr0.accessTime() == zero) &&
        (attr0.creationTime() == zero) &&
        (attr0.writeTime() == zero) &&
        (attr0.size() == 0);
    CPPUNIT_ASSERT(ok);

    Utc now;
    String childName("../../../etc");
    Directory current = Directory::current();
    ok = current.getChildAttr(attr0, childName) &&
        attr0.isDir() &&
        (attr0.creationTime() < now);
    CPPUNIT_ASSERT(ok);

    attr0.reset();
    childName = "../../../etc/122333.txt";
    ok = current.getChildAttr(attr0, childName) &&
        (!attr0.isDir()) &&
        (attr0.creationTime() < now) &&
        (attr0.size() == 6);
    CPPUNIT_ASSERT(ok);

    Directory::Attr attr1(attr0);
    ok = (attr1 == attr0);
    CPPUNIT_ASSERT(ok);

    bool isDir = attr0.isDir();
    unsigned long long time[3] = {attr0.accessTime().toFiletime(), attr0.creationTime().toFiletime(), attr0.writeTime().toFiletime()};
    unsigned long long size = attr0.size();
    Directory::Attr attr2(isDir, time, size);
    ok = (attr2 == attr0);
    CPPUNIT_ASSERT(ok);

    attr0.reset();
    childName.reset();
    String notAChild(".1..2...3");
    ok = (!current.getChildAttr(attr0, childName)) && (!current.getChildAttr(attr0, notAChild));;
    CPPUNIT_ASSERT(ok);

    attr1 = attr0;
    ok = (attr1 == attr0);
    CPPUNIT_ASSERT(ok);
}


void DirectorySuite::testCtor00()
{
    String path0("./");
    Directory dir0(path0);
    bool ok = (dir0.isOk() && (dir0.path() == "./"));
    CPPUNIT_ASSERT(ok);

    Directory dir1("./");
    ok = (dir1.isOk() && (dir1.path() == "./"));
    CPPUNIT_ASSERT(ok);

    Directory dir2("../", 2);
    ok = (dir2.isOk() && (dir2.path() == "../"));
    CPPUNIT_ASSERT(ok);
}


//
// Not a directory.
//
void
DirectorySuite::testCtor01()
{

    Directory dir0(".../");
    bool ok = ((!dir0.isOk()) && (dir0.path() == ".../"));
    CPPUNIT_ASSERT(ok);

    Directory dir1("../../../etc/empty.txt");
    ok = ((!dir1.isOk()) && (dir1.path() == "../../../etc/empty.txt"));
    CPPUNIT_ASSERT(ok);
}


void DirectorySuite::testCurrent00()
{
    Directory current = Directory::current();
    bool ok = (current.isOk() && (!current.path().empty()));
    CPPUNIT_ASSERT(ok);

    String dirName("../../../etc/");
    Directory dir(dirName);
    String files;
    String subdir;
    String subdirs;
    dir.list(files, subdirs);
    DelimitedTxt txt(subdirs, false, Directory::DELIM);
    for (; txt.next(subdir); Directory::setCurrent(current.path()))
    {
        String path(dir.path());
        path.append(subdir, 0, subdir.length() - 1);
        if (!Directory::setCurrent(path))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = Directory::setCurrent(current.path());
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - const String& Directory::find(String&, const char*);
// - const String& Directory::find(StringVec&, const char*);
//
void DirectorySuite::testFind00()
{
    String dirName("../../../etc/");
    String children;
    Directory current(Directory::current());
    bool ok = (&current.find(children, "Kmz") == &children);
    CPPUNIT_ASSERT(ok);
    StringVec childVec;
    ok = (&current.find(childVec, "kmz") == &childVec);
    CPPUNIT_ASSERT(ok);

    size_t kmzCount = 0;
    String childName;
    DelimitedTxt txt(children, false /*makeCopy*/, Directory::DELIM);
    ok = (childVec == StringVec(txt));
    CPPUNIT_ASSERT(ok);
    for (; txt.next(childName); ++kmzCount)
    {
        txt.trimLine(childName);
        Path path(childName);
        if (Str::compareKI(path.extension().ascii(), "KMZ") != 0)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    size_t okKmzCount = 0;
    current.list(children);
    txt.setTxt(children, false /*makeCopy*/);
    while (txt.next(childName))
    {
        txt.trimLine(childName);
        Path path(childName);
        String extension(path.extension());
        if (Str::compareKI(extension.ascii(), "kmz") == 0)
        {
            ++okKmzCount;
        }
    }
    ok = (kmzCount == okKmzCount);
    CPPUNIT_ASSERT(ok);
}


void DirectorySuite::testGetTemp00()
{
    String temp = Directory::getTemp();
    bool ok = (!temp.empty());
    CPPUNIT_ASSERT(ok);
}


void DirectorySuite::testHasChild00()
{
    String dirName("../../../etc/");
    String fileName("../../../etc");
    Directory current = Directory::current();
    bool ok = (current.hasChild(dirName) && (!current.hasChild(fileName)));
    CPPUNIT_ASSERT(ok);

    dirName.reset();
    ok = (!current.hasChild(dirName));
    CPPUNIT_ASSERT(ok);

    dirName = "../tmp/";
    ok = (!current.hasChild(dirName));
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - const String& Directory::list(String&);
// - const StringVec& Directory::list(StringVec&);
//
void DirectorySuite::testList00()
{
    Directory dir("../../../etc/");

    String children;
    StringVec childVec;
    bool ok = ((!dir.list(children).empty()) && (dir.list(childVec).numItems() > 0));
    CPPUNIT_ASSERT(ok);

    String childName;
    DelimitedTxt txt(children, false, Directory::DELIM);
    for (size_t i = 0; txt.next(childName); ++i)
    {
        txt.trimLine(childName);
        if ((!dir.hasChild(childName)) || (childName != childVec[i]))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - void Directory::list(String&, String&);
// - void Directory::list(StringVec&, StringVec&);
//
void DirectorySuite::testList01()
{
    Directory dir("../../../etc/");

    String files;
    String subdirs;
    dir.list(files, subdirs);
    bool ok = ((!files.empty()) && (!subdirs.empty()));
    CPPUNIT_ASSERT(ok);

    bool makeCopy = false;
    DelimitedTxt txt0(files, makeCopy, Directory::DELIM);
    DelimitedTxt txt1(subdirs, makeCopy, Directory::DELIM);
    StringVec fileVec0;
    StringVec subdirVec0;
    bool doTrimLine = true;
    txt0.vectorize(fileVec0, doTrimLine);
    txt1.vectorize(subdirVec0, doTrimLine);

    StringVec fileVec1;
    StringVec subdirVec1;
    dir.list(fileVec1, subdirVec1);
    ok = ((fileVec0 == fileVec1) && (subdirVec0 == subdirVec1));
    CPPUNIT_ASSERT(ok);
}


void DirectorySuite::testNormalizeSlashes00()
{
    String path;
    Directory::normalizeSlashes(path);
    bool ok = path.empty();
    CPPUNIT_ASSERT(ok);
}


void DirectorySuite::testOp00()
{
    Directory dir0 = Directory::current();
    Directory dir1("../");
    dir1 = dir0;
    dir0 = dir0;
    bool ok = (dir0.path() == dir1.path());
    CPPUNIT_ASSERT(ok);
}
