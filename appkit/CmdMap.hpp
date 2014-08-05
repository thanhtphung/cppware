/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_CMD_MAP_HPP
#define APPKIT_CMD_MAP_HPP

#include <string.h>
#include "syskit/Bst.hpp"
#include "syskit/CriSection.hpp"
#include "syskit/Singleton.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(appkit)

class Cmd;
class String;


//! command map aka command registry
class CmdMap: public syskit::Singleton
    //!
    //! A class representing a command map. This serves as the registry for all Cmd
    //! instances. Implemented as a per-process singleton constructed on first use.
    //! A typical Cmd registers using add() at construction and unregisters using rm()
    //! at destruction.
    //!
{

public:
    typedef bool(*cb0_t)(void* arg, const char* name, Cmd* cmd, unsigned char cmdIndex);
    typedef void(*cb1_t)(void* arg, const char* name, Cmd* cmd, unsigned char cmdIndex);
    typedef void(*updateCb_t)(void* arg, const String& cmdName, bool available);

    Cmd* find(const char* name) const;
    Cmd* find(const char* name, unsigned char& cmdIndex) const;
    unsigned int add(Cmd* cmd, unsigned int& numErrs);
    unsigned int maxNameLength() const;
    unsigned int rm(const Cmd* cmd);
    updateCb_t updateCb(void*& arg) const;
    void monitor(updateCb_t cb, void* arg = 0, updateCb_t* oldCb = 0, void** oldArg = 0);

    bool apply(cb0_t cb, void* arg = 0) const;
    void apply(cb1_t cb, void* arg = 0) const;

    static CmdMap* instance();

protected:
    CmdMap(const char* id, unsigned int initialRefCount);
    virtual ~CmdMap();

private:
    typedef struct
    {
        Cmd* cmd;
        unsigned char cmdIndex;
        char name[1]; //variable length
    } kvPair_t;

    static const char ID1[];

    syskit::Bst map_;
    syskit::CriSection mutable cs_;
    unsigned int maxNameLength_;

    updateCb_t updateCb_;
    void* updateCbArg_;

    CmdMap(const CmdMap&); //prohibit usage
    const CmdMap& operator =(const CmdMap&); //prohibit usage

    static void operator delete(void* p, size_t size);
    static void* operator new(size_t size);

    static syskit::Singleton* create(const char*, unsigned int, void*);
    static int compare(const void*, const void*);
    static int compareName(const void*, const void*);
    static void noOp(void*, const String&, bool);

};

END_NAMESPACE1

#include "syskit/BufPool.hpp"

BEGIN_NAMESPACE1(appkit)

inline void CmdMap::operator delete(void* p, size_t size)
{
    syskit::BufPool::freeBuf(p, size);
}

inline void* CmdMap::operator new(size_t size)
{
    void* buf = syskit::BufPool::allocateBuf(size);
    return buf;
}

//! Find the handler for given command.
//! Return zero if not found.
inline Cmd* CmdMap::find(const char* name) const
{
    unsigned char cmdIndex;
    Cmd* found = find(name, cmdIndex);
    return found;
}

//! Return the length of the longest command name in the map.
inline unsigned int CmdMap::maxNameLength() const
{
    return maxNameLength_;
}

END_NAMESPACE1

#endif
