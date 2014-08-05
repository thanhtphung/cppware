/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_STD_LINUX_HPP
#define APPKIT_STD_LINUX_HPP
#ifndef APPKIT_STD_HPP
#error "unsupported usage"
#endif

#include <streambuf>

BEGIN_NAMESPACE1(appkit)

//! sgetn() wrapper
inline std::streamsize sgetn(std::streambuf* sbuf, char* buf, std::streamsize size)
{
    return sbuf->sgetn(buf, size);
}

END_NAMESPACE1

#endif
