/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_CRI_SECTION_HPP
#define SYSKIT_CRI_SECTION_HPP

#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(syskit)


//! section of critical code which needs synchronization across threads
class CriSection
    //!
    //! A class representing a section of critical code which cannot be used by
    //! multiple threads concurrently. Before entering a critical section, it must
    //! be locked to prevent other threads from concurrent used. After exiting a
    //! critical section, it must be unlocked to allow its use by other threads.
    //! A critical section should be locked/unlocked by contructing/destructing a
    //! CriSection::Lock instance. Use lock() and unlock() only if CriSection::Lock
    //! cannot be used. Use tryLock() to avoid blocking calls. Recursive locks are
    //! allowed. Example:
    //!\code
    //! cs_.lock(); //cs_ is a CriSection instance
    //! thisFuncDoesNotWorkIfConcurrentlyUsed();
    //! thisFuncDoesNotWorkEitherIfConcurrentlyUsed();
    //! cs_.unlock();
    //!\endcode
    //!
{

public:
    enum
    {
        DefaultSpinCount = 6789
    };

    CriSection();
    CriSection(unsigned int spinCount);
    ~CriSection();

    bool tryLock();
    void lock();
    void unlock();


    //! critical section lock
    class Lock
        //!
        //! A class representing a critical section lock. Construct an instance
        //! to lock a critical section, and destruct it to unlock the critical
        //! section. Example:
        //!\code
        //! {
        //!   CriSection::Lock lock(cs_); //cs_ is a CriSection instance
        //!   thisFuncDoesNotWorkIfConcurrentlyUsed();
        //!   thisFuncDoesNotWorkEitherIfConcurrentlyUsed();
        //! }
        //!\endcode
        //!
    {
    public:
        Lock(CriSection& criSection);
        ~Lock();
    private:
        CriSection& criSection_;
        Lock(const Lock&); //prohibit usage
        const Lock& operator=(const Lock&); //prohibit usage
    };

private:
    criSection_t* cs_;

    CriSection(const CriSection&); //prohibit usage
    const CriSection& operator=(const CriSection&); //prohibit usage

};

END_NAMESPACE1

#endif
