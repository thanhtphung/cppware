/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "syskit/Utf8Seq.hpp"
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(syskit)

//!
//! mkdir() variance. Return true if successful or if already existent.
//!
bool mkdir(const wchar_t* path)
{
    Utf8Seq seq;
    size_t numWchars = wcslen(path) + 1;
    seq.shrink(reinterpret_cast<const utf32_t*>(path), numWchars);
    mode_t mode = 0777;
    bool ok = (::mkdir(seq.ascii(), mode) == 0) || (errno == EEXIST);
    return ok;
}

END_NAMESPACE1
