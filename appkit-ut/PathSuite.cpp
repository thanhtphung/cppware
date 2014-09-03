#include "appkit/DelimitedTxt.hpp"
#include "appkit/Directory.hpp"
#include "appkit/Path.hpp"

#include "appkit-ut-pch.h"
#include "PathSuite.hpp"

using namespace appkit;

typedef struct
{
    const char* path;
    const char* normalized;
    bool isAbsolute;
    bool isDir;
    char drive;
    size_t baseI;
    size_t baseN;
    size_t basenameI;
    size_t basenameN;
    size_t dirI;
    size_t dirN;
    size_t extI;
    size_t extN;
    size_t serverI;
    size_t serverN;
} sample_t;

const sample_t SAMPLE[] =
{
    {"D:/ef/ghi/jklm.nop", "D:/ef/ghi/jklm.nop", true, false, 'D', 10, 4, 10, 8, 0, 10, 15, 3, 0, 0},
    {"D:/ef/ghi/jklm-nop", "D:/ef/ghi/jklm-nop", true, false, 'D', 10, 8, 10, 8, 0, 10, 0, 0, 0, 0},
    {"dirname\\abc\\", "dirname/abc/", false, true, 0, 8, 4, 8, 4, 0, 8, 0, 0, 0, 0},
    {"c:\\ef\\ghi\\jklm.nop", "c:/ef/ghi/jklm.nop", true, false, 'c', 10, 4, 10, 8, 0, 10, 15, 3, 0, 0},
    {"\\\\server\\xyz.1", "//server/xyz.1", true, false, 0, 9, 3, 9, 5, 0, 9, 13, 1, 2, 6},
    {"file:///C:/oce.pdf", "C:/oce.pdf", true, false, 'C', 3, 3, 3, 7, 0, 3, 7, 3, 0, 0}
};

const size_t NUM_SAMPLES = sizeof(SAMPLE) / sizeof(SAMPLE[0]);

static bool validate(const Path& path, const sample_t& r)
{
    return ((path.asString() == r.normalized) &&
        (path.isDir() == r.isDir) &&
        (path.drive() == r.drive) &&
        (path.base() == String(r.normalized + r.baseI, r.baseN)) &&
        (path.basename() == String(r.normalized + r.basenameI, r.basenameN)) &&
        (path.dirname() == String(r.normalized + r.dirI, r.dirN)) &&
        (path.extension() == String(r.normalized + r.extI, r.extN)) &&
        (path.isAbsolute() == r.isAbsolute) &&
        (path.server() == String(r.normalized + r.serverI, r.serverN)));
}


PathSuite::PathSuite()
{
}


PathSuite::~PathSuite()
{
}


//
// Path(const String&, bool);
//
void PathSuite::testCtor00()
{
    bool ok = true;
    for (unsigned int i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        bool skipNormalization = (strcmp(r.path, r.normalized) == 0);
        Path path(String(r.path), skipNormalization);
        if (!validate(path, r))
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
}


//
// Path(const char*, bool);
//
void PathSuite::testCtor01()
{
    bool ok = true;
    for (unsigned int i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        bool skipNormalization = (strcmp(r.path, r.normalized) == 0);
        Path path(r.path, skipNormalization);
        if (!validate(path, r))
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
}


//
// Path(const char*, size_t, bool);
//
void PathSuite::testCtor02()
{
    bool ok = true;
    for (unsigned int i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        bool skipNormalization = (strcmp(r.path, r.normalized) == 0);
        Path path(r.path, strlen(r.path), skipNormalization);
        if (!validate(path, r))
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
}


//
// Non-ASCII directory names.
//
void PathSuite::testCtor04()
{
    Directory dir("../etc/");
    String files;
    String subdirs;
    dir.list(files, subdirs);

    DelimitedTxt txt(subdirs, false, Directory::DELIM);
    String subdir;
    String dirname;
    String pathname;
    bool ok = true;
    while (txt.next(subdir))
    {
        txt.trimLine(subdir);
        dirname = dir.path();
        dirname += subdir;
        pathname = dirname;
        pathname += "filename.ext";
        Path path(pathname);
        if ((path.dirname() != dirname) || (path.basename() != "filename.ext"))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void PathSuite::testFullName00()
{
    Path path("c:/de2\\fff3\\gggg4/a/./bc/../bc/../bc///dd/../../bc/ee");
    bool beautify = true;
    bool ok = (path.fullName(beautify) == "c:/de2/fff3/gggg4/a/bc/ee");
    CPPUNIT_ASSERT(ok);

    path = "a:/bb/ccc/dddd/eeeee/.././../../";
    ok = path.fullName(beautify) == "a:/bb/";
    CPPUNIT_ASSERT(ok);

    path = "//xxx////bb/ccc/./././dddd/eeeee/.././../../";
    ok = path.fullName(beautify) == "//xxx/bb/";
    CPPUNIT_ASSERT(ok);

    beautify = false;
    path = "";
    String currentDir(Directory::getCurrent());
    ok = path.fullName(beautify) == currentDir;
    CPPUNIT_ASSERT(ok);
    path = "./";
    beautify = true;
    ok = path.fullName(beautify) == currentDir;
    CPPUNIT_ASSERT(ok);
}


void PathSuite::testOp00()
{
    Path path0("/://invalid-drive/");
    path0 = path0;
    Path path1(path0);
    path0 = path1;
    bool ok = (path0 == path1) && (path1.drive() == 0) && path1.isDir();
    CPPUNIT_ASSERT(ok);

    path0.reset("///");
    ok = path0.server().empty() && (!path0.isAbsolute());
    CPPUNIT_ASSERT(ok);
    path0.reset("a//bcd/efg");
    ok = path0.server().empty();
    CPPUNIT_ASSERT(ok);
    path0.reset("//server");
    ok = path0.server().empty();
    CPPUNIT_ASSERT(ok);
}


void PathSuite::testReset00()
{
    Path path0("c:/de2\\fff3\\gggg4\\");
    Path path1("");
    path1.reset("c:/de2/fff3/gggg4/");
    bool ok = (path1 == path0);
    CPPUNIT_ASSERT(ok);

    path0.reset("c:/de2/fff3/gggg4/hijk", 18, true /*skipNormalization*/);
    ok = (path1 == path0);
    CPPUNIT_ASSERT(ok);

    path1.reset("c:\\de2\\fff3\\gggg4\\");
    path0.reset(path1.asString());
    ok = (path1 == path0);
    CPPUNIT_ASSERT(ok);
}
