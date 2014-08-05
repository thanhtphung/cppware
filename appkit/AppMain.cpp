/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "appkit-pch.h"
#include "appkit/App.hpp"

using namespace appkit;
using namespace syskit;


//!
//! Default App main entry point. If this function is not defined by application
//! code, this skeleton code will be used as the main entry point to create and
//! run the App instance. The appkit::createApp() function is used to create the
//! singleton.
//!
int main()
{
    int exitCode;
    try
    {
        Singleton::create_t create = 0;
        unsigned int initialRefCount = 0U;
        void* createArg = 0;
        App* app = App::instance(create, initialRefCount, createArg);
        App::Count lock(*app);
        exitCode = app->run();
    }
    catch (...)
    {
        exitCode = -1;
    }

    return exitCode;
}
