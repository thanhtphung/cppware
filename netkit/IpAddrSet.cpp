/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "appkit/DelimitedTxt.hpp"
#include "appkit/Str.hpp"
#include "appkit/String.hpp"
#include "appkit/U32.hpp"
#include "syskit/macros.h"

#include "netkit-pch.h"
#include "netkit/IpAddr.hpp"
#include "netkit/IpAddrSet.hpp"

using namespace appkit;

BEGIN_NAMESPACE1(netkit)


//!
//! Construct a duplicate instance of the given set.
//!
IpAddrSet::IpAddrSet(const IpAddrSet& set):
U32Set(set)
{
}


//!
//! Construct an instance. This set has an initial capacity of capacity ranges.
//! The set grows by doubling. If given capacity is zero, then an initial capacity
//! of one will be used instead. Populate it with IP addresses specified by s. Use
//! delim to override the default comma delimiter. If given delimiter is zero or a
//! hyphen, the default comma delimiter will be used.
//!
IpAddrSet::IpAddrSet(const char* s, char delim, unsigned int capacity):
U32Set(VALID_MIN, VALID_MAX, capacity)
{
    String normalized;
    normalize(normalized, s, delim);
    U32Set::add(normalized.ascii(), ',');
}


//!
//! Construct an instance. This set has an initial capacity of capacity ranges.
//! The set grows by doubling. If given capacity is zero, then an initial capacity
//! of one will be used instead. Populate it with IP addresses specified by s.
//!
IpAddrSet::IpAddrSet(const char* s, unsigned int capacity):
U32Set(VALID_MIN, VALID_MAX, capacity)
{
    String normalized;
    normalize(normalized, s, ',');
    U32Set::add(normalized.ascii(), ',');
}


//!
//! Construct an empty set. This set has an initial capacity of capacity ranges.
//! The set grows by doubling. If given capacity is zero, then an initial capacity
//! of one will be used instead.
//!
IpAddrSet::IpAddrSet(unsigned int capacity):
U32Set(VALID_MIN, VALID_MAX, capacity)
{
}


IpAddrSet::~IpAddrSet()
{
}


//!
//! Clone set. Return cloned set. The cloned set is allocated using
//! the heap and is to be freed by the user using the delete operator.
//!
Set* IpAddrSet::clone() const
{
    return new IpAddrSet(*this);
}


//!
//! Return ASCII form of this set using given range delimiter. ASCII form
//! of an empty set is an empty string. ASCII form of a non-empty set has
//! the following format: "$lo(-$hi)?[$rangeDelimiter$lo(-$hi)?]*". Sample
//! ASCII form with comma as range delimiter:
//!\code
//! 172.16.16.2-172.16.16.3,172.16.16.11,192.168.0.15-192.168.0.21
//!\endcode
//! Sample ASCII form with newline as range delimiter:
//!\code
//! 172.16.16.2-172.16.16.3
//! 172.16.16.11
//! 192.168.0.15-192.168.0.21
//!\endcode
//!
String IpAddrSet::toString(const char* delim) const
{
    char buf[1 + IpAddr::StrLength + 1];
    buf[0] = '-';

    IpAddr ipAddr;
    String rangeDelim(delim);
    String s;
    for (size_t i = 0, numRanges = U32Set::numRanges(); i < numRanges; s += rangeDelim, ++i)
    {
        unsigned int loAddr;
        unsigned int hiAddr;
        U32Set::getRange(i, loAddr, hiAddr);
        ipAddr = loAddr;
        s += ipAddr.toString(buf + 1);
        if (loAddr != hiAddr)
        {
            ipAddr = hiAddr;
            ipAddr.toString(buf + 1);
            s += buf;
        }
    }

    s.truncate(s.length() - rangeDelim.length());
    return s;
}


//
// Form a U32Set string from the given IpAddrSet string. Save result in result. Return
// reference to result. Example: result for "172.16.16.1,172.16.16.90-171.16.16.99"
// is "2886733825,2886733914-2886733923".
//
String& IpAddrSet::normalize(String& result, const char* s, char delim) const
{

    // Look at input string as delimited text.
    validateDelim(delim);
    bool makeCopy = false;
    DelimitedTxt txt(s, strlen(s), makeCopy, delim);

    // Normalize one line at a time.
    IpAddr ipAddr;
    String line;
    bool trimLine = true;
    char buf[U32::MaxDigits + 1 + U32::MaxDigits + 1];
    result.reset();
    while (txt.next(line, trimLine))
    {
        line.trimSpace(true /*trimL*/, true /*trimR*/);
        size_t n;
        unsigned int hiAddr;
        unsigned int loAddr;
        if (ipAddr.reset(line.ascii()))
        {
            n = U32::toDigits(ipAddr, buf);
        }
        else if (strToRange(line.ascii(), loAddr, hiAddr))
        {
            size_t n0 = U32::toDigits(loAddr, buf);
            buf[n0] = '-';
            size_t n1 = U32::toDigits(hiAddr, buf + n0 + 1);
            n = n0 + 1 + n1;
        }
        else
        {
            continue;
        }
        buf[n++] = ',';
        result.append(buf, n);
    }

    // Return reference to <result>.
    result.truncate(result.length() - 1);
    return result;
}


