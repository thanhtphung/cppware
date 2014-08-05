/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <cstdlib>
#include <new>

#include "syskit-pch.h"
#include "syskit/BufArena.hpp"
#include "syskit/BufPool.hpp"
#include "syskit/Foundation.hpp"
#include "syskit/RefCounted.hpp"
#include "syskit/SpinSection.hpp"
#include "syskit/sys.hpp"

const int ARENA_SIZE = 8192 * sizeof(void*); //bytes

BEGIN_NAMESPACE1(syskit)

static BufPool* s_bufPool = 0;
static const RefCounted* s_foundation = 0;


//!
//! Construct a buffer pool given its configuration. The configuration
//! is a string of triplets delimited by a semicolon. Each triplet is a
//! string delimited by a colon. Each triplet contains characteristics
//! of an individual buffer arena, namely 1) buffer size, 2) initial
//! capacity, and 3) the growth factor. Triplets with invalid or duplicate
//! buffer sizes are ignored. For example, the "4:256:256;128:0:32;"
//! configuration string specifies two buffer arenas. The first one has
//! buffer size 4 and initial capacity of 256 buffers, and it grows
//! linearly by 256 buffers as needed. The second one has buffer size
//! 128 and initial capacity of zero, and it grows by 32 buffers as
//! needed. A special configuration of "0:0:0;" can be used to disable
//! the buffer pool. For a disabled pool, buffers come from the default
//! c++ heap.
//!
BufPool::BufPool(const char* config)
{
    const char* cf;
    if (config == 0)
    {
        cf = "";
        maxBufSize_ = MaxBufSize;
    }
    else
    {
        cf = config;
        maxBufSize_ = (strcmp(config, "0:0:0;") == 0)? 0: MaxBufSize;
    }

    construct(cf);
}


BufPool::~BufPool()
{
    for (unsigned int bufSize = maxBufSize_; bufSize > 0; bufSize -= 4)
    {
        delete ss_[bufSize];
        delete arena_[bufSize];
    }
}


//!
//! Return the per-process singleton. The singleton is a default buffer pool.
//! This pool is intended to be used mainly by the syskit library internals.
//! However, use by application code is not prohibited. The method can be
//! safely used in static data.
//!
BufPool& BufPool::instance()
{
    if (s_bufPool == 0)
    {
        const Foundation& r = Foundation::instance();
        s_foundation = &r;
        s_bufPool = r.bufPool();
    }

    return *s_bufPool;
}


//!
//! Free a buffer. Result is unpredictable if given buffer address/size is
//! invalid. Return true if successful. The default c++ heap is used if
//! buffer size is not in the small-size range (1..MaxBufSize).
//!
bool BufPool::free(const void* buf, unsigned int bufSize)
{
    bool ok;
    if ((bufSize > 0) && (bufSize <= maxBufSize_))
    {
        SpinSection::Lock lock(*ss_[bufSize]);
        ok = arena_[bufSize]->freeBuf(buf);
    }
    else
    {
        delete[] static_cast<const unsigned char*>(buf);
        ok = true;
    }

    return ok;
}


bool BufPool::getArenaConfig(char* config, unsigned int& bufSize, unsigned int& capacity, int& growBy)
{
    bool ok = false;

    char* p;
    char* p0 = config;
    if ((p = strchr(p0, ':')) != 0)
    {
        *p = 0;
        char* p1 = p + 1;
        if ((p = strchr(p1, ':')) != 0)
        {
            *p++ = 0;
            if (*p)
            {
                bufSize = std::strtoul(p0, 0, 0);
                capacity = std::strtoul(p1, 0, 0);
                growBy = std::strtol(p, 0, 0);
                ok = true;
            }
        }
    }

    return ok;
}


bool BufPool::shrink()
{
    bool shrunk = false;
    for (unsigned int bufSize = maxBufSize_; bufSize > 0; bufSize -= 4)
    {
        BufArena* arena = arena_[bufSize];
        unsigned int newCap = arena->initialCap();
        SpinSection::Lock lock(*ss_[bufSize]);
        bool resized = arena->resize(newCap);
        if (resized)
        {
            shrunk = true;
        }
    }

    return shrunk;
}


bool BufPool::shrinkArena(unsigned int bufSize)
{
    bool shrunk;
    if ((bufSize > 0) && (bufSize <= maxBufSize_))
    {
        BufArena* arena = arena_[bufSize];
        unsigned int newCap = arena->initialCap();
        SpinSection::Lock lock(*ss_[bufSize]);
        shrunk = arena->resize(newCap);
    }
    else
    {
        shrunk = false;
    }

    return shrunk;
}


void BufPool::freeBuf(const void* p, size_t size)
{

    // Both s_bufPool and s_foundation have already been set
    // in BufPool::allocateBuf() or BufPool::instance().
    unsigned int bufSize = static_cast<unsigned int>(size);
    s_bufPool->free(p, bufSize);
    s_foundation->rmRef();
}


