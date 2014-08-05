/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_SILENT_INPUT_MODE_HPP
#define SYSKIT_SILENT_INPUT_MODE_HPP

#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! silent terminal input mode
class SilentInputMode
    //!
    //! Instantiate this mode to turn off terminal input echoes.
    //!
{

public:
    SilentInputMode();
    ~SilentInputMode();

private:
    HANDLE h_;
    unsigned long oldMode_;

    SilentInputMode(const SilentInputMode&); //prohibit usage
    const SilentInputMode& operator =(const SilentInputMode&); //prohibit usage

};

END_NAMESPACE1

#endif