//!
//! Add IP addresses specified by string s. Return true if at least one new
//! address was added successfully. Use delim to override the default comma
//! delimiter. If given delimiter is zero or a hyphen, the default comma
//! delimiter will be used.
//!
bool IpAddrSet::add(const char* s, char delim)
{
    String normalized;
    normalize(normalized, s, delim);
    bool modified = U32Set::add(normalized.ascii(), ',');
    return modified;
}


//!
//! Return true if this set is identical to or is a superset of given set.
//! Use delim to override the default comma delimiter. If given delimiter
//! is zero or a hyphen, the default comma delimiter will be used.
//!
bool IpAddrSet::contains(const char* s, char delim) const
{
    String normalized;
    normalize(normalized, s, delim);
    bool modified = U32Set::contains(normalized.ascii(), ',');
    return modified;
}


//!
//! See if string s is a valid specification of IP addresses. A valid string
//! specification is a sequence of tokens delimited by any white-space character
//! or a comma, and each token is either a valid dotted IP address or a valid
//! dotted IP address range ("lo-hi"). An address range is valid if the low
//! address is less than or equal to the high address. Use delim to override
//! the default comma delimiter. If given delimiter is zero or a hyphen, the
//! default comma delimiter will be used.
//!
bool IpAddrSet::isValid(const char* s, char delim)
{

    // Assume valid.
    bool valid = true;

    // Look at input string as delimited text.
    validateDelim(delim);
    bool makeCopy = false;
    DelimitedTxt txt(s, strlen(s), makeCopy, delim);

    // Validate one line at a time.
    IpAddr ipAddr;
    String line;
    bool trimLine = true;
    while (txt.next(line, trimLine))
    {
        line.trimSpace(true /*trimL*/, true /*trimR*/);
        unsigned int hiAddr;
        unsigned int loAddr;
        if ((!ipAddr.reset(line.ascii())) && (!strToRange(line.ascii(), loAddr, hiAddr)))
        {
            valid = false;
            break;
        }
    }

    // Return status.
    return valid;
}


//!
//! Remove addresses specified by string s from this set. Return true if at least
//! one address was removed successfully. The given addresses can include non-existent
//! addresses. For example, if this set has only odd addresses from 172.16.16.1
//! to 172.16.16.100, removing addresses between 172.16.16.1 and 172.16.16.50 is
//! allowed and will result in removal of odd addresses from 172.16.16.1 to
//! 172.16.16.50. Use delim to override the default comma delimiter. If given
//! delimiter is zero or a hyphen, the default comma delimiter will be used.
//!
bool IpAddrSet::rm(const char* s, char delim)
{
    String normalized;
    normalize(normalized, s, delim);
    bool modified = U32Set::rm(normalized.ascii(), ',');
    return modified;
}


//!
//! Convert string s to loAddr and hiAddr. Return true if s specifies a valid
//! dotted IP address range. A dotted IP address range is valid if the low address
//! is less than or equal to the high address.
//!
bool IpAddrSet::strToRange(const char* s, unsigned int& loAddr, unsigned int& hiAddr)
{

    // Assume string s has format "a.b.c.d-e.f.g.h" (e.g., "172.16.16.1-172.16.16.23").
    char* p;
    unsigned int abcd[8];
    for (size_t i = 0; i < 8; ++i, s = p + 1)
    {
        abcd[i] = strtoul(s, &p, 0);
        static const char s_delimiter[8] = {'.', '.', '.', '-', '.', '.', '.', 0};
        if ((p == s) || (*p != s_delimiter[i]) || (abcd[i] > 255))
        {

            // Special case for our CIDR notation. A format of "a.b.c.d/(0-32)"
            // (e.g., "172.16.16.0/24") is required. This is slightly different
            // compared to some tool like nmap. For example, an older nmap allowed
            // "a.b.c[.d]/(0-32)", and the current nmap allows "a.b.c[.d]/(1-32)".
            // TODO: be consistent with the new Subnet class.
            if ((i == 3) && (p != s) && (*p == '/'))
            {
                s = p + 1;
                unsigned int numBits = strtoul(s, &p, 0);
                if ((p != s) && (*p == 0) && (numBits <= 32))
                {
                    unsigned int mask = ((1U << numBits) - 1) << (32 - numBits);
                    loAddr = ((abcd[0] << 24) | (abcd[1] << 16) | (abcd[2] << 8) | abcd[3]) & mask;
                    hiAddr = loAddr + ~mask;
                    return true;
                }
            }

            // Not an IP address range.
            return false;
        }
    }

    // Return true if <s> specifies a valid dotted IP address range.
    loAddr = (abcd[0] << 24) | (abcd[1] << 16) | (abcd[2] << 8) | abcd[3];
    hiAddr = (abcd[4] << 24) | (abcd[5] << 16) | (abcd[6] << 8) | abcd[7];
    return (loAddr <= hiAddr);
}


//!
//! Construct an unattached IpAddrSet iterator.
//!
IpAddrSet::Itor::Itor():
U32Set::Itor()
{
}


//!
//! Construct an IpAddrSet iterator. Attach iterator to given set. A deep
//! copy of the given set is made if makeCopy is true. Normally, a deep
//! copy should be made only if the given set can change during iterating.
//!
IpAddrSet::Itor::Itor(const IpAddrSet& set, bool makeCopy):
U32Set::Itor(set, makeCopy)
{
}


//!
//! Destruct iterator.
//!
IpAddrSet::Itor::~Itor()
{
}

END_NAMESPACE1
