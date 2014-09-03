#include "appkit/DicFile.hpp"
#include "appkit/Directory.hpp"
#include "appkit/String.hpp"
#include "appkit/StringDic.hpp"
#include "appkit/TempDir.hpp"
#include "appkit/crt.hpp"
#include "syskit/Thread.hpp"

#include "appkit-ut-pch.h"
#include "DicFileSuite.hpp"

using namespace appkit;
using namespace syskit;


DicFileSuite::DicFileSuite()
{
}


DicFileSuite::~DicFileSuite()
{
}


//
// XML file w/ a /xxx/Dictionary/ element. Instance can be loaded into a dictionary
// once. Instance can be updated once.
//
void DicFileSuite::testCtor00()
{
    String srcPath("../../../etc/dic=Example.xml");
    String tempDir(Directory::getTemp());
    String dstPath(tempDir);
    dstPath += "dic=Example.xml";
    copy(srcPath, dstPath);

    bool ignoreCase = false;
    DicFile* dicFile = new DicFile(dstPath, ignoreCase);
    bool ok = dicFile->isOk();
    CPPUNIT_ASSERT(ok);

    StringDic* dic0 = dicFile->load();
    ok = (dic0->numKvPairs() > 0);
    CPPUNIT_ASSERT(ok);

    StringDic* dic1 = dicFile->load();
    ok = (dic1->numKvPairs() == 0); //already loaded once
    CPPUNIT_ASSERT(ok);
    delete dic1;

    delete dicFile;
    dicFile = new DicFile(dstPath, ignoreCase);
    ok = dicFile->save(*dic0) && (!dicFile->save(*dic0)); //save once only
    CPPUNIT_ASSERT(ok);

    delete dic0;
    delete dicFile;
}


//
// XML file w/o a /xxx/Dictionary/ element.
//
void DicFileSuite::testCtor01()
{
    String dicPath("../../../etc/good-sample.xml");
    bool ignoreCase = false;
    DicFile dicFile(dicPath, ignoreCase);
    bool ok = dicFile.isOk();
    CPPUNIT_ASSERT(ok);

    StringDic* dic0 = dicFile.load();
    ok = (dic0->numKvPairs() == 0); //no /xxx/Dictionary/ element
    CPPUNIT_ASSERT(ok);
    delete dic0;
}


void DicFileSuite::testCtor02()
{

    // Empty file.
    String path0("../../../etc/empty.txt");
    DicFile dicFile0(path0);
    bool ok = ((!dicFile0.isOk()) && (dicFile0.errDesc() != 0));
    CPPUNIT_ASSERT(ok);

    // Not a file.
    DicFile dicFile1("../../../etc/");
    ok = ((!dicFile1.isOk()) && (dicFile1.errDesc() != 0));
    CPPUNIT_ASSERT(ok);
}


void DicFileSuite::testSave00()
{
    String srcPath("../../../etc/dic=Example.xml");
    String tempDir(Directory::getTemp());
    String dstPath(tempDir);
    dstPath += "dic=Example.xml";
    copy(srcPath, dstPath);

    void* arg = &dstPath;
    unsigned int stackSizeInBytes = 0;
    bool startSuspended = true;
    Thread t0(entry00, arg, stackSizeInBytes, startSuspended);
    Thread t1(entry00, arg, stackSizeInBytes, startSuspended);
    Thread t2(entry00, arg, stackSizeInBytes, startSuspended);
    t0.resume();
    t1.resume();
    t2.resume();
    t0.waitTilDone();
    t1.waitTilDone();
    t2.waitTilDone();

    DicFile dicFile(dstPath);
    std::auto_ptr<StringDic> dic(dicFile.load());
    bool ok = ((*dic)["n0"] == "v1") && (*dic)["n0.a0"] == "v1";
    CPPUNIT_ASSERT(ok);
}


void* DicFileSuite::entry00(void* arg)
{
    const String* dicPath = static_cast<const String*>(arg);
    StringDic* dic;
    {
        DicFile dicFile(*dicPath);
        dic = dicFile.load();
    }

    (*dic)["n0"] = "v1";
    (*dic)["n0.a0"] = "v1";
    DicFile dicFile(*dicPath);
    dicFile.save(*dic);
    delete dic;
    return 0;
}
