/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <time.h>

#include "syskit-pch.h"

#include <dbghelp.h>
#pragma comment(lib,"dbghelp")

#include "syskit/AtomicWord.hpp"
#include "syskit/CallStack.hpp"
#include "syskit/Foundation.hpp"
#include "syskit/Module.hpp"
#include "syskit/Process.hpp"
#include "syskit/Thread.hpp"
#include "syskit/sys.hpp"

#define DUMP_SPEC L"%s-dump-" SIZE_T_SPECW L".txt"
#define HDR_FORMAT L"\r\n## %s pid=" SIZE_T_SPECW L"tid=" SIZE_T_SPECW L"\r\n"

const size_t ADDR_STRING_LENGTH = (sizeof(void*) == 8)? 16: 8; //"%016x" or "%08x"?
const size_t MAX_BASENAME_LENGTH = 255;
const size_t MAX_PATH_LENGTH = 4095;
const wchar_t BACKSLASH = L'\\';
const wchar_t DOT = L'.';
const wchar_t LOGS_SUBDIR[] = L"../Logs/";
const wchar_t SLASH = L'/';


BEGIN_NAMESPACE

class W
{
public:
    W(const char* s);
    ~W();
    operator const wchar_t*() const;
private:
    wchar_t* w_;
    W(const W&); //prohibit usage
    const W& operator =(const W&); //prohibit usage
};

inline W::operator const wchar_t*() const
{
    return w_;
}

W::W(const char* s)
{
    int numChars = MultiByteToWideChar(CP_UTF8, 0, s, -1, 0, 0);
    w_ = new wchar_t[numChars];
    MultiByteToWideChar(CP_UTF8, 0, s, -1, w_, numChars);
}

W::~W()
{
    delete[] w_;
}

END_NAMESPACE

BEGIN_NAMESPACE1(syskit)


CallStack::CallStack(const _CONTEXT* context, const wchar_t* dumpPath, unsigned long exceptionCode)
{

    // The sym API cannot be used concurrently. An atomic counter is used to prevent
    // concurrency here. Use of something like a static critical section would make the
    // callstack capability unavailable before and after static data construction/destruction.
    static AtomicWord::item_t s_symApiInUse = 0;
    AtomicWord* symApiInUse = reinterpret_cast<AtomicWord*>(&s_symApiInUse);
    for (; ++*symApiInUse > 1; --*symApiInUse, Thread::yield());

    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();
    if (context == 0)
    {
        _CONTEXT cr = {0};
        RtlCaptureContext(&cr);
        curFrame_ = saveStack(process, thread, &cr);
    }
    else
    {
        curFrame_ = saveStack(process, thread, context);
    }

    --*symApiInUse;
    exceptionCode_ = exceptionCode;
    setDumpPath(dumpPath);
}


CallStack::~CallStack()
{
    delete curFrame_;
    delete[] dumpPath_;
}


bool CallStack::showAddr(void* arg, const Module& module)
{
    FILE* f = static_cast<FILE*>(arg);

    // Example:
    /*
  00400000     712704 syskit-ut-d.exe
  77460000    1294336 ntdll.dll
  76010000     868352 kernel32.dll
  :
  */
    fwprintf(f, L"%0*X%11lu %s\r\n",
        ADDR_STRING_LENGTH, reinterpret_cast<size_t>(module.addr()),
        module.size(),
        module.basename());

    bool keepGoing = true;
    return keepGoing;
}


