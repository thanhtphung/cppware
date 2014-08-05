/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_STD_HPP
#define APPKIT_STD_HPP

#include <fstream>
#include "syskit/macros.h"

//
// wrappers for some class interfaces from the std namespace
//
BEGIN_NAMESPACE1(appkit)

class String;


//! std::ifstream wrapper
class ifstream: public std::ifstream
    //!
    //! std::ifstream wrapper with appkit::String support.
    //!
{

public:
    ifstream(const String& path, std::ios_base::openmode mode = std::ios_base::in);
    ~ifstream();

private:
    ifstream(const ifstream&); //prohibit usage
    const ifstream& operator =(const ifstream&); //prohibit usage

};


//! std::ofstream wrapper
class ofstream: public std::ofstream
    //!
    //! std::ofstream wrapper with appkit::String support.
    //!
{

public:
    ofstream(const String& path, std::ios_base::openmode mode = std::ios_base::out);
    ~ofstream();

private:
    ofstream(const ofstream&); //prohibit usage
    const ofstream& operator =(const ofstream&); //prohibit usage

};


//! std::wifstream wrapper
class wifstream: public std::wifstream
    //!
    //! std::wifstream wrapper with appkit::String support.
    //!
{

public:
    wifstream(const String& path, std::ios_base::openmode mode = std::ios_base::in);
    ~wifstream();

private:
    wifstream(const wifstream&); //prohibit usage
    const wifstream& operator =(const wifstream&); //prohibit usage

};


//! std::wofstream wrapper
class wofstream: public std::wofstream
    //!
    //! std::wofstream wrapper with appkit::String support.
    //!
{

public:
    wofstream(const String& path, std::ios_base::openmode mode = std::ios_base::out);
    ~wofstream();

private:
    wofstream(const wofstream&); //prohibit usage
    const wofstream& operator =(const wofstream&); //prohibit usage

};

END_NAMESPACE1

#if __linux || __CYGWIN__
#include "appkit/linux/std-linux.hpp"

#elif _WIN32
#include "appkit/win/std-win.hpp"

#else
#error "unsupported architecture"

#endif
#endif
