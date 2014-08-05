/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/macros.h"

#include "netkit-pch.h"
#include "netkit/Winsock.hpp"

static netkit::Winsock s_winsock;

BEGIN_NAMESPACE1(netkit)

const Winsock* Winsock::instance_ = &s_winsock;

END_NAMESPACE1
