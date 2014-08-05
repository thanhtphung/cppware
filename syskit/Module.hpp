/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_MODULE_HPP
#define SYSKIT_MODULE_HPP

#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


//! loaded module
class Module
{

public:
    enum
    {
        MaxVerStringLength = 23 //"nnnnn.nnnnn.nnnnn.nnnnn"
    };

    Module();
    Module(const wchar_t* path, const unsigned char* addr, unsigned int size);
    ~Module();

    bool isOk() const;
    const unsigned char* addr() const;
    const wchar_t* basename() const;
    const wchar_t* fileVerString(wchar_t verString[MaxVerStringLength + 1]) const;
    const wchar_t* path() const;
    const wchar_t* prodVerString(wchar_t verString[MaxVerStringLength + 1]) const;
    unsigned long long fileVer() const;
    unsigned long long prodVer() const;
    unsigned int size() const;

    bool reset(const wchar_t* path, const unsigned char* addr, unsigned int size);

private:
    const wchar_t* basename_;
    const wchar_t* path_;

    bool ok_;
    const unsigned char* addr_;
    unsigned int size_;
    unsigned long long fileVer_;
    unsigned long long prodVer_;

    Module(const Module&); //prohibit usage
    const Module& operator =(const Module&); //prohibit usage

    bool getVers(const wchar_t*);

    static const wchar_t* formVerString(wchar_t[MaxVerStringLength + 1], unsigned long long);

};

//! Construct instance with given path, address, and size. Constructor can
//! fail if given path identifies a module with inacessible versions. Use
//! isOk() to determine construction status.
inline Module::Module(const wchar_t* path, const unsigned char* addr, unsigned int size)
{
    path_ = 0;
    reset(path, addr, size);
}

//! Return true if instance contains valid versions.
inline bool Module::isOk() const
{
    return ok_;
}

//! Return the module's basename.
inline const wchar_t* Module::basename() const
{
    return basename_;
}

//! Return the module's path.
inline const wchar_t* Module::path() const
{
    return path_;
}

//! Return the module base address.
inline const unsigned char* Module::addr() const
{
    return addr_;
}

//! Return the module's file version.
//! Return "0.0.0.0" if unavailable.
inline const wchar_t* Module::fileVerString(wchar_t verString[MaxVerStringLength + 1]) const
{
    return formVerString(verString, fileVer_);
}

//! Return the module's product version.
//! Return "0.0.0.0" if unavailable.
inline const wchar_t* Module::prodVerString(wchar_t verString[MaxVerStringLength + 1]) const
{
    return formVerString(verString, prodVer_);
}

//! Return the module size in bytes.
inline unsigned int Module::size() const
{
    return size_;
}

//! Return the module's file version.
//! Return zero if unavailable.
inline unsigned long long Module::fileVer() const
{
    return fileVer_;
}

//! Return the module's product version.
//! Return zero if unavailable.
inline unsigned long long Module::prodVer() const
{
    return prodVer_;
}

END_NAMESPACE1

#endif
