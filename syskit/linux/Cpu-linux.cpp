/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include "syskit/Cpu.hpp"
#include "syskit/macros.h"

const char CPU_INFO[] = "/proc/cpuinfo";
const char CPU_MHZ_DELIM = ':';
const char CPU_MHZ_NAME[] = "cpu MHz";
const char NEW_LINE = '\n';
const double HERTZ_PER_MEGAHERTZ = 1000000.0;
const unsigned int BUF_SIZE = 4095;

BEGIN_NAMESPACE1(syskit)


//
// Return my processor's frequency in hertz. This method assumes the
// frequency is about the same for all CPUs in the same system. The
// value returned is the frequency of the first CPU entry the method
// sees. Return zero if the information is unavailable (most likely
// due to a coding error).
//
unsigned int Cpu::getHertz()
{

    // Return zero if the information is unavailable.
    unsigned int hertz = 0U;

    // Assume frequency is about the same for all CPUs in the same system. Read
    // one buffer from CPU_INFO. That should have at least one CPU_MHZ_NAME entry.
    // Sample entry: "cpu MHz         : 2294.849".
    int fd = open(CPU_INFO, O_RDONLY);
    if (fd >= 0)
    {
        char buf[BUF_SIZE + 1];
        buf[BUF_SIZE] = 0;
        ssize_t bytesRead = read(fd, buf, BUF_SIZE);
        if (bytesRead > 0)
        {
            char* s = strstr(buf, CPU_MHZ_NAME);
            if (s != 0)
            {
                s = strchr(s, CPU_MHZ_DELIM);
                if (s != 0)
                {
                    char* p;
                    double mHz = std::strtod(++s, &p);
                    if (p != s)
                    {
                        hertz = (unsigned int)(round(mHz * HERTZ_PER_MEGAHERTZ));
                    }
                }
            }
        }

        close(fd);
    }

    // Return my processor's frequency in Hertz.
    return hertz;
}

END_NAMESPACE1
