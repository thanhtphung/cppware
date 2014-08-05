/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_MISC_DBUG_CMD_HPP
#define APPKIT_MISC_DBUG_CMD_HPP

#include "appkit/Cmd.hpp"
#include "syskit/macros.h"

DECLARE_CLASS1(netkit, IpAddrSet)
DECLARE_CLASS1(syskit, Module)

BEGIN_NAMESPACE1(appkit)

class SysIo;


//! miscellaneous debug commands
class MiscDbugCmd: public Cmd
{

public:
    MiscDbugCmd(SysIo* sysIo);

    // Override Cmd.
    virtual ~MiscDbugCmd();
    virtual bool onRun(const CmdLine& req);
    virtual const char* usage(unsigned char cmdIndex) const;
    virtual const char* xName(unsigned char cmdIndex) const;
    virtual const char* xUsage(unsigned char cmdIndex) const;

protected:

    // Override Cmd.
    virtual bool allowReq(const CmdLine& req, const netkit::Paddr& dbugger);

private:
    typedef bool (MiscDbugCmd::*doer_t)(const CmdLine& req);

    SysIo* sysIo_;

    static doer_t doer_[];

    MiscDbugCmd(const MiscDbugCmd&); //prohibit usage
    const MiscDbugCmd& operator =(const MiscDbugCmd&); //prohibit usage

    bool addToAcl(const CmdLine&);
    bool doAclConfig(const CmdLine&);
    bool doEnv(const CmdLine&);
    bool doVersionShow(const CmdLine&);
    bool moveHome(const CmdLine&);
    bool rmFromAcl(const CmdLine&);
    bool updateAcl(const CmdLine&);

    static bool getAddrRange(const CmdLine&, netkit::IpAddrSet&);
    static bool showExe(void*, const syskit::Module&);
    static bool showModule(void*, const syskit::Module&);

};

END_NAMESPACE1

#endif
