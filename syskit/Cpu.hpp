/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_CPU_HPP
#define SYSKIT_CPU_HPP

#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


class Cpu
{

public:
    static unsigned int getMyKilohertz();
    static unsigned int myHertz();
    static unsigned int myKilohertz();

private:
    static unsigned int hertz_;
    static unsigned int kilohertz_;

    static unsigned int getHertz();

};

//! Return my processor's frequency in hertz (cycles per second). This
//! method might not be available during static data construction. Use
//! "getMyKilohertz() * 1000" instead during static data construction.
inline unsigned int Cpu::myHertz()
{
    return hertz_;
}

//! Return my processor's frequency in kilohertz (aka cycles per msec). This
//! method might not be available during static data construction. Use
//! getMyKilohertz() instead during static data construction.
inline unsigned int Cpu::myKilohertz()
{
    return kilohertz_;
}

END_NAMESPACE1

#endif
