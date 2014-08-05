/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_DATE_HPP
#define SYSKIT_DATE_HPP

#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! c date
class Date
{

public:
    enum
    {
        MaxDateStringLength = 10, //"yyyy-mm-dd"
        MaxTimeStringLength = 14  //"ddddd:hh:mm:ss"
    };

    Date(const char* date);
    operator unsigned int() const;
    const char* asString(char date[MaxDateStringLength + 1]) const;
    unsigned int asU32() const;

    static const char* formatTime(char time[MaxTimeStringLength + 1], unsigned int secs);

private:
    unsigned int date_;

    Date(const Date&); //prohibit usage
    const Date& operator =(const Date&); //prohibit usage

    static unsigned int today();
    static unsigned int normalize(const char*);

};

//! Construct instance from __DATE__ format.
inline Date::Date(const char* date)
{
    date_ = normalize(date);
}

//! Return normalized date (yyyymmdd).
inline Date::operator unsigned int() const
{
    return date_;
}

//! Return normalized date (yyyymmdd).
inline unsigned int Date::asU32() const
{
    return date_;
}

END_NAMESPACE1

#endif
