/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef NETKIT_IP_ADDR_SET_HPP
#define NETKIT_IP_ADDR_SET_HPP

#include "appkit/U32Set.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(netkit)


//! set of IPv4 addresses
class IpAddrSet: protected appkit::U32Set
    //!
    //! A class representing a set of IP addresses. Implemented as an
    //! array of contiguous ranges using U32Set. An IpAddrSet instance
    //! consists of zero or more IP addresses. Strings can be used to
    //! specify IP addresses in the set. A valid string specification
    //! is a sequence of tokens, and each token is either a valid dotted
    //! IP address or a valid dotted IP address range ("lo-hi"). The token
    //! delimiter is a comma by default but can be overridden. White-spaces
    //! surrounding the token delimiter are allowed. Unsigned integers can
    //! also be used to specify IP addresses in the set. For example,
    //! 0xac101001U is equivalent to "172.16.16.1". Example:
    //!\code
    //! IpAddrSet set;
    //! set.add("172.16.16.2-172.16.16.11,172.16.16.80");
    //! set.rm("172.16.16.4");
    //! set.rm(0xac101006, 0xac101008); //"172.16.16.6-172.16.16.8"
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

    // From Set.
    using Set::operator delete;
    using Set::operator new;
    using Set::type;

    // From U32Set.
    using U32Set::DefaultCap;
    using U32Set::VALID_MAX;
    using U32Set::VALID_MIN;
    using U32Set::compare;
    using U32Set::compareP;
    using U32Set::getRange;
    using U32Set::numRanges;
    using U32Set::pick;
    using U32Set::reset;
    using U32Set::resize;

    // Constructors.
    IpAddrSet(const IpAddrSet& set);
    IpAddrSet(const char* s, char delim, unsigned int capacity = DefaultCap);
    IpAddrSet(const char* s, unsigned int capacity = DefaultCap);
    IpAddrSet(unsigned int capacity = DefaultCap);

    // Operators.
    bool operator !=(const IpAddrSet& set) const;
    bool operator ==(const IpAddrSet& set) const;
    const IpAddrSet& operator &=(const IpAddrSet& set);
    const IpAddrSet& operator =(const IpAddrSet& set);
    const IpAddrSet& operator |=(const IpAddrSet& set);

    // Set queries.
    bool contains(const IpAddrSet& set) const;
    bool contains(const char* s, char delim = ',') const;
    bool contains(unsigned int addr) const;
    bool contains(unsigned int loAddr, unsigned int hiAddr) const;
    bool overlaps(const IpAddrSet& set) const;
    unsigned int maxAddr() const;
    unsigned int minAddr() const;
    unsigned long long numAddrs() const;

    // Set queries via range index.
    unsigned int hiAddrAt(size_t index) const;
    unsigned int loAddrAt(size_t index) const;

    // Set modifications.
    bool add(const IpAddrSet& set);
    bool add(const char* s, char delim = ',');
    bool add(unsigned int addr);
    bool add(unsigned int loAddr, unsigned int hiAddr);
    bool rm(const IpAddrSet& set);
    bool rm(const char* s, char delim = ',');
    bool rm(unsigned int addr);
    bool rm(unsigned int loAddr, unsigned int hiAddr);

    // Override Set.
    virtual ~IpAddrSet();
    virtual Set* clone() const;
    virtual appkit::String toString(const char* delim = ",") const;

    // String utilities.
    static bool isValid(const char* s, char delim = ',');
    static bool strToRange(const char* s, unsigned int& loAddr, unsigned int& hiAddr);


    //! set iterator
    class Itor: public U32Set::Itor
        //!
        //! A class representing an IpAddrSet iterator. It provides a scheme to
        //! traverse the addresses in an IpAddrSet instance. Example:
        //!\code
        //! IpAddrSet::Itor it(set);
        //! unsigned int loAddr;
        //! unsigned int hiAddr;
        //! while (it.nextRange(loAddr, hiAddr))
        //! {
        //!   //do something with each range in the set
        //! }
        //!\endcode
        //!
    {

    public:

        // Constructors and destructor.
        Itor();
        Itor(const IpAddrSet& set, bool makeCopy = true);
        ~Itor();

        // Iterator support.
        bool curAddr(unsigned int& addr) const;
        bool nextAddr(unsigned int& addr);
        bool prevAddr(unsigned int& addr);

        // Utilities.
        void attach(const IpAddrSet& set, bool makeCopy = true);

    private:
        Itor(const Itor&); //prohibit usage
        const Itor& operator =(const Itor&); //prohibit usage

    };

private:
    appkit::String& normalize(appkit::String&, const char*, char) const;

};

//! Return true if this set does not equal given set.
inline bool IpAddrSet::operator !=(const IpAddrSet& set) const
{
    return !(U32Set::operator ==(set));
}

//! Return true if this set equals given set.
inline bool IpAddrSet::operator ==(const IpAddrSet& set) const
{
    return U32Set::operator ==(set);
}

//! Compute the intersection of this set and given set and
//! save the result in this set. Return reference to self.
inline const IpAddrSet& IpAddrSet::operator &=(const IpAddrSet& set)
{
    U32Set::operator &=(set);
    return *this;
}

