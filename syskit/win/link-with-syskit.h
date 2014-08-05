/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_LINK_WITH_SYSKIT_H
#define SYSKIT_LINK_WITH_SYSKIT_H

#if _WIN32
#if _DEBUG
#pragma comment(lib,"syskitd")

#else
#pragma comment(lib,"syskit")

#endif
#endif
#endif
