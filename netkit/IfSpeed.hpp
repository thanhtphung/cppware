/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_IF_SPEED_HPP
#define NETKIT_IF_SPEED_HPP

#include "syskit/macros.h"

DECLARE_CLASS1(appkit, String)

BEGIN_NAMESPACE1(netkit)


//! network interface speed
class IfSpeed
{

public:
    IfSpeed(const IfSpeed& speed);
    IfSpeed(const appkit::String& speed);
    IfSpeed(unsigned long long speed = 0);

    operator appkit::String() const;
    operator unsigned long long() const;
    const IfSpeed& operator =(const IfSpeed& speed);
    const IfSpeed& operator =(unsigned long long speed);

    appkit::String toString() const;
    unsigned long long asU64() const;

private:
    unsigned long long v_;

};

END_NAMESPACE1

#include "appkit/String.hpp"

BEGIN_NAMESPACE1(netkit)

inline IfSpeed::IfSpeed(const IfSpeed& speed)
{
    v_ = speed.v_;
}

inline IfSpeed::IfSpeed(unsigned long long speed)
{
    v_ = speed;
}

inline IfSpeed::operator appkit::String() const
{
    return toString();
}

inline IfSpeed::operator unsigned long long() const
{
    return v_;
}

inline const IfSpeed& IfSpeed::operator =(const IfSpeed& speed)
{
    v_ = speed.v_;
    return *this;
}

inline const IfSpeed& IfSpeed::operator =(unsigned long long speed)
{
    v_ = speed;
    return *this;
}

inline unsigned long long IfSpeed::asU64() const
{
    return v_;
}

END_NAMESPACE1

#endif
