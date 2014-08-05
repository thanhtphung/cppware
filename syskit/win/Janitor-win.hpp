/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_JANITOR_WIN_HPP
#define SYSKIT_JANITOR_WIN_HPP

#include "syskit/CriSection.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


class Janitor
{

public:
    void closeAtExit(HANDLE h);
    static Janitor& instance();

private:
    CriSection cs_;
    HANDLE* handle_;
    size_t numHandles_;

    Janitor(const Janitor&); //prohibit usage
    const Janitor& operator =(const Janitor&); //prohibit usage

    Janitor();
    ~Janitor();

};

END_NAMESPACE1

#endif