bool CallStack::showModule(void* arg, const Module& module)
{
    FILE* f = static_cast<FILE*>(arg);

    // Example:
    /*
  -------:
  module: C:/p4c/ipsprobe/vc10/nettools/../../bin/vc10-win32/syskit-ut-d.exe
  fileVer: 0.0.0.0
  prodVer: 0.0.0.0
  addr: 00400000
  size: 712704
  */
    wchar_t fileVerString[Module::MaxVerStringLength + 1];
    wchar_t prodVerString[Module::MaxVerStringLength + 1];
    fwprintf(f, L"%7s:\r\n" L"%7s: %s" L"\r\n%7s: %s\r\n" L"%7s: %s\r\n" L"%7s: %0*X\r\n" L"%7s: %lu\r\n",
        L"-------",
        L"module", module.path(),
        L"fileVer", module.fileVerString(fileVerString),
        L"prodVer", module.prodVerString(prodVerString),
        L"addr", ADDR_STRING_LENGTH, reinterpret_cast<size_t>(module.addr()),
        L"size", module.size());

    bool keepGoing = true;
    return keepGoing;
}


const CallStack::Frame* CallStack::saveStack(HANDLE process, HANDLE thread, const _CONTEXT* context)
{
    _CONTEXT cr = *context;
    STACKFRAME64 frame64 = {0};

#if _M_IX86
    DWORD machineType = IMAGE_FILE_MACHINE_I386;
    frame64.AddrPC.Offset = cr.Eip;
    frame64.AddrPC.Mode = AddrModeFlat;
    frame64.AddrFrame.Offset = cr.Ebp;
    frame64.AddrFrame.Mode = AddrModeFlat;
    frame64.AddrStack.Offset = cr.Esp;
    frame64.AddrStack.Mode = AddrModeFlat;

#elif _M_X64
    DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
    frame64.AddrPC.Offset = cr.Rip;
    frame64.AddrPC.Mode = AddrModeFlat;
    frame64.AddrFrame.Offset = cr.Rsp;
    frame64.AddrFrame.Mode = AddrModeFlat;
    frame64.AddrStack.Offset = cr.Rsp;
    frame64.AddrStack.Mode = AddrModeFlat;

#else
#error "unsupported architecture"

#endif

    IMAGEHLP_MODULEW64 module = {0};
    module.SizeOfStruct = sizeof(module);

    unsigned char symBuf[sizeof(SYMBOL_INFO) + (MAX_SYM_NAME - 1 /*SYMBOL_INFO.Name[1]*/)];
    SYMBOL_INFO* p = reinterpret_cast<SYMBOL_INFO*>(symBuf);
    p->SizeOfStruct = sizeof(*p);
    p->MaxNameLen = MAX_SYM_NAME;

    IMAGEHLP_LINE64 line = {0};
    line.SizeOfStruct = sizeof(line);

    // The UNICODE interfaces (SymInitializeW, SymFromAddrW, and SymGetLineFromAddrW64)
    // are unavailable on xp-5.1 and server-2003-5.2. The older interfaces (SymInitialize,
    // SymFromAddr, and SymGetLineFromAddr64) are used to support those environments.
    // Alternatively, run-time checks via LoadLibraryW() and GetProcAddress() should
    // be considered when feasible. --ttp
    Frame* curFrame = new Frame(0);
    Frame* topFrame = curFrame;
    BOOL invadeProcess = TRUE;
    char symPath[MAX_PATH_LENGTH + 1];
    SymInitialize(process, getSymPath(symPath), invadeProcess);
    while (StackWalk64(machineType, process, thread, &frame64, &cr, 0, SymFunctionTableAccess64, SymGetModuleBase64, 0))
    {

        Frame* frame = new Frame(reinterpret_cast<const void*>(frame64.AddrPC.Offset));
        topFrame->setNext(frame);
        topFrame = frame;

        bool ok = (SymGetModuleInfoW64(process, frame64.AddrPC.Offset, &module) != 0);
        frame->setModule(ok, module.ModuleName);
        DWORD64 displacement64 = 0;
        ok = (SymFromAddr(process, frame64.AddrPC.Offset, &displacement64, p) != 0);
        frame->setSym(ok, W(p->Name), displacement64);
        DWORD displacement = 0;
        ok = (SymGetLineFromAddr64(process, frame64.AddrPC.Offset, &displacement, &line) != 0);
        frame->setPath(ok, W(line.FileName), line.LineNumber, displacement);

        if ((frame64.AddrReturn.Offset == 0) || (frame64.AddrReturn.Offset == frame64.AddrPC.Offset))
        {
            break;
        }
    }

    SymCleanup(process);
    const Frame* frame = curFrame->next();
    curFrame->setNext(0);
    delete curFrame;
    return frame;
}


