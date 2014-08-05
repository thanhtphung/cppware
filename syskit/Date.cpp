/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <ctype.h>
#include <time.h>
#include <cstdio>
#include <cstdlib>

#include "syskit-pch.h"
#include "syskit/Date.hpp"
#include "syskit/sys.hpp"

const unsigned int SECS_PER_DAY = 86400;
const unsigned int SECS_PER_HOUR = 3600;
const unsigned int SECS_PER_MINUTE = 60;

BEGIN_NAMESPACE1(syskit)


//!
//! Return normalized date ("yyyy-mm-dd").
//!
const char* Date::asString(char date[MaxDateStringLength + 1]) const
{
    int numerator = date_; //0..99991231
    div_t result = std::div(numerator, 10000);
    unsigned int yyyy = result.quot;
    int mmdd = result.rem;
    result = std::div(mmdd, 100);
    unsigned int mm = result.quot;
    unsigned int dd = result.rem;
    sprintf_s(date, MaxDateStringLength + 1, "%04u-%02u-%02u", yyyy, mm, dd);

    return date;
}


//!
//! Return given time as a relative time string with format ddddd:hh:mm:ss.
//!
const char* Date::formatTime(char time[MaxTimeStringLength + 1], unsigned int secs)
{

    // std::div() cannot be used since secs are really unsigned
    unsigned int ddddd = secs / SECS_PER_DAY;
    secs %= SECS_PER_DAY;
    unsigned int hh = secs / SECS_PER_HOUR;
    secs %= SECS_PER_HOUR;
    unsigned int mm = secs / SECS_PER_MINUTE;
    unsigned int ss = secs % SECS_PER_MINUTE;
    sprintf_s(time, MaxTimeStringLength + 1, "%05u:%02u:%02u:%02u", ddddd, hh, mm, ss);

    return time;
}


//
// Return normalized date (yyyymmdd) for __DATE__ ("Mmm dd yyyy").
//
unsigned int Date::normalize(const char* date)
{
    unsigned int yyyy = std::strtoul(date + 7, 0, 0);

    unsigned int dd = date[5] - '0';
    if (isdigit(date[4]))
    {
        dd += (date[4] - '0') * 10;
    }

    unsigned int mm;
    switch (date[0])
    {
    case 'A': //Apr, Aug
        mm = (date[1] == 'p')? 4: 8;
        break;
    case 'D': //Dec
        mm = 12;
        break;
    case 'F': //Feb
        mm = 2;
        break;
    case 'J': //Jan, Jun, Jul
        mm = (date[1] == 'a')? (1): ((date[2] == 'n')? 6: 7);
        break;
    case 'M': //Mar, May
        mm = (date[2] == 'r')? 3: 5;
        break;
    case 'N': //Nov
        mm = 11;
        break;
    case 'O': //Oct
        mm = 10;
        break;
    default: //Sep
        mm = 9;
        break;
    }

    // yyyymmdd
    unsigned int normalized = yyyy * 10000 + mm * 100 + dd;
    return normalized;
}

END_NAMESPACE1
