/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_PROCESS_HPP
#define SYSKIT_PROCESS_HPP

#include <sys/types.h>
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)

class Module;
class Utc;


//! running process
class Process
{

public:
    typedef bool(*cb0_t)(void* arg, const Module& module);
    typedef bool(*cb1_t)(void* arg, const Process& process);

    Process(size_t id);

    bool apply(cb0_t cb, void* arg = 0) const;
    size_t id() const;
    wchar_t* user() const;

    static Utc startTime();
    static bool apply(unsigned int maxPids, cb1_t cb, void* arg = 0);
    static bool idIsActive(size_t id);
    static bool setAffinityMask(size_t affinityMask);
    static size_t affinityMask();
    static size_t myId();
    static wchar_t* osVer();

private:
    size_t id_;

};

inline Process::Process(size_t id)
{
    id_ = id;
}

inline size_t Process::id() const
{
    return id_;
}

END_NAMESPACE1

#endif
