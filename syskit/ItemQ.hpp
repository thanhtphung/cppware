/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_ITEM_Q_HPP
#define SYSKIT_ITEM_Q_HPP

#include "syskit/Atomic32.hpp"
#include "syskit/Fifo.hpp"
#include "syskit/Semaphore.hpp"
#include "syskit/SpinSection.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! synchronized fifo queue
class ItemQ: private Fifo
    //!
    //! A class representing a synchronized FIFO queue of opaque items.
    //! Both blocking and non-blocking queuing operations are supported.
    //! Blocking operations can also time out if desired. Items are enqueued
    //! using the put() method and dequeued using the get() method. As an
    //! exception, urgent items can be enqueued in a LIFO manner using the
    //! expedite() method. The queue has an initial capacity of capacity()
    //! items. The capacity can grow as needed if the growth factor is set
    //! to non-zero when constructed. If the queue is growable, growth can
    //! occur when items are enqueued. Due to implementation details, the
    //! queue capacity cannot be more than MAX_CAP.
    //!
{

public:


    //!
    //! Item template.
    //!
    class Item
    {
    public:
        Item();
        static void release(const Item* item);
    protected:
        virtual ~Item();
        virtual void release() const;
    private:
        //Item(const Item&); //allow use of default copy constructor
        //const Item& operator =(const Item&); //allow use of default assignment operator
    };

    typedef bool(*cb0_t)(void* arg, Item* item, size_t index);

    static const unsigned int ETERNITY;
    static const unsigned int MAX_CAP;

    // From Fifo.
    using Fifo::DefaultCap;
    using Fifo::canGrow;
    using Fifo::capacity;
    using Fifo::growthFactor;
    using Fifo::initialCap;
    using Fifo::numItems;
    using Fifo::usagePeak;

    ItemQ(unsigned int capacity = DefaultCap, int growBy = 0);
    bool apply(cb0_t cb, void* arg = 0) const;
    bool isOk() const;
    bool expedite(Item* item, unsigned int timeoutInMsecs = ETERNITY);
    bool get(Item*& item, unsigned int timeoutInMsecs = ETERNITY);
    bool put(Item* item, unsigned int timeoutInMsecs = ETERNITY);
    void resetStat();

    virtual ~ItemQ();


    //!
    //! Available stats.
    //!
    class Stat
    {
    public:
        Stat(const ItemQ& q);
        int growthFactor() const;
        unsigned int capacity() const;
        unsigned int initialCap() const;
        unsigned int length() const;
        unsigned int numFails() const;
        unsigned int usagePeak() const;
        unsigned long long numGets() const;
        unsigned long long numPuts() const;
        void reset(const ItemQ& q);
    private:
        unsigned long long numGets_;
        unsigned long long numPuts_;
        int growBy_;
        unsigned int capacity_;
        unsigned int initialCap_;
        unsigned int length_;
        unsigned int numFails_;
        unsigned int usagePeak_;
        Stat(const Stat&); //prohibit usage
        const Stat& operator =(const Stat&); //prohibit usage
    };

protected:
    virtual bool grow();

private:
    Atomic32 numFails_;
    Semaphore inUseSlotCount_;
    Semaphore* emptySlotCount_;
    SpinSection mutable ss_;

    ItemQ(const ItemQ&); //prohibit usage
    const ItemQ& operator =(const ItemQ&); //prohibit usage

};

//! Return true if instance was constructed successfully.
inline bool ItemQ::isOk() const
{
    bool ok = emptySlotCount_->isOk() && inUseSlotCount_.isOk() && ss_.isOk();
    return ok;
}

//! Reset stats.
inline void ItemQ::resetStat()
{
    SpinSection::Lock lock(ss_);
    Fifo::resetStat();
}

//! Release given item. No-op if given item is zero.
inline void ItemQ::Item::release(const Item* item)
{
    if (item) item->release();
}

//! Get queue stats.
inline ItemQ::Stat::Stat(const ItemQ& q)
{
    reset(q);
}

//! Return the queues's growBy growth factor. A queue exponentially grows
//! by doubling if growBy is negative, and linearly grows by growBy entries
//! otherwise.
inline int ItemQ::Stat::growthFactor() const
{
    return growBy_;
}

//! Return the queue's capacity.
inline unsigned int ItemQ::Stat::capacity() const
{
    return capacity_;
}

//! Return the queue's initial capacity.
inline unsigned int ItemQ::Stat::initialCap() const
{
    return initialCap_;
}

//! Return the queue length.
inline unsigned int ItemQ::Stat::length() const
{
    return length_;
}

//! Return the failure count.
//! This is the number of unsuccessful enqueues.
inline unsigned int ItemQ::Stat::numFails() const
{
    return numFails_;
}

//! Return the usage peak.
//! This is high of the number of items in the queue.
inline unsigned int ItemQ::Stat::usagePeak() const
{
    return usagePeak_;
}

//! Return the get count.
//! This is the number of successful dequeues.
inline unsigned long long ItemQ::Stat::numGets() const
{
    return numGets_;
}

//! Return the put count.
//! This is the number of successful enqueues.
inline unsigned long long ItemQ::Stat::numPuts() const
{
    return numPuts_;
}

END_NAMESPACE1

#endif