inline const IpAddrSet& IpAddrSet::operator =(const IpAddrSet& set)
{
    U32Set::operator =(set);
    return *this;
}

//! Compute the union of this set and given set and save
//! the result in this set. Return reference to self.
inline const IpAddrSet& IpAddrSet::operator |=(const IpAddrSet& set)
{
    U32Set::operator |=(set);
    return *this;
}

//! Add addresses in set to this set. Return true
//! if at least one new address was added successfully.
inline bool IpAddrSet::add(const IpAddrSet& set)
{
    return U32Set::add(set);
}

//! Add address addr to this set. Return true if successful.
inline bool IpAddrSet::add(unsigned int addr)
{
    return U32Set::add(addr, addr);
}

//! Add given range "loAddr-hiAddr" to this set.
//! Return true if at least one new address was added successfully.
inline bool IpAddrSet::add(unsigned int loAddr, unsigned int hiAddr)
{
    return U32Set::add(loAddr, hiAddr);
}

//! Return true if this set is identical to or is a superset of given set.
inline bool IpAddrSet::contains(const IpAddrSet& set) const
{
    return U32Set::contains(set);
}

//! Return true if addr is a member of this set.
inline bool IpAddrSet::contains(unsigned int addr) const
{
    return U32Set::contains(addr, addr);
}

//! Return true if this instance contains all addresses in the given range.
inline bool IpAddrSet::contains(unsigned int loAddr, unsigned int hiAddr) const
{
    return U32Set::contains(loAddr, hiAddr);
}

//! Return true if this set and given set overlap. That
//! is, there's at least one address residing in both sets.
inline bool IpAddrSet::overlaps(const IpAddrSet& set) const
{
    return U32Set::overlaps(set);
}

//! Remove addresses in given set from this set. Return true if at
//! least one address was removed successfully. The given set can
//! include non-existent addresses. For example, if this set has
//! only odd addresses from 172.16.16.1 to 172.16.16.100, removing
//! addresses between 172.16.16.1 and 172.16.16.50 is allowed and
//! will result in removal of odd addresses from 172.16.16.1 to
//! 172.16.16.50.
inline bool IpAddrSet::rm(const IpAddrSet& set)
{
    return U32Set::rm(set);
}

//! Remove address addr from this set. Return true if successful.
inline bool IpAddrSet::rm(unsigned int addr)
{
    return U32Set::rm(addr, addr);
}

//! Remove addresses in given range from this set. Return true
//! if at least one address was removed successfully. The given
//! range can include non-existent addresses. For example,
//! if this set has only odd addresses from 172.16.16.1 to
//! 172.16.16.100, removing addresses between 172.16.16.1 and
//! 172.16.16.50 is allowed and will result in removal of odd
//! addresses from 172.16.16.1 to 172.16.16.50.
inline bool IpAddrSet::rm(unsigned int loAddr, unsigned int hiAddr)
{
    return U32Set::rm(loAddr, hiAddr);
}

//! Return the high address at given index. Don't do any error checking.
//! Behavior is unpredictable if given index is invalid.
inline unsigned int IpAddrSet::hiAddrAt(size_t index) const
{
    return U32Set::hiKeyAt(index);
}

//! Return the low address at given index. Don't do any error checking.
//! Behavior is unpredictable if given index is invalid.
inline unsigned int IpAddrSet::loAddrAt(size_t index) const
{
    return U32Set::loKeyAt(index);
}

//! Return the current maximum address in this set.
//! Return 0 if this set is empty.
inline unsigned int IpAddrSet::maxAddr() const
{
    return U32Set::maxKey();
}

//! Return the current minimum address in this set.
//! Return 0xffffffffU if this set is empty.
inline unsigned int IpAddrSet::minAddr() const
{
    return U32Set::minKey();
}

//! Return the current number of addresses in this set.
inline unsigned long long IpAddrSet::numAddrs() const
{
    return U32Set::numKeys();
}

//! Retrieve the current address. Return true if there's one. Return
//! false otherwise (if iterating has not started or if set is empty).
//! Behavior is unpredictable if the iterator is unattached.
inline bool IpAddrSet::Itor::curAddr(unsigned int& addr) const
{
    return U32Set::Itor::curKey(addr);
}

//! Retrieve the next address. Return true if there's one. Return
//! false otherwise (if set is empty or if there's no more addresses).
//! Behavior is unpredictable if the iterator is unattached. The first
//! invocation after construction or reset() will return the first
//! address.
inline bool IpAddrSet::Itor::nextAddr(unsigned int& addr)
{
    return U32Set::Itor::nextKey(addr);
}

//! Retrieve the previous address. Return true if there's one. Return
//! false otherwise (if set is empty or if there's no more addresses).
//! Behavior is unpredictable if the iterator is unattached. The first
//! invocation after construction or reset() will return the last
//! address.
inline bool IpAddrSet::Itor::prevAddr(unsigned int& addr)
{
    return U32Set::Itor::prevKey(addr);
}

//! Attach iterator to given set. A deep copy of the given set is made
//! if makeCopy is true. Normally, a deep copy should be made only if
//! the given set can change during iterating.
inline void IpAddrSet::Itor::attach(const IpAddrSet& set, bool makeCopy)
{
    return U32Set::Itor::attach(set, makeCopy);
}

END_NAMESPACE1

#endif
