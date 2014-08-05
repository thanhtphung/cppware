/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "netkit/Winsock.hpp"
#include "syskit/Foundation.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/App.hpp"

using namespace appkit;
using namespace netkit;
using namespace syskit;

static App* s_app = 0;
static Winsock* s_winsock = 0;


//!
//! Default App DLL entry point. If this function is not defined by application
//! code, this skeleton code will be used as the DLL main entry point. Application
//! code which must define its own DllMain must also implement App::instanceInDll().
//!
BOOL __stdcall DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID /*lpReserved*/)
{
    Singleton::create_t create;
    unsigned int initialRefCount;
    void* createArg;
    switch (ul_reason_for_call)
    {

    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        Foundation::instance().addRef();
        s_winsock = new Winsock;
        create = createApp;
        initialRefCount = 0U;
        createArg = 0;
        s_app = dynamic_cast<App*>(Singleton::getSingleton(App::ID1, create, initialRefCount, createArg));
        s_app->addRef();
        break;

    case DLL_PROCESS_DETACH:
        s_app->rmRef();
        s_app = 0;
        delete s_winsock;
        s_winsock = 0;
        Foundation::instance().rmRef();
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    default:
        break;
    }

    BOOL ok = TRUE;
    return ok;
}

BEGIN_NAMESPACE1(appkit)


//
// Return the singleton. This method resides in this file to force application code to
// link with the above DllMain() entry point. As a result, application code which must
// define its own DllMain must also implement this App::instanceInDll() method.
//
App* App::instanceInDll()
{

    // Use getSingleton() to be consistent with App::instance() implementation even
    // though s_app would be more efficient. This also allows the code to be copied
    // elsewhere if application code defines its own DllMain.
    if (instance_ == 0) instance_ = dynamic_cast<App*>(getSingleton(App::ID1));
    return instance_;
}

END_NAMESPACE1