//
// The executable directory is not part of the default symbol search path. This method
// return a more useful symbol search path including the executable directory where pdb
// files likely reside.
//
const char* CallStack::getSymPath(char* symPath /*[MAX_PATH_LENGTH+1]*/)
{
    wchar_t symPathW[MAX_PATH_LENGTH + 1];
    getSymPath(symPathW);
    WideCharToMultiByte(CP_UTF8, 0, symPathW, -1, symPath, MAX_PATH_LENGTH + 1, 0, 0);
    return symPath;
}


const wchar_t* CallStack::getCurDir(wchar_t curDir[MAX_PATH_LENGTH + 1])
{
    GetCurrentDirectoryW(MAX_PATH_LENGTH + 1, curDir);
    normalizeDir(curDir);
    return curDir;
}


//
// The executable directory is not part of the default symbol search path. This method
// return a more useful symbol search path including the executable directory where pdb
// files likely reside.
//
const wchar_t* CallStack::getSymPath(wchar_t* symPath /*[MAX_PATH_LENGTH+1]*/)
{
    wchar_t basename[MAX_BASENAME_LENGTH + 1];
    size_t n = getExe(symPath, basename);
    wchar_t* w = symPath + n;
    *w++ = L';';
    *w++ = L'.';
    *w++ = L'/';
    *w = 0;

    return symPath;
}


long __stdcall CallStack::filter(_EXCEPTION_POINTERS* p, unsigned long exceptionCode, const wchar_t* dumpPath)
{
    try
    {
        CallStack cs(p->ContextRecord, dumpPath, exceptionCode);
        bool showVersions = true;
        bool showAddrs = true;
        cs.dump(showAddrs, showVersions);
    }
    catch (...)
    {
        // Ignore further issues and avoid generating more callstacks recursively.
        ;
    }

    return EXCEPTION_EXECUTE_HANDLER;
}


long __stdcall CallStack::topFilter(_EXCEPTION_POINTERS* p)
{
    EXCEPTION_RECORD* er = p->ExceptionRecord;
    unsigned long exceptionCode = (er != 0)? er->ExceptionCode: 0;
    const wchar_t* dumpPath = 0;
    long rc = filter(p, exceptionCode, dumpPath);

    void* arg;
    Thread::crashCb_t crashCb = Thread::crashCb(arg);
    crashCb(arg);

    winTopFilter_t winTopFilter = Foundation::instance().winTopFilter();
    if (winTopFilter != 0)
    {
        rc = winTopFilter(p);
    }

    return rc;
}


//
// Determine directory and basename of executable.
// Return length of executable directory string.
//
size_t CallStack::getExe(wchar_t exeDir[MAX_PATH_LENGTH + 1], wchar_t basename[MAX_BASENAME_LENGTH + 1])
{
    GetModuleFileNameW(0, exeDir, MAX_PATH_LENGTH + 1);
    normalizeDir(exeDir);

    wchar_t* w = wcsrchr(exeDir, SLASH);
    w = (w != 0)? (w + 1): (exeDir);
    wcscpy_s(basename, MAX_BASENAME_LENGTH + 1, w);
    *w = 0;

    size_t length = w - exeDir;
    return length;
}


