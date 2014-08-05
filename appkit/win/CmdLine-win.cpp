/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "syskit/RefCounted.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/CmdLine.hpp"

using namespace appkit;
using namespace syskit;

BEGIN_NAMESPACE

class CmdLine1: public RefCounted
{
public:
    const CmdLine* cmdLine() const;
    virtual ~CmdLine1();
    static const CmdLine1& instance();
private:
    CmdLine* cmdLine_;
    CmdLine1();
    CmdLine1(const CmdLine1&); //prohibit usage
    const CmdLine1& operator =(const CmdLine1&); //prohibit usage
};

inline const CmdLine* CmdLine1::cmdLine() const
{
    return cmdLine_;
}

CmdLine1::CmdLine1():
RefCounted(0)
{
    const wchar_t* cmdW = GetCommandLineW();
    String cmd(cmdW);
    cmdLine_ = new CmdLine(cmd);
}

CmdLine1::~CmdLine1()
{
    delete cmdLine_;
}

const CmdLine1& CmdLine1::instance()
{
    static const CmdLine1* s_cmdLine1 = new CmdLine1;
    return *s_cmdLine1;
}

static RefCounted::Count s_cmdLine1Lock(CmdLine1::instance());

END_NAMESPACE

BEGIN_NAMESPACE1(appkit)


//!
//! Return the singleton command line for the calling process.
//! Each DLL/EXE has its own copy.
//!
const CmdLine& CmdLine::instance()
{
    const CmdLine1& cmdLine1 = CmdLine1::instance();
    return *cmdLine1.cmdLine();
}

END_NAMESPACE1
