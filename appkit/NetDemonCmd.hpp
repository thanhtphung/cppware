/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_NET_DEMON_CMD_HPP
#define APPKIT_NET_DEMON_CMD_HPP

#include "appkit/Cmd.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)

class NetDemon;
class SysIo;


//! debug commands for a networking demon application
class NetDemonCmd: public Cmd
{

public:
    NetDemonCmd(NetDemon* netDemon, SysIo* sysIo);

    // Override Cmd.
    virtual ~NetDemonCmd();
    virtual bool onRun(const CmdLine& req);
    virtual const char* usage(unsigned char cmdIndex) const;
    virtual const char* xName(unsigned char cmdIndex) const;

protected:

    // Override Cmd.
    virtual bool allowReq(const CmdLine& req, const netkit::Paddr& dbugger);

private:
    typedef bool (NetDemonCmd::*doer_t)(const CmdLine& req);

    static doer_t doer_[];

    NetDemon* netDemon_;
    SysIo* sysIo_;

    NetDemonCmd(const NetDemonCmd&); //prohibit usage
    const NetDemonCmd& operator =(const NetDemonCmd&); //prohibit usage

    bool doConfig(const CmdLine&);
    bool doExit(const CmdLine&);
    bool doShow(const CmdLine&);

};

END_NAMESPACE1

#endif
