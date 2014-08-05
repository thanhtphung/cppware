/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_NET_HPP
#define NETKIT_NET_HPP

#ifdef htonl
#undef htonl
#endif
#define htonl htonl__

#ifdef htonll
#undef htonll
#endif
#define htonll htonll__

#ifdef htons
#undef htons
#endif
#define htons htons__

#ifdef ntohl
#undef ntohl
#endif
#define ntohl ntohl__

#ifdef ntohll
#undef ntohll
#endif
#define ntohll ntohll__

#ifdef ntohs
#undef ntohs
#endif
#define ntohs ntohs__

#if __linux || __CYGWIN__
#include "netkit/linux/net-linux.hpp"

#elif _WIN32
#include "netkit/win/net-win.hpp"

#else
#error "unsupported architecture"

#endif

#endif
