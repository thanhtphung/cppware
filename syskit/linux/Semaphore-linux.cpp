/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "syskit/Semaphore.hpp"
#include "syskit/TickTime.hpp"
#include "syskit/sys.hpp"

const int INVALID_ID = -1;
const unsigned int MSECS_PER_SEC = 1000U;
const unsigned int NSECS_PER_MSEC = 1000000U;

BEGIN_NAMESPACE
#if __CYGWIN__

int semtimedop(int semid, struct sembuf *sops, size_t nsops, struct timespec* /*waitTime*/)
{
    return semop(semid, sops, nsops);
}

#endif
END_NAMESPACE

BEGIN_NAMESPACE1(syskit)

union semun
{
    int val;
    struct semid_ds* buf;
    unsigned short *array;
    struct seminfo* __buf;
};


//!
//! Construct a semaphore with given capacity. Given capacity must not be larger
//! than MAX_CAP. If it is, it is considered equivalent to MAX_CAP. The capacity
//! specifies the number of tokens available for acquisition. The construction
//! can fail due to lack of system resources. Use isOk() to determine if the
//! semaphore construction is successful.
//!
Semaphore::Semaphore(unsigned int capacity)
{
    sem_.index = 0;
    sem_.ok = 1;
    semIsMine_ = true;

    sem_.id = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0644);
    if (sem_.id != INVALID_ID)
    {
        semun arg;
        arg.val = (capacity > MAX_CAP)? MAX_CAP: capacity;
        if (semctl(sem_.id, 0, SETVAL, arg) != 0)
        {
            semctl(sem_.id, sem_.index, IPC_RMID, 0 /*arg*/);
            sem_.id = INVALID_ID;
            sem_.ok = 0;
        }
    }
}


Semaphore::~Semaphore()
{
    if (semIsMine_ && (sem_.id != INVALID_ID))
    {
        semctl(sem_.id, sem_.index, IPC_RMID, 0 /*arg*/);
    }
}


//!
//! Acquire delta tokens. delta must be non-zero and must not be larger
//! than MAX_CAP. Wait forever if necessary. Return true if successful.
//!
bool Semaphore::decrementBy(unsigned int delta)
{
    bool ok;
    if ((delta > 0) && (delta <= MAX_CAP))
    {
        struct sembuf sop;
        sop.sem_num = sem_.index;
        sop.sem_op = -delta;
        sop.sem_flg = 0;
        int rc;
        while (((rc = semop(sem_.id, &sop, 1)) != 0) && (errno == EINTR));
        ok = (rc == 0);
    }

    else
    {
        ok = false;
    }

    return ok;
}


//!
//! Acquire delta tokens. delta must be non-zero and must not be larger than
//! MAX_CAP. Wait at most waitTimeInMsecs msecs. Return true if successful.
//!
bool Semaphore::decrementBy(unsigned int delta, unsigned int waitTimeInMsecs)
{

    // Sanity check.
    if ((delta == 0) || (delta > MAX_CAP))
    {
        return false;
    }

    // Wait forever if necessary.
    struct sembuf sop;
    sop.sem_num = sem_.index;
    sop.sem_op = -delta;
    int rc;
    if (waitTimeInMsecs == ETERNITY)
    {
        sop.sem_flg = 0;
        while (((rc = semop(sem_.id, &sop, 1)) != 0) && (errno == EINTR));
    }

    // Do not wait.
    else if (waitTimeInMsecs == 0)
    {
        sop.sem_flg = IPC_NOWAIT;
        rc = semop(sem_.id, &sop, 1);
    }

    // Wait at most waitTimeInMsecs msecs.
    else
    {
        sop.sem_flg = 0;
        struct timespec waitTime;
        waitTime.tv_sec = waitTimeInMsecs / MSECS_PER_SEC;
        waitTime.tv_nsec = (waitTimeInMsecs % MSECS_PER_SEC) * NSECS_PER_MSEC;
        unsigned long long startTime = rdtsc();
        while (((rc = semtimedop(sem_.id, &sop, 1, &waitTime)) != 0) && (errno == EINTR))
        {
            unsigned long long delta = rdtsc() - startTime;
            unsigned int deltaInMsecs = static_cast<unsigned int>(delta * TickTime::msecsPerTick()); //close enough
            if (deltaInMsecs >= waitTimeInMsecs)
            {
                break;
            }
            unsigned int remainingTimeInMsecs = waitTimeInMsecs - deltaInMsecs;
            waitTime.tv_sec = remainingTimeInMsecs / MSECS_PER_SEC;
            waitTime.tv_nsec = (remainingTimeInMsecs % MSECS_PER_SEC) * NSECS_PER_MSEC;
        }
    }

    // Return true if successful.
    bool ok = (rc == 0);
    return ok;
}


//!
//! Release delta tokens. delta must be non-zero and must not be more
//! than the number of acquired tokens. Return true if successful.
//!
bool Semaphore::incrementBy(unsigned int delta)
{

    // Must use IPC_NOWAIT even though the operation should not block.
    // Seems like a semop() bug. --ttp
    struct sembuf sop;
    sop.sem_num = sem_.index;
    sop.sem_op = delta;
    sop.sem_flg = IPC_NOWAIT;

    bool ok = (semop(sem_.id, &sop, 1) == 0);
    return ok;
}


//!
//! Acquire delta tokens. delta must be non-zero and must not be larger
//! than MAX_CAP. Do not wait. Return true if successful.
//!
bool Semaphore::tryDecrementBy(unsigned int delta)
{
    bool ok;
    if ((delta > 0) && (delta <= MAX_CAP))
    {
        struct sembuf sop;
        sop.sem_num = sem_.index;
        sop.sem_op = -delta;
        sop.sem_flg = IPC_NOWAIT;
        ok = (semop(sem_.id, &sop, 1) == 0);
    }

    else
    {
        ok = false;
    }

    return ok;
}


//!
//! Detach from the system resource and allow it to persist. That is, after
//! detaching, the Semaphore instance is no longer valid, but the system
//! resource persits even after the Semaphore instance is destructed.
//!
void Semaphore::detach()
{
    sem_.id = INVALID_ID;
    sem_.index = 0;
    sem_.ok = 1;
    semIsMine_ = true;
}


//!
//! Reset semaphore value to given count.
//!
void Semaphore::reset(unsigned int count)
{
    semun arg;
    arg.val = (count > MAX_CAP)? MAX_CAP: count;
    semctl(sem_.id, sem_.index, SETVAL, arg);
}

END_NAMESPACE1
