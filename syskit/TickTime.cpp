/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/Cpu.hpp"
#include "syskit/TickTime.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)

double TickTime::msecsPerTick_ = 1.0 / Cpu::getMyKilohertz();
double TickTime::secsPerTick_ = 1000.0 / Cpu::getMyKilohertz();
unsigned long long TickTime::ticksPerMsec_ = Cpu::getMyKilohertz();

END_NAMESPACE1
