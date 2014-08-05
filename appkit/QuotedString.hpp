/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_QUOTED_STRING_HPP
#define APPKIT_QUOTED_STRING_HPP

#include "appkit/String.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)


class QuotedString
{

public:
    QuotedString(const QuotedString& quotedString);
    QuotedString(const String& str, char quote = '\"', bool escapeSingleQuotedString = true);
    ~QuotedString();

    const QuotedString& operator =(const QuotedString& quotedString);

    String dequote(bool unescapeSingleQuotedString = true) const;
    const String& asString() const;

    static bool dequote(String& str, bool unescapeSingleQuotedString = true);
    static void enquote(String& str, char quote = '\"', bool escapeSingleQuotedString = true);

private:
    String quoted_;

    static String escape(const String&);
    static unsigned char unescapeO(const syskit::utf8_t*, const syskit::utf8_t*, size_t&);
    static unsigned char unescapeX(const syskit::utf8_t*, const syskit::utf8_t*, size_t&);
    static unsigned int unescape(syskit::utf8_t*, const syskit::utf8_t*, const syskit::utf8_t*);
    static syskit::utf32_t unescapeU(const syskit::utf8_t*, const syskit::utf8_t*, size_t&);

};

inline const String& QuotedString::asString() const
{
    return quoted_;
}

END_NAMESPACE1

#endif
