/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_NET_WIN_HPP
#define NETKIT_NET_WIN_HPP
#ifndef NETKIT_NET_HPP
#error "unsupported usage"
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifdef htonl
#undef htonl
#endif
#ifdef htonll
#undef htonll
#endif
#ifdef htons
#undef htons
#endif
#ifdef ntohl
#undef ntohl
#endif
#ifdef ntohll
#undef ntohll
#endif
#ifdef ntohs
#undef ntohs
#endif
#include <winsock2.h>

#endif
