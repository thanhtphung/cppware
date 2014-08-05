/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_CALL_STACK_HPP
#define SYSKIT_CALL_STACK_HPP

#include <cstdio>
#include "syskit/macros.h"

struct _CONTEXT;
struct _EXCEPTION_POINTERS;

BEGIN_NAMESPACE1(syskit)

class Module;


class CallStack
{

public:

    class Frame
    {
    public:
        Frame(const void* eip);
        ~Frame();
        const Frame* next() const;
        const void* eip() const;
        const wchar_t* describe() const;
        const wchar_t* module() const;
        const wchar_t* path() const;
        const wchar_t* sym() const;
        unsigned long lineOff() const;
        unsigned long lineNum() const;
        unsigned long long symOff() const;
        void setModule(bool ok, const wchar_t* module);
        void setNext(Frame* frame);
        void setPath(bool ok, const wchar_t* path, unsigned long lineNum, unsigned long lineOff);
        void setSym(bool ok, const wchar_t* sym, unsigned long long off);
    private:
        Frame* next_;
        const void* eip_;
        const wchar_t mutable* desc_;
        const wchar_t* module_;
        const wchar_t* path_;
        const wchar_t* sym_;
        unsigned long lineNum_;
        unsigned long lineOff_;
        unsigned long long symOff_;
        Frame(const Frame&); //prohibit usage
        const Frame& operator =(const Frame&); //prohibit usage
    };

    CallStack(const _CONTEXT* context = 0, const wchar_t* dumpPath = 0, unsigned long exceptionCode = 0);
    ~CallStack();
    const Frame* curFrame() const;
    const wchar_t* dumpPath() const;
    unsigned long exceptionCode() const;
    wchar_t* describe() const;
    void dump(bool showAddrs = true, bool showVersions = true) const;

    static long __stdcall filter(_EXCEPTION_POINTERS* p, unsigned long exceptionCode, const wchar_t* dumpPath);
    static long __stdcall topFilter(_EXCEPTION_POINTERS* p);

private:

    class DumpFile
    {
    public:
        DumpFile(const wchar_t* dumpPath);
        ~DumpFile();
        operator FILE*() const;
    private:
        std::FILE* dump_;
        DumpFile(const DumpFile&); //prohibit usage
        const DumpFile& operator =(const DumpFile&); //prohibit usage
    };

    const Frame* curFrame_;
    const wchar_t* dumpPath_;
    unsigned long exceptionCode_;

    CallStack(const CallStack&); //prohibit usage
    const CallStack& operator =(const CallStack&); //prohibit usage

    void setDumpPath(const wchar_t*);

    static bool showAddr(void*, const Module&);
    static bool showModule(void*, const Module&);
    static const Frame* saveStack(HANDLE, HANDLE, const _CONTEXT*);
    static const char* getSymPath(char*);
    static const wchar_t* getCurDir(wchar_t[]);
    static const wchar_t* getSymPath(wchar_t*);
    static size_t getExe(wchar_t[], wchar_t basename[]);
    static void normalizeDir(wchar_t*);

};

inline const CallStack::Frame* CallStack::curFrame() const
{
    return curFrame_;
}

inline const wchar_t* CallStack::dumpPath() const
{
    return dumpPath_;
}

inline unsigned long CallStack::exceptionCode() const
{
    return exceptionCode_;
}

inline CallStack::DumpFile::operator FILE*() const
{
    return dump_;
}

inline const CallStack::Frame* CallStack::Frame::next() const
{
    return next_;
}

inline const void* CallStack::Frame::eip() const
{
    return eip_;
}

inline const wchar_t* CallStack::Frame::module() const
{
    const wchar_t* w = (module_ == 0)? L"": module_;
    return w;
}

inline const wchar_t* CallStack::Frame::path() const
{
    const wchar_t* w = (path_ == 0)? L"": path_;
    return w;
}

inline const wchar_t* CallStack::Frame::sym() const
{
    const wchar_t* w = (sym_ == 0)? L"": sym_;
    return w;
}

inline unsigned long CallStack::Frame::lineNum() const
{
    return lineNum_;
}

inline unsigned long CallStack::Frame::lineOff() const
{
    return lineOff_;
}

inline unsigned long long CallStack::Frame::symOff() const
{
    return symOff_;
}

inline void CallStack::Frame::setNext(Frame* frame)
{
    next_ = frame;
}

END_NAMESPACE1

#include "syskit/win/link-with-syskit.h"
#endif