//
// Construct pool using given configuration.
//
void BufPool::construct(const char* config)
{
    arena_[0] = 0;
    ss_[0] = 0;
    for (unsigned int bufSize = 4; bufSize <= maxBufSize_; bufSize += 4)
    {
        arena_[bufSize] = 0;
        ss_[bufSize] = new SpinSection;
    }

    // Construct individual arenas as configured.
    // Ignore invalid/duplicate buffer sizes.
    char* p1;
    char* txt0 = syskit::strdup(config);
    for (char* p0 = txt0; (p1 = strchr(p0, ';')) != 0; p0 = p1 + 1)
    {
        *p1 = 0;
        unsigned int bufSize;
        unsigned int capacity;
        int growBy;
        bool ok = getArenaConfig(p0, bufSize, capacity, growBy);
        if (ok && (bufSize > 0) && (bufSize <= maxBufSize_) && ((bufSize & 3) == 0) && (arena_[bufSize] == 0))
        {
            arena_[bufSize] = new BufArena(bufSize, capacity, growBy);
        }
    }
    delete[] txt0;

    // Construct default arenas.
    for (unsigned int bufSize = 4; bufSize <= maxBufSize_; bufSize += 4)
    {
        if (arena_[bufSize] == 0)
        {
            int growBy = ARENA_SIZE / bufSize;
            unsigned int capacity = 0;
            arena_[bufSize] = new BufArena(bufSize, capacity, growBy);
        }
        BufArena* arena = arena_[bufSize];
        arena_[bufSize - 3] = arena;
        arena_[bufSize - 2] = arena;
        arena_[bufSize - 1] = arena;
        SpinSection* ss = ss_[bufSize];
        ss_[bufSize - 3] = ss;
        ss_[bufSize - 2] = ss;
        ss_[bufSize - 1] = ss;
    }
}


//!
//! Reset stats.
//!
void BufPool::resetStat()
{
    for (unsigned int bufSize = 4; bufSize <= maxBufSize_; bufSize += 4)
    {
        SpinSection::Lock lock(*ss_[bufSize]);
        arena_[bufSize]->resetStat();
    }
}


//!
//! Allocate a buffer of given size. Return its address. Return zero
//! if none. The default c++ heap is used if buffer size is not in the
//! small-size range (1..MaxBufSize).
//!
void* BufPool::allocate(unsigned int bufSize)
{
    void* buf;
    if ((bufSize > 0) && (bufSize <= maxBufSize_))
    {
        SpinSection::Lock lock(*ss_[bufSize]);
        buf = arena_[bufSize]->allocateBuf();
    }
    else
    {
        buf = new unsigned char[bufSize];
    }

    return buf;
}


void* BufPool::allocateBuf(size_t size)
{

    // Make sure s_bufPool and s_foundation are set. This can occur
    // in either here or BufPool::instance() depending on usage.
    if (s_bufPool == 0)
    {
        const Foundation& r = Foundation::instance();
        s_foundation = &r;
        s_bufPool = r.bufPool();
    }

    s_foundation->addRef();
    unsigned int bufSize = static_cast<unsigned int>(size);
    void* p = s_bufPool->allocate(bufSize);
    return p;
}


//!
//! Reset instance with statistics for given buffer size. Use zeroes
//! if given buffer size is not in the small-size range (1..MaxBufSize).
//!
void BufPool::Stat::reset(const BufPool& pool, unsigned int bufSize)
{
    if ((bufSize <= 0) || (bufSize > pool.maxBufSize_))
    {
        bufSize_ = bufSize;
        capacity_ = 0;
        growBy_ = 0;
        initialCap_ = 0;
        numAllocs_ = 0;
        numFails_ = 0;
        numFrees_ = 0;
        numInUseBufs_ = 0;
        usagePeak_ = 0;
        return;
    }

    // These following stats are static, so no locking required.
    const BufArena& arena = *pool.arena_[bufSize];
    bufSize_ = arena.bufSize();
    growBy_ = arena.growthFactor();
    initialCap_ = arena.initialCap();

    // Treat dynamic stats as one.
    BufArena::Stat* stat;
    unsigned char buf[sizeof(BufArena::Stat)];
    {
        SpinSection::Lock lock(*pool.ss_[bufSize]);
        stat = new(buf)BufArena::Stat(arena);
        capacity_ = arena.capacity();
        numInUseBufs_ = arena.numInUseBufs();
    }

    numAllocs_ = stat->numAllocs();
    numFails_ = stat->numFails();
    numFrees_ = stat->numFrees();
    usagePeak_ = stat->usagePeak();
    stat->BufArena::Stat::~Stat();
}

END_NAMESPACE1
