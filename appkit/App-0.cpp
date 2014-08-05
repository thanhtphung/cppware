/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/App.hpp"

BEGIN_NAMESPACE1(appkit)

const char* App::buildDate_ = __DATE__;
const char* App::buildTime_ = __TIME__;

END_NAMESPACE1
