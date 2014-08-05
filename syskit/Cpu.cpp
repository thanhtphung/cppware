/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/Cpu.hpp"
#include "syskit/macros.h"

const unsigned int MSECS_PER_SEC = 1000U;

BEGIN_NAMESPACE1(syskit)

unsigned int Cpu::hertz_ = getHertz();
unsigned int Cpu::kilohertz_ = hertz_ / MSECS_PER_SEC;


//!
//! Return my processor's frequency in kilohertz (aka cycles per msec).
//!
unsigned int Cpu::getMyKilohertz()
{
    static unsigned int s_kilohertz = getHertz() / MSECS_PER_SEC;
    return s_kilohertz;
}

END_NAMESPACE1
