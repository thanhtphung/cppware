/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_SYS_IO_HPP
#define APPKIT_SYS_IO_HPP

#include <cstdio>
#include "netkit/IpAddrSet.hpp"
#include "netkit/Paddr.hpp"
#include "netkit/net.hpp"
#include "syskit/Atomic32.hpp"
#include "syskit/SpinSection.hpp"
#include "syskit/macros.h"

DECLARE_CLASS1(netkit, UdpClient)
DECLARE_CLASS1(syskit, Thread)

BEGIN_NAMESPACE1(appkit)

class Cmd;
class CmdLine;
class CmdMap;
class String;


//! I/O subsystem
class SysIo
    //!
    //! The I/O subsystem includes two dedicated agent threads to run commands.
    //! Commands which can be run must implement the Cmd interface. By default,
    //! remote access is enabled. It can be turned off using the localonly option.
    //! If remote access is enabled, the acl option specifies the access control
    //! list restricting remote access. An empty acl value allows remote access
    //! from any client, and a non-empty acl value allows remote access from only
    //! addresses in the list. Primitive logging is provided and can be customized
    //! via the onReq() and onRsp() virtual methods.
    //!
{

public:
    SysIo(const CmdLine& config, unsigned int defaultAddr = INADDR_ANY, unsigned short defaultPort = 0U);

    netkit::IpAddrSet acl() const;
    netkit::Paddr addr(size_t index) const;
    netkit::Paddr ioAddr() const;
    netkit::Paddr setIoAddr(const CmdLine& config);
    String clientConfig(size_t index) const;
    String state() const;
    bool isOk() const;
    bool logEntriesAreFlushed() const;
    bool reqLoggingIsEnabled() const;
    bool rspLoggingIsEnabled() const;
    char logPathHi() const;
    char logPathLo() const;
    int ibufSize() const;
    int obufSize() const;
    void setAcl(const netkit::IpAddrSet& acl);
    void setLogMode(bool logReq = true, bool logRsp = false, bool flushLogEntries = false);

    virtual ~SysIo();
    virtual bool onReq(const CmdLine& req, const netkit::Paddr& dbugger);
    virtual void onRsp(const CmdLine& req, const char* rsp, size_t rspLength, bool ok);

protected:
    SysIo(netkit::UdpClient* io0, netkit::UdpClient* io1);

    syskit::Atomic32* ioPort(size_t index);

private:
    enum
    {
        LogReq = 0x01,
        LogRsp = 0x02,
        FlushLogEntries = 0x80
    };

    typedef struct runArg_s
    {
        runArg_s(SysIo*, size_t);
        SysIo* sysIo;
        size_t index;
    } runArg_t;

    typedef void(*crashCb_t)(void* arg);

    crashCb_t oldCrashCb_;
    void* oldCrashArg_;

    syskit::Atomic32 ioPort_[2];
    std::FILE* volatile curLog_[2];
    netkit::IpAddrSet* acl_;
    netkit::Paddr ioAddr_;
    syskit::SpinSection mutable ss_;
    syskit::Thread* agent_[2];
    netkit::UdpClient* io_[2];
    const CmdMap* map_;
    int ibufSize_;
    int obufSize_;
    unsigned int addr_[2];
    unsigned int volatile logMask_;

    static const char LOG_PATH_HI;
    static const char LOG_PATH_LO;

    SysIo(const SysIo&); //prohibit usage
    const SysIo& operator =(const SysIo&); //prohibit usage

    netkit::UdpClient* move();
    bool allowAccessFrom(unsigned int) const;
    void formAcl(const CmdLine&);
    void formAddr(const CmdLine&);
    void loop(size_t);
    void mkIo(size_t);

    static bool showUsage(const CmdLine&, const Cmd&);
    static void bind(netkit::UdpClient&, unsigned int, unsigned short);
    static void logReq(const CmdLine&, const netkit::Paddr&, std::FILE*, bool);
    static void onCrash(void* arg);
    static void* agentEntry(void*);

};

inline syskit::Atomic32* SysIo::ioPort(size_t index)
{
    return &ioPort_[index];
}

//! Return true if entries are flushed as soon as they are logged.
inline bool SysIo::logEntriesAreFlushed() const
{
    bool flushed = ((logMask_ & FlushLogEntries) != 0);
    return flushed;
}

//! Return true if request logging is current enabled.
inline bool SysIo::reqLoggingIsEnabled() const
{
    bool enabled = ((logMask_ & LogReq) != 0);
    return enabled;
}

//! Return true if response logging is current enabled.
inline bool SysIo::rspLoggingIsEnabled() const
{
    bool enabled = ((logMask_ & LogRsp) != 0);
    return enabled;
}

//! Return the LogPath setting used for command logging support.
inline char SysIo::logPathHi() const
{
    return LOG_PATH_HI;
}

//! Return the LogPath setting used for command logging support.
inline char SysIo::logPathLo() const
{
    return LOG_PATH_LO;
}

//! Return the incoming buffer size in bytes.
inline int SysIo::ibufSize() const
{
    return ibufSize_;
}

//! Return the outgoing buffer size in bytes.
inline int SysIo::obufSize() const
{
    return obufSize_;
}

inline SysIo::runArg_s::runArg_s(SysIo* sysIo, size_t index)
{
    runArg_s::sysIo = sysIo;
    runArg_s::index = index;
}

END_NAMESPACE1

#endif