//!
//! Dump call stack. Show module addresses if showAddrs is true. Show OS and module versions
//! if showVersions is true. The call stack dump is appended to the dump file given earlier
//! when the call stack was constructed.
//!
void CallStack::dump(bool showAddrs, bool showVersions) const
{
    wchar_t timestamp[255 + 1];
    size_t bufSizeInChars = sizeof(timestamp) / sizeof(timestamp[0]);
    struct tm now;
    time_t t = time(0);
    localtime_s(&now, &t);
    wcsftime(timestamp, bufSizeInChars, L"|%c|", &now);

    // Show some environment. Example:
    /*
  ## |08/31/11 10:32:52| pid=5036 tid=8728
  cmdLine: "C:\p4c\ipsprobe\vc10\nettools\..\..\bin\vc10-win32\syskit-ut-d.exe"
  curDir: C:/p4c/ipsprobe/syskit-ut/vc10/syskit-ut
  user: ttp
  */
    DumpFile dumpFile(dumpPath_);
    Process myProc(Process::myId());
    wchar_t curDir[MAX_PATH_LENGTH + 1];
    wchar_t* user = myProc.user();
    fwprintf(dumpFile, HDR_FORMAT L"%7s: %s\r\n" L"%7s: %s\r\n" L"%7s: %s\r\n",
        timestamp, myProc.id(), Thread::myId(),
        L"cmdLine", GetCommandLineW(),
        L"curDir", getCurDir(curDir),
        L"user", user);
    delete[] user;

    // Show call stack.
    wchar_t* stack = describe();
    fwprintf(dumpFile, L"%s", stack);
    delete[] stack;

    // Show module addresses.
    FILE* arg = dumpFile;
    if (showAddrs)
    {
        fwprintf(dumpFile, L"\r\n## addr size basename\r\n");
        myProc.apply(showAddr, arg);
    }

    // Show versions.
    if (showVersions)
    {
        const wchar_t* osVer = Process::osVer();
        fwprintf(dumpFile, L"\r\n## versions\r\n" L"%7s: %s\r\n", L"osVer", osVer);
        delete[] osVer;
        myProc.apply(showModule, arg);
    }
}


void CallStack::normalizeDir(wchar_t* path)
{
    for (wchar_t* w = path; *w; ++w)
    {
        if (*w == BACKSLASH)
        {
            *w = SLASH;
        }
    }
}


//
// If zero, use a default path residing below the executable directory:
// "<exeDir>/Logs/<base>-dump-<pid>.txt", and also create the Logs
// subdirectory. Otherwise, assume the given path specifies a file in
// an existing directory.
//
void CallStack::setDumpPath(const wchar_t* dumpPath)
{
    wchar_t buf[MAX_PATH_LENGTH + 32 + 1]; //32 should be enough for something like "Logs/-dump-nnnnn.txt"
    if (dumpPath == 0)
    {

        wchar_t base[MAX_BASENAME_LENGTH + 1];
        wchar_t exeDir[MAX_PATH_LENGTH + 1];
        getExe(exeDir, base);
        wchar_t* w = wcsrchr(base, DOT);
        if (w != 0)
        {
            *w = 0;
        }

        size_t bufSizeInChars = sizeof(buf) / sizeof(buf[0]);
        size_t n = swprintf(buf, bufSizeInChars, L"%s%s", exeDir, LOGS_SUBDIR);
        CreateDirectoryW(buf, 0);

        size_t pid = Process::myId();
        bufSizeInChars -= n;
        swprintf(buf + n, bufSizeInChars, DUMP_SPEC, base, pid);
        dumpPath = buf;
    }

    dumpPath_ = strdup(dumpPath);
}


