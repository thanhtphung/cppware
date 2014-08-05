/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_THREAD_KEY_HPP
#define SYSKIT_THREAD_KEY_HPP

#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(syskit)


//! thread key (aka thread-specific data or thread local storage)
class ThreadKey
    //!
    //! A class representing a thread key (more commonly known as thread-specific
    //! data or thread local storage). Each instance can be associated with different
    //! thread-specific values. For example, thread A and thread B can map different
    //! values to one ThreadKey instance.
    //!
{

public:
    ThreadKey();
    ~ThreadKey();

    bool isOk() const;
    void setValue(void* value);
    void* value() const;

private:
    threadKey_t key_;

    static const threadKey_t INVALID_KEY;

    ThreadKey(const ThreadKey&); //prohibit usage
    const ThreadKey& operator=(const ThreadKey&); //prohibit usage

};

//! Return true if instance was successfully constructed.
inline bool ThreadKey::isOk() const
{
    return (key_ != INVALID_KEY);
}

END_NAMESPACE1

#endif
