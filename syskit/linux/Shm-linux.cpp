/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "syskit/Shm.hpp"
#include "syskit/sys.cpp"

#if __CYGWIN__
#define __key key
#endif

const syskit::shm_t INVALID_ID = -1;

BEGIN_NAMESPACE

static unsigned char* attach(int shm, unsigned int& key, size_t& size)
{

    // Attach to given system resource.
    shmid_ds shmInfo = {{0}, 0};
    if (shmctl(shm, IPC_STAT, &shmInfo) == 0)
    {
        void* addr = shmat(shm, 0, 0);
        if (addr != reinterpret_cast<void*>(-1))
        {
            key = shmInfo.shm_perm.__key;
            size = shmInfo.shm_segsz;
            return static_cast<unsigned char*>(addr);
        }
    }

    // Indicate attach failure.
    key = 0;
    size = 0;
    return 0;
}

END_NAMESPACE

BEGIN_NAMESPACE1(syskit)


Shm::Shm(const Shm& shm)
{
    addr_ = attach(shm.shm_, key_, size_);
    isCreator_ = false;
    shm_ = (addr_ != 0)? shm.shm_: INVALID_ID;
}


//!
//! Construct a shared memory segment with the given key and size in bytes.
//! Reuse existing shared memory segment if one already exists. Create a
//! new one otherwise. Given size must be non-zero and must not exceed the
//! platform-specific limit. The construction can fail due to lack of system
//! resources or due to an unsupported size. Use isOk() to determine if the
//! shared memory segment construction is successful.
//!
Shm::Shm(unsigned int key, size_t size)
{

    // Exclusively create shared memory segment of given size.
    // Reuse existing shared memory segment if one already exists.
    addr_ = 0;
    key_ = 0;
    size_ = 0;
    int flags = (size == 0)? (0): (IPC_CREAT | IPC_EXCL | 0644);
    shm_ = shmget(key, size, flags);
    if (shm_ == INVALID_ID)
    {
        isCreator_ = false;
        if (errno == EEXIST)
        {
            shm_ = shmget(key, size, 0);
        }
    }
    else
    {
        isCreator_ = (flags != 0);
    }

    // Attach to the shared memory segment.
    if (shm_ != INVALID_ID)
    {
        addr_ = attach(shm_, key_, size_);
        if ((addr_ == 0) || ((size_ != size) && (size > 0)))
        {
            if (isCreator_)
            {
                shmctl(shm_, IPC_RMID, 0);
            }
            shm_ = INVALID_ID;
        }
    }
}


//!
//! Construct a shared memory segment with the given size in bytes. Given
//! size must be non-zero and must not exceed the platform-specific limit.
//! The construction can fail due to lack of system resources or due to
//! an unsupported size. Use isOk() to determine if the shared memory
//! segment construction is successful.
//!
Shm::Shm(size_t size)
{

    // Create shared memory segment of given size.
    addr_ = 0;
    isCreator_ = true;
    key_ = 0;
    size_ = 0;
    shm_ = shmget(IPC_PRIVATE, size, IPC_CREAT | IPC_EXCL | 0644);

    // Attach to the created shared memory segment.
    if (shm_ != INVALID_ID)
    {
        addr_ = attach(shm_, key_, size_);
        if (addr_ == 0)
        {
            shmctl(shm_, IPC_RMID, 0);
            shm_ = INVALID_ID;
        }
    }
}


//!
//! Destruct the shared memory segment.
//!
Shm::~Shm()
{

    // Detach from the shared memory segment.
    if (addr_ != 0)
    {
        shmdt(addr_);
        if (isCreator_)
        {
            shmctl(shm_, IPC_RMID, 0);
        }
    }
}


//!
//! Remove the system resource even if this instance is not the creator.
//! Return true if successful. After removal, the Shm instance is no
//! longer valid.
//!
bool Shm::rm()
{
    bool ok;
    if (addr_ != 0)
    {
        shmdt(addr_);
        ok = (shmctl(shm_, IPC_RMID, 0) == 0);
        addr_ = 0;
        isCreator_ = false;
        shm_ = INVALID_ID;
        size_ = 0;
    }

    else
    {
        ok = false;
    }

    return ok;
}


//!
//! Detach from the system resource and allow it to persist. That is, after
//! detaching, the Shm instance is no longer valid, but the system resource
//! persits even after the Shm instance is destructed.
//!
void Shm::detach()
{
    if (addr_ != 0)
    {
        shmdt(addr_);
        addr_ = 0;
        isCreator_ = false;
        shm_ = INVALID_ID;
        size_ = 0;
    }
}

END_NAMESPACE1
