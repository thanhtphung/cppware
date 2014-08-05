/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <fstream>
#include "syskit/macros.h"
#include "appkit/String.hpp"
#include "appkit/std.hpp"

BEGIN_NAMESPACE1(appkit)


//!
//! std::ifstream wrapper
//!
ifstream::ifstream(const String& path, std::ios_base::openmode mode):
std::ifstream(path.ascii(), mode)
{
}


//!
//! std::ofstream wrapper
//!
ofstream::ofstream(const String& path, std::ios_base::openmode mode):
std::ofstream(path.ascii(), mode)
{
}


//!
//! std::wifstream wrapper
//!
wifstream::wifstream(const String& path, std::ios_base::openmode mode):
std::wifstream(path.ascii(), mode)
{
}


//!
//! std::wofstream wrapper
//!
wofstream::wofstream(const String& path, std::ios_base::openmode mode):
std::wofstream(path.ascii(), mode)
{
}

END_NAMESPACE1
