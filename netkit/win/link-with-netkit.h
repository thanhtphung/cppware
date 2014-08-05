/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_LINK_WITH_NETKIT_H
#define NETKIT_LINK_WITH_NETKIT_H

#if _WIN32
#if _DEBUG
#pragma comment(lib,"netkitd")

#else
#pragma comment(lib,"netkit")

#endif
#endif
#endif
