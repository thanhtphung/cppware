/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_WIN_CMD_HPP
#define APPKIT_WIN_CMD_HPP

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include "appkit/Cmd.hpp"
#include "syskit/RefCounted.hpp"
#include "syskit/macros.h"

DECLARE_CLASS1(syskit, Semaphore)
DECLARE_CLASS1(syskit, SpinSection)

BEGIN_NAMESPACE1(appkit)

class WinApp;


//! windows command handler
class WinCmd: public Cmd, public syskit::RefCounted
    //!
    //! A class representing a windows command handler. The onRun() and onFinalRun()
    //! methods are invoked when a command is run. The methods should return true/false
    //! to indicate command validation success/failure. A failure will result in a
    //! command usage hint. The onRun() method is invoked in a dedicated agent task,
    //! and the onFinalRun() method is invoked in the WinApp messaging thread. Multiple
    //! onRun() invocations can occur concurrently while multiple onFinalRun() invocations
    //! cannot. That is, onFinalRun() is sequentially invoked.
    //!
{

public:

    // Override Cmd.
    virtual bool onRun(const CmdLine& req);
    virtual bool sprint(const CmdLine& req, String& rsp);

protected:
    WinCmd(const WinApp* winApp, const char* nameSet);
    syskit::SpinSection& ss() const;

    virtual ~WinCmd();
    virtual bool onFinalRun(const CmdLine& req) = 0;

private:
    HWND messagingWin_;
    bool cmdIsValid_;
    syskit::Semaphore* sem_;
    syskit::SpinSection* ss_; //serialize onFinalRun()
    const WinApp* winApp_;
    unsigned int messagingId_;

    WinCmd(const WinCmd&); //prohibit usage
    const WinCmd& operator =(const WinCmd&); //prohibit usage

    friend class WinApp;

};

END_NAMESPACE1

#endif
