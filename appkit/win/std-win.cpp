/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <fstream>
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/String.hpp"
#include "appkit/std.hpp"

BEGIN_NAMESPACE1(appkit)


//!
//! std::ifstream wrapper
//!
ifstream::ifstream(const String& path, std::ios_base::openmode mode):
std::ifstream(path.widen(), mode, std::ios_base::_Openprot)
{
}


//!
//! std::ofstream wrapper
//!
ofstream::ofstream(const String& path, std::ios_base::openmode mode):
std::ofstream(path.widen(), mode, std::ios_base::_Openprot)
{
}


//!
//! std::wifstream wrapper
//!
wifstream::wifstream(const String& path, std::ios_base::openmode mode):
std::wifstream(path.widen(), mode, std::ios_base::_Openprot)
{
}


//!
//! std::wofstream wrapper
//!
wofstream::wofstream(const String& path, std::ios_base::openmode mode):
std::wofstream(path.widen(), mode, std::ios_base::_Openprot)
{
}

END_NAMESPACE1
