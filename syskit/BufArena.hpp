/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_BUF_ARENA_HPP
#define SYSKIT_BUF_ARENA_HPP

#include "syskit/Growable.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! buffer arena
class BufArena: public Growable
    //!
    //! A class representing a buffer arena. Initially, a maximum of
    //! capacity() buffers can be produced and recycled. The capacity
    //! can grow as needed if the growth factor is set to non-zero when
    //! constructed or afterwards using setGrowth(). Each buffer has a
    //! fixed size of bufSize() bytes. Buffers are managed using the
    //! allocateBuf() and freeBuf() methods. Buffers are allocated in
    //! a least-recently-used manner.
    //!
{

public:

    // Constructors.
    BufArena(unsigned int bufSize, unsigned int capacity, int growBy = 0);

    // Buffer management.
    bool freeBuf(const void* buf);
    void reset();
    void resetStat();
    void* allocateBuf();

    // Getters.
    unsigned int bufSize() const;
    unsigned int numAvailBufs() const;
    unsigned int numInUseBufs() const;
    unsigned int usagePeak() const;

    // Utilities.
    bool ownsAddr(const void* addr) const;
    bool ownsBuf(const void* buf) const;
    static bool bufIsAvail(const BufArena& arena, const void* buf);
    static bool bufIsInUse(const BufArena& arena, const void* buf);

    // Override Growable.
    virtual ~BufArena();
    virtual bool resize(unsigned int newCap);


    //! buffer arena stats
    class Stat
        //!
        //! Available stats.
        //!
    {
    public:
        Stat(const BufArena& arena);
        unsigned int numFails() const;
        unsigned int usagePeak() const;
        unsigned long long numAllocs() const;
        unsigned long long numFrees() const;
        void reset(const BufArena& arena);
    private:
        unsigned long long numAllocs_;
        unsigned long long numFrees_;
        unsigned int numFails_;
        unsigned int usagePeak_;
        Stat(const Stat&); //prohibit usage
        const Stat& operator =(const Stat&); //prohibit usage
    };

private:
    typedef struct link_s
    {
        struct link_s* next;
        void* magicMark;
    } link_t;

    //
    // Bucket
    // - Buffers are allocated per bucket. If an arena is not growable,
    //   only one bucket is used. If an arena is growable, it grows by
    //   one bucket at a time. Multiple buckets are linked in a single
    //   linked list, and it is assumed the number of buckets is usually
    //   small (less than 32?).
    //
    class Bucket
    {
    public:
        Bucket(unsigned int bufSize, unsigned int capacity);
        ~Bucket();
        Bucket* next();
        bool ownsAddr(const void* addr) const;
        const Bucket* next() const;
        unsigned char* buf0() const;
        unsigned char* bufN() const;
        unsigned int capacity() const;
        unsigned int inUse() const;
        void decrementInUse() const;
        void setInUse(unsigned int inUse) const;
        void setNext(Bucket* next);
        static void deleteAll(const Bucket* bucket);
    private:
        Bucket* next_;
        unsigned char* buf0_; //first buffer
        unsigned char* bufN_; //the invalid Nth buffer
        unsigned int capacity_;
        unsigned int mutable inUse_;
        Bucket(const Bucket&); //prohibit usage
        const Bucket& operator =(const Bucket&); //prohibit usage
    };

    Bucket* bucket_;
    link_t* firstAvail_;
    link_t* lastAvail_;
    unsigned long long numAllocs_;
    unsigned long long numFrees_;
    unsigned int bufSize_;
    unsigned int numFails_;
    unsigned int numInUseBufs_;
    unsigned int usagePeak_;
    bool useMagicMark_;

    BufArena(const BufArena&); //prohibit usage
    const BufArena& operator =(const BufArena&); //prohibit usage

    void freeFullBuckets();
    void shrink(size_t);
    void useBucket(const Bucket&);

};

//! Determine if given address refers to an in-use buffer. Return
//! true if yes. If the buffer size is small (sizeof(void*)), this
//! method uses linear search. Otherwise, available buffers are
//! marked, so the magic mark is used to determine if a buffer is
//! in-use.
inline bool BufArena::bufIsInUse(const BufArena& arena, const void* buf)
{
    return (!bufIsAvail(arena, buf));
}

//! Return the buffer size of each buffer produced and recycled by this arena.
inline unsigned int BufArena::bufSize() const
{
    return bufSize_;
}

//! Return the number of available buffers. This number changes when the arena's
//! buffers are allocated or freed. It can also change when the arena grows or
//! shrinks.
inline unsigned int BufArena::numAvailBufs() const
{
    return (capacity() - numInUseBufs_);
}

//! Return the number of buffers in use.
inline unsigned int BufArena::numInUseBufs() const
{
    return numInUseBufs_;
}

//! Return the usage peak.
//! This is high of the number of buffers in use.
inline unsigned int BufArena::usagePeak() const
{
    return usagePeak_;
}

//! Reset stats.
inline void BufArena::resetStat()
{
    numAllocs_ = 0U;
    numFails_ = 0U;
    numFrees_ = 0U;
    usagePeak_ = numInUseBufs_;
}

inline BufArena::Bucket* BufArena::Bucket::next()
{
    return next_;
}

inline bool BufArena::Bucket::ownsAddr(const void* addr) const
{
    return (addr >= buf0_) && (addr < bufN_);
}

inline const BufArena::Bucket* BufArena::Bucket::next() const
{
    return next_;
}

inline unsigned char* BufArena::Bucket::buf0() const
{
    return buf0_;
}

inline unsigned char* BufArena::Bucket::bufN() const
{
    return bufN_;
}

inline unsigned int BufArena::Bucket::capacity() const
{
    return capacity_;
}

// Return the number of buffers in use.
// Available during shrink attempts only.
inline unsigned int BufArena::Bucket::inUse() const
{
    return inUse_;
}

inline void BufArena::Bucket::decrementInUse() const
{
    --inUse_;
}

inline void BufArena::Bucket::setInUse(unsigned int inUse) const
{
    inUse_ = inUse;
}

inline void BufArena::Bucket::setNext(Bucket* next)
{
    next_ = next;
}

//! Return the failure count.
//! This is the number of unsuccessful allocations.
inline unsigned int BufArena::Stat::numFails() const
{
    return numFails_;
}

//! Return the usage peak.
//! This is high of the number of buffers in use.
inline unsigned int BufArena::Stat::usagePeak() const
{
    return usagePeak_;
}

//! Return the allocation count.
//! This is the number of successful allocations.
inline unsigned long long BufArena::Stat::numAllocs() const
{
    return numAllocs_;
}

//! Return the free count.
//! This is the number of successful frees.
inline unsigned long long BufArena::Stat::numFrees() const
{
    return numFrees_;
}

//! Reset instance with statistics from given arena.
inline void BufArena::Stat::reset(const BufArena& arena)
{
    numAllocs_ = arena.numAllocs_;
    numFails_ = arena.numFails_;
    numFrees_ = arena.numFrees_;
    usagePeak_ = arena.usagePeak_;
}

END_NAMESPACE1

#endif
