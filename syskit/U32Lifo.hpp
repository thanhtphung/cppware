/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_U32_LIFO_HPP
#define SYSKIT_U32_LIFO_HPP

#include "syskit/U32Fifo.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! last-in-first-out queue of 32-bit numbers (aka stack)
class U32Lifo: private U32Fifo
    //!
    //! A class representing a LIFO queue of 32-bit numbers. A LIFO queue is
    //! more commonly known as a stack. The queue has an initial capacity
    //! of capacity() items. The capacity can grow as needed if the
    //! growth factor is set to non-zero when constructed or afterwards
    //! using setGrowth(). If the queue is growable, growth can occur when
    //! items are pushed softly into the queue. For a LIFO queue, items are
    //! normally added at the top using push() and removed also from the top
    //! using pop(). In a full queue, items can also be pushed hard with
    //! pushHard() causing the bottom item to fall out. Implemented using
    //! U32Fifo with the LIFO's top equivalent to the FIFO's head. Example:
    //!\code
    //! U32Lifo q;
    //! q.push(item0);
    //! :
    //! unsigned int item1;
    //! q.pop(item1);
    //!\endcode
    //!
{

public:

    // From Growable.
    using Growable::INVALID_INDEX;
    using Growable::canGrow;
    using Growable::capacity;
    using Growable::growthFactor;
    using Growable::initialCap;

    // From U32Fifo.
    using U32Fifo::DefaultCap;
    using U32Fifo::item_t;
    using U32Fifo::numItems;
    using U32Fifo::peek;
    using U32Fifo::reset;
    using U32Fifo::resize;

    // Constructors.
    U32Lifo(const U32Lifo& q);
    U32Lifo(unsigned int capacity = DefaultCap, int growBy = 0);

    // Operators.
    const U32Lifo& operator =(const U32Lifo& q);

    // Queue management.
    bool pop(item_t& item);
    bool push(item_t item);
    bool pushHard(item_t item);
    bool pushHard(item_t item, bool& someWasDropped, item_t& droppedItem);

    // Override U32Fifo.
    virtual ~U32Lifo();

};

//! Pop top item up. Return true if successful (also return popped item in item).
//! Return false otherwise (queue is empty).
inline bool U32Lifo::pop(item_t& item)
{
    return U32Fifo::rm(item);
}

//! Push given item down. Return true if successful. Return false otherwise (queue
//! is already full).
inline bool U32Lifo::push(item_t item)
{
    return U32Fifo::addAtHead(item);
}

//! Push given item down hard. If the queue is full, the bottom item will fall out.
//! Return true if successful. Return false otherwise (queue has zero capacity).
inline bool U32Lifo::pushHard(item_t item)
{
    return U32Fifo::addAtHeadCircularly(item);
}

//! Push given item down hard. If the queue is full, the bottom item will fall out.
//! Return true if successful. Return false otherwise (queue has zero capacity). This
//! method also provides information on the dropped item, if any.
inline bool U32Lifo::pushHard(item_t item, bool& someWasDropped, item_t& droppedItem)
{
    return U32Fifo::addAtHeadCircularly(item, someWasDropped, droppedItem);
}

END_NAMESPACE1

#endif
