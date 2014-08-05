/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <new>
#include <string>
#include "appkit/CmdLine.hpp"
#include "appkit/DelimitedTxt.hpp"
#include "syskit/RefCounted.hpp"
#include "syskit/macros.h"

using namespace appkit;
using namespace syskit;

const char CMD_LINE[] = "/proc/self/cmdline";
const unsigned int BUF_SIZE = 4096;

BEGIN_NAMESPACE

class CmdLine1: public RefCounted
{
public:
    const CmdLine* cmdLine() const;
    virtual ~CmdLine1();
    virtual void destroy() const;
    static const CmdLine1& instance();
private:
    CmdLine* cmdLine_;
    CmdLine1();
    String getCmdLine();
    String normalize(const String&);
};

inline const CmdLine* CmdLine1::cmdLine() const
{
    return cmdLine_;
}

CmdLine1::CmdLine1():
RefCounted(0)
{
    String cmd00 = getCmdLine();
    String cmdLine = normalize(cmd00);
    cmdLine_ = new CmdLine(cmdLine);
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

void CmdLine1::destroy() const
{
    delete this;
}

String CmdLine1::getCmdLine()
{
    String cmd00;
    int fd = open(CMD_LINE, O_RDONLY);
    if (fd >= 0)
    {
        for (;;)
        {
            char buf[BUF_SIZE];
            ssize_t bytesRead = read(fd, buf, sizeof(buf));
            if (bytesRead > 0)
            {
                cmd00.append(buf, bytesRead);
                continue;
            }
            break;
        }
        close(fd);
    }

    return cmd00;
}

String CmdLine1::normalize(const String& cmd00)
{
    String cmdLine;
    const char* arg;
    size_t length;
    DelimitedTxt it(cmd00, false, 0);
    while (it.next(arg, length))
    {
        if (!cmdLine.empty())
        {
            cmdLine.append(1, ' ');
        }

        // Argument already in quotes.
        if (((arg[0] == '\'') || (arg[0] == '"')) && (arg[length - 1] == arg[0]))
        {
            cmdLine.append(arg, length);
        }

        // Enquote each argument just in case it contains some whitespace.
        else
        {
            cmdLine.append(1, '"');
            cmdLine.append(arg, length);
            cmdLine.append(1, '"');
        }
    }

    return cmdLine;
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
