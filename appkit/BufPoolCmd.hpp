/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_BUF_POOL_CMD_HPP
#define APPKIT_BUF_POOL_CMD_HPP

#include "appkit/Cmd.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)


//! dbug commands to troubleshoot BufPool usage
class BufPoolCmd: public Cmd
{

public:
    BufPoolCmd();

    virtual ~BufPoolCmd();
    virtual bool onRun(const CmdLine& req);
    virtual const char* xName(unsigned char cmdIndex) const;

private:
    typedef bool (BufPoolCmd::*doer_t)(const CmdLine& req);

    static doer_t doer_[];

    BufPoolCmd(const BufPoolCmd&); //prohibit usage
    const BufPoolCmd& operator =(const BufPoolCmd&); //prohibit usage

    bool doResetStat(const CmdLine&);
    bool doShow(const CmdLine&);
    bool doShrink(const CmdLine&);

};

END_NAMESPACE1

#endif
