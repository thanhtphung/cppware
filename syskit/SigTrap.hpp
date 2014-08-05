/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_SIG_TRAP_HPP
#define SYSKIT_SIG_TRAP_HPP

#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! signal trap
class SigTrap
{

public:
    SigTrap();
    ~SigTrap();

    bool empty() const;

private:
    bool empty_;

    SigTrap(const SigTrap&); //prohibit usage
    const SigTrap& operator =(const SigTrap&); //prohibit usage

};

//! Return true if the trap is empty.
inline bool SigTrap::empty() const
{
    return empty_;
}

END_NAMESPACE1

#endif
