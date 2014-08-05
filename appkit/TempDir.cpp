/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/Atomic32.hpp"
#include "syskit/Process.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/Directory.hpp"
#include "appkit/TempDir.hpp"
#include "appkit/Toeprint.hpp"
#include "appkit/U64.hpp"
#include "appkit/Uword.hpp"
#include "appkit/crt.hpp"

using namespace syskit;

BEGIN_NAMESPACE1(appkit)


TempDir::TempDir(bool deleteAllOnDestruction):
path_(mkMyDir())
{
    deleteAll_ = deleteAllOnDestruction;
}


TempDir::~TempDir()
{
    if (path_.length() > Directory::getTemp().length())
    {

        if (deleteAll_)
        {
            Directory dir(path_);
            dir.applyChildFirst(deleteAll, 0);
        }

        rmdir(path_);
    }
}


String TempDir::mkMyDir()
{
    static Atomic32 s_curInstance;

    String tail(1, '.');
    Uword uword(Process::myId());
    tail += uword.toString();
    tail += '-';
    uword = s_curInstance++;
    tail += uword.toString();
    tail += Directory::MARKER;

    String tempPath(Directory::getTemp());
    String myTempDir(tempPath);
    Toeprint toeprint(myTempDir, tail);
    myTempDir.append(toeprint, Toeprint::Length);
    U64 u64(Process::startTime().asNsecs());
    String head(u64.toString());
    toeprint.reset(head, myTempDir);
    myTempDir.append(toeprint, Toeprint::Length);

    myTempDir += tail;
    return (mkdir(myTempDir) == 0)? myTempDir: tempPath;
}


void TempDir::deleteAll(void* /*arg*/, const Directory& parent, const String& childName)
{
    String path(parent.path());
    path += childName;
    Directory::nameIsDir(childName)? rmdir(path): unlink(path);
}

END_NAMESPACE1
