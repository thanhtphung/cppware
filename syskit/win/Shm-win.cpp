/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <aclapi.h>
#include <cstdio>

#include "syskit-pch.h"
#include "syskit/Shm.hpp"
#include "syskit/macros.h"
#include "syskit/win/Janitor-win.hpp"

BEGIN_NAMESPACE


class AdminSa
{

public:
    AdminSa();
    ~AdminSa();
    operator SECURITY_ATTRIBUTES*();

private:
    ACL* acl_;
    PSID sid_;
    SECURITY_ATTRIBUTES sa_;
    SECURITY_DESCRIPTOR sd_;
    bool ok_;

    AdminSa(const AdminSa&); //prohibit usage
    const AdminSa& operator =(const AdminSa&); //prohibit usage

};

inline AdminSa::operator SECURITY_ATTRIBUTES*()
{
    SECURITY_ATTRIBUTES* sa = ok_? &sa_: 0;
    return sa;
}


AdminSa::AdminSa()
{
    acl_ = 0;
    ok_ = false;
    sid_ = 0;

    // SID for the Administrators group.
    SID_IDENTIFIER_AUTHORITY authority = SECURITY_NT_AUTHORITY;
    AllocateAndInitializeSid(&authority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &sid_);
    if (sid_ == 0)
    {
        return;
    }

    // Allow the Administrators group full access.
    EXPLICIT_ACCESS_W ea = {0};
    ea.grfAccessPermissions = KEY_ALL_ACCESS;
    ea.grfAccessMode = SET_ACCESS;
    ea.grfInheritance = NO_INHERITANCE;
    ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea.Trustee.TrusteeType = TRUSTEE_IS_GROUP;
    ea.Trustee.ptstrName = static_cast<LPWSTR>(sid_);
    SetEntriesInAclW(1, &ea, 0, &acl_);
    if (acl_ == 0)
    {
        return;
    }

    InitializeSecurityDescriptor(&sd_, SECURITY_DESCRIPTOR_REVISION);
    BOOL daclPresent = TRUE;
    BOOL daclDefaulted = FALSE;
    if (!SetSecurityDescriptorDacl(&sd_, daclPresent, acl_, daclDefaulted))
    {
        return;
    }

    ok_ = true;
    sa_.nLength = sizeof(sa_);
    sa_.lpSecurityDescriptor = &sd_;
    sa_.bInheritHandle = FALSE;
}


AdminSa::~AdminSa()
{
    LocalFree(acl_);
    if (sid_ != 0)
    {
        FreeSid(sid_);
    }
}

END_NAMESPACE

BEGIN_NAMESPACE1(syskit)


Shm::Shm(const Shm& shm)
{

    // Assume failure.
    addr_ = 0;
    isCreator_ = false;
    key_ = 0;
    shm_ = 0;
    size_ = 0;

    HANDLE procHandle = GetCurrentProcess();
    HANDLE mapHandle;
    if (DuplicateHandle(procHandle, shm.shm_, procHandle, &mapHandle, 0, 0, DUPLICATE_SAME_ACCESS) == 0)
    {
        return;
    }

    void* mappedAddr = MapViewOfFile(mapHandle, FILE_MAP_WRITE, 0, 0, 0);
    if (mappedAddr == 0)
    {
        CloseHandle(mapHandle);
        return;
    }

    // Need a 4-byte overhead that holds the shared memory segment
    // size to be consistent with unix-like shared memory segments.
    addr_ = static_cast<unsigned char*>(mappedAddr)+4;
    key_ = shm.key_;
    shm_ = mapHandle;
    size_ = shm.size_;
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

    // Assume failure.
    addr_ = 0;
    isCreator_ = false;
    key_ = 0;
    shm_ = 0;
    size_ = 0;

    // Need a 4-byte overhead that holds the shared memory segment size to be
    // consistent with unix-like shared memory segments. Some shared memory
    // operations can fail in certain run-time environments w/o full admin access,
    // so do try to run w/ full admin access. Failure in getting full admin access
    // can be ignored as it will be reflected elsewhere properly.
    wchar_t keyStr[14 + 8 + 1]; //"Global\\syskit-xxxxxxxx"
    swprintf(keyStr, 14 + 8 + 1, L"Global\\syskit-%08x", key);
    unsigned int sizeLo = static_cast<unsigned int>(size + 4);
    unsigned int sizeHi = 0;
    AdminSa sa;
    HANDLE mapHandle = CreateFileMappingW(INVALID_HANDLE_VALUE, sa, PAGE_READWRITE, sizeHi, sizeLo, keyStr);
    if (mapHandle == 0)
    {
        return;
    }

    isCreator_ = (GetLastError() != ERROR_ALREADY_EXISTS);
    void* mappedAddr = MapViewOfFile(mapHandle, FILE_MAP_WRITE, 0, 0, 0);
    if (mappedAddr == 0)
    {
        CloseHandle(mapHandle);
        return;
    }

    // Detect mismatched size.
    if ((isCreator_ && (size == 0)) ||
        ((!isCreator_) && (*static_cast<unsigned int*>(mappedAddr) != size) && (size > 0)))
    {
        UnmapViewOfFile(mappedAddr);
        CloseHandle(mapHandle);
        return;
    }

    // Save size in the 4-byte overhead.
    if (isCreator_)
    {
        *static_cast<unsigned int*>(mappedAddr) = static_cast<unsigned int>(size);
    }
    addr_ = static_cast<unsigned char*>(mappedAddr)+4;
    key_ = key;
    shm_ = mapHandle;
    size_ = *static_cast<const unsigned int*>(mappedAddr);
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

    // Assume failure.
    addr_ = 0;
    isCreator_ = false;
    key_ = 0;
    shm_ = 0;
    size_ = 0;

    // Sanity check.
    if (size == 0)
    {
        return;
    }

    // Need a 4-byte overhead that holds the shared memory segment size to be
    // consistent with unix-like shared memory segments. Some shared memory
    // operations can fail in certain run-time environments w/o full admin access,
    // so do try to run w/ full admin access. Failure in getting full admin access
    // can be ignored as it will be reflected elsewhere properly.
    unsigned int sizeLo = static_cast<unsigned int>(size + 4);
    unsigned int sizeHi = 0;
    AdminSa sa;
    HANDLE mapHandle = CreateFileMappingW(INVALID_HANDLE_VALUE, sa, PAGE_READWRITE, sizeHi, sizeLo, 0);
    if (mapHandle == 0)
    {
        return;
    }

    void* mappedAddr = MapViewOfFile(mapHandle, FILE_MAP_WRITE, 0, 0, 0);
    if (mappedAddr == 0)
    {
        CloseHandle(mapHandle);
        return;
    }

    addr_ = static_cast<unsigned char*>(mappedAddr)+4;
    isCreator_ = true;
    shm_ = mapHandle;
    size_ = size;
}


//!
//! Destruct the shared memory segment.
//!
Shm::~Shm()
{
    if (addr_ != 0)
    {
        UnmapViewOfFile(addr_ - 4);
        CloseHandle(shm_);
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
        UnmapViewOfFile(addr_ - 4);
        CloseHandle(shm_);
        addr_ = 0;
        isCreator_ = false;
        shm_ = 0;
        size_ = 0;
        ok = true;
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
        UnmapViewOfFile(addr_ - 4);
        Janitor& janitor = Janitor::instance();
        janitor.closeAtExit(shm_);
        isCreator_ = false;
        addr_ = 0;
        shm_ = 0;
        size_ = 0;
    }
}

END_NAMESPACE1
