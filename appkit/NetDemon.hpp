/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_NET_DEMON_HPP
#define APPKIT_NET_DEMON_HPP

#include "appkit/Demon.hpp"
#include "syskit/macros.h"

DECLARE_CLASS1(netkit, Paddr)
DECLARE_CLASS1(netkit, Winsock)
DECLARE_CLASS1(syskit, MappedFile)
DECLARE_CLASS1(syskit, SigTrap)

BEGIN_NAMESPACE1(appkit)

class CmdAgent;
class NetDemonCmd;
class String;
class SysIo;


//! networking demon application w/ built-in command handlers
class NetDemon: public Demon
{

public:
    NetDemon(const CmdLine& cmdLine,
        const String& name,
        bool startInExeDir = true,
        const char* defaultIoAddr = "0.0.0.0:0",
        const wchar_t* confDir = DEFAULT_CONF_DIR,
        const wchar_t* logDir = DEFAULT_LOG_DIR);

    CmdAgent* loopbackAgent() const;
    bool restartOnCrash() const;
    const String& alias() const;
    const SysIo* sysIo() const;
    unsigned int logMask() const;
    void setAlias(const String& alias);
    void setLogMask(unsigned int logMask);
    void setRestartMode(bool restartOnCrash);

    // Override App.
    virtual const char* usage() const;
    virtual const char* xUsage() const;
    virtual int onRun();
    virtual int run();

    // Override Demon.
    virtual bool stop(bool restart = false);
    virtual void onStart();
    virtual void onStop();
    virtual void onSysShutdown();

    virtual void onConfig(const CmdLine& config);

    static NetDemon* instance();

protected:
    netkit::Paddr defaultIoAddr() const;
    void setDefaultIoAddr(const char* defaultIoAddr);

    virtual ~NetDemon();

private:
    CmdAgent* loopbackAgent_;
    NetDemonCmd* cmd_;
    netkit::Paddr* defaultIoAddr_;
    syskit::SigTrap* sigTrap_;
    String* alias_;
    SysIo* sysIo_;
    netkit::Winsock* winsock_;
    bool restartOnCrash_;
    unsigned int volatile logMask_;

    NetDemon(const NetDemon&); //prohibit usage
    const NetDemon& operator =(const NetDemon&); //prohibit usage

    syskit::MappedFile* mapRcFile() const;
    unsigned int countDumps() const;
    void runStartUpCmds(const syskit::MappedFile&) const;

};

inline CmdAgent* NetDemon::loopbackAgent() const
{
    return loopbackAgent_;
}

//! Return true if demon will be automatically restarted on crash.
inline bool NetDemon::restartOnCrash() const
{
    return restartOnCrash_;
}

//! Return the demon's alias.
inline const String& NetDemon::alias() const
{
    return *alias_;
}

inline const SysIo* NetDemon::sysIo() const
{
    return sysIo_;
}

//! Return current log mask. Logging is minimal if most or all bits are
//! off and maximal if most or all bits are on.
inline unsigned int NetDemon::logMask() const
{
    return logMask_;
}

inline void NetDemon::setRestartMode(bool restartOnCrash)
{
    restartOnCrash_ = restartOnCrash;
}

END_NAMESPACE1

#endif