//!
//! Describe call stack as a string.
//! The returned string is to be freed by caller using the delete[] operator when done.
//!
wchar_t* CallStack::describe() const
{
    wchar_t hdr[255 + 1];
    size_t bufSizeInChars = sizeof(hdr) / sizeof(hdr[0]);
    size_t hdrLength = swprintf(hdr, bufSizeInChars, L"\r\n## call stack (exceptionCode=%08x)\r\n", exceptionCode_);

    // Compute required string length for all frames.
    size_t n = hdrLength;
    for (const Frame* frame = curFrame_; frame != 0; frame = frame->next())
    {
        const wchar_t* w = frame->describe();
        n += wcslen(w);
    }
    ++n;

    // Describe call stack. Example:
    /*
  ## call stack (exceptionCode=00000000)
  00411504: syskit-ut-d!MiscSuite::testCallStack00#188
  c:/p4c/ipsprobe/syskit-ut/miscsuite.cpp+37#37
  004230BC: syskit-ut-d!CppUnit::TestCaller<MiscSuite>::runTest#19
  c:/p4c/ipsprobe/include/cppunit/testcaller.h+173#0
  :
  */
    wchar_t* desc = new wchar_t[n];
    wchar_t* w = desc;
    memcpy(w, hdr, hdrLength * sizeof(desc[0]));
    w += hdrLength;
    for (const Frame* frame = curFrame_; frame != 0; frame = frame->next())
    {
        const wchar_t* p = frame->describe();
        n = wcslen(p);
        memcpy(w, p, n * sizeof(w[0]));
        w += n;
    }
    *w = 0;

    return desc;
}


CallStack::DumpFile::DumpFile(const wchar_t* dumpPath)
{
    const wchar_t* mode = L"ab+";
    _wfopen_s(&dump_, dumpPath, mode);
    if (dump_ == 0)
    {
        dump_ = stderr; //last resort
    }
}


CallStack::DumpFile::~DumpFile()
{
    if (dump_ != stderr)
    {
        std::fclose(dump_);
    }
}


CallStack::Frame::Frame(const void* eip)
{
    desc_ = 0;
    eip_ = eip;
    path_ = 0;
    lineNum_ = 0;
    lineOff_ = 0;
    module_ = 0;
    next_ = 0;
    sym_ = 0;
    symOff_ = 0;
}


CallStack::Frame::~Frame()
{
    delete[] desc_;
    delete[] path_;
    delete[] sym_;
    delete[] module_;
    delete next_;
}


//!
//! Describe frame as a string.
//!
const wchar_t* CallStack::Frame::describe() const
{
    if (desc_ != 0)
    {
        return desc_;
    }

    wchar_t buf[8191 + 1];
    size_t bufSizeInChars = sizeof(buf) / sizeof(buf[0]);
    const wchar_t* sym = Frame::sym();
    size_t n = (*sym == 0)?
        swprintf(buf, bufSizeInChars, L"%0*X: %s\r\n", ADDR_STRING_LENGTH, eip_, module()):
        swprintf(buf, bufSizeInChars, L"%0*X: %s!%s#%llu\r\n", ADDR_STRING_LENGTH, eip_, module(), sym, symOff_);

    const wchar_t* path = Frame::path();
    if (*path != 0)
    {
        bufSizeInChars -= n;
        swprintf(buf + n, bufSizeInChars, L"%*s%s+%lu#%lu\r\n", ADDR_STRING_LENGTH + 2, L"", path, lineNum_, lineOff_);
    }

    desc_ = strdup(buf);
    return desc_;
}


void CallStack::Frame::setModule(bool ok, const wchar_t* module)
{
    if (ok && (module != 0) && (module[0] != 0))
    {
        module_ = strdup(module);
    }
}


void CallStack::Frame::setPath(bool ok, const wchar_t* path, unsigned long lineNum, unsigned long lineOff)
{
    if (ok && (path != 0) && (path[0] != 0))
    {
        wchar_t* w = strdup(path);
        normalizeDir(w);
        path_ = w;
        lineNum_ = lineNum;
        lineOff_ = lineOff;
    }
}


void CallStack::Frame::setSym(bool ok, const wchar_t* sym, unsigned long long off)
{
    if (ok && (sym != 0) && (sym[0] != 0))
    {
        sym_ = strdup(sym);
        symOff_ = off;
    }
}

END_NAMESPACE1
