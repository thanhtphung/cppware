/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/App.hpp"
#include "appkit/CmdLine.hpp"

using namespace syskit;

BEGIN_NAMESPACE1(appkit)


//!
//! Create and return the App singleton. If this function is not defined by application
//! code, this skeleton code will be used to create a minimal App instance as needed.
//!
Singleton* createApp(const char* /*id*/, unsigned int initialRefCount, void* /*arg*/)
{
    App::setInitialRefCount(initialRefCount);

    const CmdLine& cmdLine = CmdLine::instance();
    App* app1 = new App(cmdLine);
    return app1;
}

END_NAMESPACE1
