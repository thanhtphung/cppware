/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "appkit/Bool.hpp"
#include "appkit/CmdLine.hpp"
#include "appkit/DelimitedTxt.hpp"
#include "appkit/S32.hpp"
#include "appkit/StringVec.hpp"
#include "appkit/U32.hpp"

#include "netkit-pch.h"
#include "netkit/CapConfig.hpp"

using namespace appkit;

const int BUF_SIZE = 0x1000000; //16MB
const int CAP_LENGTH = 1518; //bytes (MTU + RoEtherPkt::VlanHdrLength)
const int CAP_LENGTH_MAX = 131072; //bytes
const int CAP_LENGTH_MIN = 64; //bytes
const int LOOP_TIMEOUT = 300; //msecs
const unsigned int AGENT_STACK_SIZE = 131072; //128KB

BEGIN_NAMESPACE1(netkit)


//!
//! Construct a default instance.
//!
CapConfig::CapConfig():
name_()
{
    filter_ = 0;
    reset();
}


CapConfig::CapConfig(const CapConfig& config):
name_(config.name_)
{
    filter_ = 0;
    copyFrom(config);
}


//!
//! Construct instance from given command line. Three options can be specified in
//! the command line: capconfig, capfilter, and capname. The capconfig option is a
//! colon-delimited string containing the following parameters: agentstacksize,
//! bepromiscuous, bufsize, capicpkts, caplength, capogpkts, and looptimeout. The
//! default capconfig option is: "131072:true:0x1000000:true:1518:true:300". The
//! capfilter option is a string containing a pcap-format filter. The default
//! capfilter option is an empty string. The capname option allows free-format
//! naming of network traffic captures. The default capname option is an empty
//! string.
//!
CapConfig::CapConfig(const CmdLine& cmdLine):
name_()
{
    filter_ = 0;
    reset(cmdLine);
}


CapConfig::~CapConfig()
{
    delete[] filter_;
}


//!
//! Return string form.
//!
String CapConfig::asString(bool includeFilter, bool includeName) const
{
    String s("capconfig=");
    s += U32(agentStackSize_);
    s += ':';
    s += bePromiscuous_? "true": "false";
    s += ':';
    s += S32(bufSize_);
    s += ':';
    s += capIcPkts_? "true": "false";
    s += ':';
    s += S32(capLength_);
    s += ':';
    s += capOgPkts_? "true": "false";
    s += ':';
    s += S32(loopTimeout_);
    s += '\n';

    if (includeFilter)
    {
        s += "capfilter=\"";
        s += filter_;
        s += "\"\n";
    }

    if (includeName)
    {
        s += "capname=";
        s += '\"';
        s += name_;
        s += "\"\n";
    }

    return s;
}


void CapConfig::copyFrom(const CapConfig& config)
{
    agentStackSize_ = config.agentStackSize_;
    bePromiscuous_ = config.bePromiscuous_;
    bufSize_ = config.bufSize_;
    capIcPkts_ = config.capIcPkts_;
    capLength_ = config.capLength_;
    capOgPkts_ = config.capOgPkts_;
    loopTimeout_ = config.loopTimeout_;

    size_t size = strlen(config.filter_) + 1;
    char* s = new char[size];
    memcpy(s, config.filter_, size);
    delete[] filter_;
    filter_ = s;
}


//!
//! Reset instance with default values.
//!
void CapConfig::reset()
{
    agentStackSize_ = AGENT_STACK_SIZE;
    bePromiscuous_ = true;
    bufSize_ = BUF_SIZE;
    capIcPkts_ = true;
    capLength_ = CAP_LENGTH;
    capOgPkts_ = true;
    loopTimeout_ = LOOP_TIMEOUT;

    size_t size = 1;
    char* s = new char[size];
    s[0] = 0;
    delete[] filter_;
    filter_ = s;

    name_.reset();
}


//!
//! Reset instance with given command line. Three options can be specified in
//! the command line: capconfig, capfilter, and capname. The capconfig option is a
//! colon-delimited string containing the following parameters: agentstacksize,
//! bepromiscuous, bufsize, capicpkts, caplength, capogpkts, and looptimeout. The
//! default capconfig option is: "131072:true:0x1000000:true:1518:true:300". The
//! capfilter option is a string containing a pcap-format filter. The default
//! capfilter option is an empty string. The capname option allows free-format
//! naming of network traffic captures. The default capname option is an empty
//! string.
//!
void CapConfig::reset(const CmdLine& cmdLine)
{
    delete[] filter_;

    bool useDefaults = true;
    const String* defaultV = 0;
    String optK("capconfig");
    const String* optV = cmdLine.opt(optK, defaultV);
    if (optV != 0)
    {
        bool makeCopy = false;
        char delim = ':';
        DelimitedTxt txt(*optV, makeCopy, delim);

        int growBy = 0;
        unsigned int capacity = 7 + 1;
        bool trimLines = true;
        StringVec vec(txt, trimLines, capacity, growBy);
        if (vec.numItems() == 7)
        {
            agentStackSize_ = U32(&vec[0], AGENT_STACK_SIZE /*defaultV*/);
            bePromiscuous_ = Bool(&vec[1], true /*defaultV*/);
            bufSize_ = S32(&vec[2], BUF_SIZE /*defaultV*/);
            capIcPkts_ = Bool(&vec[3], true /*defaultV*/);
            capLength_ = S32::isValid(vec[4].ascii(), CAP_LENGTH_MIN, CAP_LENGTH_MAX)? S32(vec[4]): CAP_LENGTH;
            capOgPkts_ = Bool(&vec[5], true /*defaultV*/);
            loopTimeout_ = S32(&vec[6], LOOP_TIMEOUT /*defaultV*/);
            size_t size = 1;
            char* s = new char[size];
            s[0] = 0;
            filter_ = s;
            useDefaults = false;
            name_.reset();
        }
    }

    if (useDefaults)
    {
        filter_ = 0;
        reset();
    }

    optK = "capfilter";
    optV = cmdLine.opt(optK, defaultV);
    if (optV != 0)
    {
        size_t size = optV->byteSize();
        char* s = new char[size];
        memcpy(s, optV->raw(), size);
        delete[] filter_;
        filter_ = s;
    }

    optK = "capname";
    name_ = *cmdLine.opt(optK, &name_);
}

END_NAMESPACE1
