/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_STR_HPP
#define APPKIT_STR_HPP

#include <sys/types.h>
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)

class String;


//! c-string (aka array of null-terminated characters)
class Str
    //!
    //! A class serving as a namespace for miscellaneous string-related utilities.
    //! Comparison function naming hints:
    //!\code
    //!     K: compare keys
    //!    KI: compare keys, ignore case
    //!   KIN: compare keys, ignore case, up to n characters
    //!   KIR: compare keys, ignore case, reverse the sense of comparison
    //!    KR: compare keys, reverse the sense of comparison
    //!     P: compare keys given pointers
    //!    PI: compare keys given pointers, ignore case
    //!   PIR: compare keys given pointers, ignore case, reverse the sense of comparison
    //!    PR: compare keys given pointers, reverse the sense of comparison
    //!\endcode
    //!
{

public:
    static bool isAscii(const char* s, size_t length);

    static String& stripSpace(String& result, const char* s, char delim = ',');
    static String& stripSpace(String& result, const char* s, size_t length, char delim = ',');
    static char* stripSpace(char* s, char delim = ',');

    static String& trimSpace(String& result, const char* s, bool trimL, bool trimR);
    static String& trimSpace(String& result, const char* s, size_t length, bool trimL, bool trimR);
    static char* trimSpace(char* s, bool trimL, bool trimR);

    static int compareK(const void* item0, const void* item1);
    static int compareKI(const void* item0, const void* item1);
    static int compareKIR(const void* item0, const void* item1);
    static int compareKR(const void* item0, const void* item1);
    static int compareP(const void* item0, const void* item1);
    static int comparePI(const void* item0, const void* item1);
    static int comparePIR(const void* item0, const void* item1);
    static int comparePR(const void* item0, const void* item1);

    static int compareKIN(const char* item0, const char* item1, size_t n);

};

const char* strcasestr(const char* haystack, const char* needle);

END_NAMESPACE1

#endif
