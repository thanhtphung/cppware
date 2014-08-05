/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/sys.hpp"

#include "netkit-pch.h"
#include "netkit/S32SocOpt.hpp"

using namespace syskit;

BEGIN_NAMESPACE1(netkit)


bool S32SocOpt::getV(name_e n, int& v) const
{
    bool ok;
    int optV;
    socklen_t optVSize = sizeof(optV);
    if ((!getsockopt(soc_, SOL_SOCKET, n, reinterpret_cast<char*>(&optV), &optVSize)) &&
        (optVSize == sizeof(optV)))
    {
        v = optV;
        ok = true;
    }

    else
    {
        ok = false;
    }

    return ok;
}

END_NAMESPACE1
