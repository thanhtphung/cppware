/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_CRT_HPP
#define APPKIT_CRT_HPP

//! \file crt.hpp

#include <cstdio>
#include "syskit/sys.hpp"

//
// c-run-time wrappers
//
BEGIN_NAMESPACE1(appkit)

class String;

bool getenv(const String& k, String& v, bool expandEnv = false);
bool getline(String& line, size_t maxLength = 4095, bool doTrimLine = true, bool doEchoInput = true);
bool putenv(const String& k, const String* v);
int access(const String& path, int mode);
int chdir(const String& path);
int chmod(const String& path, int mode);
int copy(const String& srcPath, const String& dstPath, bool failIfExists = true);
int link(const String& oldPath, const String& newPath);
int mkdir(const String& path);
int mkdirRecursively(const String& path, bool skipNormalization = false);
int rename(const String& oldPath, const String& newPath);
int rmdir(const String& path);
int sprintf(String& buf, const char* format, ...) GCC_ATTR((format(printf, 2, 3)));
int swprintf(String& buf, const wchar_t* format, ...);
int unlink(const String& path);
std::FILE* fopen(const String& path, const String& mode);

END_NAMESPACE1

#if _WIN32
#include "appkit/win/crt-win.hpp"

#endif
#endif
