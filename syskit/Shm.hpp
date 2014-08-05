/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_SHM_HPP
#define SYSKIT_SHM_HPP

#include <string.h>
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(syskit)


//! shared memory segment
class Shm
    //!
    //! A class representing a shared memory segment.
    //!
{

public:
    Shm(const Shm& shm);
    Shm(size_t size);
    Shm(unsigned int key, size_t size);
    ~Shm();

    bool isCreator() const;
    bool isOk() const;
    bool rm();
    size_t size() const;
    unsigned char* addr() const;
    unsigned char* addr(size_t& size) const;
    unsigned int key() const;
    void detach();

private:
    bool isCreator_;
    shm_t shm_;
    size_t size_;
    unsigned char* addr_;
    unsigned int key_;

    const Shm& operator =(const Shm&); //prohibit usage

};

//! Return true if this instance is the creator of the attached system resource.
inline bool Shm::isCreator() const
{
    return isCreator_;
}

//! Return true if instance was successfully constructed.
inline bool Shm::isOk() const
{
    return (addr_ != 0);
}

//! Return the shared memory segment size in bytes.
inline size_t Shm::size() const
{
    return size_;
}

//! Return the shared memory segment address.
inline unsigned char* Shm::addr() const
{
    return addr_;
}

//! Return the shared memory segment address.
//! Also return the shared memory segment size.
inline unsigned char* Shm::addr(size_t& size) const
{
    size = size_;
    return addr_;
}

//! Return the shared memory segment key.
inline unsigned int Shm::key() const
{
    return key_;
}

END_NAMESPACE1

#endif
