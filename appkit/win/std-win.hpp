/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_STD_WIN_HPP
#define APPKIT_STD_WIN_HPP
#ifndef APPKIT_STD_HPP
#error "unsupported usage"
#endif

#include <streambuf>

BEGIN_NAMESPACE1(appkit)

//! sgetn() wrapper
inline std::streamsize sgetn(std::streambuf* sbuf, char* buf, std::streamsize size)
{
#if _MSC_VER < 1600
    return sbuf->_Sgetn_s(buf, size, size);
#else
    return sbuf->sgetn(buf, size);
#endif
}

END_NAMESPACE1

#endif
