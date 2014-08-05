/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <stddef.h>
#include "syskit/Thread.hpp"
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/Cmd.hpp"
#include "appkit/CmdMap.hpp"
#include "appkit/Str.hpp"
#include "appkit/StringVec.hpp"
#include "appkit/Tokenizer.hpp"

using namespace syskit;

BEGIN_NAMESPACE1(appkit)

const char CmdMap::ID1[] = "appkit::CmdMap"; //singleton ID


CmdMap::CmdMap(const char* id, unsigned int initialRefCount):
RefCounted(initialRefCount),
Singleton(id, initialRefCount),
map_(compare, 32 /*capacity*/, -1 /*growBy*/),
cs_(CriSection::DefaultSpinCount)
{
    maxNameLength_ = 0;

    updateCb_ = noOp;
    updateCbArg_ = 0;
}


CmdMap::~CmdMap()
{
    for (size_t i = map_.numItems(); i > 0; delete[] static_cast<const unsigned char*>(map_.peek(--i)));
}


//!
//! Find and return the handler for given command.
//!
Cmd* CmdMap::find(const char* name, unsigned char& cmdIndex) const
{
    Cmd* found = 0;
    void* foundItem;
    {
        CriSection::Lock lock(cs_);
        bool ok = map_.find(name, compareName, foundItem);
        if (ok)
        {
            const kvPair_t* kv = static_cast<const kvPair_t*>(foundItem);
            found = kv->cmd;
            cmdIndex = kv->cmdIndex;
        }
    }

    // Return result.
    // Return zero if not found.
    return found;
}


//!
//! Return per-process singleton. Construct on first use. Destruct at last use.
//! Application code is responsible in making sure first use and last use occurs
//! in one DLL/EXE binary.
//!
CmdMap* CmdMap::instance()
{
    static CmdMap* s_cmdMap = dynamic_cast<CmdMap*>(getSingleton(ID1, create, 1U /*initialRefCount*/, 0 /*createArg*/));
    for (; s_cmdMap == 0; Thread::yield());
    return s_cmdMap;
}


//!
//! Return current update callback and its opaque argument. The callback is invoked
//! when a command is added to or removed from the map.
//!
CmdMap::updateCb_t CmdMap::updateCb(void*& arg) const
{
    CriSection::Lock lock(cs_);
    arg = updateCbArg_;
    return updateCb_;
}


//
// This method should be invoked just once per process via Singleton::getSingleton().
//
Singleton* CmdMap::create(const char* id, unsigned int initialRefCount, void* /*arg*/)
{
    CmdMap* cmdMap = new CmdMap(id, initialRefCount);
    static CmdMap::Count s_lock(*cmdMap, true /*skipAddRef*/); //destruct at unload or exit
    return cmdMap;
}


//!
//! Iterate the map. Invoke callback at each command. The callback should
//! return true to continue iterating and should return false to abort
//! iterating. Return false if the callback aborted the iterating. Return
//! true otherwise. The map must not be updated during the iterating. Behavior
//! is unpredictable if it is udpated directly or indirectly by the callback.
//!
bool CmdMap::apply(cb0_t cb, void* arg) const
{
    bool ok = true;
    {
        CriSection::Lock lock(cs_);
        for (size_t i = 0; i < map_.numItems(); ++i)
        {
            const kvPair_t* kv = static_cast<const kvPair_t*>(map_.peek(i));
            if (!cb(arg, kv->name, kv->cmd, kv->cmdIndex))
            {
                ok = false;
                break;
            }
        }
    }

    // Return true if the iterating was not aborted.
    return ok;
}


//
// Compare opaques as kvPair_t pointers.
//
int CmdMap::compare(const void* item0, const void* item1)
{
    const char* k0 = static_cast<const kvPair_t*>(item0)->name;
    const char* k1 = static_cast<const kvPair_t*>(item1)->name;
    return Str::compareKI(k0, k1);
}


int CmdMap::compareName(const void* item0, const void* item1)
{
    const char* k0 = static_cast<const char*>(item0);
    const char* k1 = static_cast<const kvPair_t*>(item1)->name;
    return Str::compareKI(k0, k1);
}


