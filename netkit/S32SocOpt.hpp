/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_S32_SOC_OPT_HPP
#define NETKIT_S32_SOC_OPT_HPP

#include "netkit/net.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(netkit)


//! socket option as a signed 32-bit value
class S32SocOpt
{

public:
    enum name_e
    {
        N_ERROR = SO_ERROR,
        N_RCVBUF = SO_RCVBUF,
        N_SNDBUF = SO_SNDBUF,
        N_TYPE = SO_TYPE
    };

    S32SocOpt(SOCKET soc);

    bool getV(name_e n, int& v) const;
    bool setV(name_e n, int v);
    int getV(name_e n) const;

private:
    SOCKET soc_;

    S32SocOpt(const S32SocOpt&); //prohibit usage
    S32SocOpt& operator=(const S32SocOpt&); //prohibit usage

};

inline S32SocOpt::S32SocOpt(SOCKET soc)
{
    soc_ = soc;
}

inline bool S32SocOpt::setV(name_e n, int v)
{
    return (!setsockopt(soc_, SOL_SOCKET, n, reinterpret_cast<const char*>(&v), sizeof(v)));
}

inline int S32SocOpt::getV(name_e n) const
{
    int v;
    return getV(n, v)? v :-1;
}

END_NAMESPACE1

#endif
