/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit/BufPool.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/BufPoolCmd.hpp"
#include "appkit/U32.hpp"
#include "appkit/crt.hpp"

using namespace syskit;

// Supported command set.
const char CMD_SET[] =
" bufpool-resetstat"
" bufpool-show"
" bufpool-shrink"
;

// Extended names. One per command. Must match supported command set.
const char* const X_NAME[] =
{
    "reset BufPool stats", //bufpool-resetstat
    "show buffer arenas",  //bufpool-show
    "shrink buffer arenas" //bufpool-shrink
};

BEGIN_NAMESPACE1(appkit)


// Command doers. One per command. Must match supported command set.
BufPoolCmd::doer_t BufPoolCmd::doer_[] =
{
    &BufPoolCmd::doResetStat, //bufpool-resetstat
    &BufPoolCmd::doShow,      //bufpool-show
    &BufPoolCmd::doShrink     //bufpool-shrink
};


BufPoolCmd::BufPoolCmd():
Cmd(CMD_SET)
{
}


BufPoolCmd::~BufPoolCmd()
{
}


//
// bufpool-resetstat
//
bool BufPoolCmd::doResetStat(const CmdLine& req)
{
    BufPool& pool = BufPool::instance();
    pool.resetStat();
    respond(req, "Done.");

    bool cmdIsValid = true;
    return cmdIsValid;
}


//
// bufpool-show
//
bool BufPoolCmd::doShow(const CmdLine& req)
{
    String rsp;
    const BufPool& pool = BufPool::instance();
    unsigned int maxBufSize = pool.maxBufSize();
    if (maxBufSize == 0)
    {
        rsp = "None.";
    }

    else
    {

        // Header.
        String buf;
        rsp += (sprintf(buf, "%6s%8s%6s%6s%8s%8s%11s%11s%6s\n",
            "size", "cap", "cap0", "grow", "inUse", "wmark", "allocs", "frees", "fails"), buf);
        rsp += (sprintf(buf, "%6s%8s%6s%6s%8s%8s%11s%11s%6s\n",
            "----", "---", "----", "----", "-----", "-----", "------", "-----", "-----"), buf);

        // Body.
        for (unsigned int bufSize = 0; bufSize <= maxBufSize; bufSize += 4)
        {
            BufPool::Stat stat(pool, bufSize);
            if ((stat.capacity() > 0) || (stat.usagePeak() > 0))
            {
                rsp += (sprintf(buf, "%6u%8u%6u%6d%8u%8u%11llu%11llu%6u\n",
                    bufSize,
                    stat.capacity(),
                    stat.initialCap(),
                    stat.growthFactor(),
                    stat.numInUseBufs(),
                    stat.usagePeak(),
                    stat.numAllocs(),
                    stat.numFrees(),
                    stat.numFails()), buf);
            }
        }
    }

    respond(req, rsp);
    bool cmdIsValid = true;
    return cmdIsValid;
}


//
// bufpool-shrink [--arena=xxx]
//
bool BufPoolCmd::doShrink(const CmdLine& req)
{
    String optK("arena");
    U32 arena(req.opt(optK), 0 /*defaultV*/);

    BufPool& pool = BufPool::instance();
    (arena == 0)? pool.shrink(): pool.shrinkArena(arena);
    respond(req, "Done.");

    bool cmdIsValid = true;
    return cmdIsValid;
}


//!
//! Run command using dedicated agent threads provided by the Cmd framework. Return
//! true if command is valid. A false return results in some command usage hints as
//! response to the request.
//!
bool BufPoolCmd::onRun(const CmdLine& req)
{
    doer_t doer = doer_[cmdIndex(req)];
    bool cmdIsValid = (this->*doer)(req);
    return cmdIsValid;
}


const char* BufPoolCmd::xName(unsigned char cmdIndex) const
{
    return X_NAME[cmdIndex];
}

END_NAMESPACE1
