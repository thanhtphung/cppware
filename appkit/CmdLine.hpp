/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_CMD_LINE_HPP
#define APPKIT_CMD_LINE_HPP

#include "appkit/String.hpp"
#include "appkit/StringDic.hpp"
#include "appkit/StringVec.hpp"
#include "syskit/sys.hpp"

BEGIN_NAMESPACE1(appkit)


//! command line
class CmdLine
    //!
    //! A class representing a command line. A command line consists of a
    //! command name, optional options, and optional arguments. An option
    //! starts with a single hyphen ('-') or a double hyphen ("--"), and an
    //! argument does not. Options must be specified before arguments are
    //! given. If options are duplicated, only the last one is significant.
    //! For example, in the "cmd -n=1 -n=2" command line, the "-n=1" option
    //! would be ignored. This implementation uses a vector to hold arguments
    //! and a map to hold options. This class supports the following syntax:
    //!\code
    //!   cmdName [-opt[=opt]]* [arg...]
    //!   cmdName [--opt[=opt]]* [arg...]
    //!\endcode
    //! Use instance() to access the singleton command line for the calling
    //! process. Use arg() to access the command line arguments. arg(0) is
    //! the command name. arg(1) is the first argument. Use opt() to access
    //! the options. opt() returns zero if the requested option is not in
    //! the command line arguments. opt() returns an empty string for
    //! options which have no values. Parsing of options is done in a case
    //! insensitive manner. Helper classes are provided to ease parameter value
    //! extraction. Refer to classes like U32 and U64 for more details. Example:
    //!\code
    //! const CmdLine& cmdLine = CmdLine::instance();
    //! if (cmdLine.numArgs() == 1) //no arguments?
    //! {
    //!   :
    //! }
    //!\endcode
    //!
{

public:

    // Constructors and destructor.
    CmdLine(const CmdLine& cmdLine);
    CmdLine(const String& cmd);
    CmdLine(const char* cmd = "");
    CmdLine(const char* cmd, size_t length);
    ~CmdLine();

    // Operators.
    bool operator ==(const CmdLine& cmdLine) const;
    const CmdLine& operator =(const CmdLine& cmdLine);
    const CmdLine& operator =(const String& cmd);
    const CmdLine& operator =(const char* cmd);
    static void operator delete(void* p, size_t size);
    static void operator delete(void* p, void* buf);
    static void* operator new(size_t size);
    static void* operator new(size_t size, void* buf);

    // Getters.
    bool vectorizeOpts(StringVec& k, StringVec& v) const;
    const String& asString() const;
    const String* arg(size_t argI, const String* defaultV = 0) const;
    const String* opt(const String& optK, const String* defaultV = 0) const;
    const String* opt(const char* optK, const String* defaultV = 0) const;
    const StringDic& optDic() const;
    const StringVec& argVec() const;
    unsigned int numArgs() const;
    unsigned int numOpts() const;

    // Setters.
    void reset(const char* cmd, size_t length);

    // User-managed tags.
    void setTag(void* tag) const;
    void* tag() const;

    // Utilities.
    static const CmdLine& instance();

private:
    String cmd_;
    StringDic optDic_;
    StringVec argVec_;
    void mutable* tag_;

    bool parseOpt(const String&);
    void copyArg(const syskit::Vec&);
    void markWhiteSpace(syskit::utf8_t*);
    void refresh();
    void parseArg(const String&);
    void parseCmd();

};

END_NAMESPACE1

#include "syskit/BufPool.hpp"

BEGIN_NAMESPACE1(appkit)

//! Reset instance with given command line.
inline const CmdLine& CmdLine::operator =(const String& cmd)
{
    cmd_ = cmd;
    refresh();
    return *this;
}

//! Reset instance with given command line.
inline const CmdLine& CmdLine::operator =(const char* cmd)
{
    cmd_ = cmd;
    refresh();
    return *this;
}

inline void CmdLine::operator delete(void* p, size_t size)
{
    syskit::BufPool::freeBuf(p, size);
}

inline void CmdLine::operator delete(void* /*p*/, void* /*buf*/)
{
}

inline void* CmdLine::operator new(size_t size)
{
    void* buf = syskit::BufPool::allocateBuf(size);
    return buf;
}

inline void* CmdLine::operator new(size_t /*size*/, void* buf)
{
    return buf;
}

//! Look at all options as two string vectors. Return true if successful.
//! Return false otherwise (result is partial due to insufficient capacity
//! in provided vectors).
inline bool CmdLine::vectorizeOpts(StringVec& k, StringVec& v) const
{
    bool ok = optDic_.vectorize(k, v);
    return ok;
}

//! Return all options as a string dictionary.
inline const StringDic& CmdLine::optDic() const
{
    return optDic_;
}

//! Return all arguments as a string vector.
inline const StringVec& CmdLine::argVec() const
{
    return argVec_;
}

//! Return the requested argument. Argument 0 is the command name.
//! Argument 1 is the first argument in the command line. Return
//! given default value if the requested argument is not in the
//! command line.
inline const String* CmdLine::arg(size_t argI, const String* defaultArg) const
{
    const String* s = (argI < argVec_.numItems())? &(argVec_.peek(argI)): defaultArg;
    return s;
}

//! Return the value of the requested option. Return given default
//! value if the requested option is not in the command line. Return
//! an empty string if the requested option exists without a value.
inline const String* CmdLine::opt(const String& optK, const String* defaultV) const
{
    const String* found = optDic_.find(optK, defaultV);
    return found;
}

//! Return the value of the requested option. Return given default
//! value if the requested option is not in the command line. Return
//! an empty string if the requested option exists without a value.
inline const String* CmdLine::opt(const char* optK, const String* defaultV) const
{
    const String* found = optDic_.find(optK, defaultV);
    return found;
}

//! Return the unparsed command line string.
inline const String& CmdLine::asString() const
{
    return cmd_;
}

//! Return the number of arguments found in the command line.
inline unsigned int CmdLine::numArgs() const
{
    return argVec_.numItems();
}

//! Return the number of options found in the command line.
inline unsigned int CmdLine::numOpts() const
{
    return optDic_.numKvPairs();
}

//! Reset instance with length characters from given command line.
inline void CmdLine::reset(const char* cmd, size_t length)
{
    cmd_.reset(cmd, length);
    refresh();
}

//! Attach given tag to the instance.
inline void CmdLine::setTag(void* tag) const
{
    tag_ = tag;
}

//! Return the attached tag.
inline void* CmdLine::tag() const
{
    return tag_;
}

END_NAMESPACE1

#endif
