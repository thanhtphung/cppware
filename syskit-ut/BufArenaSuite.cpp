#include "syskit/BufArena.hpp"
#include "syskit/BufPool.hpp"

#include "syskit-ut-pch.h"
#include "BufArenaSuite.hpp"

using namespace syskit;


BufArenaSuite::BufArenaSuite()
{
}


BufArenaSuite::~BufArenaSuite()
{
}


void BufArenaSuite::testBufPool00()
{
    BufPool& pool = BufPool::instance();
    unsigned int bufSize = 0;
    void* buf = pool.allocate(bufSize);
    bool ok = (buf != 0);
    CPPUNIT_ASSERT(ok);
    ok = pool.free(buf, bufSize);
    CPPUNIT_ASSERT(ok);
}


void BufArenaSuite::testBufPool01()
{
    const char* config = "0:0:0;";
    BufPool pool(config);
    unsigned int bufSize = 0;
    void* buf = pool.allocate(bufSize);
    bool ok = (buf != 0);
    CPPUNIT_ASSERT(ok);
    ok = pool.free(buf, bufSize);
    CPPUNIT_ASSERT(ok);
}


void BufArenaSuite::testBufPool02()
{
    const char* config = "4:256:256;8:32:-1;128:0:32;";
    BufPool pool(config);
    unsigned int maxBufSize = pool.maxBufSize();
    bool ok = (maxBufSize == BufPool::MaxBufSize);
    CPPUNIT_ASSERT(ok);

    for (unsigned int bufSize = 0; bufSize <= maxBufSize; ++bufSize)
    {
        void* buf = pool.allocate(bufSize);
        if ((buf == 0) || (!pool.free(buf, bufSize)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    void** buf4 = new void*[999];
    void** buf128 = new void*[999];
    for (size_t i = 0; i < 999; ++i)
    {
        buf4[i] = pool.allocate(4);
        buf128[i] = pool.allocate(128);
        if ((buf4[i] == 0) || (buf128[i] == 0))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    for (size_t i = 0; i < 999; ++i)
    {
        if ((!pool.free(buf4[i], 4)) || (!pool.free(buf128[i], 128)))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = (!pool.free(buf4[0], 4));
    CPPUNIT_ASSERT(ok);
    delete[] buf128;
    delete[] buf4;

    ok = (!pool.shrinkArena(0));
    CPPUNIT_ASSERT(ok);
    ok = (!pool.shrinkArena(8));
    CPPUNIT_ASSERT(ok);
    ok = pool.shrinkArena(128);
    CPPUNIT_ASSERT(ok);
    ok = pool.shrink();
    CPPUNIT_ASSERT(ok);

    pool.resetStat();
    ok = (BufPool::Stat(pool, 9999).usagePeak() == 0);
    CPPUNIT_ASSERT(ok);
    ok = (BufPool::Stat(pool, 4).usagePeak() == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Interfaces under test:
// - BufArena::BufArena(unsigned int, unsigned int, int);
// - bool BufArena::freeBuf(const void*);
// - bool BufArena::resize(unsigned int);
// - void* BufArena::allocateBuf();
// - BufArena::Stat::Stat(const BufArena&);
//
void BufArenaSuite::testCtor00()
{
    {
        BufArena arena(32 /*bufSize*/, 8 /*capacity*/, 8 /*growBy*/);
        validate(arena);
    }

    BufArena arena(4 /*bufSize*/, 8 /*capacity*/, 8 /*growBy*/);
    validate(arena);
}


//
// Interfaces under test:
// - BufArena::BufArena(unsigned int, unsigned int, int);
//
void BufArenaSuite::testCtor01()
{

    // Buffer size too small.
    BufArena arena0(0 /*bufSize*/, 32 /*capacity*/, 0 /*growBy*/);
    bool ok = ((arena0.numAvailBufs() == 32) &&
        (arena0.numInUseBufs() == 0) &&
        (arena0.usagePeak() == 0) &&
        (arena0.bufSize() == sizeof(void*)));
    CPPUNIT_ASSERT(ok);

    BufArena arena1(sizeof(void*) /*bufSize*/, 32 /*capacity*/, 0 /*growBy*/);
    ok = ((arena1.numAvailBufs() == 32) &&
        (arena1.numInUseBufs() == 0) &&
        (arena1.usagePeak() == 0) &&
        (arena1.bufSize() == sizeof(void*)));
    CPPUNIT_ASSERT(ok);

    BufArena arena2(16 /*bufSize*/, 32 /*capacity*/, 0 /*growBy*/);
    ok = ((arena2.numAvailBufs() == 32) &&
        (arena2.numInUseBufs() == 0) &&
        (arena2.usagePeak() == 0) &&
        (arena2.bufSize() == 16));
    CPPUNIT_ASSERT(ok);

    arena2.reset();
    while (arena2.allocateBuf() != 0);
    BufArena::Stat stat(arena2);
    ok = (stat.numFails() == 1) && (stat.usagePeak() == 32) && (stat.numAllocs() == 32) && (stat.numFrees() == 0);
    CPPUNIT_ASSERT(ok);
    arena2.reset();
    stat.reset(arena2);
    ok = (stat.numFails() == 1) && (stat.usagePeak() == 32) && (stat.numAllocs() == 32) && (stat.numFrees() == 32);
    CPPUNIT_ASSERT(ok);
    while (arena2.allocateBuf() != 0);
    stat.reset(arena2);
    ok = (stat.numFails() == 2) && (stat.usagePeak() == 32) && (stat.numAllocs() == 64) && (stat.numFrees() == 32);
    CPPUNIT_ASSERT(ok);
}


void BufArenaSuite::validate(BufArena& arena)
{
    void* p[64] = {0};
    bool ok = true;
    for (size_t i = 1; i <= 64; ++i)
    {
        void* buf = arena.allocateBuf();
        if ((buf == 0) ||
            (arena.numInUseBufs() != i) ||
            (arena.usagePeak() != i) ||
            BufArena::bufIsAvail(arena, buf) ||
            (!BufArena::bufIsInUse(arena, buf)) ||
            (!arena.ownsAddr(buf)) ||
            (!arena.ownsBuf(buf)))
        {
            ok = false;
            break;
        }
        p[i - 1] = buf;
    }
    CPPUNIT_ASSERT(ok);

    bool shrunk = arena.resize(arena.initialCap());
    ok = (!shrunk);
    CPPUNIT_ASSERT(ok);

    BufArena::Stat stat(arena);
    ok = (stat.numFails() == 0) && (stat.usagePeak() == 64) && (stat.numAllocs() == 64) && (stat.numFrees() == 0);
    CPPUNIT_ASSERT(ok);

    for (size_t i = 64; i > 0;)
    {
        void* buf = p[--i];
        if ((!arena.freeBuf(buf)) ||
            (arena.numInUseBufs() != i) ||
            (arena.usagePeak() != 64) ||
            (!BufArena::bufIsAvail(arena, buf)) ||
            BufArena::bufIsInUse(arena, buf))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    shrunk = arena.resize(48) && (arena.capacity() == 48);
    CPPUNIT_ASSERT(ok);
    shrunk = arena.resize(arena.initialCap()) && (arena.capacity() == arena.initialCap());
    CPPUNIT_ASSERT(ok);

    stat.reset(arena);
    ok = (stat.numFails() == 0) && (stat.usagePeak() == 64) && (stat.numAllocs() == 64) && (stat.numFrees() == 64);
    CPPUNIT_ASSERT(ok);
}
