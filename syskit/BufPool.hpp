/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_BUF_POOL_HPP
#define SYSKIT_BUF_POOL_HPP

#include <sys/types.h>
#include "syskit/macros.h"

// Be aware of some conflicts with vc macros.
#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma push_macro("free")
#undef free
#endif

BEGIN_NAMESPACE1(syskit)

class BufArena;
class SpinSection;


//! pool of small-sized buffers
class BufPool
    //!
    //! A class representing a pool of small-sized buffers. Small-sized
    //! buffers are allocated in bulk. Each pool can be used to produce
    //! and recycle buffers of small sizes (up to MaxBufSize). Buffers
    //! are managed using the allocate() and free() methods. Buffers are
    //! allocated in a least-recently-used manner.
    //!
{

public:
    enum
    {
        MaxBufSize = 128 * sizeof(void*)
    };

    BufPool(const char* config = 0);
    ~BufPool();

    // Buffer management.
    bool free(const void* buf, unsigned int bufSize);
    void* allocate(unsigned int bufSize);

    // Utilities.
    bool shrink();
    bool shrinkArena(unsigned int bufSize);
    unsigned int maxBufSize() const;
    void resetStat();
    static BufPool& instance();
    static void freeBuf(const void* p, size_t size);
    static void* allocateBuf(size_t size);


    //! buffer pool stats
    class Stat
        //!
        //! Available stats. Per individual buffer arena.
        //!
    {
    public:
        Stat(const BufPool& pool, unsigned int bufSize);
        int growthFactor() const;
        unsigned int bufSize() const;
        unsigned int capacity() const;
        unsigned int initialCap() const;
        unsigned int numAvailBufs() const;
        unsigned int numFails() const;
        unsigned int numInUseBufs() const;
        unsigned int usagePeak() const;
        unsigned long long numAllocs() const;
        unsigned long long numFrees() const;
        void reset(const BufPool& pool, unsigned int bufSize);
    private:
        int growBy_;
        unsigned int bufSize_;
        unsigned int capacity_;
        unsigned int initialCap_;
        unsigned int numFails_;
        unsigned int numInUseBufs_;
        unsigned int usagePeak_;
        unsigned long long numAllocs_;
        unsigned long long numFrees_;
        Stat(const Stat&); //prohibit usage
        const Stat& operator =(const Stat&); //prohibit usage
    };

private:
    BufArena* arena_[MaxBufSize + 1];
    SpinSection mutable* ss_[MaxBufSize + 1];
    unsigned int maxBufSize_;

    BufPool(const BufPool&); //prohibit usage
    const BufPool& operator =(const BufPool&); //prohibit usage

    void construct(const char*);

    static bool getArenaConfig(char*, unsigned int&, unsigned int&, int&);

};

//! Return the effective maximum buffer size in bytes. By default, this is the
//! same as MaxBufSize. For a disabled buffer pool which uses buffers from the
//! default c++ heap, this value is zero.
inline unsigned int BufPool::maxBufSize() const
{
    return maxBufSize_;
}

//! Get stats for given buffer size. Use zeroes if given buffer size
//! is not in the small-size range (1..MaxBufSize).
inline BufPool::Stat::Stat(const BufPool& pool, unsigned int bufSize)
{
    reset(pool, bufSize);
}

//! Return the arena's growBy growth factor. An arena exponentially grows
//! by doubling if growBy is negative, and linearly grows by growBy buffers
//! otherwise.
inline int BufPool::Stat::growthFactor() const
{
    return growBy_;
}

//! Return the arena's buffer size.
inline unsigned int BufPool::Stat::bufSize() const
{
    return bufSize_;
}

//! Return the arena's capacity.
inline unsigned int BufPool::Stat::capacity() const
{
    return capacity_;
}

//! Return the arena's initial capacity.
inline unsigned int BufPool::Stat::initialCap() const
{
    return initialCap_;
}

//! Return the number of available buffers. This number changes when the arena's
//! buffers are allocated or freed. It can also change when the arena grows or
//! shrinks.
inline unsigned int BufPool::Stat::numAvailBufs() const
{
    unsigned int avail = (capacity_ - numInUseBufs_);
    return avail;
}

//! Return the arena's failure count.
//! This is the number of unsuccessful allocations.
inline unsigned int BufPool::Stat::numFails() const
{
    return numFails_;
}

//! Return the number of buffers in use.
inline unsigned int BufPool::Stat::numInUseBufs() const
{
    return numInUseBufs_;
}

//! Return the arena's usage peak.
//! This is high of the number of buffers in use.
inline unsigned int BufPool::Stat::usagePeak() const
{
    return usagePeak_;
}

//! Return the arena's allocation count.
//! This is the number of successful allocations.
inline unsigned long long BufPool::Stat::numAllocs() const
{
    return numAllocs_;
}

//! Return the arena's free count.
//! This is the number of successful frees.
inline unsigned long long BufPool::Stat::numFrees() const
{
    return numFrees_;
}

END_NAMESPACE1

// Be aware of some conflicts with vc macros.
#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma pop_macro("free")
#endif

#endif
