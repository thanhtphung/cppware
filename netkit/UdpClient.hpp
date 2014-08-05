/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_UDP_CLIENT_HPP
#define NETKIT_UDP_CLIENT_HPP

#if __linux || __CYGWIN__
#include "netkit/linux/UdpClient-linux.hpp"

#elif _WIN32
#include "netkit/win/UdpClient-win.hpp"

#else
#error "unsupported platform"

#endif
#endif
