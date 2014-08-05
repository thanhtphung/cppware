/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_D64_FIFO_HPP
#define SYSKIT_D64_FIFO_HPP

#include "syskit/Growable.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! first-in-first-out queue of 64-bit doubles
class D64Fifo: public Growable
    //!
    //! A class representing a FIFO queue of 64-bit doubles. The queue has
    //! an initial capacity of capacity() items. The capacity can grow
    //! as needed if the growth factor is set to non-zero when constructed
    //! or afterwards using setGrowth(). If the queue is growable, growth
    //! can occur when items are added non-circularly. For a FIFO queue,
    //! items are normally added at the tail and removed from the head. As
    //! exceptions, items can be added at the head and can also be removed
    //! from the tail. Implemented using a contiguous array. Example:
    //!\code
    //! D64Fifo q;
    //! q.add(item0);
    //! :
    //! double item1;
    //! q.rm(item1);
    //!\endcode
    //!
{

public:
    enum
    {
        DefaultCap = 64
    };

    typedef double item_t;

    // Constructors.
    D64Fifo(const D64Fifo& q);
    D64Fifo(unsigned int capacity = DefaultCap, int growBy = 0);

    // Operators.
    const D64Fifo& operator =(const D64Fifo& q);

    // Queue management.
    bool add(item_t item);
    bool addAtHead(item_t item);
    bool addAtHeadCircularly(item_t item);
    bool addAtHeadCircularly(item_t item, bool& someWasDropped, item_t& droppedItem);
    bool addCircularly(item_t item);
    bool addCircularly(item_t item, bool& someWasDropped, item_t& droppedItem);
    bool rm(item_t& item);
    bool rmFromTail(item_t& item);
    void reset();
    void resetStat();

    // Getters.
    unsigned int numItems() const;
    unsigned int usagePeak() const;
    item_t peek(size_t index) const;

    // Override Growable.
    virtual ~D64Fifo();
    virtual bool resize(unsigned int newCap);


    //! queue stats
    class Stat
        //!
        //! Available stats.
        //!
    {
    public:
        Stat(const D64Fifo& q);
        unsigned int numFails() const;
        unsigned int usagePeak() const;
        unsigned long long numAdds() const;
        unsigned long long numRms() const;
        void reset(const D64Fifo& q);
    private:
        unsigned long long numAdds_;
        unsigned long long numRms_;
        unsigned int numFails_;
        unsigned int usagePeak_;
        Stat(const Stat&); //prohibit usage
        const Stat& operator =(const Stat&); //prohibit usage
    };

private:
    item_t* item_;
    unsigned long long numAdds_;
    unsigned long long numRms_;
    unsigned int head_;
    unsigned int numFails_;
    unsigned int numItems_;
    unsigned int tail_;
    unsigned int usagePeak_;

    void construct(const D64Fifo&);

};

//! Peek at given index and return the residing item. Don't do any error
//! checking. Behavior is unpredictable if given index is invalid. This
//! provides easy access to all items in the queue. The item at index zero
//! corresponds to the item at the head of the queue, and the item at index
//! numItems()-1 corresponds to the item at the tail of the queue.
inline D64Fifo::item_t D64Fifo::peek(size_t index) const
{
    size_t i = head_ + index;
    return (i < capacity())? item_[i]: item_[i - capacity()];
}

//! Add given item to the head of the queue in a circular manner.
//! If the queue is full, the item at the tail is removed from the
//! queue before the given item is added. Return true if successful.
//! Return false otherwise (queue has a zero capacity).
inline bool D64Fifo::addAtHeadCircularly(item_t item)
{
    item_t droppedItem;
    if (numItems_ >= capacity()) rmFromTail(droppedItem);
    return addAtHead(item);
}

//! Add given item to the head of the queue in a circular manner. If
//! the queue is full, the item at the tail is removed from the queue
//! before the given item is added. Return true if successful. Return
//! false otherwise (queue has a zero capacity). This method also
//! provides information on the dropped item, if any.
inline bool D64Fifo::addAtHeadCircularly(item_t item, bool& someWasDropped, item_t& droppedItem)
{
    someWasDropped = (numItems_ >= capacity())? rmFromTail(droppedItem): false;
    return addAtHead(item);
}

//! Add given item to the tail of the queue in a circular manner.
//! If the queue is full, the item at the head is removed from the
//! queue before the given item is added. Return true if successful.
//! Return false otherwise (queue has a zero capacity).
inline bool D64Fifo::addCircularly(item_t item)
{
    item_t droppedItem;
    if (numItems_ >= capacity()) rm(droppedItem);
    return add(item);
}

//! Add given item to the tail of the queue in a circular manner. If
//! the queue is full, the item at the head is removed from the queue
//! before the given item is added. Return true if successful. Return
//! false otherwise (queue has a zero capacity). This method also
//! provides information on the dropped item, if any.
inline bool D64Fifo::addCircularly(item_t item, bool& someWasDropped, item_t& droppedItem)
{
    someWasDropped = (numItems_ >= capacity())? rm(droppedItem): false;
    return add(item);
}

//! Return the current number of items in the queue.
inline unsigned int D64Fifo::numItems() const
{
    return numItems_;
}

//! Return the usage peak.
//! This is high of the number of items in the queue.
inline unsigned int D64Fifo::usagePeak() const
{
    return usagePeak_;
}

//! Reset the queue by removing all items.
inline void D64Fifo::reset()
{
    numRms_ += numItems_;
    numItems_ = 0;
    head_ = tail_;
}

//! Reset stats.
inline void D64Fifo::resetStat()
{
    numAdds_ = 0ULL;
    numFails_ = 0U;
    numRms_ = 0ULL;
    usagePeak_ = numItems_;
}

//! Get queue stats.
inline D64Fifo::Stat::Stat(const D64Fifo& q)
{
    reset(q);
}

//! Return the failure count.
//! This is the number of unsuccessful adds.
inline unsigned int D64Fifo::Stat::numFails() const
{
    return numFails_;
}

//! Return the usage peak.
//! This is high of the number of items in the queue.
inline unsigned int D64Fifo::Stat::usagePeak() const
{
    return usagePeak_;
}

//! Return the add count.
//! This is the number of successful adds.
inline unsigned long long D64Fifo::Stat::numAdds() const
{
    return numAdds_;
}

//! Return the remove count.
//! This is the number of successful removes.
inline unsigned long long D64Fifo::Stat::numRms() const
{
    return numRms_;
}

END_NAMESPACE1

#endif
