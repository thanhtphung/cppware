/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/TempDir.hpp"
#include "appkit/TempFile.hpp"
#include "appkit/crt.hpp"

BEGIN_NAMESPACE1(appkit)


TempFile::TempFile(const TempDir& tempDir, const String& basename):
path_(tempDir.path())
{
    path_ += basename;
}


TempFile::~TempFile()
{
    unlink(path_);
}

END_NAMESPACE1
