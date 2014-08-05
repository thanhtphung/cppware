/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_DEV_NULL_HPP
#define SYSKIT_DEV_NULL_HPP

#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! null device aka /dev/null
class DevNull
{

public:
    DevNull(int fd);
    DevNull();
    ~DevNull();

    operator int() const;

    int fd() const;

private:
    int clone_;
    int null_;
    int orig_;

    DevNull(const DevNull&); //prohibit usage
    const DevNull& operator =(const DevNull&); //prohibit usage

};

inline DevNull::operator int() const
{
    return null_;
}

inline int DevNull::fd() const
{
    return null_;
}

END_NAMESPACE1

#endif
