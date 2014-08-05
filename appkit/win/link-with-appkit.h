/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_LINK_WITH_APPKIT_H
#define APPKIT_LINK_WITH_APPKIT_H

// Define the App singleton to be created by the default main entry point provided by
// the appkit library. The className parameter is the name of the (derived) App which
// has a default constructor.
#define DEFINE_APP1(className)\
namespace appkit\
{\
    const char* App::buildDate_ = __DATE__;\
    const char* App::buildTime_ = __TIME__;\
    syskit::Singleton* createApp(const char* /*id*/, unsigned int initialRefCount, void* /*arg*/)\
    {\
        App::setInitialRefCount(initialRefCount);\
        return new className; \
    }\
}

// Define the WinMain entry point for MFC application which uses the App singleton.
#define DEFINE_WIN_MAIN_FOR_MFC()\
extern int AFXAPI AfxWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, int nCmdShow);\
extern "C" int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)\
{\
    return AfxWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);\
}

#if _WIN32
#if _DEBUG
#pragma comment(lib,"appkitd")

#else
#pragma comment(lib,"appkit")

#endif
#endif
#endif
