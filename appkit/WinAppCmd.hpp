/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_WIN_APP_CMD_HPP
#define APPKIT_WIN_APP_CMD_HPP

#include "appkit/WinCmd.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)

class SysIo;
class WinApp;


//! debug commands for a windows application
class WinAppCmd: public WinCmd
{

public:
    WinAppCmd(WinApp* winApp, SysIo* sysIo);
    bool exitInProgress() const;

    // Override Cmd.
    virtual bool onRun(const CmdLine& req);
    virtual const char* usage(unsigned char cmdIndex) const;
    virtual const char* xName(unsigned char cmdIndex) const;

protected:

    // Override Cmd.
    virtual bool allowReq(const CmdLine& req, const netkit::Paddr& dbugger);

    // Override WinCmd.
    virtual ~WinAppCmd();
    virtual bool onFinalRun(const CmdLine& req);

private:
    typedef bool (WinAppCmd::*doer_t)(const CmdLine& req);

    static doer_t doer_[];

    SysIo* sysIo_;
    WinApp* winApp_;
    bool exitInProgress_;

    WinAppCmd(const WinAppCmd&); //prohibit usage
    const WinAppCmd& operator =(const WinAppCmd&); //prohibit usage

    bool doConfig(const CmdLine&);
    bool doExit(const CmdLine&);
    bool doShow(const CmdLine&);

};

inline bool WinAppCmd::exitInProgress() const
{
    return exitInProgress_;
}

END_NAMESPACE1

#endif
