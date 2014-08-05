/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
//
// Precompiled headers for the appkit project.
//
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>

#include <errno.h>
#include <math.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <strstream>

#include <cppunit/Test.h>
#include <cppunit/TestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include "netkit/IpAddr.hpp"
#include "netkit/IpAddrSet.hpp"
#include "netkit/IpDevice.hpp"
#include "netkit/Paddr.hpp"
#include "netkit/Subnet.hpp"
#include "netkit/UdpClient.hpp"
#include "netkit/Winsock.hpp"
#include "netkit/net.hpp"
#include "syskit/Atomic32.hpp"
#include "syskit/Atomic64.hpp"
#include "syskit/Bom.hpp"
#include "syskit/BufPool.hpp"
#include "syskit/CallStack.hpp"
#include "syskit/Date.hpp"
#include "syskit/Fifo.hpp"
#include "syskit/Foundation.hpp"
#include "syskit/ItemQ.hpp"
#include "syskit/MappedFile.hpp"
#include "syskit/MappedTxtFile.hpp"
#include "syskit/Module.hpp"
#include "syskit/Process.hpp"
#include "syskit/RefCounted.hpp"
#include "syskit/RefVec.hpp"
#include "syskit/Semaphore.hpp"
#include "syskit/SigTrap.hpp"
#include "syskit/Thread.hpp"
#include "syskit/Tree.hpp"
#include "syskit/U16Heap.hpp"
#include "syskit/U32Heap.hpp"
#include "syskit/U64Heap.hpp"
#include "syskit/Utc.hpp"
#include "syskit/Utf16Seq.hpp"
#include "syskit/Utf8.hpp"
#include "syskit/Vec.hpp"
#include "syskit/sys.hpp"

// Precompiled header marker.
#include "appkit-pch.h"
