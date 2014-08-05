#ifndef NETKIT_NET_LINUX_HPP
#define NETKIT_NET_LINUX_HPP
#ifndef NETKIT_NET_HPP
#error "unsupported usage"
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

#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>

typedef int SOCKET;

#ifndef closesocket
#define closesocket close
#endif

#ifndef INVALID_SOCKET
const SOCKET INVALID_SOCKET = -1;
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR (-1)
#endif

#endif
