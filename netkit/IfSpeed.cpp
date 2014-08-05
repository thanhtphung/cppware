/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "appkit/D64.hpp"
#include "appkit/Str.hpp"
#include "appkit/String.hpp"
#include "appkit/crt.hpp"
#include "syskit/macros.h"

#include "netkit-pch.h"
#include "netkit/IfSpeed.hpp"

using namespace appkit;

const unsigned long long GBPS = 1000000000ULL;
const unsigned long long KBPS = 1000ULL;
const unsigned long long MBPS = 1000000ULL;

BEGIN_NAMESPACE1(netkit)


IfSpeed::IfSpeed(const String& speed)
{
    size_t bytesUsed;
    double d64 = D64(speed, &bytesUsed);

    if (bytesUsed > 0)
    {
        String unit(speed.ascii() + bytesUsed);
        unit.trimSpace();
        if (Str::compareKI(unit.ascii(), "Gbps") == 0)
        {
            d64 *= GBPS;
        }
        else if (Str::compareKI(unit.ascii(), "Mbps") == 0)
        {
            d64 *= MBPS;
        }
        else if (Str::compareKI(unit.ascii(), "Kbps") == 0)
        {
            d64 *= KBPS;
        }
    }

    v_ = static_cast<unsigned long long>(d64 + 0.5);
}


String IfSpeed::toString() const
{
    String s;
    if (v_ == 0)
    {
        s = "Unknown";
        return s;
    }

    // What unit makes the most sense?
    double speed = static_cast<double>(v_);
    const char* unit;
    if (v_ >= GBPS)
    {
        speed /= GBPS;
        unit = "Gbps";
    }
    else if (v_ >= MBPS)
    {
        speed /= MBPS;
        unit = "Mbps";
    }
    else if (v_ >= KBPS)
    {
        speed /= KBPS;
        unit = "Kbps";
    }
    else
    {
        unit = "bps";
    }

    sprintf(s, "%.4g %s", speed, unit);
    return s;
}

END_NAMESPACE1