//!
//! Add given command handler to map. Each handler is responsible for at least
//! zero but not more than 256 commands. Return the number of commands successfully
//! mapped. Also return the number of erroneous names (duplicate or overlimit).
//!
unsigned int CmdMap::add(Cmd* cmd, unsigned int& numErrs)
{
    bool ok = true;
    unsigned int numCmds;
    String cmdName;
    StringVec added;
    unsigned char cmdIndex = 0;
    Tokenizer tokenizer(cmd->nameSet());
    {
        CriSection::Lock lock(cs_);
        unsigned int numItems = map_.numItems();
        while (tokenizer.next(cmdName))
        {
            size_t kvSize = offsetof(kvPair_t, name) + cmdName.length() + 1;
            unsigned char* p = new unsigned char[kvSize];
            kvPair_t* kv = reinterpret_cast<kvPair_t*>(p);
            kv->cmd = cmd;
            kv->cmdIndex = cmdIndex++;
            memcpy(kv->name, cmdName.raw(), cmdName.byteSize());
            if (!map_.addIfNotFound(kv))
            {
                ok = false;
                delete[] p;
            }
            else
            {
                added.add(cmdName);
                if (cmdName.length() > maxNameLength_)
                {
                    maxNameLength_ = cmdName.length();
                }
            }
            if (cmdIndex == 0)
            {
                if (tokenizer.peek())
                {
                    ok = false;
                }
                break;
            }
        }
        numCmds = map_.numItems() - numItems;
    }

    // Announce command availability.
    bool available = true;
    for (size_t i = 0, addCount = added.numItems(); i < addCount; ++i)
    {
        updateCb_(updateCbArg_, added.peek(i), available);
    }

    // Return number of mapped and unmapped commands.
    numErrs = ok? (0): (tokenizer.countTokens() - numCmds);
    return numCmds;
}


//!
//! Remove given command handler from map. Each handler is responsible
//! for at least one but not more than 256 commands. Return the number
//! of commands successfully unmapped.
//!
unsigned int CmdMap::rm(const Cmd* cmd)
{
    bool findMaxNameLength = false;
    unsigned int numCmds;
    String cmdName;
    StringVec removed;
    Tokenizer tokenizer(cmd->nameSet());
    {
        CriSection::Lock lock(cs_);
        unsigned int numItems = map_.numItems();
        while (tokenizer.next(cmdName))
        {
            void* removedItem;
            if (map_.rm(cmdName.ascii(), compareName, removedItem))
            {
                removed.add(cmdName);
                if (cmdName.length() == maxNameLength_)
                {
                    findMaxNameLength = true;
                }
                delete[] static_cast<const unsigned char*>(removedItem);
            }
        }
        numCmds = numItems - map_.numItems();
    }

    // A command with a long name was removed,
    // so find the length of the longest command name.
    if (findMaxNameLength)
    {
        size_t maxNameLength = 0;
        CriSection::Lock lock(cs_);
        for (size_t i = 0; i < map_.numItems(); ++i)
        {
            const kvPair_t* kv = static_cast<const kvPair_t*>(map_.peek(i));
            size_t nameLength = strlen(kv->name);
            if (nameLength == maxNameLength_)
            {
                maxNameLength = maxNameLength_;
                break; //no change since another command name is as long
            }
            if (nameLength > maxNameLength)
            {
                maxNameLength = nameLength;
            }
        }
        maxNameLength_ = static_cast<unsigned int>(maxNameLength);
    }

    // Announce command unavailability.
    bool available = false;
    for (size_t i = 0, removeCount = removed.numItems(); i < removeCount; ++i)
    {
        updateCb_(updateCbArg_, removed.peek(i), available);
    }

    // Return number of commands successfully unmapped.
    return numCmds;
}


//!
//! Iterate the map. Invoke callback at each command. The map must not
//! be updated during the iterating. Behavior is unpredictable if it is
//! udpated directly or indirectly by the callback.
//!
void CmdMap::apply(cb1_t cb, void* arg) const
{
    CriSection::Lock lock(cs_);
    for (size_t i = 0; i < map_.numItems(); ++i)
    {
        const kvPair_t* kv = static_cast<const kvPair_t*>(map_.peek(i));
        cb(arg, kv->name, kv->cmd, kv->cmdIndex);
    }
}


//!
//! Monitor updates to the map. Invoke given callback when a command is added to
//! or removed from the map. Also return the previous update callback and its opaque
//! argument if oldCb and/or oldArg is non-zero.
//!
void CmdMap::monitor(updateCb_t cb, void* arg, updateCb_t* oldCb, void** oldArg)
{
    CriSection::Lock lock(cs_);
    if (oldCb)
    {
        *oldCb = updateCb_;
    }

    if (oldArg)
    {
        *oldArg = updateCbArg_;
    }

    updateCbArg_ = arg;
    updateCb_ = (cb == 0)? noOp: cb;
}


void CmdMap::noOp(void* /*arg*/, const String& /*cmdName*/, bool /*available*/)
{
}

END_NAMESPACE1
